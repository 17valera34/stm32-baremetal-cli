#include <stdbool.h>
#include <string.h>

#include "drivers/gpio.h"
#include "drivers/uart.h"
#include "middle/parser.h"
#include "middle/obc_core.h"
#include "stm32f1xx.h"

#define MAX_ARGC 4


/**
 * @brief Main execution entry point. Implements the flight software superloop.
 */
int main(void)
{
    /* Initialize OBC core subsystems and communication interfaces */
    obc_core_init();

    uart1_init();
    uart_send_str("CLI Base Initialized. Waiting for commands...\r\n");

    char local_line[64U];
    char* argv[MAX_ARGC];

    /* Spacecraft core runtime loop */
    while (true)
    {
        /* Process commands instantly once flagged by the UART DMA IDLE routine */
        if (get_is_ready())
        {
            strcpy(local_line, (char*)get_cmd_buf());
            clear_cmd_ready();
            uint8_t argc = cli_parser(local_line, argv, MAX_ARGC);
            cli_execute(argc, argv);
        }
    }
}