#include <lib/colorize/colorize.h>
#include <stdio.h>
#include <stdlib.h>

void assert_status(int expression, char *template_string) {
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