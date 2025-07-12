#include "crypto.h"
#include <unistd.h>

void crypto_gen_keypair(keypair_t *kp) {
    crypto_box_keypair(kp->pk, kp->sk);
}

int cryptography(int sockfd, int is_initiator, keypair_t *local,
                     unsigned char k_rx[32], unsigned char k_tx[32]) {
    unsigned char peer_pk[32];
    if (is_initiator) {
        if (write_all(sockfd, local->pk, 32) != 32) return -1;
        if (read_all(sockfd, peer_pk, 32) != 32) return -1;
        return crypto_kx_client_session_keys(k_rx, k_tx, local->pk, local->sk, peer_pk);
    } else {
        if (read_all(sockfd, peer_pk, 32) != 32) return -1;
        if (write_all(sockfd, local->pk, 32) != 32) return -1;
        return crypto_kx_server_session_keys(k_rx, k_tx, local->pk, local->sk, peer_pk);
    }
}

int crypto_encrypt(unsigned char *out, const unsigned char *msg, size_t mlen,
                   const unsigned char nonce[24], const unsigned char key[32]) {
    return crypto_secretbox_easy(out, msg, mlen, nonce, key);
}

int crypto_decrypt(unsigned char *out, const unsigned char *cipher, size_t clen,
                   const unsigned char nonce[24], const unsigned char key[32]) {
    return crypto_secretbox_open_easy(out, cipher, clen, nonce, key);
}