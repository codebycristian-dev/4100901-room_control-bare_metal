#include "room_control.h"
#include "gpio.h"
#include "systick.h"
#include "uart.h"
#include "tim.h"
#define HEARTBEAT_PIN 5U
#define HEARTBEAT_INTERVAL_MS 500U

// ---------------------------------------------------------------------------
// Definiciones internas
// ---------------------------------------------------------------------------
typedef enum
{
    ROOM_IDLE,
    ROOM_OCCUPIED
} room_state_t;

// Variables estáticas
static room_state_t current_state = ROOM_IDLE;
static uint32_t last_event_time = 0;  // Marca de tiempo del último evento
static uint32_t last_button_time = 0; // Para debounce (ms)
#define HEARTBEAT_PIN 5U              // PA5
#define HEARTBEAT_INTERVAL_MS 500U
// ---------------------------------------------------------------------------
// Funciones internas auxiliares
// ---------------------------------------------------------------------------
static void set_state(room_state_t new_state)
{
    current_state = new_state;

    if (new_state == ROOM_OCCUPIED)
    {
        set_gpio(GPIOA, LD2_PIN);
        tim3_ch1_pwm_set_duty_cycle(100); // 100% brillo
        uart_send_string("Estado: OCCUPIED\r\n");
    }
    else
    {
        clear_gpio(GPIOA, LD2_PIN);
        tim3_ch1_pwm_set_duty_cycle(0); // LED apagado
        uart_send_string("Estado: IDLE\r\n");
    }

    last_event_time = systick_get_tick();
}

// ---------------------------------------------------------------------------
// Funciones públicas
// ---------------------------------------------------------------------------

void room_control_app_init(void)
{
    // Estado inicial: IDLE, LED apagado, PWM al valor inicial
    tim3_ch1_pwm_set_duty_cycle(PWM_INITIAL_DUTY);
    clear_gpio(GPIOA, LD2_PIN);
    current_state = ROOM_IDLE;
    uart_send_string("[room_control] Inicializado.\r\n");
}

void room_control_on_button_press(void)
{
    uint32_t now = systick_get_tick();

    // Debounce: ignora pulsos dentro de 200 ms
    if ((now - last_button_time) < 200)
        return;

    last_button_time = now;

    if (current_state == ROOM_IDLE)
    {
        set_state(ROOM_OCCUPIED);
    }
    else
    {
        set_state(ROOM_IDLE);
    }
}

void room_control_on_uart_receive(char received_char)
{
    switch (received_char)
    {
    case 'h':
    case 'H':
        tim3_ch1_pwm_set_duty_cycle(100);
        uart_send_string("PWM al 100%\r\n");
        break;

    case 'l':
    case 'L':
        tim3_ch1_pwm_set_duty_cycle(0);
        uart_send_string("PWM al 0%\r\n");
        break;

    case 'o':
    case 'O':
        set_state(ROOM_OCCUPIED);
        break;

    case 'i':
    case 'I':
        set_state(ROOM_IDLE);
        break;

    default:
        // Echo del carácter recibido
        uart_send(received_char);
        break;
    }
}
void room_control_update(void)
{
    static uint32_t last_update_tick = 0;
    uint32_t now = systick_get_tick();

    // --- Actualización principal cada 100 ms ---
    if (now - last_update_tick >= 100U)
    {
        last_update_tick = now;

        // Aquí iría la lógica principal del control del cuarto
        // (por ejemplo, lectura de sensores, control de luces, etc.)
    }

    // --- Heartbeat LED (PA5) ---
    static uint32_t last_heartbeat_tick = 0;

    if (now - last_heartbeat_tick >= HEARTBEAT_INTERVAL_MS)
    {
        last_heartbeat_tick = now;
        gpio_toggle_pin(GPIOA, HEARTBEAT_PIN);
    }
}