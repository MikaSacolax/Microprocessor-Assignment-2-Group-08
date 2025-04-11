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
    bl      install_alarm_isrs                                 @ Install the alarms in the RAM Vector
    bl      btns_init                                          @ Initialise the gpio pin

    pop     {pc}

/**
    @brief: Function that handles installing the alarm interrupt in the vector table
*/
install_alarm_isrs:
    ldr     r0, =(PPB_BASE + M0PLUS_VTOR_OFFSET)
    ldr     r1, [r0]                                           @ load the vector table base address into r1

    // install alarm0 isr (irq0)
    ldr     r2, =alarm_isr                                     @ get the value from thumb func
    str     r2, [r1, ALRM_ISR_OFFSET]                          @ Write to the interrupt at alarm isr offset

    // install alarm1 isr (irq1)
    ldr     r2, =alarm1_isr                                    @ Get the value from thumb func alarm1 isr
    str     r2, [r1, ALRM1_ISR_OFFSET]                         @ Write to the interrupt at alarm1 isr

    // nvic setup for irq0 (timer0 alarm0) and irq1 (timer1 alarm1)
    ldr     r1, =((1 << 0) | (1 << 1))                         @ mask (0b11)
    ldr     r0, =(PPB_BASE + M0PLUS_NVIC_ICPR_OFFSET)          @ Get the interrut vector table 
    str     r1, [r0]                                           @ Clear any pending interrupts for alarm0 and alarm1
    ldr     r0, =(PPB_BASE + M0PLUS_NVIC_ISER_OFFSET)          @ Get the interrupt vector table address
    str     r1, [r0]                                           @ Enable the interrupts for alarm0 and alarm1

    bx     lr                                                  @ Return back after function has completed.

/**
    @brief: This function handles installing the gpio interrupt
*/
install_gpio_isr:
    ldr     r0, =(PPB_BASE + M0PLUS_VTOR_OFFSET)               @ Get RAM Vector Table offset
    ldr     r1, [r0]                                           @ copy Table address in register r1
    ldr     r0, = gpio_isr                                     @ Get the action done by gpio pin 21
    // Store the address of GPIO handler to 
    str     r0, [r1, GPIO_ISR_OFFSET]                          @ Store the state from thumb func at gpio address in the vector table.
    
    // Reload IRQ0 for bank0
    ldr     r0, =(1 << 13)                                     
    ldr     r1, =(PPB_BASE + M0PLUS_NVIC_ICPR_OFFSET)          @ Get the interrupt vector table address
    str     r0, [r1]                                           @ Clear any pending interrupts
    ldr     r1, =(PPB_BASE + M0PLUS_NVIC_ISER_OFFSET)          @ Get the interrupt vector table address
    str     r0, [r1]                                           @ Enable the interrupt.

    // Exit subroutine  
    bx      lr                                                 @ Return back after completing.

/**
    @brief: This function should enable the buttons on RP2040
*/
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

/**
    @brief: This function should set the alarm0 and alarm1
 */

alarm_set:
    // Enable the alarm interrupt timer in memory
    ldr     r0, =(TIMER_BASE + TIMER_INTE_OFFSET)              @ Get the alarm vector table address to enable the 
    ldr     r1, =0b11                                          @ enable both alarm interrupts
    str     r1, [r0]                                           @ enable alarm0 and alarm1

    // Set the duration of the alarm to our Space Threshold
    ldr    r0, =(TIMER_BASE + TIMER_TIMELR_OFFSET)             @ Get the 32 bit timer address
    ldr    r0, [r0]                                            @ Get the value of the timer recorded
    ldr    r1, =SPACE_THRESHOLD                                @ Load the threshold for space duration
    adds   r1, r1, r0                                          @ Add the timer value with space duratoin
    ldr    r0, =(TIMER_BASE + TIMER_ALARM0_OFFSET)             @ Load the address of alarm0
    str    r1, [r0]                                            @ Set alarm0 to check for space. (1s)

    // Set the alarm1 for ending morse code
    ldr    r0, =(TIMER_BASE + TIMER_TIMELR_OFFSET)             @ Get the 32 bit imer address
    ldr    r0, [r0]                                            @ get the value of the timer recorded
    ldr    r1, =END_MORSE_CODE                                 @ load the threshold for ending morse code string
    adds   r1, r1, r0                                          @ Add the timer value with ending morse code time duration
    ldr    r0, =(TIMER_BASE + TIMER_ALARM1_OFFSET)             @ Get the address of alarm1
    str    r1, [r0]                                            @ Set alarm1 to check if morse code string has ended (2s)

    bx     lr                                                  @ Return back from function

