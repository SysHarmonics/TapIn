#include <stdio.h>
#include <string.h>

// All colors are bright

// \033[] is the escape sequence to set the color
//the colors are separated by semicolon(;).
//foreground, instensity is 1.
char *BLACK = "\033[30;1m";
char *GREEN = "\033[32;1m";
char *YELLOW = "\033[33;1m";
char *WHITE = "\033[37;1m";
char *RED = "\033[31;1m";
char *RESET = "\033[0m";



//Decided to limit the input string to be 256 bytes to limit over
//consumption.
void print_with_colors(char *color, char *input) {
    if (!valid_color(color)) { return ; }
    printf("%s%s%s\n", color, input, RESET);
}

int valid_color(char *color) {
    int i = 0;
    int found = 0;
    char *all_colors[] = {BLACK, GREEN, YELLOW, WHITE, RED};
    int len = sizeof(all_colors) / sizeof(all_colors[0]);

    while (!found && i < len) {
        found = all_colors[i] == color;
        i++;
    }

    if (!found) {
        printf("Invalid color passed. See colorize.c for full list.\n");
    }


    return found;
}

void confirm_colors() {
    char *colors[] = {BLACK, GREEN, YELLOW, WHITE, RED};
    for (int i = 0; i < 5; i++) {
        print_with_colors(colors[i], "Hello World!");
    }
}

main() {
    print_with_colors("ORANGE", "Hello World!");
    print_with_colors("PURPLE", "Hello World!");
    print_with_colors(GREEN, "Hello World!");
}