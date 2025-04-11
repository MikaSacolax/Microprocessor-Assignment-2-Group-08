#include "asm_interface.h"
#include <hardware/sync.h>
#include <string.h>

#define MORSE_BUFFER_SIZE 1024

extern volatile unsigned char morse_code_buffer[];
extern volatile uint32_t current;
extern volatile uint32_t sequence_complete_flag;
extern volatile uint32_t new_char_flag;

bool asm_interface_is_sequence_complete() {
  uint32_t ints = save_and_disable_interrupts();
  bool complete = (sequence_complete_flag != 0);
  restore_interrupts_from_disabled(ints);
  return complete;
}

bool asm_interface_has_new_char() {
  uint32_t ints = save_and_disable_interrupts();
  bool has_new = (new_char_flag != 0);
  restore_interrupts_from_disabled(ints);
  return has_new;
}

void asm_interface_clear_sequence_complete_flag() {
  uint32_t ints = save_and_disable_interrupts();
  sequence_complete_flag = 0;
  restore_interrupts_from_disabled(ints);
}

void asm_interface_clear_new_char_flag() {
  uint32_t ints = save_and_disable_interrupts();
  new_char_flag = 0;
  restore_interrupts_from_disabled(ints);
}

void asm_interface_flush_state() {
  uint32_t ints = save_and_disable_interrupts();
  memset((void *)morse_code_buffer, 0, MORSE_BUFFER_SIZE);
  current = 0;
  sequence_complete_flag = 0;
  new_char_flag = 0;
  restore_interrupts_from_disabled(ints);
}

// returns size of morse
size_t asm_interface_get_morse_input(char *buffer, size_t buffer_len) {
  if (!buffer || buffer_len == 0) {
    return 0;
  }

  uint32_t ints = save_and_disable_interrupts();
  size_t len_to_copy = (size_t)current;

  // make sure we don't overflow the buffer
  if (len_to_copy >= buffer_len) {
    len_to_copy = buffer_len - 1;
  }

  // cast volatile away for standard libary functions like strncpy
  strncpy(buffer, (const char *)morse_code_buffer, len_to_copy);
  buffer[len_to_copy] = '\0';

  restore_interrupts_from_disabled(ints);
  return len_to_copy;
}

char asm_interface_get_last_char() {
  uint32_t ints = save_and_disable_interrupts();
  char last_char = '\0';
  uint32_t current_index = current;

  if (current_index > 0 && current_index <= MORSE_BUFFER_SIZE) {
    last_char = (char)morse_code_buffer[current_index - 1];
  }

  restore_interrupts_from_disabled(ints);
  return last_char;
}

// returns true if cleared a space
bool asm_interface_check_and_clear_initial_space() {
  uint32_t ints = save_and_disable_interrupts();
  bool cleared = false;
  if (current == 1 && morse_code_buffer[0] == ' ') {
    current = 0;
    morse_code_buffer[0] = '\0';
    new_char_flag = 0;
    cleared = true;
  }
  restore_interrupts_from_disabled(ints);
  return cleared;
}