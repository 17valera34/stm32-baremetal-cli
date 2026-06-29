#include "gpio.h"

#include "stm32f1xx.h"

void gpio_init(void)
{
    /* Enable GPIOC clocking */
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;

    /* Clear configuration for pins 13, 14, 15 */
    GPIOC->CRH &= ~(GPIO_CRH_MODE13 | GPIO_CRH_MODE14 | GPIO_CRH_MODE15);
    GPIOC->CRH &= ~(GPIO_CRH_CNF13 | GPIO_CRH_CNF14 | GPIO_CRH_CNF15);

    /* Configure PC13, PC14, PC15 as Push-Pull Output, max speed 2MHz */
    GPIOC->CRH |= GPIO_CRH_MODE13_1 | GPIO_CRH_MODE14_1 | GPIO_CRH_MODE15_1;
}

void c13_on(void)
{  //
    GPIOC->ODR &= ~GPIO_ODR_ODR13;
}

void c13_off(void)
{  //
    GPIOC->ODR |= GPIO_ODR_ODR13;
}

void c13_toggle(void)
{  //
    GPIOC->ODR ^= GPIO_ODR_ODR13;
}

void c14_on(void)
{  //
    GPIOC->ODR &= ~GPIO_ODR_ODR14;
}

void c14_off(void)
{  //
    GPIOC->ODR |= GPIO_ODR_ODR14;
}

void c14_toggle(void)
{  //
    GPIOC->ODR ^= GPIO_ODR_ODR14;
}

void c15_on(void)
{  //
    GPIOC->ODR &= ~GPIO_ODR_ODR15;
}

void c15_off(void)
{  //
    GPIOC->ODR |= GPIO_ODR_ODR15;
}

void c15_toggle(void)
{  //
    GPIOC->ODR ^= GPIO_ODR_ODR15;
}