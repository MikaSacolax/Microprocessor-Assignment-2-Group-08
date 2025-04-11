/** @file */
#ifndef SDK_ASM_HELPERS_H
#define SDK_ASM_HELPERS_H

#include <pico/stdlib.h>

/**
 * @brief Initialise a GPIO pin – see SDK for detail on gpio_init().
 * 
 * @param pin 
 */
void asm_gpio_init(uint pin);

/**
 * @brief Set direction of a GPIO pin – see SDK for detail on gpio_set_dir().
 * 
 * @param pin 
 * @param out 
 */
void asm_gpio_set_dir(uint pin, bool out);

/**
 * @brief Get the value of a GPIO pin – see SDK for detail on gpio_get().
 * 
 * @param pin 
 * @return true 
 * @return false 
 */
bool asm_gpio_get(uint pin);

/**
 * @brief Set the value of a GPIO pin – see SDK for detail on gpio_put()
 * 
 * @param pin 
 * @param value 
 */
void asm_gpio_put(uint pin, bool value);

/**
 * @brief Enable falling-edge interrupt – see SDK for detail on gpio_set_irq_enabled()
 * 
 * @param pin 
 */
void asm_gpio_set_irq(uint pin);

#endif