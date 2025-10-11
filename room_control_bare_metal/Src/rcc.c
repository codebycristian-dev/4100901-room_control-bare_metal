#include "rcc.h"

void rcc_init(void)
{
    RCC->AHB2ENR |= (1 << 0); // Habilita reloj GPIOA
    RCC->AHB2ENR |= (1 << 2); // Habilita reloj GPIOC
}

void rcc_syscfg_clock_enable(void)
{
    RCC->APB2ENR |= (1 << 0);
}