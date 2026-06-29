#include "parser.h"

#include <stdint.h>
#include <string.h>

#include "drivers/uart.h"
#include "middle/obc_core.h"

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

void cli_execute(uint8_t cmd, char* argv[])
{
    if (cmd == 0)
    {
        return;
    }

    if (strcmp(argv[0], "eps") == 0)
    {
        if (strcmp(argv[1], "on") == 0)
        {
            obc_set_sub_system_state(OBC_SYS_EPS, SYS_STATE_ON);
            uart_send_str("OK: EPS Subsystem Powered ON\r\n");
        }
        else if (strcmp(argv[1], "off") == 0)
        {
            obc_set_sub_system_state(OBC_SYS_EPS, SYS_STATE_OFF);
            uart_send_str("OK: EPS Subsystem Powered OFF\r\n");
        }
        else if (strcmp(argv[1], "toggle") == 0)
        {
            obc_set_sub_system_state(OBC_SYS_EPS, SYS_STATE_TOGGLE);
            uart_send_str("OK: EPS Subsystem Toggled\r\n");
        }
        else
        {
            uart_send_str("ERROR: Invalid argument for EPS\r\n");
        }
    }
    else if (strcmp(argv[0], "adcs") == 0)
    {
        if (strcmp(argv[1], "on") == 0)
        {
            obc_set_sub_system_state(OBC_SYS_ADCS, SYS_STATE_ON);
            uart_send_str("OK: ADCS Actuators Enabled\r\n");
        }
        else if (strcmp(argv[1], "off") == 0)
        {
            obc_set_sub_system_state(OBC_SYS_ADCS, SYS_STATE_OFF);
            uart_send_str("OK: ADCS Actuators Disabled\r\n");
        }
        else if (strcmp(argv[1], "toggle") == 0)
        {
            obc_set_sub_system_state(OBC_SYS_ADCS, SYS_STATE_TOGGLE);
            uart_send_str("OK: ADCS Toggled\r\n");
        }
        else
        {
            uart_send_str("ERROR: Invalid argument for ADCS\r\n");
        }
    }
    else if (strcmp(argv[0], "payload") == 0)
    {
        if (strcmp(argv[1], "on") == 0)
        {
            obc_set_sub_system_state(OBC_SYS_PAYLOAD, SYS_STATE_ON);
            uart_send_str("OK: Payload Subsystem Active\r\n");
        }
        else if (strcmp(argv[1], "off") == 0)
        {
            obc_set_sub_system_state(OBC_SYS_PAYLOAD, SYS_STATE_OFF);
            uart_send_str("OK: Payload Subsystem Suspended\r\n");
        }
        else if (strcmp(argv[1], "toggle") == 0)
        {
            obc_set_sub_system_state(OBC_SYS_PAYLOAD, SYS_STATE_TOGGLE);
            uart_send_str("OK: Payload Toggled\r\n");
        }
        else
        {
            uart_send_str("ERROR: Invalid argument for Payload\r\n");
        }
    }
    else
    {
        uart_send_str("ERROR: Unknown OBC command\r\n");
    }
}