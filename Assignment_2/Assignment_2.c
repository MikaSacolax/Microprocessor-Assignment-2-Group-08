#include <stdio.h>
#include <stdbool.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "hardware/gpio.h"
#include "hardware/clocks.h"
#include "ws2812.pio.h"


// Assigning required GPIO pins
#define IS_RGBW true        // Will use RGBW format
#define NUM_PIXELS 1        // There is 1 WS2812 device in the chain
#define WS2812_PIN 28       // The GPIO pin that the WS2812 connected to
#define GP_21 PIN 21

// UART configuration
#define UART_ID uart0
#define UART_BAUD 9600

// Initialising Functions
void UART_Init(void);
void UART_SendString(const char *str);
void Display_Message(void);
static inline void ws2812_program_init(PIO pio, uint sm, uint offset, uint pin, float freq, bool rgbw);

int main(void) {
    
    stdio_init_all();
    UART_Init();

    PIO pio = pio0;
    uint offset = pio_add_program(pio, &ws2812_program);
    ws2812_program_init(pio, 0, offset, WS2812_PIN, 800000, IS_RGBW);
    Display_Message();
    
    while (true){
        put_pixel(urgb_u32(0x00, 0x00, 0x7F));  // Set pixel to blue
    }
    

}

static inline void ws2812_program_init(PIO pio, uint sm, uint offset, uint pin, float freq, bool rgbw) {

    pio_gpio_init(pio, pin);
    pio_sm_set_consecutive_pindirs(pio, sm, pin, 1, true);

    pio_sm_config c = ws2812_program_get_default_config(offset);
    sm_config_set_sideset_pins(&c, pin);
    sm_config_set_out_shift(&c, false, true, rgbw ? 32 : 24);
    sm_config_set_fifo_join(&c, PIO_FIFO_JOIN_TX);

    int cycles_per_bit = ws2812_T1 + ws2812_T2 + ws2812_T3;
    float div = clock_get_hz(clk_sys) / (freq * cycles_per_bit);
    sm_config_set_clkdiv(&c, div);

    pio_sm_init(pio, sm, offset, &c);
    pio_sm_set_enabled(pio, sm, true);
}

/**
 * @brief Wrapper function used to call the underlying PIO
 *        function that pushes the 32-bit RGB colour value
 *        out to the LED serially using the PIO0 block. The
 *        function does not return until all of the data has
 *        been written out.
 * 
 * @param pixel_grb The 32-bit colour value generated by urgb_u32()
 */
static inline void put_pixel(uint32_t pixel_grb) {
    pio_sm_put_blocking(pio0, 0, pixel_grb << 8u);
}


/**
 * @brief Function to generate an unsigned 32-bit composit GRB
 *        value by combining the individual 8-bit paramaters for
 *        red, green and blue together in the right order.
 * 
 * @param r     The 8-bit intensity value for the red component
 * @param g     The 8-bit intensity value for the green component
 * @param b     The 8-bit intensity value for the blue component
 * @return uint32_t Returns the resulting composit 32-bit RGB value
 */
static inline uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b) {
    return  ((uint32_t) (r) << 8)  |
            ((uint32_t) (g) << 16) |
            (uint32_t) (b);
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

