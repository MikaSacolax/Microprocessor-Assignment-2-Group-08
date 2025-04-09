#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {

  uint32_t level_number;
  bool show_morse_hint;

} LevelConfig;

extern const LevelConfig level_configs[];

typedef enum {
  GAME_STATE_WAITING_MENU_INPUT,
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
void display_player_input(const GameContext *context);
bool check_answer(GameContext *context);
void display_result(const GameContext *context);

#endif