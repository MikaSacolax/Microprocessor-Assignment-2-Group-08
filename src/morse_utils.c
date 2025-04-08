#include "morse_utils.h"
#include <ctype.h>
#include <hardware/sync.h>
#include <hardware/timer.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const MorseMap morseTable[MORSE_TABLE_SIZE] = {
    {'A', ".-"},    {'B', "-..."},  {'C', "-.-."},  {'D', "-.."},
    {'E', "."},     {'F', "..-."},  {'G', "--."},   {'H', "...."},
    {'I', ".."},    {'J', ".---"},  {'K', "-.-"},   {'L', ".-.."},
    {'M', "--"},    {'N', "-."},    {'O', "---"},   {'P', ".--."},
    {'Q', "--.-"},  {'R', ".-."},   {'S', "..."},   {'T', "-"},
    {'U', "..-"},   {'V', "...-"},  {'W', ".--"},   {'X', "-..-"},
    {'Y', "-.--"},  {'Z', "--.."},  {'0', "-----"}, {'1', ".----"},
    {'2', "..---"}, {'3', "...--"}, {'4', "....-"}, {'5', "....."},
    {'6', "-...."}, {'7', "--..."}, {'8', "---.."}, {'9', "----."}};

const char *to_morse(char c) {
  // make every character a capital letter
  c = toupper((unsigned char)c);

  // iterate through the struct loopup table
  for (int i = 0; i < MORSE_TABLE_SIZE; i++) {
    if (morseTable[i].character == c) {
      return morseTable[i].morse;
    }
  }

  // return nullterm if not found
  return "";
}

char from_morse(const char *code) {
  for (int i = 0; i < MORSE_TABLE_SIZE; i++) {
    if (strcmp(morseTable[i].morse, code) == 0) {
      return morseTable[i].character;
    }
  }
  return '?';
}

char rand_char() {
  srand(time_us_64());
  int num = rand() % 36;

  if (num <= 9) {
    return ('0' + num);
  } else {
    return ('A' + num - 9);
  }
}

void flush_asm_state() {
  uint32_t ints = save_and_disable_interrupts();
  for (uint32_t i = 0; i < MORSE_BUFFER_SIZE; i++) {
    morse_code_buffer[i] = '\0';
  }
  current = 0;

  sequence_complete_flag = 0;
  new_char_flag = 0;

  restore_interrupts_from_disabled(ints);
}

char from_morse_len(const char *morse_segment, size_t len) {
  if (len == 0 || morse_segment == NULL) {
    return '?';
  }

  for (int i = 0; i < MORSE_TABLE_SIZE; i++) {
    if (strlen(morseTable[i].morse) == len) {
      // strncmp is safe because it won't read past len bytes
      if (strncmp(morseTable[i].morse, morse_segment, len) == 0) {
        return morseTable[i].character;
      }
    }
  }
  return '?';
}

void get_morse_input_interactive(char *output_buffer, size_t buffer_size) {
  if (output_buffer == NULL || buffer_size == 0) {
    return;
  }

  flush_asm_state();

  uint32_t last_displayed_index = 0;
  uint32_t current_len_local = 0;

  while (!sequence_complete_flag) {

    if (new_char_flag) {
      uint32_t ints = save_and_disable_interrupts();
      new_char_flag = 0;
      current_len_local = current;
      restore_interrupts_from_disabled(ints);

      while (last_displayed_index < current_len_local) {
        if (last_displayed_index < MORSE_BUFFER_SIZE) {
          putchar(morse_code_buffer[last_displayed_index]);
        }
        last_displayed_index++;
      }
    }
    busy_wait_us(500);
  }

  {
    uint32_t final_len = 0;
    uint32_t ints = save_and_disable_interrupts();
    sequence_complete_flag = 0;
    final_len = current;

    if (final_len >= buffer_size) {
      final_len = buffer_size - 1;
    }
    if (final_len >= MORSE_BUFFER_SIZE) {
      final_len = MORSE_BUFFER_SIZE - 1;
    }

    strncpy(output_buffer, (const char *)morse_code_buffer, final_len);
    output_buffer[final_len] = '\0';

    restore_interrupts_from_disabled(ints);

    printf("\n");
  }
}