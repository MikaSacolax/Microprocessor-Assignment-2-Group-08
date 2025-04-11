/** @file */
#include "display_utils.h"
#include "game_logic.h"
#include "hardware/pio.h"
#include "morse_utils.h"
#include "ws2812.h"
#include "ws2812.pio.h"
#include <hardware/sync.h>
#include <hardware/timer.h>
#include <hardware/watchdog.h>
#include <pico/stdio.h>
#include <pico/time.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * @brief Function that handles button events and Morse code input buffer. Fully written in Arm Assembly.
 * 
 */
void main_asm();

// function declarations
/**
 * @brief Responsible for handling and calling all functions required for the start screen and level selection.
 * 
 * @param context 
 */
void main_menu(GameContext *context);

int main() {
  stdio_init_all(); // Initialise all basic IO
  srand(time_us_64());
  sleep_ms(5000); // time to allow serial monitor connection

  main_asm();

  PIO pio = pio0;
  uint sm = 0;
  uint offset = pio_add_program(pio, &ws2812_program);
#define WS2812_PIN_MAIN 28
#define IS_RGBW_MAIN true
  ws2812_program_init(pio, sm, offset, WS2812_PIN_MAIN, 800000, IS_RGBW_MAIN);

  // pico sdk docs say 8388 is the max
  watchdog_enable(8388, 1);

  GameContext game_context;
  initialize_game_context(&game_context);

  while (true) {
    switch (game_context.current_state) {

    case GAME_STATE_MAIN_MENU:
      main_menu(&game_context);
      break;

    case GAME_STATE_START_LEVEL:
      handle_start_level(&game_context);
      break;

    case GAME_STATE_PRESENT_CHALLENGE:
      handle_present_challenge(&game_context);
      break;

    case GAME_STATE_WAITING_INPUT:
      handle_waiting_input(&game_context);
      break;

    case GAME_STATE_CHECK_ANSWER:
      handle_check_answer(&game_context);
      break;

    case GAME_STATE_SHOW_RESULT:
      handle_show_result(&game_context);
      break;

    case GAME_STATE_LEVEL_COMPLETE:
      handle_level_complete(&game_context);
      break;

    case GAME_STATE_GAME_COMPLETE:
      handle_game_complete(&game_context);
      break;

    case GAME_STATE_GAME_OVER:
      handle_game_over(&game_context);
      break;

    default:
      printf("Error - Unknown state.\n");
      initialize_game_context(&game_context);
      busy_wait_ms(2000);
      break;
    }
  }

  return 0;
}

void main_menu(GameContext *context) {
  print_main_menu();

  while (context->current_state == GAME_STATE_MAIN_MENU) {
    printf("\nEnter Morse for Level (1-%d): ", NUM_LEVELS);
    fflush(stdout);
    asm_interface_flush_state();

    bool input_aborted = false;
    while (true) {
      bool sequence_complete = false;
      char last_char = '\0';

      if (asm_interface_has_new_char()) {
        watchdog_update();
        asm_interface_clear_new_char_flag();
        if (!asm_interface_check_and_clear_initial_space()) {
          last_char = asm_interface_get_last_char();
        }
      }
      if (asm_interface_is_sequence_complete()) {
        watchdog_update();
        asm_interface_clear_sequence_complete_flag();
        sequence_complete = true;
      }

      if (last_char != '\0') {
        printf("%c", last_char);
        fflush(stdout);
      }

      if (sequence_complete) {
        break;
      }

      busy_wait_us(1000);
    }

    char level_select_buffer[16];
    asm_interface_get_morse_input(level_select_buffer,
                                  sizeof(level_select_buffer));

    char decoded_char = from_morse(level_select_buffer);

    int selected_level_index = -1;
    if (decoded_char >= '1' && decoded_char < ('1' + NUM_LEVELS)) {
      selected_level_index = decoded_char - '1';
    }

    if (selected_level_index != -1) {
      printf("\nSelected Level %d ('%s'). Starting game!\n",
             selected_level_index + 1, level_select_buffer);
      watchdog_update();
      busy_wait_ms(1500);
      context->current_level_index = selected_level_index;
      context->current_state = GAME_STATE_START_LEVEL;
    } else {
      printf("\nInvalid level selection ('%s' -> '%c'). Please try again.\n",
             level_select_buffer, decoded_char ? decoded_char : ' ');
      watchdog_update();
      busy_wait_ms(1000);
      printf("\n");
    }
  }
}