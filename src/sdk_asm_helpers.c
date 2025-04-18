/** @file */
#include <hardware/gpio.h>
#include <hardware/sync.h>
#include <pico/stdlib.h>
#include <pico/time.h>

#include "sdk_asm_helpers.h"

// Initialise a GPIO pin – see SDK for detail on gpio_init()
void asm_gpio_init(uint pin) { gpio_init(pin); }

// Set direction of a GPIO pin – see SDK for detail on gpio_set_dir()
void asm_gpio_set_dir(uint pin, bool out) { gpio_set_dir(pin, out); }

// Get the value of a GPIO pin – see SDK for detail on gpio_get()
bool asm_gpio_get(uint pin) { return gpio_get(pin); }

// Set the value of a GPIO pin – see SDK for detail on gpio_put()
void asm_gpio_put(uint pin, bool value) { gpio_put(pin, value); }

// Enable falling-edge interrupt – see SDK for detail on gpio_set_irq_enabled()
void asm_gpio_set_irq(uint pin) {
  gpio_set_irq_enabled(pin, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true);
}