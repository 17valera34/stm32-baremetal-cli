#include "uart.h"

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "stm32f1xx.h"

#define RX_BUF_SIZE 64
#define CMD_BUF_SIZE 64

/* Telemetry and command communication buffers */
uint8_t rx_buf[RX_BUF_SIZE];
uint8_t cmd_buf[CMD_BUF_SIZE];
uint32_t cmd_idx                      = 0;
static volatile uint8_t command_ready = 0;

/**
 * @brief Initialize USART1 with DMA RX/TX and Idle Line Interrupts
 */
void uart1_init(void)
{
    /* Enable peripheral clocking (GPIOA, USART1, DMA1) */
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_USART1EN;
    RCC->AHBENR |= RCC_AHBENR_DMA1EN;

    GPIOA->CRH &= ~(uint32_t)(GPIO_CRH_MODE9 | GPIO_CRH_MODE10);
    GPIOA->CRH &= ~(uint32_t)(GPIO_CRH_CNF9 | GPIO_CRH_CNF10);

    GPIOA->CRH |= (uint32_t)(GPIO_CRH_MODE9_0 | GPIO_CRH_MODE9_1);
    GPIOA->CRH |= (uint32_t)(GPIO_CRH_CNF9_1);

    GPIOA->CRH |= (uint32_t)(GPIO_CRH_CNF10_0);

    /* Set Baud rate to 9600 bps @ 8MHz SYSCLK */
    USART1->BRR = (8000000U + 9600U / 2U) / 9600U;

    USART1->CR1 |= USART_CR1_UE | USART_CR1_TE | USART_CR1_RE;

    DMA1_Channel4->CCR &= ~(uint32_t)DMA_CCR_EN;
    while ((DMA1_Channel4->CCR & DMA_CCR_EN) != 0U)
    {
        __NOP();
    }

    /* Configure DMA1 Channel 4: Memory-to-Peripheral, Memory Increment, High Priority */
    DMA1_Channel4->CCR  = (uint32_t)(DMA_CCR_DIR | DMA_CCR_MINC | DMA_CCR_PL_1);
    DMA1_Channel4->CPAR = (uint32_t)&(USART1->DR);
    DMA1_Channel4->CMAR = 0U;
    USART1->CR3 |= (uint32_t)USART_CR3_DMAT;

    USART1->CR3 |= USART_CR3_DMAR;
    USART1->CR1 |= USART_CR1_IDLEIE;

    /* Configure DMA1 Channel 5 (RX) base addresses and data counter */
    DMA1_Channel5->CPAR  = (uint32_t)&USART1->DR;
    DMA1_Channel5->CMAR  = (uint32_t)rx_buf;
    DMA1_Channel5->CNDTR = RX_BUF_SIZE;

    DMA1_Channel5->CCR |= DMA_CCR_MINC | DMA_CCR_CIRC | DMA_CCR_PL_0;

    NVIC_EnableIRQ(USART1_IRQn);

    DMA1_Channel5->CCR |= DMA_CCR_EN;
}

uint8_t uart_is_busy(void)
{
    /* Check if DMA Channel 4 is enabled and still has bytes to transfer */
    if (((DMA1_Channel4->CCR & DMA_CCR_EN) != 0U) && (DMA1_Channel4->CNDTR > 0U))
    {
        return 1U;
    }

    /* Check if UART Transmission Complete flag is cleared */
    if ((USART1->SR & USART_SR_TC) == 0U)
    {
        return 1U;
    }

    return 0U; /* UART is free */
}

uint8_t uart_send_dma(const char* buf, uint16_t len)
{
    if ((buf == NULL) || (len == 0))
    {
        return 0U;
    }

    /* Wait until previous transmission finishes safely */
    while (uart_is_busy() != 0U)
    {
        __NOP();
    }

    /* Disable DMA channel to reconfigure memory pointer and counter */
    DMA1_Channel4->CCR &= ~(uint32_t)DMA_CCR_EN;

    /* Clear all DMA Channel 4 global and transfer flags */
    DMA1->IFCR = (uint32_t)(DMA_IFCR_CGIF4 | DMA_IFCR_CTCIF4 | DMA_IFCR_CHTIF4 | DMA_IFCR_CTEIF4);

    /* Clear UART Transmission Complete flag to prevent premature busy triggers */
    volatile uint32_t temp = USART1->SR;
    (void)temp;
    USART1->SR &= ~(uint32_t)USART_SR_TC;

    /* Set buffer address, transfer length and kick off the hardware */
    DMA1_Channel4->CMAR  = (uint32_t)buf;
    DMA1_Channel4->CNDTR = len;
    DMA1_Channel4->CCR |= (uint32_t)DMA_CCR_EN;

    return 1U;
}

void uart_send_str(const char* str)
{
    if (str != NULL)
    {
        /* Dynamically calculate length and invoke DMA TX */
        uart_send_dma(str, (uint16_t)strlen(str));
    }
}

void USART1_IRQHandler(void)
{
    /* Handle Idle Line Detection Interrupt */
    if (USART1->SR & USART_SR_IDLE)
    {
        /* Clear IDLE flag by reading SR followed by DR */
        volatile uint32_t dummy = USART1->SR;
        dummy                   = USART1->DR;
        (void)dummy;

        /* Calculate numbers of received bytes from DMA counter */
        uint16_t bytes_received = RX_BUF_SIZE - DMA1_Channel5->CNDTR;
        if (bytes_received > 0)
        {
            /* Extract the last character in the buffer */
            char last_char = (char)rx_buf[bytes_received - 1];

            /* Check for command termination sequences (\r or \n) */
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
                /* Append character to internal command buffer if space permits */
                if (cmd_idx < CMD_BUF_SIZE - 1)
                {
                    cmd_buf[cmd_idx++] = last_char;
                }
            }
        }
    }
}

/**
 * @brief Check if a valid command has been received
 * @return 1 if ready, 0 otherwise
 */
uint8_t get_is_ready(void)
{  //
    return command_ready;
}

/**
 * @brief Reset command buffer index and ready flag for the next transaction
 */
void clear_cmd_ready(void)
{
    cmd_idx       = 0;
    command_ready = 0;
}

uint8_t* get_cmd_buf(void)
{  //
    return cmd_buf;
}