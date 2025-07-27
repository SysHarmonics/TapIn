#include "TapIn.h"
#include "crypto/crypto.h"


#include <stdio.h>

int tapped_in(int fd, int initiator,
                const char *invite_code,
                const char *password,
                unsigned char k_rx[32],
                unsigned char k_tx[32]) {
    keypair_t self;
    crypto_gen_keypair(&self);
    if (!invite_code || !password) {
        fprintf(stderr, "invite code and password are required\n");
        return -1;
    }

    if (key_exchange(fd, initiator, &self, invite_code, password, k_rx, k_tx) != 0) {
        fprintf(stderr, "Key exchange has failed\n");
        return -1;
    }
    return 0;
}