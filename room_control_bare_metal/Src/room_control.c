#include "room_control.h"

#include "gpio.h"    // Para controlar LEDs
#include "systick.h" // Para obtener ticks y manejar tiempos
#include "uart.h"    // Para enviar mensajes
#include "tim.h"     // Para controlar el PWM

// Estados de la sala
typedef enum
{
    ROOM_IDLE,
    ROOM_OCCUPIED
} room_state_t;

// Variable de estado global
room_state_t current_state = ROOM_IDLE;

void room_control_app_init(void)
{
    // TODO: Implementar inicializaciones específicas de la aplicación
}

void room_control_on_button_press(void)
{
    // TODO: Implementar la lógica para manejar la pulsación del botón usando estados
    // Ejemplo: Si idle, cambiar a occupied; si occupied, cambiar a idle
}

void room_control_on_uart_receive(char received_char)
{
    switch (received_char)
    {
    case 'h':
    case 'H':
        // TODO: Set PWM to 100%
        break;
    case 'l':
    case 'L':
        // TODO: Set PWM to 0%
        break;
    case 'O':
    case 'o':
        // TODO: Cambiar estado a occupied
        break;
    case 'I':
    case 'i':
        // TODO: Cambiar estado a idle
        break;
    default:
        // TODO: Echo the character
        break;
    }
}

void room_control_update(void)
{
    // TODO: Implementar lógica periódica, como timeouts para apagar LED en estado occupied
    // Ejemplo: Si estado occupied y han pasado 3s desde button press, cambiar a idle y apagar LED
}