#ifndef ASM_INTERFACE_H
#define ASM_INTERFACE_H

#include <stdbool.h>
#include <stddef.h>

// be careful to make this exactly equal to the buffer defined in the assembly
#define MORSE_BUFFER_SIZE 1024

/**
 * @brief Returns whether the sequence_complete_flag has been triggered by Assembly code. Checking if the input has been completed.
 * 
 * @return true 
 * @return false 
 */
bool asm_interface_is_sequence_complete();

/**
 * @brief Returns whether the new_char_flag has been triggered by Assembly code. Checks whenever a new character get completed.
 * 
 * @return true 
 * @return false 
 */
bool asm_interface_has_new_char();

/**
 * @brief Clears the sequence_complete_flag such that the arm code can trigger it.
 * 
 */
void asm_interface_clear_sequence_complete_flag();

/**
 * @brief Clears the new_char_flag such that the arm code can trigger it.
 * 
 */
void asm_interface_clear_new_char_flag();

/**
 * @brief Allocates new memory for morse_code_buffer effectively clearing it.
 * 
 */
void asm_interface_flush_state();

/**
 * @brief Returns size of morse input.
 * 
 * @param buffer 
 * @param buffer_len 
 * @return size_t 
 */
size_t asm_interface_get_morse_input(char *buffer, size_t buffer_len);

/**
 * @brief Returns the last character to be added to morse_code_buffer.
 * 
 * @return char 
 */
char asm_interface_get_last_char();

// returns true if cleared a space
/**
 * @brief Checks if a space character was added initially and removes it. Returns true if cleared the space character.
 * 
 * @return true 
 * @return false 
 */
bool asm_interface_check_and_clear_initial_space();

#endif