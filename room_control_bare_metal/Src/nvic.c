#include "nvic.h"
#include "rcc.h" // Para rcc_syscfg_clock_enable
#include "uart.h"
static void nvic_enable_irq(uint32_t IRQn)
{
    NVIC->ISER[IRQn / 32U] |= (1UL << (IRQn % 32U));
}
static void nvic_set_priority(uint32_t IRQn, uint8_t priority)
{
    NVIC->IP[IRQn] = priority << 4;
}

void nvic_exti_pc13_button_enable(void)
{
    rcc_syscfg_clock_enable();

    SYSCFG->EXTICR[3] &= ~(0x000FU << 4);
    SYSCFG->EXTICR[3] |= (0x0002U << 4);

    EXTI->IMR1 |= (1U << 13);

    EXTI->FTSR1 |= (1U << 13);
    EXTI->RTSR1 &= ~(1U << 13);

    nvic_enable_irq(EXTI15_10_IRQn);

    nvic_set_priority(EXTI15_10_IRQn, 1);
}
void nvic_usart2_irq_enable(void)
{
    // Habilitar interrupción de recepción en USART2
    USART2->CR1 |= (1U << 5); // RXNEIE
    nvic_enable_irq(USART2_IRQn);
    nvic_set_priority(USART2_IRQn, 2);
}