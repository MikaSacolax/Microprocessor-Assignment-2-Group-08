#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {

  uint32_t level_number;
  bool show_morse_hint;

} LevelConfig;

const LevelConfig level_configs[] = {
    {.level_number = 1, .show_morse_hint = true},
    {.level_number = 2, .show_morse_hint = false}};

const int NUM_LEVELS = sizeof(level_configs) / sizeof(level_configs[0]);

typedef enum {
  GAME_STATE_START_LEVEL,
  GAME_STATE_PRESENT_CHALLENGE,
  GAME_STATE_WAITING_INPUT,
  GAME_STATE_CHECK_ANSWER,
  GAME_STATE_SHOW_RESULT
} GameState;

typedef struct {
  GameState current_state;
  int current_level_index;
  LevelConfig current_config;
  char target_char;
  const char *target_morse;
  char last_input_decoded;
  bool last_answer_correct;
} GameContext;

void setup_level(GameContext *context, int level_index);
void generate_challenge(GameContext *context);
void display_challenge(const GameContext *context);

#endif