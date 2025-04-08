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
const char *to_morse(char c);
char from_morse(const char *morse);

#endif
