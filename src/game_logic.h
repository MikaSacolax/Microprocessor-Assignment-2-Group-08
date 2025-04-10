#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#include "asm_interface.h"
#include <stdbool.h>
#include <stdint.h>

#define ROUNDS_PER_LEVEL 5
#define RESULT_DISPLAY_MS 3000
#define MAX_LIVES 3

typedef struct {

  uint32_t level_number;
  bool show_morse_hint;

} LevelConfig;

extern const LevelConfig level_configs[];
extern const uint32_t NUM_LEVELS;
extern const uint32_t MAX_LEVEL_INDEX;

typedef enum {
  GAME_STATE_MAIN_MENU,
  GAME_STATE_START_LEVEL,
  GAME_STATE_PRESENT_CHALLENGE,
  GAME_STATE_WAITING_INPUT,
  GAME_STATE_CHECK_ANSWER,
  GAME_STATE_SHOW_RESULT,
  GAME_STATE_LEVEL_COMPLETE,
  GAME_STATE_GAME_COMPLETE,
  GAME_STATE_GAME_OVER
} GameState;

typedef struct GameContext {
  GameState current_state;
  int current_level_index;
  LevelConfig current_config;
  const char *target_challenge;
  char target_morse_buffer[MORSE_BUFFER_SIZE];
  const char *target_morse;
  char last_input_decoded[MORSE_BUFFER_SIZE];
  bool last_answer_correct;
  int challenges_attempted_this_level;
  int correct_challenges_this_level;
  int current_lives;
} GameContext;

typedef enum { LED_OFF, LED_RED, LED_ORANGE, LED_GREEN, LED_BLUE } LedColor;
// void set_rgb_led(LedColor color);

// game setup functions
void initialize_game_context(GameContext *context);
void setup_level(GameContext *context, int level_index);

// game flow logic functions
void generate_challenge(GameContext *context);
bool check_answer(GameContext *context);

// state handlers called from main
void handle_start_level(GameContext *context);
void handle_present_challenge(GameContext *context);
void handle_waiting_input(GameContext *context);
void handle_check_answer(GameContext *context);
void handle_show_result(GameContext *context);
void handle_level_complete(GameContext *context);
void handle_game_complete(GameContext *context);
void handle_game_over(GameContext *context);

// input/display coordination
void update_and_display_player_input(GameContext *context, bool is_waiting);

#endif