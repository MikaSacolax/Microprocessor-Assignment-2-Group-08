#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "pico/time.h"


// Declare the external variables so the compiler knows they exist:
extern unsigned char morse_code_buffer[];
extern uint32_t current;

int Hearts = 3;

void print_morse_code();

// Declare the main assembly code entry point.
void main_asm();

// Turn Morse into Latin characters
char Morse_to_char (char input[5]);

// Turn Latin characters into Morse
char* Char_to_morse (char input);

// Random character
char Rand_char();

// Levels
void Level_1 ();
void Level_2 ();
void Level_3 ();
void Level_4 ();

// Initialise a GPIO pin – see SDK for detail on gpio_init()
void asm_gpio_init(uint pin) {
 gpio_init(pin);
}

// Set direction of a GPIO pin – see SDK for detail on gpio_set_dir()
void asm_gpio_set_dir(uint pin, bool out) {
 gpio_set_dir(pin, out);
}

// Get the value of a GPIO pin – see SDK for detail on gpio_get()
bool asm_gpio_get(uint pin) {
 return gpio_get(pin);
}

// Set the value of a GPIO pin – see SDK for detail on gpio_put()
void asm_gpio_put(uint pin, bool value) {
 gpio_put(pin, value);
}

// Enable falling-edge interrupt – see SDK for detail on gpio_set_irq_enabled()
void asm_gpio_set_irq(uint pin) {
    gpio_set_irq_enabled(pin, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true);
   }

void print_morse_code()
{
    printf("Morse code: %s\n", morse_code_buffer);
}


/*
 * Main entry point for the code - simply calls the main assembly function.
 */
 int main() {
    stdio_init_all(); // Initialise all basic IO
    printf("Assignment #2...\n"); // Basic print to console
    main_asm();
    printf("AHHHHHHHHHHHH");
    Level_1 ();
    return 0;
}



void Level_1 (){
    
    int iterations = 5;
    
    while (Hearts > 0 && iterations > 0)
    {
        int start_index = current;

        char character_to_spell = Rand_char();
        printf("Level 1 character: %c  Morse: %s\n",character_to_spell, Char_to_morse(character_to_spell));


        if (current - start_index > 6)
        {
            printf("Wrong");
            Hearts = Hearts - 1;
            iterations = 5;
            continue;
        }
        else {
            char input [5] = "     ";
            int x = 0;
            while (start_index != current)
            {
               input[x] = morse_code_buffer[start_index];
               x = x + 1;
               start_index = start_index + 1;
            }
            
            if (strcmp(input, Char_to_morse(character_to_spell)) == 0)
            {
                printf("Congrats on getting it right");
                iterations = iterations - 1;

                if (Hearts < 3)
                {
                    Hearts = Hearts + 1;
                }

                continue;
            }
            else {
                printf("Wrong");
                Hearts = Hearts - 1;
                iterations = 5;
                continue;
            }
            

        }
        

    }
    
    if (Hearts == 0)
    {
        //Break or go to start, Doing something I don't know
        printf("Run out of hearts");
    }
    
    Level_2();

}

void Level_2 (){

}

void Level_3 (){

}

void Level_4 (){

}


char Rand_char(){
    srand(time_us_64());
    int num = rand() % 35;

    if (num <= 9)
    {
        return ('0' + num);
    }
    else {
        return('A' + num - 9);
    }
    
}


char Morse_to_char (char input[5]) {

    if(strcmp(input, ".-   ")) return 'A';
    if(strcmp(input, "-... ")) return 'B';
    if(strcmp(input, "-.-. ")) return 'C';
    if(strcmp(input, "-..  ")) return 'D';
    if(strcmp(input, ".    ")) return 'E';
    if(strcmp(input, "..-. ")) return 'F';
    if(strcmp(input, "--.  ")) return 'G';
    if(strcmp(input, ".... ")) return 'H';
    if(strcmp(input, "..   ")) return 'I';
    if(strcmp(input, ".--- ")) return 'J';
    if(strcmp(input, "-.-  ")) return 'K';
    if(strcmp(input, ".-.. ")) return 'L';
    if(strcmp(input, "--   ")) return 'M';
    if(strcmp(input, "-.   ")) return 'N';
    if(strcmp(input, "---  ")) return 'O';
    if(strcmp(input, ".--. ")) return 'P';
    if(strcmp(input, "--.- ")) return 'Q';
    if(strcmp(input, ".-.  ")) return 'R';
    if(strcmp(input, "...  ")) return 'S';
    if(strcmp(input, "-    ")) return 'T';
    if(strcmp(input, "..-  ")) return 'U';
    if(strcmp(input, "...- ")) return 'V';
    if(strcmp(input, ".--  ")) return 'W';
    if(strcmp(input, "-..- ")) return 'X';
    if(strcmp(input, "-.-- ")) return 'Y';
    if(strcmp(input, "--.. ")) return 'Z';

    if(strcmp(input, ".----")) return '0';
    if(strcmp(input, "..---")) return '1';
    if(strcmp(input, "...--")) return '2';
    if(strcmp(input, "....-")) return '3';
    if(strcmp(input, ".....")) return '4';
    if(strcmp(input, "-....")) return '5';
    if(strcmp(input, "--...")) return '6';
    if(strcmp(input, "---..")) return '7';
    if(strcmp(input, "----.")) return '8';
    if(strcmp(input, "-----")) return '9';

    return '0';

}

char* Char_to_morse (char input) {

    if(input == 'A') return  ".-    ";
    if(input == 'B') return  "-...  ";
    if(input == 'C') return  "-.-.  ";
    if(input == 'D') return  "-..   ";
    if(input == 'E') return  ".     ";
    if(input == 'F') return  "..-.  ";
    if(input == 'G') return  "--.   ";
    if(input == 'H') return  "....  ";
    if(input == 'I') return  "..    ";
    if(input == 'J') return  ".---  ";
    if(input == 'K') return  "-.-   ";
    if(input == 'L') return  ".-..  ";
    if(input == 'M') return  "--    ";
    if(input == 'N') return  "-.    ";
    if(input == 'P') return  "---   ";
    if(input == 'O') return  ".--.  ";
    if(input == 'Q') return  "--.-  ";
    if(input == 'R') return  ".-.   ";
    if(input == 'S') return  "...   ";
    if(input == 'T') return  "-     ";
    if(input == 'U') return  "..-   ";
    if(input == 'V') return  "...-  ";
    if(input == 'W') return  ".--   ";
    if(input == 'X') return  "-..-  ";
    if(input == 'Y') return  "-.--  ";
    if(input == 'Z') return  "--..  ";

    if(input == '0') return  ".----";
    if(input == '1') return  "..---";
    if(input == '2') return  "...--";
    if(input == '3') return  "....-";
    if(input == '4') return  ".....";
    if(input == '5') return  "-....";
    if(input == '6') return  "--...";
    if(input == '7') return  "---..";
    if(input == '8') return  "----.";
    if(input == '9') return  "-----";

    return "    ";

}