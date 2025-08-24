#include <lib/colorize/colorize.h>
#include <stdio.h>
#include <stdlib.h>

void assert_status(int expression, char *template_string) {
  int strlen = sizeof(template_string) / sizeof(template_string[0]);
  // Account for null terminator (2 chars) and passed|failed (6)
  if (strlen > 248) {
    printf("Input too large. Consider shortening to a length <= 248 chars.;");
    return;
  }

  char formatted_output[256];
  if (expression) {
    snprintf(formatted_output, "%s passed.\n", template_string);
    print_with_colors(stdout, GREEN, formatted_output);
  } else {
    snprintf(formatted_output, "%s failed.\n", template_string);
    print_with_colors(stderr, RED, formatted_output);
    exit(1);
  }
}
