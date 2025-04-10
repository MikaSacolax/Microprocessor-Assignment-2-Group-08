#include "morse_utils.h"
#include <ctype.h>
#include <hardware/sync.h>
#include <hardware/timer.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "display_utils.h"

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

char word_list[WORD_LIST_COUNT][30] = {
    "SKETCH",      "MOUTH",       "INDEX",        "SHOUT",
    "FADE",        "INCIDENT",    "EXPERIENCE",   "RETIREMENT",
    "PENETRATE",   "PRODUCER",    "UNCERTAINTY",  "POLICY",
    "ASSAULT",     "MUSHROOM",    "EXTENSION",    "EGG",
    "STRANGE",     "LOOTING",     "ORGAN",        "GUITAR",
    "DIAGRAM",     "SERVE",       "REQUIREMENT",  "OFFER",
    "SURVIVAL",    "NONSENSE",    "CLEAN",        "COLORFUL",
    "OPERATIONAL", "HIGHWAY",     "TUBE",         "FRESH",
    "LIFT",        "MAIN",        "AISLE",        "OPINION",
    "VICTORY",     "KEEP",        "SEE",          "EXPERIENCED",
    "OVEREAT",     "EXPORT",      "SPOT",         "PAYMENT",
    "PARTICLE",    "POTENTIAL",   "SATISFIED",    "TRICK",
    "POSSESSION",  "STUN",        "PRESTIGE",     "OPINION",
    "HARASS",      "HOUSEWIFE",   "REGISTRATION", "BOTTOM",
    "REST",        "FINE",        "COVERAGE",     "SEASONAL",
    "ASTONISHING", "SEA",         "RITUAL",       "UNDERSTANDING",
    "HABIT",       "BEAR",        "SWITCH",       "GRANDFATHER",
    "KNOT",        "BOAT",        "MONK",         "TIPTOE",
    "DISGRACE",    "FREIGHT",     "UNLIKE",       "PUDDING",
    "NATIONALIST", "FLOOR",       "ASSAULT",      "INDOOR",
    "DISCIPLINE",  "LOUD",        "LEGISLATION",  "SKATE",
    "DREAM",       "LUNCH",       "LINEN",        "WEST",
    "TRAINING",    "TRAP",        "TOTAL",        "KNOW",
    "CRUELTY",     "ELECTRONICS", "VIEW",         "FACTOR",
    "PERFORATE",   "EXTORT",      "INTEGRITY",    "BRAINSTORM"};

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

const char *rand_challenge(GameContext *context) {
  static char
      char_buffer[2]; // static so we don't have to malloc or pass in a buffer

  if (context->current_level_index < 2) { // character for level 1 and 2
    int num = rand() % 36;

    if (num <= 9) {
      char_buffer[0] = '0' + num;
      char_buffer[1] = '\0';
      return char_buffer;
    } else {
      char_buffer[0] = 'A' + (num - 10);
      char_buffer[1] = '\0';
    }
  } else {
    int num = rand() % WORD_LIST_COUNT;
    return word_list[num];
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
  output_buffer[0] = '\0';

  while (true) {
    bool sequence_has_completed = false;
    char char_to_print = '\0';
    uint32_t ints = save_and_disable_interrupts();
    if (sequence_complete_flag) {
      sequence_has_completed = true;
      sequence_complete_flag = 0;
    }
    if (new_char_flag) {
      new_char_flag = 0;

      uint32_t asm_current_index = current;
      if (asm_current_index == 1 && morse_code_buffer[0] == ' ') {
        current = 0;
      } else if (asm_current_index > 0) {
        uint32_t added_char_index_in_asm_buf = asm_current_index - 1;
        if (added_char_index_in_asm_buf < MORSE_BUFFER_SIZE) {
          char_to_print = (char)morse_code_buffer[added_char_index_in_asm_buf];
        }
      }
    }

    restore_interrupts_from_disabled(ints);

    if (char_to_print != '\0') {
      printf("%c", char_to_print);
    }

    if (sequence_has_completed) {
      break;
    }
    busy_wait_us(500);
  }
  uint32_t final_asm_len = 0;
  uint32_t ints = save_and_disable_interrupts();
  final_asm_len = strlen((const char *)morse_code_buffer);
  restore_interrupts_from_disabled(ints);
  size_t copy_len = final_asm_len;
  if (copy_len >= buffer_size) {
    copy_len = buffer_size - 1;
  }

  ints = save_and_disable_interrupts();
  strncpy(output_buffer, (const char *)morse_code_buffer, copy_len);
  restore_interrupts_from_disabled(ints);
  output_buffer[copy_len] = '\0';
}