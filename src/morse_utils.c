#include "morse_utils.h"
#include "game_logic.h"
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
  if (!code)
    return '?';
  for (int i = 0; i < MORSE_TABLE_SIZE; i++) {
    if (strcmp(morseTable[i].morse, code) == 0) {
      return morseTable[i].character;
    }
  }
  return '?';
}

char *word_to_morse(const char *word, char *output_buffer, size_t buffer_size) {
  if (!word || !output_buffer || buffer_size == 0) {
    return NULL;
  }

  output_buffer[0] = '\0';
  size_t current_len = 0;
  bool first_char = true;

  for (size_t i = 0; word[i] != '\0'; ++i) {
    const char *morse_char = to_morse(word[i]);
    if (!morse_char || morse_char[0] == '\0')
      continue; // skip unknown characters

    size_t morse_char_len = strlen(morse_char);
    size_t space_len =
        first_char ? 0 : 1; // space needed before non-first chars

    // buffer length check before adding
    if (current_len + space_len + morse_char_len + 1 > buffer_size) {
      fprintf(stderr, "Error: word_to_morse buffer overflow.\n");
      output_buffer[current_len] = '\0';
      return output_buffer;
    }

    if (!first_char) {
      strcat(output_buffer, " ");
      current_len++;
    }

    strcat(output_buffer, morse_char);
    current_len += morse_char_len;
    first_char = false;
  }

  return output_buffer;
}

// decodes a space separated morse string into characters
void decode_morse_word(const char *morse_input, char *output_buffer,
                       size_t buffer_size) {
  if (!morse_input || !output_buffer || buffer_size == 0) {
    if (output_buffer && buffer_size > 0)
      output_buffer[0] = '\0';
    return;
  }

  output_buffer[0] = '\0';
  size_t output_len = 0;

  // need a mutable copy for strtok
  char morse_copy[MORSE_BUFFER_SIZE];
  strncpy(morse_copy, morse_input, MORSE_BUFFER_SIZE - 1);
  morse_copy[MORSE_BUFFER_SIZE - 1] = '\0';

  char *morse_char = strtok(morse_copy, " ");

  while (morse_char != NULL) {
    char decoded_c = from_morse(morse_char);

    output_buffer[output_len++] = decoded_c;
    output_buffer[output_len] = '\0';

    morse_char = strtok(NULL, " "); // get next character
  }
}

const char *rand_challenge(GameContext *context) {
  static char char_buffer[2];

  if (context->current_level_index < 2) {
    int num = rand() % MORSE_TABLE_SIZE;

    char_buffer[0] = morseTable[num].character;
    char_buffer[1] = '\0';
    return char_buffer;

  } else {
    int num = rand() % WORD_LIST_COUNT;
    return word_list[num];
  }
}
