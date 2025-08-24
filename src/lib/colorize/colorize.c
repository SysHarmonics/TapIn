#include <stdio.h>
#include <string.h>
#define MAX_INPUT_LEN 256
// All colors are bright.
// \033[ is the escape sequence to set the color.
// The color settings are separated by semicolon(;)
// with their integer values determining their effect.
// Argument position doesn't matter just the ranges.
// For more info visit https://en.wikipedia.org/wiki/ANSI_escape_code.
char *BLACK = "\033[30;1m";
char *GREEN = "\033[32;1m";
char *YELLOW = "\033[33;1m";
char *WHITE = "\033[37;1m";
char *RED = "\033[31;1m";
char *RESET = "\033[0m";

int valid_color(char *color) {
  int found = 0;
  char *all_colors[] = {BLACK, GREEN, YELLOW, WHITE, RED};
  int len = sizeof(all_colors) / sizeof(all_colors[0]);

  for (int i = 0; i < len; i++) {
    found = strcmp(all_colors[i], color) == 0;
    if (found) {
      return found;
    }
  }

  fprintf(stderr, "Invalid color received. See colorize.c for full list.\n");

  return found;
}

/*
output_stream should be stdout, or stderr.
Flushing stream manually for two reasons:
  1. So the stream can properly reset to the original foreground value.
  2. So we don't end up with an extra newline if the user adds a newline.
*/
void print_with_colors(FILE *output_stream, char *color, char *input) {
  if (!valid_color(color)) {
    return;
  }

  if (strlen(input) > MAX_INPUT_LEN - 2) {
    printf("Input too large. Consider shortening to a length <= 254 chars.;");
    return;
  }
  fprintf(output_stream, "%s%s%s", color, input, RESET);
  fflush(output_stream);
}

void confirm_colors() {
  char *colors[] = {BLACK, GREEN, YELLOW, WHITE, RED};
  for (int i = 0; i < 5; i++) {
    print_with_colors(stdout, colors[i], "Hello World!\n");
  }
}
