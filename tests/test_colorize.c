#include <stdio.h>
#include <string.h>
#include <lib/colorize/colorize.h>
#include <stdlib.h>

void _assert_status(int expression, char *template_string) {
    char formatted_output[256];
    if (expression) {
        sprintf(formatted_output, "%s passed.\n", template_string);
        print_with_colors(stdout, GREEN, formatted_output);
    } else {
        sprintf(formatted_output, "%s failed.\n", template_string);
        print_with_colors(stderr, RED, formatted_output);
        exit(1);
    }
}

int main() {
    //We've exported constants from colorize
    //mapped to escape codes. We should only accept color constants
    //we've created.

    _assert_status(valid_color("ORANGE") == 0, "Invalid color test");
    _assert_status(valid_color("\033[2") == 0, "Invalid color test");
    _assert_status(valid_color(BLACK) == 0, "Valid color test");
    _assert_status(valid_color(RED) == 1, "Valid color test");
    _assert_status(valid_color(WHITE) == 1, "Valid color test");
    confirm_colors(); //demo of print_with_colors.
}


