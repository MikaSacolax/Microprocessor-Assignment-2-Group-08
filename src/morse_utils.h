#ifndef MORSE_UTILS_H
#define MORSE_UTILS_H

#include "game_logic.h"
#include <stddef.h>
#include <stdint.h>

extern volatile unsigned char morse_code_buffer[];
extern volatile uint32_t current;
extern volatile uint32_t sequence_complete_flag;
extern volatile uint32_t new_char_flag;
const uint32_t WORD_LIST_COUNT = 20;

// structure to hold a character and morse representation for easy lookups
typedef struct {
  char character;
  const char *morse;
} MorseMap;

// better than #define for debugging
// can be used for array size as opposed to standard types
enum { MORSE_TABLE_SIZE = 36, MORSE_BUFFER_SIZE = 1024 };

// conversion funcs
const char *to_morse(char c);
char from_morse(const char *morse);

// random char generation
const char *rand_challenge(GameContext *context);

// flushes the morse buffer with null terminators
void flush_asm_state();

char from_morse_len(const char *morse_segment, size_t len);

void get_morse_input_interactive(char *output_buffer, size_t buffer_size);

#endif
