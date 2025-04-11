#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#include "asm_interface.h"
#include "ws2812.h"
#include <stdbool.h>
#include <stdint.h>

#define ROUNDS_PER_LEVEL 5
#define RESULT_DISPLAY_MS 3000
#define MAX_LIVES 3

/**
 * @brief Struct to allocate whether each level has a hint or not.
 * 
 */
typedef struct {

  uint32_t level_number;
  bool show_morse_hint;

} LevelConfig;

/**
 * @brief List of each level and there hint state.
 * 
 */
extern const LevelConfig level_configs[];
extern const uint32_t NUM_LEVELS;
extern const uint32_t MAX_LEVEL_INDEX;

/**
 * @brief Enum of all possible game states.
 * 
 */
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

/**
 * @brief Struct containing all info on the current game state and level state.
 * 
 */
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

/**
 * @brief Enum of possible colours for the led.
 * 
 */
typedef enum { LED_OFF, LED_RED, LED_ORANGE, LED_GREEN, LED_BLUE } LedColor;

/**
 * @brief Set the led color by lives object.
 * 
 * @param lives 
 */
void set_led_color_by_lives(int lives);


// game setup functions


/**
 * @brief Initialize the inital game_context such as game state and lives.
 * 
 * @param context 
 */
void initialize_game_context(GameContext *context);

/**
 * @brief Set the up level object and all other depends in game_context
 * 
 * @param context 
 * @param level_index 
 */
void setup_level(GameContext *context, int level_index);


// game flow logic functions


/**
 * @brief Calls all the required functions to generate each challenge.
 * 
 * @param context 
 */
void generate_challenge(GameContext *context);

/**
 * @brief Check the player's answer against the target.
 * 
 * @param context 
 * @return true 
 * @return false 
 */
bool check_answer(GameContext *context);


// state handlers called from main


/**
 * @brief Calls setup_level and changes the game state.
 * 
 * @param context 
 */
void handle_start_level(GameContext *context);

/**
 * @brief Calls update_and_display_player_input and changes the game state.
 * 
 * @param context 
 */
void handle_present_challenge(GameContext *context);

/**
 * @brief Handles waiting for an input from the Arm Assembly code and changes the game state.
 * 
 * @param context 
 */
void handle_waiting_input(GameContext *context);

/**
 * @brief Calls check_answer and changes the game state.
 * 
 * @param context 
 */
void handle_check_answer(GameContext *context);

/**
 * @brief Handles the functions required to print the results, generate the next challenge and changes the game state.
 * 
 * @param context 
 */
void handle_show_result(GameContext *context);

/**
 * @brief Prints the level complete banner and changes the game state.
 * 
 * @param context 
 */
void handle_level_complete(GameContext *context);

/**
 * @brief Prints the game won banner and changes the game state.
 * 
 * @param context 
 */
void handle_game_complete(GameContext *context);

/**
 * @brief Print the game over banner and changes the game state.
 * 
 * @param context 
 */
void handle_game_over(GameContext *context);

// input/display coordination
/**
 * @brief Handles updating the display when an input happens.
 * 
 * @param context 
 * @param is_waiting 
 */
void update_and_display_player_input(GameContext *context, bool is_waiting);

#endif