/** @file */
#include "game_logic.h"
#include "asm_interface.h"
#include "display_utils.h"
#include "morse_utils.h"
#include "ws2812.h"
#include <hardware/sync.h>
#include <hardware/timer.h>
#include <hardware/watchdog.h>
#include <pico/time.h>
#include <stdio.h>
#include <string.h>

const LevelConfig level_configs[] = {
    {.level_number = 1, .show_morse_hint = true},
    {.level_number = 2, .show_morse_hint = false},
    {.level_number = 3, .show_morse_hint = true},
    {.level_number = 4, .show_morse_hint = false}};

const uint32_t NUM_LEVELS = sizeof(level_configs) / sizeof(level_configs[0]);
const uint32_t MAX_LEVEL_INDEX = NUM_LEVELS - 1;

void initialize_game_context(GameContext *context) {
  memset(context, 0, sizeof(GameContext));
  context->current_state = GAME_STATE_MAIN_MENU;
  context->current_level_index = 0;
  context->current_lives = MAX_LIVES;
  context->ideal_time_ms = 0;
  context->last_score_percentage = 0;
  put_pixel(BLUE);
}

void set_led_color_by_lives(int lives) {
  switch (lives) {
  case 3:
    put_pixel(GREEN);
    break;
  case 2:
    put_pixel(YELLOW);
    break;
  case 1:
    put_pixel(ORANGE);
    break;
  case 0:
    put_pixel(RED);
    break;
  default:
    put_pixel(BLUE);
    break;
  }
}

void setup_level(GameContext *context, int level_index) {
  context->current_level_index = level_index;
  context->current_config = level_configs[level_index];
  context->current_state = GAME_STATE_PRESENT_CHALLENGE;

  context->challenges_attempted_this_level = 0;
  context->correct_challenges_this_level = 0;
  set_led_color_by_lives(context->current_lives);
}

void generate_challenge(GameContext *context) {
  context->target_challenge = rand_challenge(context);
  word_to_morse(context->target_challenge, context->target_morse_buffer,
                MORSE_BUFFER_SIZE);
  context->target_morse = context->target_morse_buffer;
  context->last_input_decoded[0] = '\0';
  context->ideal_time_ms = calculate_ideal_time_ms(context->target_morse);
  set_led_color_by_lives(context->current_lives);
}

// check the player's answer against the target
bool check_answer(GameContext *context) {
  char final_input_morse[MORSE_BUFFER_SIZE];
  asm_interface_get_morse_input(final_input_morse, MORSE_BUFFER_SIZE);

  bool is_correct = (strcmp(final_input_morse, context->target_morse) == 0);
  context->last_answer_correct = is_correct;

  decode_morse_word(final_input_morse, context->last_input_decoded,
                    MORSE_BUFFER_SIZE);

  context->challenges_attempted_this_level++;
  if (is_correct) {
    context->correct_challenges_this_level++;
  } else {
    context->current_lives--;
    context->correct_challenges_this_level = 0;
  }

  return is_correct;
}

void update_and_display_player_input(GameContext *context, bool is_waiting) {
  char current_input_morse[MORSE_BUFFER_SIZE];
  char decoded_input_buffer[MORSE_BUFFER_SIZE];

  size_t current_len =
      asm_interface_get_morse_input(current_input_morse, MORSE_BUFFER_SIZE);

  // protect against buffer overflow
  if (current_len >= MORSE_BUFFER_SIZE - 1) {
    printf("Warning - input buffer full.\n");
    decoded_input_buffer[0] = '?';
    decoded_input_buffer[1] = '\0';
  } else if (current_len > 0) {
    decode_morse_word(current_input_morse, decoded_input_buffer,
                      MORSE_BUFFER_SIZE);
  } else {
    // if buffer is empty
    decoded_input_buffer[0] = '\0';
  }

  display_challenge_screen(context, current_input_morse, decoded_input_buffer,
                           is_waiting);
}

/// state handlers ////

void handle_start_level(GameContext *context) {
  setup_level(context, context->current_level_index);
  generate_challenge(context);
  context->current_state = GAME_STATE_PRESENT_CHALLENGE;
}

void handle_present_challenge(GameContext *context) {
  asm_interface_flush_state();
  update_and_display_player_input(context,
                                  true); // pass true for "waiting for input"
  context->current_state = GAME_STATE_WAITING_INPUT;
  context->challenge_start_time_us = time_us_64();
}

