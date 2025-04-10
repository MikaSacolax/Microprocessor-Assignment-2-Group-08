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

void setup_level(GameContext *context, int level_index) {
  context->current_level_index = level_index;
  context->current_config = level_configs[level_index];
  context->current_state = GAME_STATE_PRESENT_CHALLENGE;
  generate_challenge(context);
}

void generate_challenge(GameContext *context) {
  context->target_char = rand_char(context);
  context->target_morse = to_morse(context->target_char);
  while (strlen(context->target_morse) == 0) {
    context->target_char = rand_char(context);
    context->target_morse = to_morse(context->target_char);
  }
}

void display_challenge(const GameContext *context) {
  clear_screen();
  printf(
      "============================================================== Level %d "
      "==============================================================\n\n",
      context->current_config.level_number);
  
  if (context->current_level_index == 1 || context->current_level_index == 2)
  {
    printf("\t\t\t\t\t\t\tTarget Character:  %s\n", context->target_char);
  }
  else {
    printf("\t\t\t\t\t\t\tTarget Word:  %s\n", context->target_char);
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
  char decoded_char = '?';

  uint32_t ints = save_and_disable_interrupts();
  current_len = current;
  restore_interrupts_from_disabled(ints);

  // safe read
  if (current_len >= MORSE_BUFFER_SIZE) {
    current_len = MORSE_BUFFER_SIZE - 1;
    decoded_char = '?';

    printf("\t\t\t\t\t\t\tWarning - overflow.\n");
  } else {
    if (current_len > 0) {
      decoded_char =
          from_morse_len((const char *)morse_code_buffer, current_len);
    } else {
      decoded_char = ' ';
    }
  }

  // make it look non-scrolling
  clear_screen();
  printf(
      "============================================================== Level %d "
      "==============================================================\n\n",
      context->current_config.level_number);
  
  if (context->current_level_index == 1 || context->current_level_index == 2){
    printf("\t\t\t\t\t\t\tTarget Character:  %s\n", context->target_char);
  }
  else {
    printf("\t\t\t\t\t\t\tTarget Word:  %s\n", context->target_char);
  }
  
  if (context->current_config.show_morse_hint) {
    printf("\t\t\t\t\t\t\tMorse Code Hint:   %s\n", context->target_morse);
  }

  printf("\n\t\t\t\t\t\t\tYour Input Morse:  [");

  for (uint32_t i = 0; i < current_len; ++i) {
    putchar(morse_code_buffer[i]);
  }
  printf("]\n");

  printf("\t\t\t\t\t\t\tDecodes To:        [%s]\n", decoded_char);
  printf("\n\t\t\t\t\t\t\tInputting...\n");
  centre_display();
}

// use after sequence complete flag is set (since the buffer will be null
// terminated)
bool check_answer(GameContext *context) {
  bool is_correct =
      (strcmp((const char *)morse_code_buffer, context->target_morse) == 0);

  context->last_answer_correct = is_correct;
  context->last_input_decoded = from_morse((const char *)morse_code_buffer);

  return is_correct;
}

void display_result(const GameContext *context) {
  clear_screen();
  printf("============================================================== Level "
         "%d Result "
         "==============================================================\n\n",
         context->current_config.level_number);

  
  if (context->current_level_index == 1 || context->current_level_index == 2) {
    printf("\t\t\t\t\t\t\tTarget Character: %s (%s)\n", context->target_char,
            context->target_morse);
  }
  else {
    printf("\t\t\t\t\t\t\tTarget Word: %s (%s)\n", context->target_char,
            context->target_morse);
  }

  printf("\t\t\t\t\t\t\tYour Input:       %s -> %s\n",
         (const char *)morse_code_buffer, context->last_input_decoded);

  printf("\t\t\t\t\t\t\tResult: %s\n",
         context->last_answer_correct ? "CORRECT!" : "INCORRECT!");

  printf("\n\t\t\t\t\t\t\tNext challenge in %d seconds...\n", 3);
  centre_display();
  busy_wait_ms(3000);
}