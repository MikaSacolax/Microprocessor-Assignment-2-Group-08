#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "pico/time.h"
#include "hardware/sync.h"

// Declare the external variables so the compiler knows they exist:
extern unsigned char morse_code_buffer[];
extern uint32_t current;
extern uint32_t flag;
extern uint32_t current;
extern uint32_t start_index;

int Hearts = 3;

void print_morse_code();

// Declare the main assembly code entry point.
void main_asm();

// Turn Morse into Latin characters
char Morse_to_char (char input[6]);

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
    
    while (true)
    {
        printf("\nMorse code: ");
        flag = 0;

        while (flag == 0)
                {
                    __wfi();
                }
        
        printf("\nCurrent: %i, Start_index: %i", current, start_index);

        if (current - start_index > 6)
        {
            printf("\nYour input is too long: %i ", current - start_index);
            current = current + 1;
            continue;
        }


        char input [6] = "     ";
        int x = 0;
        while (start_index <= current)
        {
            input[x] = morse_code_buffer[start_index];
            printf("%c", morse_code_buffer[start_index]);
            x = x + 1;
            start_index = start_index + 1;
        }

        printf("%c, %s", Morse_to_char(input), input);

        switch (Morse_to_char(input))
        {
        case '1':
            Level_1();
            break;
        case '2':
            Level_2();
            break;
        case '3':
            Level_3();
            break;
        case '4':
            Level_4();
            break;
        
        default:
            printf("Not a valid input, Try Again.");
            current = current + 1;
            break;
        }

    }
    


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

        //main_asm();

        if (current - start_index > 6)
        {
            printf("Wrong, your input does not equal a singular character.");
            Hearts = Hearts - 1;
            iterations = 5;
            continue;
        }
        else {
            char input [6] = "     ";
            int x = 0;
            while (start_index != current)
            {
               input[x] = morse_code_buffer[start_index];
               x = x + 1;
               start_index = start_index + 1;
               
            }

            printf("Your inputted character: %c", Morse_to_char(input));

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


char Morse_to_char (char input[6]) {

    if(strcmp(input, ".- ") == 0) return 'A';
    if(strcmp(input, "-... ") == 0) return 'B';
    if(strcmp(input, "-.-. ") == 0) return 'C';
    if(strcmp(input, "-..  ") == 0) return 'D';
    if(strcmp(input, ".    ") == 0) return 'E';
    if(strcmp(input, "..-. ") == 0) return 'F';
    if(strcmp(input, "--.  ") == 0) return 'G';
    if(strcmp(input, ".... ") == 0) return 'H';
    if(strcmp(input, "..   ") == 0) return 'I';
    if(strcmp(input, ".--- ") == 0) return 'J';
    if(strcmp(input, "-.-  ") == 0) return 'K';
    if(strcmp(input, ".-.. ") == 0) return 'L';
    if(strcmp(input, "--   ") == 0) return 'M';
    if(strcmp(input, "-.   ") == 0) return 'N';
    if(strcmp(input, "---  ") == 0) return 'O';
    if(strcmp(input, ".--. ") == 0) return 'P';
    if(strcmp(input, "--.- ") == 0) return 'Q';
    if(strcmp(input, ".-.  ") == 0) return 'R';
    if(strcmp(input, "...  ") == 0) return 'S';
    if(strcmp(input, "-    ") == 0) return 'T';
    if(strcmp(input, "..-  ") == 0) return 'U';
    if(strcmp(input, "...- ") == 0) return 'V';
    if(strcmp(input, ".--  ") == 0) return 'W';
    if(strcmp(input, "-..- ") == 0) return 'X';
    if(strcmp(input, "-.-- ") == 0) return 'Y';
    if(strcmp(input, "--.. ") == 0) return 'Z';

    if(strcmp(input, ".----") == 0) return '0';
    if(strcmp(input, "..---") == 0) return '1';
    if(strcmp(input, "...--") == 0) return '2';
    if(strcmp(input, "....-") == 0) return '3';
    if(strcmp(input, ".....") == 0) return '4';
    if(strcmp(input, "-....") == 0) return '5';
    if(strcmp(input, "--...") == 0) return '6';
    if(strcmp(input, "---..") == 0) return '7';
    if(strcmp(input, "----.") == 0) return '8';
    if(strcmp(input, "-----") == 0) return '9';

    return '?';

}

char* Char_to_morse (char input) {

    if(input == 'A') return  ".-   ";
    if(input == 'B') return  "-... ";
    if(input == 'C') return  "-.-. ";
    if(input == 'D') return  "-..  ";
    if(input == 'E') return  ".    ";
    if(input == 'F') return  "..-. ";
    if(input == 'G') return  "--.  ";
    if(input == 'H') return  ".... ";
    if(input == 'I') return  "..   ";
    if(input == 'J') return  ".--- ";
    if(input == 'K') return  "-.-  ";
    if(input == 'L') return  ".-.. ";
    if(input == 'M') return  "--   ";
    if(input == 'N') return  "-.   ";
    if(input == 'P') return  "---  ";
    if(input == 'O') return  ".--. ";
    if(input == 'Q') return  "--.- ";
    if(input == 'R') return  ".-.  ";
    if(input == 'S') return  "...  ";
    if(input == 'T') return  "-    ";
    if(input == 'U') return  "..-  ";
    if(input == 'V') return  "...- ";
    if(input == 'W') return  ".--  ";
    if(input == 'X') return  "-..- ";
    if(input == 'Y') return  "-.-- ";
    if(input == 'Z') return  "--.. ";

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

    return "?????";

}