#include <colorize.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_INPUT_LEN 256

void assert_status(int expression, char *template_string) {
  // Account for null terminator (2 chars) and passed|failed (6)
  if (strlen(template_string) > MAX_INPUT_LEN - 10) {
    printf("Input too large. Consider shortening to a length <= 246 chars.;");
    return;
  }

  char formatted_output[MAX_INPUT_LEN];
  if (expression) {
    snprintf(formatted_output, MAX_INPUT_LEN, "%s passed.\n", template_string);
    print_with_colors(stdout, GREEN, formatted_output);
  } else {
    snprintf(formatted_output, MAX_INPUT_LEN, "%s failed.\n", template_string);
    print_with_colors(stderr, RED, formatted_output);
    exit(1);
  }
}
