#include "game_logic.h"
#include "src/display_utils.h"
#include "src/morse_utils.h"
#include <hardware/sync.h>
#include <stdio.h>
#include <string.h>

void setup_level(GameContext *context, int level_index) {
  context->current_level_index = level_index;
  context->current_config = level_configs[level_index];
  context->current_state = GAME_STATE_PRESENT_CHALLENGE;
  printf("\n--- Starting Level %d ---\n", context->current_config.level_number);
  generate_challenge(context);
}

void generate_challenge(GameContext *context) {
  context->target_char = rand_char();
  context->target_morse = to_morse(context->target_char);
  while (strlen(context->target_morse) == 0) {
    context->target_char = rand_char();
    context->target_morse = to_morse(context->target_char);
  }
  printf("New challenge generated.\n"); // for debug purposes
}

void display_challenge(const GameContext *context) {
  clear_screen();
  printf("===== Level %d =====\n\n", context->current_config.level_number);
  printf("Target Character:  %c\n", context->target_char);

  if (context->current_config.show_morse_hint) {
    printf("Morse Code Hint:   %s\n", context->target_morse);
  }

  printf("Waiting for your input\n-> ");
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

    printf("Warning - overflow.\n");
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
  printf("===== Level %d =====\n\n", context->current_config.level_number);
  printf("Target Character:  %c\n", context->target_char);
  if (context->current_config.show_morse_hint) {
    printf("Morse Code Hint:   %s\n", context->target_morse);
  }

  printf("\nYour Input Morse:  [");

  for (uint32_t i = 0; i < current_len; ++i) {
    putchar(morse_code_buffer[i]);
  }
  printf("]\n");

  printf("Decodes To:        [%c]\n", decoded_char);
  printf("\nInputting...\n");
}

// use after sequence complete flag is set (since the buffer will be null
// terminated)
bool check_answer(GameContext *ctx) {
  bool is_correct =
      (strcmp((const char *)morse_code_buffer, ctx->target_morse) == 0);

  ctx->last_answer_correct = is_correct;
  ctx->last_input_decoded = from_morse((const char *)morse_code_buffer);

  return is_correct;
}