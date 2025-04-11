#ifndef ASM_INTERFACE_H
#define ASM_INTERFACE_H

#include <stdbool.h>
#include <stddef.h>

// be careful to make this exactly equal to the buffer defined in the assembly
#define MORSE_BUFFER_SIZE 1024

bool asm_interface_is_sequence_complete();
bool asm_interface_has_new_char();

void asm_interface_clear_sequence_complete_flag();
void asm_interface_clear_new_char_flag();
void asm_interface_flush_state();

// returns size of morse input
size_t asm_interface_get_morse_input(char *buffer, size_t buffer_len);

char asm_interface_get_last_char();

// returns true if cleared a space
bool asm_interface_check_and_clear_initial_space();

#endif