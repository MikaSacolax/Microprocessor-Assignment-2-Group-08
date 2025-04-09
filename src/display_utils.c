#include "display_utils.h"

#include <stdio.h>

extern unsigned char morse_code_buffer[];

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
    printf("                               |                   Shaan • Marco • Mika • Ashlin                      |\n");
    printf("                               |                                                                      |\n");
    printf("                               -----------------------------------------------------------------------\n\n\n\n\n");

    printf("                              USE GP21 BUTTON TO ENTER THE MORSE SEQUENCE FOR THE LEVEL NUMBER TO BEGIN\n\n");
    printf("                                          .----\tLEVEL 01\tCHARS (EASY)\n");
    printf("                                          ..---\tLEVEL 02\tCHARS (HARD)\n");
}
// clang-format on

void clear_screen() {
  printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"
         "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
}

void print_morse_buffer() { printf("%s\n", morse_code_buffer); }

void centre_display() { printf("\n\n\n\n\n\n\n\n"); }