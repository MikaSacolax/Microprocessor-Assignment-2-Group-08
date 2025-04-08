#ifndef MORSE_UTILS_H
#define MORSE_UTILS_H

#include <stdint.h>

// structure to hold a character and morse representation for easy lookups
typedef struct {
  char character;
  const char *morse;
} MorseMap;

// better than #define for debugging
const uint32_t MORSE_TABLE_SIZE = 36;

// conversion funcs
const char *to_morse(char c);
char from_morse(const char *morse);

// random char generation
char rand_char();

#endif
