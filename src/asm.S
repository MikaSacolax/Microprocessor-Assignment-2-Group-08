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
.equ ALRM1_ISR_OFFSET, 0x44                                    @ Alarm1 is #17 in interrupt vector table

.equ GPIO_BTN_FALL_MSK, 0x00400000                             @ Bit-22 for falling-edge event on GP21
.equ GPIO_BTN_RISE_MSK, 0x00800000                             @ Bit-23 for rising-edge event on GP21

.equ DOT_DASH_THRESHOLD, 300000                                @ To check if dot or dash is pressed time in ms.
.equ SPACE_THRESHOLD, 1000000                                  @ A space is added after 1 second
.equ END_MORSE_CODE, 2000000

@ Entry point to the ASM portion of the program
main_asm:
    push    {lr}
    bl      install_gpio_isr                                   @ Install the GPIO ISR in the RAM Vector
    bl      install_alarm_isrs
    bl      btns_init                                          @ Initialise the gpio pin

    pop     {pc}

install_alarm_isrs:
    // Get the base address of the interrupts and write to them.
    ldr     r0, =(PPB_BASE + M0PLUS_VTOR_OFFSET)
    ldr     r1, [r0]                                           @ load the vector table base address into r1

    // install alarm0 isr (irq0)
    ldr     r2, =alarm_isr
    str     r2, [r1, ALRM_ISR_OFFSET]

    // install alarm1 isr (irq1)
    ldr     r2, =alarm1_isr
    str     r2, [r1, ALRM1_ISR_OFFSET]

    // nvic setup for irq0 (timer0 alarm0) and irq1 (timer1 alarm1)
    ldr     r1, =((1 << 0) | (1 << 1))                         @ mask (0b11)
    ldr     r0, =(PPB_BASE + M0PLUS_NVIC_ICPR_OFFSET)
    str     r1, [r0]
    ldr     r0, =(PPB_BASE + M0PLUS_NVIC_ISER_OFFSET)
    str     r1, [r0]

    bx     lr

install_gpio_isr:
    // Get the address of the RAM Vector table using 
    ldr     r0, =(PPB_BASE + M0PLUS_VTOR_OFFSET)               @ Get RAM Vector Table offset
    ldr     r1, [r0]                                           @ copy Table address in register r1
    ldr     r0, = gpio_isr                                     @ Get the action done by gpio pin 21
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
    // Enable the alarm interrupt timer in memory
    ldr     r0, =(TIMER_BASE + TIMER_INTE_OFFSET)
    ldr     r1, =0b11                                          @ enable both alarm interrupts
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

// ISR for only alarm0, space detection
.thumb_func
alarm_isr:
    push    {lr}
    // we don't have to check inactivity durations anymore because
    // we've split the logic into two interrupts
store_space:
    movs    r0, #' '
    ldr     r1, =morse_code_buffer 
    ldr     r2, =current 
    ldr     r2, [r2]
    adds    r1, r1, r2
    strb    r0, [r1]                                           @ store space in buffer

    adds    r2, r2, #1
    ldr     r3, =current
    str     r2, [r3]                                           @ increment current

    movs    r0, #1
    ldr     r1, =new_char_flag
    str     r0, [r1]

    // clear only the alarm0 interrupt
    ldr     r0, =(TIMER_BASE + TIMER_INTR_OFFSET)
    ldr     r1, =1                                             @ mask to clear only alarm0
    str     r1, [r0]

    pop     {pc}

// ISR for alarm1, end of sequence detections
.thumb_func
alarm1_isr:
    push    {lr}

    // decrement the index to overwrite the space
    ldr     r2, =current
    ldr     r2, [r2]

    // check if index is 0 before decrementing it
    cmp     r2, #0
    beq     skip_decrement                                     @ skip decrementing if index is 0

    subs    r2, r2, #1

skip_decrement:
    ldr     r1, =morse_code_buffer
    adds    r1, r1, r2


    // alarm1 means 2 seconds passed, end the sequence
    // load the null terminator
    movs    r0, #0
    strb    r0, [r1]                                           @ null terminate the string in the buffer

    movs    r0, #1
    ldr     r1, =sequence_complete_flag
    str     r0, [r1]                                           @ set flag to 1 to signal to the C code


    // for cleanup, acknowledge both the alarm interrupt flags
    ldr     r0, =(TIMER_BASE + TIMER_INTR_OFFSET)
    ldr     r1, =0b11                                          @ mask to clear both alarms
    str     r1, [r0]

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

    movs   r0, #1
    ldr    r1, =new_char_flag
    str    r0, [r1]

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

    movs   r0, #1
    ldr    r1, =new_char_flag
    str    r0, [r1]
    
gpio_end:
    // Acknowledge and clear interrupt
    bl      alarm_set
    //bl      print_morse_code
    ldr     r0, =(IO_BANK0_BASE + IO_BANK0_INTR2_OFFSET) @ get the vector table address
    ldr     r1, =0b11
    lsls    r1, #22
    str     r1, [r0]

    pop     {pc}

@ Set data alignment
.global last_timestamp
.global morse_code_buffer
.global current
.global sequence_complete_flag
.global new_char_flag
.data
    .align 4
morse_code_buffer: .space 1024                                @ create space to check for morse code input
.data
    .align 4
current: .word 0                                              @ keep track of current input
.data  
    .align 4
dot: .asciz "Make dot"
    .align 4
dash: .asciz "Make dash"
.data
    .align 4
last_timestamp: .word 0                                       @ Capture last timestamp
    .align 4
end_morse: .asciz "\nYour morse code has completed\n"
.section .data
format: .asciz "%c"
.data
    .align 4
sequence_complete_flag: .word 0                                              @ Flag to signal when c can take over
.data
    .align 4
new_char_flag: .word 0