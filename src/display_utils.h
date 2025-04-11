#ifndef DISPLAY_UTILS_H
#define DISPLAY_UTILS_H

#include <stdbool.h>

typedef struct GameContext GameContext; // forward declaration

/**
 * @brief Prints the opening main menu.
 * 
 */
void print_main_menu();

/**
 * @brief Clears the serial monitor.
 * 
 */
void clear_screen();

/**
 * @brief Helper to draw hearts <3 <3 <3.
 * 
 * @param lives 
 */
void draw_hearts(int lives);

/**
 * @brief Singular printf statement to center the display.
 * 
 */
void centre_display();

// input and decoded input

/**
 * @brief Displays main challenge screen including target, hint (if on) and waiting for input.
 * 
 * @param context 
 * @param current_input 
 * @param decoded_input 
 * @param is_waiting 
 */
void display_challenge_screen(const GameContext *context,
                              const char *current_input,
                              const char *decoded_input, bool is_waiting);


/**
 * @brief Displays results screen including target, hint (if on) and player input and output.
 * 
 * @param context 
 * @param final_input 
 * @param countdown_secs 
 */
void display_result_screen(const GameContext *context, const char *final_input,
                           int countdown_secs);

#endif