#ifndef OBC_CORE_H
#define OBC_CORE_H

#include <stdint.h>

typedef enum
{
    OBC_SYS_EPS     = 0U,
    OBC_SYS_ADCS    = 1U,
    OBC_SYS_PAYLOAD = 2U
} obc_sub_system_t;

typedef enum
{
    SYS_STATE_ON     = 0U,
    SYS_STATE_OFF    = 1U,
    SYS_STATE_TOGGLE = 2U
} obc_state_system_t;

void obc_core_init(void);
void obc_set_sub_system_state(obc_sub_system_t sys, obc_state_system_t state);

#endif