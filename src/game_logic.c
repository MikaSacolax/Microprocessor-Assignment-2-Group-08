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