#include "invite.h"
#include <sodium.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>

static void base32_encode(const unsigned char *in, size_t inlen, char *out) {
    const char *alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";
    size_t j = 0;
    for (size_t i = 0; i < inlen; i += 5) {
        uint64_t buffer = 0;
        size_t remain = inlen - i >= 5 ? 5 : inlen - i;
        for (size_t k = 0; k < remain; ++k) {
            buffer |= ((uint64_t)in[i + k]) << (32 - 8 * k);
        }
        for (size_t b = 0; b < (remain * 8 + 4) / 5; ++b) {
            out[j++] = alphabet[(buffer >> (35 - b * 5)) & 0x1F];
        }
    }
    out[j] = '\0';
}

static int base32_decode(const char *in, unsigned char *out, size_t outlen) {
    const char *alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";
    size_t inlen = strlen(in);
    uint64_t buffer = 0;
    int bits = 0, count = 0;
    size_t j = 0;

    for (size_t i = 0; i < inlen; ++i) {
        const char *p = strchr(alphabet, in[i]);
        if (!p) continue;
        buffer = (buffer << 5) | (p - alphabet);
        bits += 5;
        if (bits >= 8) {
            if (j >= outlen) return -1;
            out[j++] = (buffer >> (bits - 8)) & 0xFF;
            bits -= 8;
        }
    }
    return j;
}

int invite_generate(char *out, size_t outlen, const char *password, const char *ip, const char *port) {
    unsigned char nonce[crypto_aead_chacha20poly1305_ietf_NPUBBYTES];
    randombytes_buf(nonce, sizeof nonce);

    char combo[128];
    snprintf(combo, sizeof(combo), "%s:%s", ip, port);

    unsigned char ciphertext[128];
    unsigned long long clen;
    crypto_aead_chacha20poly1305_ietf_encrypt(
        ciphertext, &clen,
        (const unsigned char *)combo, strlen(combo),
        NULL, 0, NULL, nonce,
        (const unsigned char *)password);

    unsigned char full[sizeof(nonce) + clen];
    memcpy(full, nonce, sizeof(nonce));
    memcpy(full + sizeof(nonce), ciphertext, clen);

    base32_encode(full, sizeof(full), out);
    return 0;
}

int invite_parse(const char *invite, const char *password, char *ip_out, char *port_out) {
    unsigned char decoded[128];
    int binlen = base32_decode(invite, decoded, sizeof(decoded));
    if (binlen < (int)crypto_aead_chacha20poly1305_ietf_NPUBBYTES + crypto_aead_chacha20poly1305_ietf_ABYTES)
        return -1;

    unsigned char *nonce = decoded;
    unsigned char *ciphertext = decoded + crypto_aead_chacha20poly1305_ietf_NPUBBYTES;
    size_t clen = binlen - crypto_aead_chacha20poly1305_ietf_NPUBBYTES;

    unsigned char decrypted[128];
    unsigned long long mlen;
    if (crypto_aead_chacha20poly1305_ietf_decrypt(
            decrypted, &mlen,
            NULL,
            ciphertext, clen,
            NULL, 0,
            nonce, (const unsigned char *)password) != 0)
        return -1;

    decrypted[mlen] = '\0';
    char *colon = strchr((char *)decrypted, ':');
    if (!colon) return -1;
    *colon = '\0';
    strcpy(ip_out, (char *)decrypted);
    strcpy(port_out, colon + 1);

    return 0;
}