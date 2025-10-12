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
void rcc_tim3_clock_enable(void)
{
    // Habilita el reloj para TIM3 (bit 1 del registro APB1ENR1 en muchos STM32L4/F3)
    RCC->APB1ENR1 |= (1U << 1);
}