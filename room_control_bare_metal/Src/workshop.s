// --- Ejercicio: Encender LED por 3 segundos con un botón ---
    .section .text
    .syntax unified
    .thumb

    .global main
    .global init_led
    .global init_button
    .global init_systick
    .global SysTick_Handler

// --- Sección de datos para el contador de tiempo ---------------------------
    .section .data
    .align 4
led_timer_countdown: .word 0   @Variable global de laRam Sirve como contador regresivo 

// --- Sección de código -----------------------------------------------------
    .section .text

// --- Definiciones de registros para LD2 (PA5) -------------------------------
    .equ RCC_BASE,       0x40021000         @ Base de RCC
    .equ RCC_AHB2ENR,    RCC_BASE + 0x4C    @ Enable GPIOx clock (AHB2ENR)
    .equ GPIOA_BASE,     0x48000000         @ Base de GPIOA
    .equ GPIOA_MODER,    GPIOA_BASE + 0x00  @ Mode register
    .equ GPIOA_ODR,      GPIOA_BASE + 0x14  @ Output data register
    .equ LD2_PIN,        5                  @ Pin del LED LD2

// --- Definiciones de registros para el Botón B1 (PC13) ----------------------
    .equ GPIOC_BASE,     0x48000800         @ Base de GPIOC
    .equ GPIOC_MODER,    GPIOC_BASE + 0x00  @ Mode register
    .equ GPIOC_IDR,      GPIOC_BASE + 0x10  @ Input data register
    .equ B1_PIN,         13                 @ Pin del Botón B1

// --- Definiciones de registros para SysTick ---------------------------------
    .equ SYST_CSR,       0xE000E010         @ Control interrupció 
    .equ SYST_RVR,       0xE000E014         @ Reload value register
    .equ SYST_CVR,       0xE000E018         @ registro valor cada que llega a cero interrupción
    .equ HSI_FREQ,       4000000            @ Reloj interno por defecto (4 MHz)

// --- Programa principal -----------------------------------------------------
main:
    bl init_led
    bl init_button      // Llama a la nueva función para configurar el botón
    bl init_systick
    
    // Apagar el LED al inicio para asegurar un estado conocido
    movw r0, #:lower16:GPIOA_ODR
    movt r0, #:upper16:GPIOA_ODR
    ldr  r1, [r0]
    bic  r1, r1, #(1 << LD2_PIN) // Pone a 0 el bit del LED
    str  r1, [r0]

    b wait_for_press    // Inicia el bucle principal esperando una pulsación

// --- Bucle 1: Esperar a que se presione el botón -----------------------------
wait_for_press:
    // Lee el estado del botón. Sigue en el bucle mientras no esté presionado.
    movw r0, #:lower16:GPIOC_IDR
    movt r0, #:upper16:GPIOC_IDR
    ldr  r1, [r0]
    ubfx r6, r1, #B1_PIN, #1  // r6 = estado del botón (0=presionado)

    cmp r6, #0           // ¿Está presionado?
    bne wait_for_press   // Si no, sigue esperando

    // --- El botón ha sido presionado ---
    
    // 1. Encender el LED
    movw r0, #:lower16:GPIOA_ODR
    movt r0, #:upper16:GPIOA_ODR
    ldr  r1, [r0]
    orr  r1, r1, #(1 << LD2_PIN) // Pone a 1 el bit del LED para encenderlo
    str  r1, [r0]

    // 2. Iniciar el contador de 3 segundos
    movw r2, #:lower16:led_timer_countdown
    movt r2, #:upper16:led_timer_countdown
    movs r3, #3
    str  r3, [r2]      // Pone el valor 3 en nuestra variable contador

    // 3. Pausa para debounce y saltar a esperar que se suelte el botón
    ldr r4, =40000
delay_loop:
    subs r4, r4, #1
    bne delay_loop
    
    b wait_for_release

// --- Bucle 2: Esperar a que se suelte el botón ------------------------------
wait_for_release: @estado del boton 
    // Se queda aquí mientras el botón siga presionado
    movw r0, #:lower16:GPIOC_IDR
    movt r0, #:upper16:GPIOC_IDR
    ldr  r1, [r0]
    ubfx r6, r1, #B1_PIN, #1

    cmp r6, #1           // ¿Ha sido liberado?
    bne wait_for_release // Si no, sigue esperando
    
    b wait_for_press     // Una vez liberado, vuelve al estado inicial

// --- Inicialización de GPIOA PA5 para el LED LD2 -----------------------------
init_led:
    movw  r0, #:lower16:RCC_AHB2ENR; movt r0, #:upper16:RCC_AHB2ENR
    ldr   r1, [r0]; orr r1, r1, #(1 << 0); str r1, [r0]
    movw  r0, #:lower16:GPIOA_MODER; movt r0, #:upper16:GPIOA_MODER
    ldr   r1, [r0]; bic r1, r1, #(0b11 << (LD2_PIN * 2)); orr r1, r1, #(0b01 << (LD2_PIN * 2)); str r1, [r0]
    bx    lr

// --- NUEVO: Inicialización de GPIOC PC13 para el Botón B1 -------------------
init_button:
    // 1. Habilitar el reloj para el Puerto C (bit 2)
    movw  r0, #:lower16:RCC_AHB2ENR; movt r0, #:upper16:RCC_AHB2ENR
    ldr   r1, [r0]; orr r1, r1, #(1 << 2); str r1, [r0]
    
    // 2. Configurar el pin 13 como entrada (valor 00 en el registro MODER)
    movw  r0, #:lower16:GPIOC_MODER; movt r0, #:upper16:GPIOC_MODER
    ldr   r1, [r0]; bic r1, r1, #(0b11 << (B1_PIN * 2)); str r1, [r0]
    bx    lr

// --- Inicialización de Systick para generar un "tick" de 1s -----------------
init_systick:
    movw  r0, #:lower16:SYST_RVR; movt r0, #:upper16:SYST_RVR
    movw  r1, #:lower16:HSI_FREQ; movt r1, #:upper16:HSI_FREQ
    subs  r1, r1, #1; str r1, [r0]
    movw  r0, #:lower16:SYST_CSR; movt r0, #:upper16:SYST_CSR
    movs  r1, #(1 << 0)|(1 << 1)|(1 << 2); str r1, [r0]
    bx    lr

// --- Manejador de la interrupción SysTick (Lógica del contador) -------------
    .thumb_func
SysTick_Handler:
    // Carga la dirección y el valor de nuestro contador
    movw  r0, #:lower16:led_timer_countdown
    movt  r0, #:upper16:led_timer_countdown
    ldr   r1, [r0]

    // Comprueba si el contador está activo (es mayor que 0)
    cmp   r1, #0
    beq   exit_handler    // Si es 0, salir.

    // Si está activo, lo decrementa en 1
    subs  r1, r1, #1
    str   r1, [r0]        // Guarda el nuevo valor

    // Comprueba si el contador AHORA es 0 (significa que los 3s acaban de terminar)
    cmp   r1, #0
    bne   exit_handler    // Si no es 0, todavía no es tiempo, salir.

    // --- Si llegamos aquí, el tiempo ha expirado ---
    // Apagar el LED
    movw  r2, #:lower16:GPIOA_ODR
    movt  r2, #:upper16:GPIOA_ODR
    ldr   r3, [r2]
    bic   r3, r3, #(1 << LD2_PIN) // Pone a 0 el bit del LED
    str   r3, [r2]

exit_handler:
    bx    lr