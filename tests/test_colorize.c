#include <stdio.h>
#include <string.h>
#include <lib/colorize/colorize.h>
#include "test_helpers.h"
#include <stdlib.h>

int main() {
    // We've exported constants from colorize mapped to escape codes. 
    // We should only accept color constants we've created.

    assert_status(valid_color("ORANGE") == 0, "Invalid color test");
    assert_status(valid_color("\033[2") == 0, "Invalid color test");
    assert_status(valid_color(BLACK) == 1, "Valid color test");
    assert_status(valid_color(RED) == 1, "Valid color test");
    assert_status(valid_color(WHITE) == 1, "Valid color test");
    confirm_colors(); //demo of print_with_colors.
}


