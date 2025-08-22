#include "crypto.h"
#include "socket/socket.h"
#include "common.h"

#include <sodium.h>
#include <unistd.h>

void generated_keypair(keypair_t *kp) {
    crypto_kx_keypair(kp->pk, kp->sk);
}

int key_exchange(int sockfd, int initiator, keypair_t *local,
                 unsigned char k_rx[SHARED_KEY_LEN],
                 unsigned char k_tx[SHARED_KEY_LEN]) {
    unsigned char peer_pk[PUBKEY_LEN];

    if (initiator) {
        // debug

        DEBUG_PRINT("[*] Sending public key...\n");
        if (write_all(sockfd, local->pk, PUBKEY_LEN) != PUBKEY_LEN) return -1;


        DEBUG_PRINT("[*] Waiting for peer public key...\n");
        if (read_all(sockfd, peer_pk, PUBKEY_LEN) != PUBKEY_LEN) return -1;

        DEBUG_PRINT("[*] Deriving session keys (client)...\n");
        if (crypto_kx_client_session_keys(k_rx, k_tx, local->pk, local->sk, peer_pk) != 0)
            return -1;

    } else {

        DEBUG_PRINT("[*] Waiting for initiator's public key...\n");
        if (read_all(sockfd, peer_pk, PUBKEY_LEN) != PUBKEY_LEN) return -1;

        DEBUG_PRINT("[*] Sending own public key...\n");
        if (write_all(sockfd, local->pk, PUBKEY_LEN) != PUBKEY_LEN) return -1;

        DEBUG_PRINT("[*] Deriving session keys (server)...\n");
        if (crypto_kx_server_session_keys(k_rx, k_tx, local->pk, local->sk, peer_pk) != 0)
            return -1;

    }
    return 0;

}

int encrypt_msg(unsigned char *out, const unsigned char *msg, size_t mlen,
            const unsigned char nonce[NONCE_LEN],
            const unsigned char key[SHARED_KEY_LEN]) {
    return crypto_secretbox_easy(out, msg, mlen, nonce, key);
}

int decrypt_msg(unsigned char *out, const unsigned char *cipher, size_t clen,
            const unsigned char nonce[NONCE_LEN],
            const unsigned char key[SHARED_KEY_LEN]) {
    return crypto_secretbox_open_easy(out, cipher, clen, nonce, key);
}