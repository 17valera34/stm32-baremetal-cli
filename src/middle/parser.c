#include "parser.h"

#include <stdint.h>

#include "drivers/uart.h"

uint8_t cli_parser(char* line, char* argv[], uint8_t max_args)
{
    uint8_t argc = 0;

    while (*line && argc < max_args)
    {
        while (*line == ' ')
        {
            line++;
        }

        if (*line == '\0')
        {
            break;
        }

        argv[argc++] = line;

        while (*line && *line != ' ')
        {
            line++;
        }

        if (*line == ' ')
        {
            *line++ = '\0';
        }
    }

    return argc;
}

void cli_execute(uint8_t argc, char* argv[])
{
    if (argc == 0)
    {
        return;
    }

    if (strcmp(argv[0], "led0") == 0)
    {
        if (strcmp(argv[1], "on") == 0)
        {
            uart_send_str("OK: LED1 turned ON\r\n");
        }
        else
        {
            if (strcmp(argv[1], "off") == 0)
            {
                uart_send_str("OK: LED1 turned OFF\r\n");
            }
        }
    }
}