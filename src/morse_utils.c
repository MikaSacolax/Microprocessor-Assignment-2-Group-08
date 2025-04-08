#include "morse_utils.h"
#include <ctype.h>
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

const char *toMorse(char c) {
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

char fromMorse(const char *code) {
  for (int i = 0; i < MORSE_TABLE_SIZE; i++) {
    if (strcmp(morseTable[i].morse, code) == 0) {
      return morseTable[i].character;
    }
  }
  return '?';
}