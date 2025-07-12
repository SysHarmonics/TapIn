#include "TapIn.h"
#include "crypto/crypto.h"
#include "socket/socket.h"
#include <stdio.h>

int perform_secure_handshake(int fd, int initiator,
                             unsigned char k_rx[32], unsigned char k_tx[32]) {
    keypair_t self; crypto_gen_keypair(&self);
    if (crypto_handshake(fd, initiator, &self, k_rx, k_tx) != 0) {
        fprintf(stderr, "[handshake] libsodium kx failed\n");
        return -1;
    }
    return 0;
}