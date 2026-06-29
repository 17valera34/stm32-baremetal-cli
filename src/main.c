#include <stdbool.h>

#include "drivers/uart.h"
#include "stm32f1xx.h"

int main(void)
{
    uart1_init();
    uart_send_str("CLI Base Initialized. Waiting for commands...\r\n");

    while (true)
    {
        if (get_is_ready())
        {
            uart_send_str("receive command: ");
            uart_send_str((char*)get_cmd_buf());
            uart_send_str("\r\n");

            clear_cmd_ready();
        }
    }
}