#include "obc_core.h"

#include "drivers/gpio.h"

/**
 * @brief Initialize OBC core logic and set default safe states for all subsystems
 */
void obc_core_init(void)
{
    gpio_init();

    /* Ensure all spacecraft subsystems start in safe powered-off state */
    c13_off();
    c14_off();
    c15_off();
}

/**
 * @brief Route abstract subsystem commands to physical hardware drivers
 * @param sys Target CubeSat subsystem (EPS, ADCS, or Payload)
 * @param state Requested operational state (ON, OFF, or TOGGLE)
 */
void obc_set_sub_system_state(obc_sub_system_t sys, obc_state_system_t state)
{
    switch (sys)
    {
        case OBC_SYS_EPS:
            if (state == SYS_STATE_ON)
            {
                c13_on();
            }
            else if (state == SYS_STATE_OFF)
            {
                c13_off();
            }
            else if (state == SYS_STATE_TOGGLE)
            {
                c13_toggle();
            }
            break;

        case OBC_SYS_ADCS:
            if (state == SYS_STATE_ON)
            {
                c14_on();
            }
            else if (state == SYS_STATE_OFF)
            {
                c14_off();
            }
            else if (state == SYS_STATE_TOGGLE)
            {
                c14_toggle();
            }
            break;

        case OBC_SYS_PAYLOAD:
            if (state == SYS_STATE_ON)
            {
                c15_on();
            }
            else if (state == SYS_STATE_OFF)
            {
                c15_off();
            }
            else if (state == SYS_STATE_TOGGLE)
            {
                c15_toggle();
            }
            break;

        default:
            break;
    }
}