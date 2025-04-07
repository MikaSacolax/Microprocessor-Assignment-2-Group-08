#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include <stdio.h>


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

void print_morse_code()
{
    printf("Morse code: %s\n", morse_code_buffer);
}

/*
 * Main entry point for the code - simply calls the main assembly function.
 */
 int main() {
    stdio_init_all(); // Initialise all basic IO
    printf("Assignment #2...\n"); // Basic print to console
    main_asm();
    return 0;
}