/**
    @brief: This thumb func checks if space is detected
 */
.thumb_func
alarm_isr:
    push    {lr}                                             @ push link register to stack
    // we don't have to check inactivity durations anymore because
    // we've split the logic into two interrupts
store_space:    
    movs    r0, #' '                                         @ load the space character in register r0
    ldr     r1, =morse_code_buffer                           @ load the address of morse code buffer from memory in register r1.
    ldr     r2, =current                                     @ load the address of current pointer from memmor address in register r2.
    ldr     r2, [r2]                                         @ get value of current stored at the memory adderss
    adds    r1, r1, r2                                       @ add the morse code buffer with current so that it points to morse code buffer
    strb    r0, [r1]                                         @ store space in buffer

    adds    r2, r2, #1                                       @ update current by one
    ldr     r3, =current                                     @ load the address oc current
    str     r2, [r3]                                         @ store the updated value of current at address

    movs    r0, #1                                           @ store 1 in register r0
    ldr     r1, =new_char_flag                               @ get the address of new character flag
    str     r0, [r1]                                         @ set flag to 1.

    // clear only the alarm0 interrupt
    ldr     r0, =(TIMER_BASE + TIMER_INTR_OFFSET)            @ Get the raw interrupts register address
    ldr     r1, =1                                           @ mask to clear only alarm0
    str     r1, [r0]                                         @ clear alarm0 to indicate space character is dealt with

    pop     {pc}                                             @ Pop link register from stack and return.


/**
    @brief: This function should handle ending morse code string if inactivity is >= 2s.
 */
.thumb_func
alarm1_isr:
    push    {lr}                                            @ Save link register to stack

    // decrement the index to overwrite the space
    ldr     r2, =current                                   @ load the memory address of current
    ldr     r2, [r2]                                       @ load the value of current in register r2

    // check if index is 0 before decrementing it
    cmp     r2, #0
    beq     skip_decrement                                 @ skip decrementing if index is 0

    subs    r2, r2, #1                                     @ update value of current

skip_decrement:
    ldr     r1, =morse_code_buffer                         @ load the address of morse code buffer from memory
    adds    r1, r1, r2                                     @ add the morse code buffer with current value so that current points to morse code buffer.


    // alarm1 means 2 seconds passed, end the sequence
    // load the null terminator
    movs    r0, #0                                         @ load character 0 in register r0
    strb    r0, [r1]                                       @ null terminate the string in the buffer

    movs    r0, #1                                         @ load character 1 in register r0
    ldr     r1, =sequence_complete_flag                    @ set flag at sequence_complete_flag to 1
    str     r0, [r1]                                       @ set flag to 1 to signal to the C code


    // for cleanup, acknowledge both the alarm interrupt flags
    ldr     r0, =(TIMER_BASE + TIMER_INTR_OFFSET)          @ Get the raw interrupt register address
    ldr     r1, =0b11                                      @ mask to clear both alarms
    str     r1, [r0]                                       @ clear alarm0 and alarm1 interrut

    pop     {pc}                                           @ pop back link register from stack and return.

/**
    @brief: This subrotuine should handle either inputting a dot or dash
 */
