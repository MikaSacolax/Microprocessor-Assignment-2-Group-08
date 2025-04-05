#include "hardware/regs/addressmap.h"
#include "hardware/regs/io_bank0.h"
#include "hardware/regs/timer.h"
#include "hardware/regs/m0plus.h"

.syntax unified                                                @ Specify unified assembly syntax
.cpu    cortex-m0plus                                          @ Specify CPU type is Cortex M0+
.thumb                                                         @ Specify thumb assembly for RP2040
.global main_asm                                               @ Provide program starting address to the linker
.align 4                                                       @ Specify code alignment

.equ GPIO_BTN, 21                                              @ Specify pin for the "input" button
.equ GPIO_DIR_IN, 0                                            @ Specify input direction for a GPIO pin
.equ GPIO_DIR_OUT, 1                                           @ Specify output direction for a GPIO pin

.equ GPIO_ISR_OFFSET, 0x74                                     @ GPIO is int #13 (vector table entry 29)
.equ ALRM_ISR_OFFSET, 0x40                                     @ Alarm0 is #13 in interrupt vector table

.equ GPIO_BTN_FALL_MSK, 0x00400000                             @ Bit-22 for falling-edge event on GP21
.equ GPIO_BTN_RISE_MSK, 0x00800000                             @ Bit-23 for rising-edge event on GP21

.equ DOT_DASH_THRESHOLD, 300000                                @ To check if dot or dash is pressed time in microseconds (.3 s)
.equ SPACE_THRESHOLD, 1000000                                  @ A space is added after 1 second
.equ END_MORSE_CODE, 2000000                                   @ End of morse threshold time in microseconds (2s)

@ Entry point to the ASM portion of the program
main_asm:
    mov     r5, r14        ////////////////////////////////////////////// Please do not modify r5
    bl      install_gpio_isr                                   @ Install the GPIO ISR in the RAM Vector
    bl      install_alarm_isr
    bl      btns_init                                          @ Initialise the gpio pin
loop:
    wfi
    b       loop                                               @ Infinite loop

install_alarm_isr:
    // Get the base address of the interrupts and write to them.
    ldr     r0, =(PPB_BASE + M0PLUS_VTOR_OFFSET)
    ldr     r1, [r0] 
    ldr     r2, =alarm_isr
    str     r2, [r1, ALRM_ISR_OFFSET]

    // Reload IRQ0 for timer0
    ldr     r1, =(1 << 0)
    ldr     r0, =(PPB_BASE + M0PLUS_NVIC_ICPR_OFFSET)
    str     r1, [r0]
    ldr     r0, =(PPB_BASE + M0PLUS_NVIC_ISER_OFFSET)
    str     r1, [r0]

    bx     lr

install_gpio_isr:
    // Get the address of the RAM Vector table using 
    ldr     r0, =(PPB_BASE + M0PLUS_VTOR_OFFSET)               @ Get RAM Vector Table offset
    ldr     r1, [r0]                                           @ copy Table address in register r1
    ldr     r0, =gpio_isr                                     @ Get the action done by gpio pin 21
    // Store the address of GPIO handler to 
    str     r0, [r1, GPIO_ISR_OFFSET]
    
    // Reload IRQ0 for bank0
    ldr     r0, =(1 << 13)
    ldr     r1, =(PPB_BASE + M0PLUS_NVIC_ICPR_OFFSET)
    str     r0, [r1]
    ldr     r1, =(PPB_BASE + M0PLUS_NVIC_ISER_OFFSET)
    str     r0, [r1]

    // Exit subroutine
    bx      lr

btns_init:
    push    {lr}                                               @ Save link register to stack
    movs    r0, #GPIO_BTN                                      @ load gpio pin in register r0
    bl      asm_gpio_init                                      @ Initialise the gpio pin 
    movs    r0, #GPIO_BTN                                      @ load gpio pin in register r0
    movs    r1, #GPIO_DIR_IN                                   @ Load gpio input in register r1
    bl      asm_gpio_set_dir                                   @ Set gpio pin 21 as input pin
    movs    r0, #GPIO_BTN                                      @ load gpio pin in register r0
    bl      asm_gpio_set_irq                                   @ Enable rising and falling edge detection for GP21
    pop     {pc}                                               @ restore the link register and return

alarm_set:
    // Enable the alarm interrupt timers in memory
    ldr     r0, =(TIMER_BASE + TIMER_INTE_OFFSET)
    ldr     r1, =0b11                                          @ Enable alarm 0 (bit 0) and 1 (bit 1)
    str     r1, [r0]

    // Set the duration of the alarm to our Space Threshold
    ldr    r0, =(TIMER_BASE + TIMER_TIMELR_OFFSET)
    ldr    r0, [r0]
    ldr    r1, =SPACE_THRESHOLD
    adds   r1, r1, r0 
    ldr    r0, =(TIMER_BASE + TIMER_ALARM0_OFFSET)
    str    r1, [r0]

    // Set the alarm1 for ending morse code
    ldr    r0, =(TIMER_BASE + TIMER_TIMELR_OFFSET)
    ldr    r0, [r0]
    ldr    r1, =END_MORSE_CODE
    adds   r1, r1, r0
    ldr    r0, =(TIMER_BASE + TIMER_ALARM1_OFFSET)
    str    r1, [r0]
    
    bx     lr 

