#include <hardware/sync.h>
#include <pico/stdio.h>
#include <pico/time.h>
#include <stdint.h>
#include <stdio.h>

#include "display_utils.h"
#include "sdk_asm_helpers.c"

// globals
const uint32_t MORSE_BUFFER_SIZE = 1024;

// declare the external variables so the compiler knows they exist:
extern volatile unsigned char morse_code_buffer[];
extern volatile uint32_t current;
extern volatile uint32_t sequence_complete_flag;
extern volatile uint32_t new_char_flag;

// Declare the main assembly code entry point.
void main_asm();

// function declarations
void main_menu();

int main() {
  stdio_init_all(); // Initialise all basic IO
  sleep_ms(5000);   // time to allow serial monitor connection

  main_asm();

  while (true) {
    if (new_char_flag) {
      new_char_flag = 0;
      printf(" <- character entered.\n");
    } else
      __wfi();
  }

  return 0;
}

void main_menu() { print_main_menu(); }

void level_1() {}

void level_2() {}

void level_3() {}

void level_4() {}

void level_5() {}