// Assignment_2.c
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/watchdog.h"  // Include the Watchdog Timer library

// Include the watchdog timer setup functions
#include "watchdog_timer.c"

// Declare the external variables so the compiler knows they exist:
extern unsigned char morse_code_buffer[];

void print_morse_code();

// Declare the main assembly code entry point.
void main_asm();

// Initialise a GPIO pin – see SDK for detail on gpio_init()
void asm_gpio_init(uint pin) {
    gpio_init(pin);
}

// Set direction of a GPIO pin – see SDK for detail on gpio_set_dir()
void asm_gpio_set_dir(uint pin, bool out) {
    gpio_set_dir(pin, out);
}

// Get the value of a GPIO pin – see SDK for detail on gpio_get()
bool asm_gpio_get(uint pin) {
    return gpio_get(pin);
}

// Set the value of a GPIO pin – see SDK for detail on gpio_put()
void asm_gpio_put(uint pin, bool value) {
    gpio_put(pin, value);
}

// Enable falling-edge interrupt – see SDK for detail on gpio_set_irq_enabled()
void asm_gpio_set_irq(uint pin) {
    gpio_set_irq_enabled(pin, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true);
}

// Function to print Morse code
void print_morse_code() {
    printf("Morse code: %s\n", morse_code_buffer);
}

/*
 * Main entry point for the code - simply calls the main assembly function.
 */
int main() {
    stdio_init_all(); // Initialise all basic IO
    printf("Assignment #2...\n"); // Basic print to console

    // Initialize the Watchdog Timer with a 9-second timeout (9000 milliseconds)
    setup_watchdog_timer();  // Call the function from watchdog_timer.c

    // Call the main assembly function
    main_asm();

    // Main game loop or other code
    while (1) {
        // Check for user input or valid game logic
        if (is_valid_input()) {
            // Feed the watchdog timer to prevent reset
            feed_watchdog();  // Call the function from watchdog_timer.c

            // Process the input (e.g., Morse code or button press)
            print_morse_code();
        }

        // Other game logic...
        
        // Add some delay to avoid busy-waiting
        sleep_ms(100);  // Adjust as needed
    }

    return 0;
}
