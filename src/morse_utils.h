#ifndef MORSE_UTILS_H
#define MORSE_UTILS_H

#include <stddef.h>

#define WORD_LIST_COUNT 100

typedef struct GameContext GameContext;

// structure to hold a character and morse representation for easy lookups
typedef struct {
  char character;
  const char *morse;
} MorseMap;

// better than #define for debugging
// can be used for array size as opposed to standard types
enum { MORSE_TABLE_SIZE = 36 };

// conversion funcs
const char *to_morse(char c);
char from_morse(const char *morse);

char *word_to_morse(const char *word, char *output_buffer, size_t buffer_size);
void decode_morse_word(const char *morse_input, char *output_buffer,
                       size_t buffer_size);

// random char generation
const char *rand_challenge(GameContext *context);

#endif
