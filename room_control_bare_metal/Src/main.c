#include "gpio.h"
#include "systick.h"
#include "rcc.h"
#include "uart.h"
#include "tim.h"
#include "nvic.h"
#include "room_control.h"

// NO declares ms_counter aquí. Lo define systick.c y se usa con 'extern' en otros módulos.

int main(void)
{
    // --- Relojes base ---
    rcc_init();

    // --- GPIO: LD2 (PA5) y Botón B1 (PC13) ---
    init_gpio(GPIOA, 5, 0x01, 0x00, 0x01, 0x00, 0x00);  // salida push-pull
    init_gpio(GPIOC, 13, 0x00, 0x00, 0x01, 0x01, 0x00); // entrada con pull-up

    // --- Ticks y UART ---
    init_systick();   // SysTick 1 ms (ms_counter se incrementa en SysTick_Handler)
    init_gpio_uart(); // PA2/PA3 AF7
    init_uart();      // 115200 8N1

    // --- App: PWM en PA6 + estados (IDLE/OCCUPIED) ---
    room_control_app_init();

    // --- Interrupciones: botón y UART RX ---
    nvic_exti_pc13_button_enable();
    nvic_usart2_irq_enable();

    uart_send_string("Sistema Inicializado!\r\n");

    // --- Bucle principal: solo tareas periódicas de la app ---
    while (1)
    {
        room_control_update(); // timeout 3 s y tareas de sala
    }
}