void handle_waiting_input(GameContext *context) {
  bool state_changed = false;

  if (asm_interface_has_new_char()) {
    watchdog_update();
    asm_interface_clear_new_char_flag();

    bool initial_space_cleared = asm_interface_check_and_clear_initial_space();

    if (!initial_space_cleared) {
      update_and_display_player_input(context,
                                      false); // false for "inputting.."
    }
  }

  if (asm_interface_is_sequence_complete()) {
    watchdog_update();
    asm_interface_clear_sequence_complete_flag();
    context->current_state = GAME_STATE_CHECK_ANSWER;
    state_changed = true;
  }

  if (!state_changed) {
    busy_wait_us(500);
  }
}

void handle_check_answer(GameContext *context) {
  watchdog_update();
  check_answer(context);
  set_led_color_by_lives(context->current_lives);
  context->current_state = GAME_STATE_SHOW_RESULT;
  uint64_t challenge_end_time_us = time_us_64();
  context->last_challenge_duration_ms =
      (uint32_t)((challenge_end_time_us - context->challenge_start_time_us) /
                 1000);

  if (context->last_challenge_duration_ms > 0) {
    // (ideal / actual) * 100
    // Use 64-bits to stop overflow before division
    uint64_t ideal_x_100 = (uint64_t)context->ideal_time_ms * 100;
    context->last_score_percentage =
        (uint32_t)(ideal_x_100 / context->last_challenge_duration_ms);
  } else {
    context->last_score_percentage = 0; // avoid div by zero
  }
}

void handle_show_result(GameContext *context) {
  watchdog_update();
  char final_input_morse[MORSE_BUFFER_SIZE];
  asm_interface_get_morse_input(final_input_morse, MORSE_BUFFER_SIZE);

  display_result_screen(context, final_input_morse, RESULT_DISPLAY_MS / 1000);
  busy_wait_ms(RESULT_DISPLAY_MS);

  watchdog_update();

  if (context->current_lives <= 0) {
    context->current_state = GAME_STATE_GAME_OVER;
  } else if (context->correct_challenges_this_level >= ROUNDS_PER_LEVEL) {
    context->current_state = GAME_STATE_LEVEL_COMPLETE;
  } else {
    generate_challenge(context);
    context->current_state = GAME_STATE_PRESENT_CHALLENGE;
  }
}

void handle_level_complete(GameContext *context) {
  watchdog_update();
  clear_screen();
  // clang-format off
  printf("=======================================================================\n");
  printf("                         Level %d Complete!\n", context->current_config.level_number);
  printf("                    You finished with %d lives left.\n", context->current_lives);
  printf("=======================================================================\n\n");
  // clang-format on

  if (context->current_level_index < MAX_LEVEL_INDEX) {
    printf("Advancing to next level...\n");
    busy_wait_ms(2500);
    watchdog_update();
    context->current_level_index++;
    context->current_state = GAME_STATE_START_LEVEL;
  } else {
    context->current_state = GAME_STATE_GAME_COMPLETE;
  }
  put_pixel(BLUE);
  fflush(stdout);
}

void handle_game_over(GameContext *context) {
  put_pixel(RED);
  watchdog_update();
  clear_screen();
  // clang-format off
  printf("=======================================================================\n");
  printf("                              GAME OVER\n");
  printf("                       You ran out of lives!\n");
  printf("=======================================================================\n\n");
  printf("                      Returning to Main Menu...\n");
  // clang-format on
  fflush(stdout);
  busy_wait_ms(4000);
  watchdog_update();
  context->current_state = GAME_STATE_MAIN_MENU;
  initialize_game_context(context);
}

void handle_game_complete(GameContext *context) {
  watchdog_update();
  clear_screen();
  put_pixel(BLUE);

  // clang-format off
  printf("***********************************************************************\n");
  printf("                          CONGRATULATIONS!\n");
  printf("                       You have won the game!\n");
  printf("***********************************************************************\n\n");
  printf("                     Returning to Main Menu...\n");
  // clang-format on
  fflush(stdout);
  busy_wait_ms(4000);
  watchdog_update();
  context->current_state = GAME_STATE_MAIN_MENU;
  initialize_game_context(context);
}