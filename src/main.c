#include <pico/stdio.h>
#include <pico/time.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "display_utils.h"
#include "morse_utils.h"

// globals
const uint32_t MORSE_BUFFER_SIZE = 1024;

// declare the external variables so the compiler knows they exist:
extern unsigned char morse_code_buffer[];
extern uint32_t current;
extern uint32_t flag;

// Declare the main assembly code entry point.
void main_asm();

// function declarations
void main_menu();

int main() {
  stdio_init_all(); // Initialise all basic IO
  sleep_ms(5000);   // time to allow serial monitor connection

  return 0;
}

void main_menu() { print_main_menu(); }

void level_1() {}

void level_2() {}

void level_3() {}

void level_4() {}

void level_5() {}