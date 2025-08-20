#ifndef COLORIZE_H
#define COLORIZE_H

extern char *BLACK;
extern char *GREEN;
extern char *YELLOW;
extern char *WHITE;
extern char *RED;
extern char *RESET;

void print_with_colors(FILE *output_stream, char *color, char *input);
int valid_color(char *color);
void confirm_colors();

#endif
