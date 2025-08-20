#include <stdio.h>
#include <string.h>
#include <sodium.h>
#include "crypto/crypto.h"

#define NONCE_SIZE 24
#define EXPECT(cond, msg) \
    if (!(cond)) { \
        fprintf(stderr, "[!] %s:%d: %s\n", __FILE__, __LINE__, msg); \
        return 1; \
    }

int test_encrypt_decrypt_roundtrip() {
    const char *msg = "test message";
    size_t mlen = strlen(msg);
    unsigned char key[crypto_secretbox_KEYBYTES];
    unsigned char nonce[NONCE_SIZE];
    unsigned char ciphertext[mlen + crypto_secretbox_MACBYTES];
    unsigned char decrypted[mlen];

    randombytes_buf(key, sizeof key);
    randombytes_buf(nonce, sizeof nonce);

    encrypt_msg(ciphertext, (unsigned char *)msg, mlen, nonce, key);
    int result = decrypt_msg(decrypted, ciphertext, mlen + crypto_secretbox_MACBYTES, nonce, key);

    EXPECT(result == 0, "decryption should succeed");
    EXPECT(memcmp(msg, decrypted, mlen) == 0, "decrypted message mismatch");

    printf("[+] Encrypt/decrypt roundtrip passed.\n");
    return 0;
}

int test_decrypt_with_wrong_key() {
    const char *msg = "hello again";
    size_t mlen = strlen(msg);
    unsigned char key1[crypto_secretbox_KEYBYTES];
    unsigned char key2[crypto_secretbox_KEYBYTES];
    unsigned char nonce[NONCE_SIZE];
    unsigned char ciphertext[mlen + crypto_secretbox_MACBYTES];
    unsigned char decrypted[mlen];

    randombytes_buf(key1, sizeof key1);
    randombytes_buf(key2, sizeof key2);
    randombytes_buf(nonce, sizeof nonce);

    encrypt_msg(ciphertext, (unsigned char *)msg, mlen, nonce, key1);
    int result = decrypt_msg(decrypted, ciphertext, mlen + crypto_secretbox_MACBYTES, nonce, key2);
    EXPECT(result != 0, "decryption should fail with wrong key");

    printf("[+] Decryption fails with wrong key.\n");
    return 0;
}

int test_decrypt_with_tampered_ciphertext() {
    const char *msg = "this will fail";
    size_t mlen = strlen(msg);
    unsigned char key[crypto_secretbox_KEYBYTES];
    unsigned char nonce[NONCE_SIZE];
    unsigned char ciphertext[mlen + crypto_secretbox_MACBYTES];
    unsigned char decrypted[mlen];

    randombytes_buf(key, sizeof key);
    randombytes_buf(nonce, sizeof nonce);

    encrypt_msg(ciphertext, (unsigned char *)msg, mlen, nonce, key);
    ciphertext[5] ^= 0xFF; // tamper the ciphertext

    int result = decrypt_msg(decrypted, ciphertext, mlen + crypto_secretbox_MACBYTES, nonce, key);
    EXPECT(result != 0, "decryption should fail with tampered ciphertext");

    printf("[+] Tampered ciphertext correctly rejected.\n");
    return 0;
}

int main() {
    if (sodium_init() < 0) {
        fprintf(stderr, "libsodium init failed\n");
        return 1;
    }

    int failures = 0;
    failures += test_encrypt_decrypt_roundtrip();
    failures += test_decrypt_with_wrong_key();
    failures += test_decrypt_with_tampered_ciphertext();

    if (failures == 0) {
        printf("[+] All crypto tests passed.\n");
        return 0;
    } else {
        fprintf(stderr, "[!] Some crypto tests failed.\n");
        return 1;
    }
}
