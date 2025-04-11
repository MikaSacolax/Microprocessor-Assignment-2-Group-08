#ifndef MORSE_UTILS_H
#define MORSE_UTILS_H

#include <stddef.h>
#include <stdint.h>

#define WORD_LIST_COUNT 100

typedef struct GameContext GameContext;

/**
 * @brief Structure to hold a character and morse representation for easy
 * lookups.
 *
 */
typedef struct {
  char character;
  const char *morse;
} MorseMap;

// better than #define for debugging
// can be used for array size as opposed to standard types
enum { MORSE_TABLE_SIZE = 36 };

// conversion funcs

/**
 * @brief Converts a character to it's Morse equivalent.
 *
 * @param c
 * @return const char*
 */
const char *to_morse(char c);

/**
 * @brief Converts Morse for a singular character to a character.
 *
 * @param morse
 * @return char
 */
char from_morse(const char *morse);

/**
 * @brief Converts as string of characters to it's Morse equivalent.
 *
 * @param word
 * @param output_buffer
 * @param buffer_size
 * @return char*
 */
char *word_to_morse(const char *word, char *output_buffer, size_t buffer_size);

/**
 * @brief Converts Morse to a string of characters.
 *
 * @param morse_input
 * @param output_buffer
 * @param buffer_size
 */
void decode_morse_word(const char *morse_input, char *output_buffer,
                       size_t buffer_size);

// random char generation
/**
 * @brief Generates a random character or word depending on level.
 *
 * @param context
 * @return const char*
 */
const char *rand_challenge(GameContext *context);

/**
 * @brief calculates the ideal time in milliseconds to complete a level
 *        assumes 0.1s per dot, 0.3s per dash, 1s per space, 2s for sequence end
 *
 * @param morse_string
 * @return uint32_t ideal time in millisecond
 */
uint32_t calculate_ideal_time_ms(const char *morse_string);

#endif
