#include <stdio.h>
#include <string.h>
#include <lib/colorize/colorize.h>
#include <assert.h>

void _assert_status(int expression, char *output) {
    if (expression == 0) {
        print_with_colors(GREEN, sprintf("%s passed.", output));
    } else {
        print_with_colors(RED, sprintf("%s failed.", output));
        exit(1);
    }
}

int main() {
    //We've exported constants from colorize
    //mapped to escape codes. We should only accept color constants
    //we've created.

    _assert_status(valid_color("ORANGE") == 1, "Invalid color test");
    _assert_status(valid_color("\033[2") == 1, "Invalid color test");
    _assert_status(valid_color(BLACK) == 0, "Valid color test");
    _assert_status(valid_color(RED) == 0, "Valid color test");
    _assert_status(valid_color(WHITE) == 0, "Valid color test");
    confirm_colors(); //demo of print_with_colors.
}