.thumb_func
gpio_isr:
    push    {lr}                                               @ Save link register to stack

    // Get the GPIO Vector table address
    ldr     r0, =(IO_BANK0_BASE + IO_BANK0_PROC0_INTS2_OFFSET) @ Get the address for the GPIO interrupt status
    ldr     r1, [r0]                                           @ Save the address in a register r1


    ldr     r2, =GPIO_BTN_FALL_MSK                             @ load the mask to check for falling edge event in register r2
    movs    r3, r1                                             @ load GPIO value in register r3.
    ands    r3, r2                                             @ check if gp21 was pressed
    cmp     r3, r2                                             @ check if button is pressed
    beq     btn_press                                          @ get timestamp

    // Check if it is rising edge (button release)

btn_rise:
    ldr    r2, =GPIO_BTN_RISE_MSK                              @ load the button rise mask in register r2.
    movs   r3, r1                                              @ laod GPIO value in register r3.
    ands   r3, r2                                              @ and r3 and r2 to check if it's a rising edge event.
    cmp    r3, r2                                              @ check if it's rising edge
    beq    btn_release                                         @ handle logic if it's rising edge

    b      gpio_end                                            @ brnach to end of subroutine.

btn_press:
    ldr     r0, =(TIMER_BASE + TIMER_TIMELR_OFFSET)            @ Get the 32 bit timer address
    ldr     r0, [r0]                                           @ get the value of timer
    ldr     r2, =last_timestamp                                @ get the last timestamp address from memory in register r2.
    str     r0, [r2]                                           @ store the timer value at last recorded timestamp address.
    b       gpio_end                                           @ branch to end of subroutine

btn_release:
      // Get the Timer address
    ldr     r0, =(TIMER_BASE + TIMER_TIMELR_OFFSET)            @ Get the lower 32 bits of timer
    ldr     r0, [r0]                                           @ This is our current timer

    // Compute press duration
    ldr    r2, =last_timestamp                                 @ get the address of the last timestamp from memory in register r2.
    ldr    r2, [r2]                                            @ load the last recorded last time
    subs   r3, r0, r2                                          @ time pressed = release time - press time

    // Logic to check if it is dot or dash
    ldr    r2, =DOT_DASH_THRESHOLD                             @ check if it is dot or dash
    cmp    r3, r2                                              @ check if dot or dash
    blt    store_dot                                           @ store dash if timer < 300000 ms

store_dash:
    movs   r2, #'-'                                           @ load dash in resgister r2
    ldr    r3, =current                                       @ load the address of current from memory
    ldr    r3, [r3]                                           @ laod the value of current in register r3
    ldr    r4, =morse_code_buffer                             @ load the morse code buffer address from memory
    adds   r4, r3, r4                                         @ add current to address of morse code buffer so it points to the morse code buffer
    strb   r2, [r4]                                           @ store dot at morse code buffer memory
    adds   r3, r3, #1                                         @ update current by one
    ldr    r4, =current                                       @ get address of current.
    str    r3, [r4]                                           @ store updated value at address

    movs   r0, #1                                             @ load 1 in register r0
    ldr    r1, =new_char_flag                                 @ load address of new character flag
    str    r0, [r1]                                           @ Set flag to 1

    b      gpio_end                                           @ branch to end of subroutine

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

    movs   r0, #1                                             @ load 1 in register r0
    ldr    r1, =new_char_flag                                 @ get address of new character flag
    str    r0, [r1]                                           @ Set flag to 1
    
gpio_end:
    // Acknowledge and clear interrupt
    bl      alarm_set                                         @ call function alarm_set to enable alarms
    //bl      print_morse_code
    ldr     r0, =(IO_BANK0_BASE + IO_BANK0_INTR2_OFFSET)      @ get the vector table address 
    ldr     r1, =0b11                                         @ load 0b11 into register r1
    lsls    r1, #22                                           @ shift bits by 22 to handle gpio interrupt
    str     r1, [r0]                                          @ clear the interrupt

    pop     {pc}                                              @ pop link register from stack and return.

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