// This subroutine handles the space and ending morse code string logic
.thumb_func
alarm_isr:
    push    {lr}
    // Check if inactivity is more than 1s

    // read current time
    ldr     r0, =(TIMER_BASE + TIMER_TIMELR_OFFSET)
    ldr     r0, [r0]                                           @ current time

    // get timestamp of last button event
    ldr     r1, =last_timestamp
    ldr     r1, [r1]                                           @ time of last button event

    // get duration
    subs    r2, r0, r1                                         @ duration = current time - last time

    // check if duration is greater than the threshold to end the morse code and branch if true
    ldr     r3, =END_MORSE_CODE
    cmp     r2, r3
    bgt     handle_end_morse                                   @ if duration > end threshold, branch to the end handler

    // check if duration is greater than the threshold to write a space and branch if true
    ldr     r3, =SPACE_THRESHOLD
    cmp     r2, r3
    bgt     store_space

    // clear the isr but should never reach here if the interrupt is set up properly
    b       alarm_isr_clear 

handle_end_morse:
    // add nullterm to buffer
    movs    r0, #0                                             @ nullterm = 0
    ldr     r1, =morse_code_buffer                             @ get address of start of buffer
    ldr     r2, =current
    ldr     r2, [r2]                                           @ get the current index
    adds    r1, r1, r2                                         @ add byte index to start of buffer pointer
    strb    r0, [r1]                                           @ store nullterm

game_over:
    bx      r5

store_space:
    movs    r0, #' '
    ldr     r1, =morse_code_buffer 
    ldr     r2, =current 
    ldr     r2, [r2]
    adds    r1, r2, r1
    strb    r0, [r1]
    adds    r2, r2, #1
    ldr     r3, =current 
    str     r2, [r3]

alarm_isr_clear:
    ldr     r0, =(TIMER_BASE + TIMER_INTR_OFFSET)
    ldr     r1, =0b11
    str     r1, [r0]

    // Don't reset the alarms here, they need to be reset when the buttons are pressed

    pop     {pc}

// This subroutine should hold the morse code input buffer logic
.thumb_func
gpio_isr:
    push    {lr}                                               @ Save link register to stack

    // Get the GPIO Vector table address
    ldr     r0, =(IO_BANK0_BASE + IO_BANK0_PROC0_INTS2_OFFSET)
    ldr     r1, [r0]                                           @ Save the address in a register r1


    ldr     r2, =GPIO_BTN_FALL_MSK                             @ load the mask to check for falling edge event in register r2
    movs    r3, r1
    ands    r3, r2                                             @ check if gp21 was pressed
    cmp     r3, r2                                             @ check if button is pressed
    beq     btn_press                                          @ get timestamp

    // Check if it is rising edge (button release)

btn_rise:
    ldr    r2, =GPIO_BTN_RISE_MSK
    movs   r3, r1
    ands   r3, r2
    cmp    r3, r2
    beq    btn_release

    b      gpio_end

btn_press:
    ldr     r0, =(TIMER_BASE + TIMER_TIMELR_OFFSET)
    ldr     r0, [r0]
    ldr     r2, =last_timestamp
    str     r0, [r2] 
    b       gpio_end

btn_release:
      // Get the Timer address
    ldr     r0, =(TIMER_BASE + TIMER_TIMELR_OFFSET)            @ Get the lower 32 bits of timer
    ldr     r0, [r0]                                           @ This is our current timer

    // Compute press duration
    ldr    r2, =last_timestamp 
    ldr    r2, [r2]
    subs   r3, r0, r2                                         @ time pressed = release time - press time

    // Logic to check if it is dot or dash
    ldr    r2, =DOT_DASH_THRESHOLD                            @ check if it is dot or dash
    cmp    r3, r2                                             @ check if dot or dash
    blt    store_dot                                          @ store dash if timer < 300000 ms

store_dash:
    movs   r2, #'-'                                           @ load dash in resgister r2
    ldr    r3, =current 
    ldr    r3, [r3]
    ldr    r4, =morse_code_buffer
    adds   r4, r3, r4 
    strb   r2, [r4]
    adds   r3, r3, #1
    ldr    r4, =current 
    str    r3, [r4]

    b      gpio_end

store_dot:
    movs   r2, #'.'                                           @ set register r2 to store dot
    ldr    r3, =current                                       @ get current pointer
    ldr    r3, [r3]                                           @ get address of current pointer
    ldr    r4, =morse_code_buffer
    adds   r4, r3, r4                                         @ let current point to morse_code_buffer 
    strb   r2, [r4]                                           @ store dot at morse_code_buffer
    adds   r3, r3, #1                                         @ update current by one index
    ldr    r4, =current
    str    r3, [r4]                                           @ update current in memory
    
gpio_end:
    // Acknowledge and clear interrupt
    bl      alarm_set
    // better not to do I/O inside an ISR because it's slow and blocking
    ldr     r0, =(IO_BANK0_BASE + IO_BANK0_INTR2_OFFSET) @ get the vector table address
    ldr     r1, =0b11
    lsls    r1, #22
    str     r1, [r0]

    pop     {pc}

@ Set data alignment
.global last_timestamp
.global morse_code_buffer
.global current
.data

    .align 4
morse_code_buffer: .space 1024                                @ create space to check for morse code input
    .align 4
current: .word 0                                              @ keep track of current input
    .align 4
dot: .asciz "Make dot"
    .align 4
dash: .asciz "Make dash"
    .align 4
last_timestamp: .word 0                                       @ Capture last timestamp
    .align 4
end_morse: .asciz "Your morse code has completed"
