#ifndef UART_H
#define UART_H

#include <stdint.h>

void uart1_init(void);
void uart_send_str(const char* str);
uint8_t get_is_ready(void);
uint8_t* get_cmd_buf(void);
void clear_cmd_ready(void);

uint8_t uart_is_busy(void);
uint8_t uart_send_dma(const char *buf, uint16_t len);

#endif