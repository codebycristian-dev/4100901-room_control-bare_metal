#include "gpio.h"
#include "systick.h"
#include "rcc.h"
#include "uart.h"
#include "nvic.h"
#include "tim.h" // Agregado: para habilitar IRQs

static volatile uint32_t ms_counter = 0;
char rx_buffer[256];
uint8_t rx_index = 0;

// --- Programa principal ------------------------------------------------------
int main(void)
{
    // Inicialización de periféricos
    rcc_init();
    init_gpio(GPIOA, 5, 0x01, 0x00, 0x01, 0x00, 0x00);  // LED
    init_gpio(GPIOC, 13, 0x00, 0x00, 0x01, 0x01, 0x00); // Botón
    init_systick();
    init_gpio_uart();
    init_uart();

    // >>> NUEVO: PWM en PA6 (TIM3_CH1, AF2), 1 kHz y duty al 50 %
    tim3_ch1_pwm_init(1000U);
    tim3_ch1_pwm_set_duty_cycle(100U);
    // <<<

    // Habilitar interrupciones externas
    nvic_exti_pc13_button_enable();
    nvic_usart2_irq_enable();

    // Mensaje de inicio
    uart_send_string("Se inició el programa\r\n");
    // --- Fade PWM PA6 (TIM3_CH1) no bloqueante ---

    while (1)
    {
        // --- Fade PWM PA6 (TIM3_CH1) ---
        static uint8_t fade_duty = 0;
        static int8_t fade_dir = +1;
        static uint32_t last_ms = 0;

        if ((ms_counter - last_ms) >= 10U)
        {
            last_ms = ms_counter;

            fade_duty = (uint8_t)((int)fade_duty + fade_dir);

            if (fade_duty >= 100U)
            {
                fade_duty = 100U;
                fade_dir = -1;
            }
            else if (fade_duty == 0U)
            {
                fade_duty = 0U;
                fade_dir = +1;
            }

            tim3_ch1_pwm_set_duty_cycle(fade_duty);
        }
        // Botón presionado (sin interrupción, como respaldo)
        if (read_gpio(GPIOC, 13) != 0)
        {
            ms_counter = 0;
            set_gpio(GPIOA, 5);
        }

        // Temporizador para apagar el LED luego de 3s
        if (ms_counter >= 3000)
        {
            clear_gpio(GPIOA, 5);
        }
    }
}

// --- SysTick Handler ---------------------------------------------------------
void SysTick_Handler(void)
{
    ms_counter++;
}

// --- ISR para botón PC13 (EXTI13) --------------------------------------------
void EXTI15_10_IRQHandler(void)
{
    if (EXTI->PR1 & (1U << 13)) // Verifica si fue EXTI13
    {
        EXTI->PR1 |= (1U << 13); // Limpia el flag
        uart_send_string(">> Botón presionado (prioridad alta)\r\n");
    }
}

// --- ISR para UART2 ----------------------------------------------------------
void USART2_IRQHandler(void)
{
    if (USART2->ISR & (1U << 5)) // RXNE
    {
        char c = USART2->RDR;

        if (rx_index < sizeof(rx_buffer) - 1)
        {
            rx_buffer[rx_index++] = c;

            if (c == '\r' || c == '\n')
            {
                rx_buffer[rx_index] = '\0'; // Null terminate
                uart_send_string("Recibido: ");
                uart_send_string(rx_buffer);
                uart_send_string("\r\n");
                rx_index = 0;
            }
        }
    }
}