#ifndef COLORIZE_H
#define COLORIZE_H

extern char *BLACK;
extern char *GREEN;
extern char *YELLOW;
extern char *WHITE;
extern char *RED;
extern char *RESET;
//What we need to do is take in some input - 8 chracters
//What should be our maximum input?
//256 bytes take up 256 characters.
// \u001b[32;1m
//The escape sequence wouldn't be considered a printable character so 
// I think we're ok

void print_with_colors(char *color, char *input);

#endif
