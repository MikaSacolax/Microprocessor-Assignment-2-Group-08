#include <hardware/gpio.h>
#include <hardware/sync.h>
#include <pico/stdlib.h>
#include <pico/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "display_utils.h"
#include "morse_utils.h"

// Declare the external variables so the compiler knows they exist:
extern unsigned char morse_code_buffer[];
extern uint32_t current;
extern uint32_t flag;
extern uint32_t current;
extern uint32_t start_index;

int Hearts = 3;

void print_morse_code();

// Declare the main assembly code entry point.
void main_asm();

// Random character
char Rand_char();

// Levels
void Level_1();
void Level_2();
void Level_3();
void Level_4();

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

void print_morse_code() { printf("Morse code: %s\n", morse_code_buffer); }

/*
 * Main entry point for the code - simply calls the main assembly function.
 */
int main() {
  stdio_init_all(); // Initialise all basic IO
  sleep_ms(5000);
  print_main_menu();

  printf("Assignment #2...\n"); // Basic print to console

  while (true) {
    printf("\nMorse code: ");
    flag = 0;

    while (flag == 0) {
      __wfi();
    }

    printf("\nCurrent: %i, Start_index: %i", current, start_index);

    if (current - start_index > 6) {
      printf("\nYour input is too long: %i ", current - start_index);
      current = current + 1;
      continue;
    }

    char input[6] = "     ";
    int x = 0;
    while (start_index <= current) {
      input[x] = morse_code_buffer[start_index];
      printf("%c", morse_code_buffer[start_index]);
      x = x + 1;
      start_index = start_index + 1;
    }

    printf("%c, %s", from_morse(input), input);

    switch (from_morse(input)) {
    case '1':
      Level_1();
      break;
    case '2':
      Level_2();
      break;
    case '3':
      Level_3();
      break;
    case '4':
      Level_4();
      break;

    default:
      printf("Not a valid input, Try Again.");
      current = current + 1;
      break;
    }
  }

  Level_1();
  return 0;
}

void main_menu() {}

void Level_1() {

  int iterations = 5;

  while (Hearts > 0 && iterations > 0) {
    int start_index = current;

    char character_to_spell = Rand_char();
    printf("Level 1 character: %c  Morse: %s\n", character_to_spell,
           to_morse(character_to_spell));

    // main_asm();

    if (current - start_index > 6) {
      printf("Wrong, your input does not equal a singular character.");
      Hearts = Hearts - 1;
      iterations = 5;
      continue;
    } else {
      char input[6] = "     ";
      int x = 0;
      while (start_index != current) {
        input[x] = morse_code_buffer[start_index];
        x = x + 1;
        start_index = start_index + 1;
      }

      printf("Your inputted character: %c", from_morse(input));

      if (strcmp(input, to_morse(character_to_spell)) == 0) {
        printf("Congrats on getting it right");
        iterations = iterations - 1;

        if (Hearts < 3) {
          Hearts = Hearts + 1;
        }

        continue;
      } else {
        printf("Wrong");
        Hearts = Hearts - 1;
        iterations = 5;
        continue;
      }
    }
  }

  if (Hearts == 0) {
    // Break or go to start, Doing something I don't know
    printf("Run out of hearts");
  }

  Level_2();
}

void Level_2() {}

void Level_3() {}

void Level_4() {}

char Rand_char() {
  srand(time_us_64());
  int num = rand() % 35;

  if (num <= 9) {
    return ('0' + num);
  } else {
    return ('A' + num - 9);
  }
}