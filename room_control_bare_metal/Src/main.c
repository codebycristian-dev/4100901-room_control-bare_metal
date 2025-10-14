#include "gpio.h"
#include "systick.h"
#include "rcc.h"
#include "uart.h"
#include "nvic.h"
#include "tim.h"
#include "room_control.h"

volatile uint32_t ms_counter = 0;

int main(void)
{
    rcc_init();
    init_gpio(GPIOA, 5, 0x01, 0x00, 0x01, 0x00, 0x00);  // LED
    init_gpio(GPIOC, 13, 0x00, 0x00, 0x01, 0x01, 0x00); // Botón
    init_systick();
    init_gpio_uart();
    init_uart();
    tim3_ch1_pwm_init(1000U);

    nvic_exti_pc13_button_enable();
    nvic_usart2_irq_enable();

    uart_send_string("Se inició el programa\r\n");

    room_control_app_init();

    while (1)
    {
        room_control_update();
    }
}

// --- SysTick Handler ---
void SysTick_Handler(void)
{
    ms_counter++;
}

uint32_t systick_get_tick(void)
{
    return ms_counter;
}

// --- ISR Botón ---
void EXTI15_10_IRQHandler(void)
{
    if (EXTI->PR1 & (1U << 13))
    {
        EXTI->PR1 |= (1U << 13);
        room_control_on_button_press();
    }
}

// --- ISR UART2 ---
void USART2_IRQHandler(void)
{
    if (USART2->ISR & (1U << 5))
    {
        char c = USART2->RDR;
        room_control_on_uart_receive(c);
    }
}