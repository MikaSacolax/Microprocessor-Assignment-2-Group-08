#include <stdio.h>
#include <stdbool.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "hardware/gpio.h"

// Assigning required GPIO pins
#define RED_PIN 17
#define GREEN_PIN 18
#define BLUE_PIN 19
#define GP_21 PIN 21

// UART configuration
#define UART_ID uart0
#define UART_BAUD 9600

// Initialising Functions
void UART_Init(void);
void UART_SendString(const char *str);
void RGB_LED_Init(void);
void Set_LED_Colour(char color);
void Display_Message(void);

int main(void) {
    
    stdio_init_all();
    UART_Init();
    RGB_LED_Init();
    Set_LED_Colour('B'); // Set LED to blue since game has not started
    Display_Message();

}

void UART_Init(void) {
    uart_init(UART_ID, UART_BAUD);            // Initialize UART with specified baud rate
    gpio_set_function(0, GPIO_FUNC_UART);     // TX (GPIO0) for UART0
    gpio_set_function(1, GPIO_FUNC_UART);     // RX (GPIO1) for UART0
    uart_set_hw_flow(UART_ID, false, 0);      // Disable hardware flow control
    uart_set_format(UART_ID, 8, 1, UART_PARITY_NONE); // 8 data bits, 1 stop bit, no parity
}

void UART_SendString(const char *str) {
    while (*str) {
        uart_putc(UART_ID, *str);  // Send character via UART
        str++;
    }
}

void RGB_LED_Init(void) {
    // Set GPIO pins as outputs for RGB LED
    gpio_init(RED_PIN);
    gpio_init(GREEN_PIN);
    gpio_init(BLUE_PIN);

    gpio_set_dir(RED_PIN, GPIO_OUT);
    gpio_set_dir(GREEN_PIN, GPIO_OUT);
    gpio_set_dir(BLUE_PIN, GPIO_OUT);
}

void Set_LED_Colour(char color) {
    switch (color) {
        case 'B':  // Blue for no game in progress
            gpio_put(RED_PIN, 0);
            gpio_put(GREEN_PIN, 0);
            gpio_put(BLUE_PIN, 1);
            break;
        case 'R':  // Red for game in progress
            gpio_put(RED_PIN, 1);
            gpio_put(GREEN_PIN, 0);
            gpio_put(BLUE_PIN, 0);
            break;
        case 'G':  // Green for correct input or other states
            gpio_put(RED_PIN, 0);
            gpio_put(GREEN_PIN, 1);
            gpio_put(BLUE_PIN, 0);
            break;
        default:
            // Turn off all LEDs
            gpio_put(RED_PIN, 0);
            gpio_put(GREEN_PIN, 0);
            gpio_put(BLUE_PIN, 0);
            break;
    }
}

void Display_Message(void) {
    UART_SendString("\t********************************************\t\r\n");
    UART_SendString("\t*    Welcome to the Morse Code Game!       *\t\r\n");
    UART_SendString("\t*        CSU23021-GR08                     *\t\r\n");
    UART_SendString("\t********************************************\t\r\n");
    // LEARN MORSE output
    UART_SendString("\t#       #######    #    ######  #     #\t\r\n");
    UART_SendString("\t#       #         # #   #     # ##    #\t\r\n");
    UART_SendString("\t#       #        #   #  #     # # #   #\t\r\n");
    UART_SendString("\t#       ######  #     # ######  #  #  #\t\r\n");
    UART_SendString("\t#       #       ####### #   #   #   # #\t\r\n");
    UART_SendString("\t#       #       #     # #    #  #    ##\t\r\n");
    UART_SendString("\t####### ####### #     # #     # #     #\t\r\n");
    UART_SendString("\n");
    UART_SendString("\t#     # ####### ######   #####   ######\t\r\n");
    UART_SendString("\t##   ## #     # #     # #     # #      \t\r\n");
    UART_SendString("\t# # # # #     # #     # #       #      \t\r\n");
    UART_SendString("\t#  #  # #     # ######   #####  #####  \t\r\n");
    UART_SendString("\t#     # #     # #   #         # #      \t\r\n");
    UART_SendString("\t#     # #     # #    #  #     # #      \t\r\n");
    UART_SendString("\t#     # ####### #     #  ####   #######\t\r\n");

    //Outputting Instructions
    UART_SendString("\t********************************************\t\r\n");
    UART_SendString("\n\t\t\tINSTRUCTIONS\t\t\t\r\n");
    UART_SendString("\tUSE GP21 TO ENTER A SEQUENCE TO BEGIN\t\r\n");
    UART_SendString("\t\".----\" - LEVEL 01 - CHARS (EASY)\t\r\n");
    UART_SendString("\t\"..---\" - LEVEL 02 - CHARS (HARD)\t\r\n");
    UART_SendString("\t\"...--\" - LEVEL 03 - WORDS (EASY)\t\r\n");
    UART_SendString("\t\"....-\" - LEVEL 04 - WORDS (HARD)\t\r\n");
    UART_SendString("\tENTER THE MORSE CODE FOR YOUR DESIRED LEVEL: \t\r\n");
}

