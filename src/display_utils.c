#include "display_utils.h"
#include "game_logic.h"
#include <hardware/timer.h>
#include <stdio.h>

extern volatile unsigned char morse_code_buffer[];

// clang-format off
void print_main_menu() {
    clear_screen();
    printf("                                                                         (`-')                        _  (`-')                          \n");
    printf("                                                                .->   <-.(OO )      .->        .->    \\-.(OO )                          \n");
    printf("                                                             ,---(`-'),------,)(`-')----. ,--.(,--.   _.'    \\     .----.               \n");
    printf("                                                            '  .-(OO )|   /`. '( OO).-.  '|  | |(`-')(_...--''    / .-.  \\              \n");
    printf("                                                            |  | .-, \\|  |_.' |( _) | |  ||  | |(OO )|  |_.' |    \\ '-'  /              \n");
    printf("                                                            |  | '.(_/|  .   .' \\|  |)|  ||  | | |  \\|  .___.'    / /`.  \\              \n");
    printf("                                                            |  '-'  | |  |\\  \\   '  '-'  '\\  '-'(_ .'|  |         \\ '-'  /              \n");
    printf("                                                             `-----'  `--' '--'   `-----'  `-----'   `--'          `---''               \n");
    printf("<-. (`-')                (`-')  (`-').->(`-')  _                         _(`-')    (`-')  _               (`-')  _ <-. (`-')   (`-')  _ \n");
    printf("   \\(OO )_      .->   <-.(OO )  ( OO)_  ( OO).-/     _             .->  ( (OO ).-> ( OO).-/        .->    (OO ).-/    \\(OO )_  ( OO).-/ \n");
    printf(",--./  ,-.)(`-')----. ,------,)(_)--\\_)(,------.     \\-,-----.(`-')----. \\    .'_ (,------.     ,---(`-') / ,---.  ,--./  ,-.)(,------. \n");
    printf("|   `.'   |( OO).-.  '|   /`. '/    _ / |  .---'      |  .--./( OO).-.  ''`'-..__) |  .---'    '  .-(OO ) | \\ /`.\\ |   `.'   | |  .---' \n");
    printf("|  |'.'|  |( _) | |  ||  |_.' |\\_..`--.(|  '--.      /_) (`-')( _) | |  ||  |  ' |(|  '--.     |  | .-, \\ '-'|_.' ||  |'.'|  |(|  '--.  \n");
    printf("|  |   |  | \\|  |)|  ||  .   .'.-._)   \\|  .--'      ||  |OO ) \\|  |)|  ||  |  / : |  .--'     |  | '.(_/(|  .-.  ||  |   |  | |  .--'  \n");
    printf("|  |   |  |  '  '-'  '|  |\\  \\ \\       /|  `---.    (_'  '--'\\  '  '-'  '|  '-'  / |  `---.    |  '-'  |  |  | |  ||  |   |  | |  `---. \n");
    printf("`--'   `--'   `-----' `--' '--' `-----' `------'       `-----'   `-----' `------'  `------'     `-----'   `--' `--'`--'   `--' `------' \n\n\n\n");

    printf("                               -----------------------------------------------------------------------\n");
    printf("                               |                                                                      |\n");
    printf("                               |                       Created by Group  8                            |\n");
    printf("                               |                                                                      |\n");
    printf("                               |              Shaan • Marco • Mika • Ashlin • Achal                   |\n");
    printf("                               |                                                                      |\n");
    printf("                               -----------------------------------------------------------------------\n\n\n\n\n");

    busy_wait_ms(3000);
    clear_screen();
    printf("USE GP21 BUTTON TO ENTER THE MORSE SEQUENCE FOR THE LEVEL NUMBER\n\n");
    printf("          .----  LEVEL 1  (Chars - Hint On)\n");
    printf("          ..---  LEVEL 2  (Chars - Hint Off)\n");
    printf("          ...--  LEVEL 3  (Words - Hint On)\n");
    printf("          ....-  LEVEL 4  (Words - Hint Off)\n");
    printf("\n");
}
// clang-format on

void clear_screen() {
  // apparently a fast way to clear most terminals
  printf("\033[2J\033[H");
  fflush(stdout);
}

// for vertical spacing
void centre_display() { printf("\n\n\n\n\n\n\n\n"); }

void display_challenge_screen(const GameContext *context,
                              const char *current_input,
                              const char *decoded_input, bool is_waiting) {
  clear_screen();
  printf("=========================== Level %d: Challenge %d/%d "
         "===========================\n\n",
         context->current_config.level_number,
         context->challenges_attempted_this_level + 1, ROUNDS_PER_LEVEL);

  // figure out label based on level type
  const char *target_label = (context->current_level_index < 2)
                                 ? "Target Character:"
                                 : "Target Word:     ";
  printf("\t\t%s %s\n", target_label, context->target_challenge);

  if (context->current_config.show_morse_hint) {
    printf("\t\tMorse Code Hint:  %s\n", context->target_morse);
  }

  printf("\n");
  printf("\t\tYour Input Morse: [%s]\n", current_input ? current_input : "");
  printf("\t\tDecodes To:       [%s]\n", decoded_input ? decoded_input : "");
  printf("\n");

  if (is_waiting) {
    printf("\t\tWaiting for your input...\n");
  } else {
    printf("\t\tInputting...\n");
  }

  centre_display();
  fflush(stdout); // immediately show output
}

void display_result_screen(const GameContext *context, const char *final_input,
                           int countdown_secs) {
  clear_screen();
  printf("============================= Level %d: Result "
         "==============================\n\n",
         context->current_config.level_number);

  // figure out label based on level type
  const char *target_label = (context->current_level_index < 2)
                                 ? "Target Character:"
                                 : "Target Word:     ";

  printf("\t\t%s %s (%s)\n", target_label, context->target_challenge,
         context->target_morse);
  printf("\t\tYour Input:       %s -> %s\n", final_input ? final_input : "",
         context->last_input_decoded);
  printf("\n");
  printf("\t\tResult: %s\n",
         context->last_answer_correct ? "CORRECT!" : "INCORRECT!");
  printf("\n");

  if (countdown_secs > 0) {
    printf("\t\tNext challenge in %d seconds...\n", countdown_secs);
  } else {
    printf("\t\tLoading next challenge...\n");
  }

  centre_display();
  fflush(stdout);
}