#include "uart.h"

#include <stdint.h>

#include "stm32f1xx.h"

#define RX_BUF_SIZE 64
#define CMD_BUF_SIZE 64

uint8_t rx_buf[RX_BUF_SIZE];
uint8_t cmd_buf[CMD_BUF_SIZE];
uint32_t cmd_idx                      = 0;
static volatile uint8_t command_ready = 0;  // Добавил volatile для надежности в циклах

void uart1_init(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_USART1EN;
    RCC->AHBENR |= RCC_AHBENR_DMA1EN;

    GPIOA->CRH &= ~(uint32_t)(GPIO_CRH_MODE9 | GPIO_CRH_MODE10);
    GPIOA->CRH &= ~(uint32_t)(GPIO_CRH_CNF9 | GPIO_CRH_CNF10);

    GPIOA->CRH |= (uint32_t)(GPIO_CRH_MODE9_0 | GPIO_CRH_MODE9_1);
    GPIOA->CRH |= (uint32_t)(GPIO_CRH_CNF9_1);

    GPIOA->CRH |= (uint32_t)(GPIO_CRH_CNF10_0);

    USART1->BRR = (8000000U + 9600U / 2U) / 9600U;

    USART1->CR1 |= USART_CR1_UE | USART_CR1_TE | USART_CR1_RE;
    USART1->CR3 |= USART_CR3_DMAR;

    USART1->CR1 |= USART_CR1_IDLEIE;

    NVIC_EnableIRQ(USART1_IRQn);

    DMA1_Channel5->CPAR  = (uint32_t)&USART1->DR;
    DMA1_Channel5->CMAR  = (uint32_t)rx_buf;
    DMA1_Channel5->CNDTR = RX_BUF_SIZE;

    DMA1_Channel5->CCR |= DMA_CCR_MINC | DMA_CCR_CIRC | DMA_CCR_PL_0;
    DMA1_Channel5->CCR |= DMA_CCR_EN;
}

void uart_send_str(const char* str)
{
    while (*str)
    {
        while (!(USART1->SR & USART_SR_TXE))
        {
            __NOP();
        }

        USART1->DR = *str++;
    }
}

void USART1_IRQHandler(void)
{
    if (USART1->SR & USART_SR_IDLE)
    {
        volatile uint32_t dummy = USART1->SR;
        dummy                   = USART1->DR;
        (void)dummy;

        uint16_t bytes_received = RX_BUF_SIZE - DMA1_Channel5->CNDTR;
        if (bytes_received > 0)
        {
            char last_char = (char)rx_buf[bytes_received - 1];

            if (last_char == '\r' || last_char == '\n')
            {
                if (cmd_idx > 0)
                {
                    cmd_buf[cmd_idx] = '\0';
                    command_ready    = 1;
                }
            }
            else
            {
                if (cmd_idx < CMD_BUF_SIZE - 1)
                {
                    cmd_buf[cmd_idx++] = last_char;
                }
            }
        }
    }
}

uint8_t get_is_ready(void)
{  //
    return command_ready;
}

void clear_cmd_ready(void)
{
    cmd_idx       = 0;
    command_ready = 0;
}

uint8_t* get_cmd_buf(void)
{  //
    return cmd_buf;
}