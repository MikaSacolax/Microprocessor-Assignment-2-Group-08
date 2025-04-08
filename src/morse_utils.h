#ifndef MORSE_UTILS_H
#define MORSE_UTILS_H

// structure to hold a character and morse representation for easy lookups
typedef struct {
  char character;
  const char *morse;
} MorseMap;

// better than #define for debugging
enum { MORSE_TABLE_SIZE = 36 };

// conversion funcs
const char *toMorse(char c);
char fromMorse(const char *morse);

#endif
