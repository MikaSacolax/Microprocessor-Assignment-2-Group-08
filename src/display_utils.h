#ifndef DISPLAY_UTILS_H
#define DISPLAY_UTILS_H

#include <stdbool.h>

typedef struct GameContext GameContext; // forward declaration

void print_main_menu();
void clear_screen();
void draw_hearts(int lives);
void centre_display();

// displays main challenge screen including target, hint (if on), current player
// input and decoded input
void display_challenge_screen(const GameContext *context,
                              const char *current_input,
                              const char *decoded_input, bool is_waiting);

// displays results screen after attempt
void display_result_screen(const GameContext *context, const char *final_input,
                           int countdown_secs);

#endif