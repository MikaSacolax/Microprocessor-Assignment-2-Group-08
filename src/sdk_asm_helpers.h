
#ifndef SDK_ASM_HELPERS_H
#define SDK_ASM_HELPERS_H

#include <pico/stdlib.h>

void asm_gpio_init(uint pin);
void asm_gpio_set_dir(uint pin, bool out);
bool asm_gpio_get(uint pin);
void asm_gpio_put(uint pin, bool value);
void asm_gpio_set_irq(uint pin);

#endif