#include "game_logic.h"
#include "display_utils.h"
#include "morse_utils.h"
#include <hardware/sync.h>
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

void setup_level(GameContext *context, int level_index) {
  context->current_level_index = level_index;
  context->current_config = level_configs[level_index];
  context->current_state = GAME_STATE_PRESENT_CHALLENGE;
  generate_challenge(context);
}

void generate_challenge(GameContext *context) {
  context->target_challenge = rand_challenge(context);
  word_to_morse(context->target_challenge, context->target_morse_buffer,
                MORSE_BUFFER_SIZE);
  context->target_morse = context->target_morse_buffer;
}

void display_challenge(const GameContext *context) {
  clear_screen();
  printf(
      "============================================================== Level %d "
      "==============================================================\n\n",
      context->current_config.level_number);

  if (context->current_level_index < 2) {
    printf("\t\t\t\t\t\t\tTarget Character: %s\n", context->target_challenge);
  } else {
    printf("\t\t\t\t\t\t\tTarget Word:      %s\n", context->target_challenge);
  }

  if (context->current_config.show_morse_hint) {
    printf("\t\t\t\t\t\t\tMorse Code Hint:   %s\n", context->target_morse);
  }

  printf("\n\t\t\t\t\t\t\tYour Input Morse:  []\n");
  printf("\t\t\t\t\t\t\tDecodes To:        []\n");

  printf("\t\t\t\t\t\t\tWaiting for your input...\n");
  centre_display();
}

void display_player_input(const GameContext *context) {
  uint32_t current_len;
  char decoded_input_buffer[MORSE_BUFFER_SIZE];
  decoded_input_buffer[0] = '\0';

  uint32_t ints = save_and_disable_interrupts();
  current_len = current;
  char safe_morse_buffer[MORSE_BUFFER_SIZE];
  strncpy(safe_morse_buffer, (const char *)morse_code_buffer, current_len);
  safe_morse_buffer[current_len] = '\0';
  restore_interrupts_from_disabled(ints);

  if (current_len >= MORSE_BUFFER_SIZE) {
    current_len = MORSE_BUFFER_SIZE - 1;
    decoded_input_buffer[MORSE_BUFFER_SIZE - 1] = '\0';
    printf("\t\t\t\t\t\t\tWarning - input buffer overflow.\n");
  } else {
    if (current_len > 0) {
      decode_morse_word(safe_morse_buffer, decoded_input_buffer,
                        MORSE_BUFFER_SIZE);
    } else {
      strcpy(decoded_input_buffer, " ");
    }
  }

  // make it look non-scrolling
  clear_screen();
  printf(
      "============================================================== Level %d "
      "==============================================================\n\n",
      context->current_config.level_number);

  if (context->current_level_index < 2) {
    printf("\t\t\t\t\t\t\tTarget Character: %s\n", context->target_challenge);
  } else {
    printf("\t\t\t\t\t\t\tTarget Word:      %s\n", context->target_challenge);
  }

  if (context->current_config.show_morse_hint) {
    printf("\t\t\t\t\t\t\tMorse Code Hint:   %s\n", context->target_morse);
  }

  printf("\n\t\t\t\t\t\t\tYour Input Morse:  [");

  printf("%s", safe_morse_buffer);
  printf("]\n");

  printf("\t\t\t\t\t\t\tDecodes To:       [%s]\n", decoded_input_buffer);
  printf("\n\t\t\t\t\t\t\tInputting...\n");
  centre_display();
}

// use after sequence complete flag is set (since the buffer will be null
// terminated)
bool check_answer(GameContext *context) {
  char final_input_morse[MORSE_BUFFER_SIZE];
  uint32_t ints = save_and_disable_interrupts();
  strncpy(final_input_morse, (const char *)morse_code_buffer,
          MORSE_BUFFER_SIZE - 1);
  final_input_morse[MORSE_BUFFER_SIZE - 1] = '\0';
  restore_interrupts_from_disabled(ints);

  bool is_correct = (strcmp(final_input_morse, context->target_morse) == 0);
  context->last_answer_correct = is_correct;

  decode_morse_word(final_input_morse, context->last_input_decoded,
                    MORSE_BUFFER_SIZE);

  return is_correct;
}

void display_result(const GameContext *context) {
  clear_screen();
  printf("============================================================== Level "
         "%d Result "
         "==============================================================\n\n",
         context->current_config.level_number);

  if (context->current_level_index < 2) {
    printf("\t\t\t\t\t\t\tTarget Character: %s (%s)\n",
           context->target_challenge, context->target_morse);
  } else {
    printf("\t\t\t\t\t\t\tTarget Word:      %s (%s)\n",
           context->target_challenge, context->target_morse);
  }

  char final_input_morse[MORSE_BUFFER_SIZE];
  uint32_t ints = save_and_disable_interrupts();
  strncpy(final_input_morse, (const char *)morse_code_buffer,
          MORSE_BUFFER_SIZE - 1);
  final_input_morse[MORSE_BUFFER_SIZE - 1] = '\0';
  restore_interrupts_from_disabled(ints);

  printf("\t\t\t\t\t\t\tYour Input:       %s -> %s\n", final_input_morse,
         context->last_input_decoded);

  printf("\t\t\t\t\t\t\tResult: %s\n",
         context->last_answer_correct ? "CORRECT!" : "INCORRECT!");

  printf("\n\t\t\t\t\t\t\tNext challenge in %d seconds...\n", 3);
  centre_display();
  busy_wait_ms(3000);
}