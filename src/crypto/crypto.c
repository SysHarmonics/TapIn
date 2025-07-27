#include "crypto.h"
#include "socket/socket.h"

#include <sodium.h>
#include <unistd.h>

void generated_keypair(keypair_t *kp) {
    crypto_box_keypair(kp->pk, kp->sk);
}

int key_exchange(int sockfd, int initiator, keypair_t *local,
                 unsigned char k_rx[SHARED_KEY_LEN],
                 unsigned char k_tx[SHARED_KEY_LEN]) {
    unsigned char peer_pk[PUBKEY_LEN];

    if (initiator) {
        if (write_all(sockfd, local->pk, PUBKEY_LEN) != PUBKEY_LEN) return -1;
        if (read_all(sockfd, peer_pk, PUBKEY_LEN) != PUBKEY_LEN) return -1;

        if (crypto_kx_client_session_keys(k_rx, k_tx, local->pk, local->sk, peer_pk) != 0)
            return -1;
    } else {
        if (read_all(sockfd, peer_pk, PUBKEY_LEN) != PUBKEY_LEN) return -1;
        if (write_all(sockfd, local->pk, PUBKEY_LEN) != PUBKEY_LEN) return -1;

        if (crypto_kx_server_session_keys(k_rx, k_tx, local->pk, local->sk, peer_pk) != 0)
            return -1;
    }

    return 0;
}

int encrypt(unsigned char *out, const unsigned char *msg, size_t mlen,
            const unsigned char nonce[NONCE_LEN],
            const unsigned char key[SHARED_KEY_LEN]) {
    return crypto_secretbox_easy(out, msg, mlen, nonce, key);
}

int decrypt(unsigned char *out, const unsigned char *cipher, size_t clen,
            const unsigned char nonce[NONCE_LEN],
            const unsigned char key[SHARED_KEY_LEN]) {
    return crypto_secretbox_open_easy(out, cipher, clen, nonce, key);
}