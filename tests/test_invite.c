#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sodium.h>
#include "invite/invite.h"

#define EXPECT(cond, msg) \
    if (!(cond)) { \
        fprintf(stderr, "[!] %s:%d: %s\n", __FILE__, __LINE__, msg); \
        return 1; \
    }

int test_invite_generate_and_parse() {
    char invite[INVITE_LEN];
    char ip_out[INET_ADDRSTRLEN] = {0};
    char port_out[8] = {0};

    int gen_result = invite_generate(invite, sizeof invite, "secret", "127.0.0.1", "4444");
    EXPECT(gen_result == 0, "invite_generate should return 0");

    int parse_result = invite_parse(invite, "secret", ip_out, port_out);
    EXPECT(parse_result == 0, "invite_parse should return 0");
    EXPECT(strcmp(ip_out, "127.0.0.1") == 0, "IP mismatch");
    EXPECT(strcmp(port_out, "4444") == 0, "Port mismatch");

    return 0;
}

int test_invite_parse_wrong_password() {
    char invite[INVITE_LEN];
    char ip_out[INET_ADDRSTRLEN] = {0};
    char port_out[8] = {0};

    invite_generate(invite, sizeof invite, "secret", "10.0.0.1", "1234");
    int result = invite_parse(invite, "wrongpass", ip_out, port_out);
    EXPECT(result != 0, "invite_parse should fail with wrong password");

    return 0;
}

int main() {
    if (sodium_init() < 0) {
        fprintf(stderr, "libsodium init failed\n");
        return 1;
    }

    int failures = 0;

    failures += test_invite_generate_and_parse();
    failures += test_invite_parse_wrong_password();

    if (failures == 0) {
        printf("[+] All invite tests passed!\n");
        return 0;
    } else {
        fprintf(stderr, "[!] Some invite tests failed.\n");
        return 1;
    }
}
