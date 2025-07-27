#ifndef INVITE_H
#define INVITE_H

#include <stddef.h>
#include <stdint.h>

#define INVITE_LEN 128

int invite_generate(char *out, size_t outlen, const char *password, const char *ip, const char *port);
int invite_parse(const char *invite, const char *password, char *ip_out, char *port_out);

#endif