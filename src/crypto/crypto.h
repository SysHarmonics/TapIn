#ifndef TAPIN_CRYPTO_H
#define TAPIN_CRYPTO_H
#include <stddef.h>
#include <sodium.h>

#define PUBKEY_LEN 32
#define SECKEY_LEN 32
#define NONCE_LEN  24
#define SHARED_KEY_LEN 32

typedef struct {
    unsigned char pk[PUBKEY_LEN];
    unsigned char sk[SECKEY_LEN];
} keypair_t;

void generated_keypair(keypair_t *kp);

int key_exchange(int sockfd, int initiator, keypair_t *local,
                unsigned char k_rx[SHARED_KEY_LEN],
                unsigned char k_tx[SHARED_KEY_LEN]);
int encrypt_msg(unsigned char *out, const unsigned char *msg, size_t mlen,
            const unsigned char nonce[NONCE_LEN],
            const unsigned char key[SHARED_KEY_LEN]);
int decrypt_msg(unsigned char *out, const unsigned char *cipher, size_t clen,
            const unsigned char nonce[NONCE_LEN],
            const unsigned char key[SHARED_KEY_LEN]);
#endif