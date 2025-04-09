#include <hardware/sync.h>
#include <hardware/timer.h>
#include <hardware/watchdog.h> //  Watchdog header
#include <pico/stdio.h>
#include <pico/time.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "display_utils.h"
#include "game_logic.h"
#include "morse_utils.h"
#include "sdk_asm_helpers.h"

// Declare the external variables so the compiler knows they exist:
extern volatile unsigned char morse_code_buffer[];
extern volatile uint32_t current;
extern volatile uint32_t sequence_complete_flag;
extern volatile uint32_t new_char_flag;

const uint32_t NUM_LEVELS = 2;

// Declare the main assembly code entry point.
void main_asm();

// Function declarations
void main_menu(GameContext *context);

int main() {
  stdio_init_all(); // Initialise all basic IO
  sleep_ms(5000);   // Give time for the serial monitor to connect

  //  Enable the Watchdog Timer for 9 seconds
  watchdog_enable(9000, 1); // 9000 ms timeout, pause on debug = true

  main_asm(); // Call assembly setup if needed

  GameContext game_context;
  main_menu(&game_context);

  while (true) {
    switch (game_context.current_state) {

    case GAME_STATE_START_LEVEL:
      setup_level(&game_context, game_context.current_level_index);
      break;

    case GAME_STATE_PRESENT_CHALLENGE:
      display_challenge(&game_context);
      flush_asm_state();
      game_context.current_state = GAME_STATE_WAITING_INPUT;
      break;

    case GAME_STATE_WAITING_INPUT:
      if (new_char_flag) {
        uint32_t current_len = 0;
        bool skip_this_char = false;

        uint32_t ints = save_and_disable_interrupts();
        new_char_flag = 0;
        current_len = current;

        // Skip accidental leading space
        if (current_len == 1 && morse_code_buffer[0] == ' ') {
          current = 0;
          skip_this_char = true;

          uint32_t now = timer_hw->timelr;
          uint32_t new_alarm1_time = now + 2000000;
          timer_hw->alarm[1] = new_alarm1_time;
          timer_hw->intr = 0b10;
        }
        restore_interrupts_from_disabled(ints);

        if (!skip_this_char) {
          display_player_input(&game_context);
        }
      }

      if (sequence_complete_flag) {
        uint32_t ints = save_and_disable_interrupts();
        sequence_complete_flag = 0;
        restore_interrupts_from_disabled(ints);

        game_context.current_state = GAME_STATE_CHECK_ANSWER;
        break;
      }

      if (game_context.current_state == GAME_STATE_WAITING_INPUT) {
        busy_wait_us(500);
        watchdog_update(); // Refresh the watchdog timer during active waiting
      }
      break;

    case GAME_STATE_CHECK_ANSWER:
      check_answer(&game_context);
      game_context.current_state = GAME_STATE_SHOW_RESULT;
      break;

    case GAME_STATE_SHOW_RESULT:
      display_result(&game_context);

      if (game_context.last_answer_correct) {
        game_context.current_level_index =
            game_context.current_level_index == 0 ? 1 : 0;
        game_context.current_config =
            level_configs[game_context.current_level_index];
      }

      generate_challenge(&game_context);
      game_context.current_state = GAME_STATE_PRESENT_CHALLENGE;
      break;

    default:
      printf("Error - Unknown state.\n");
      game_context.current_state = GAME_STATE_START_LEVEL;
      game_context.current_level_index = 0;
      break;
    }
  }

  return 0;
}

void main_menu(GameContext *context) {
  print_main_menu();

  while (true) {
    flush_asm_state();
    size_t buffer_size = 6;
    char level_select_buffer[buffer_size];
    memset(level_select_buffer, '\0', buffer_size);

    printf("\n                                        Level input: ");

    get_morse_input_interactive(level_select_buffer, buffer_size);

    char decoded_char = from_morse(level_select_buffer);

    if (decoded_char == '1' || decoded_char == '2') {
      context->current_level_index = decoded_char - '1';

      context->current_state = GAME_STATE_START_LEVEL;
      printf("\n                                        Selected Level %d. "
             "Starting game!\n\n",
             context->current_level_index + 1);
      busy_wait_ms(1000);
      break;
    } else {
      printf("\n                                        Invalid level "
             "selection ('%s' -> '%c'). Try again:\n",
             level_select_buffer, decoded_char);
    }
  }
}