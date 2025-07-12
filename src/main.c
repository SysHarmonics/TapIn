#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <getopt.h>
#include <pthread.h>

#include "socket/socket.h"
#include "crypto/crypto.h"
#include "TapIn/TapIn.h"

#define NONCE_SIZE 24

typedef struct {
    int fd;
    unsigned char k_rx[32];
    unsigned char k_tx[32];
} peer_t;


static void *receive_loop(void *arg);
static void *send_loop(void *arg);

int main(int argc, char *argv[]) {
    int opt;
    const char *listen_port = NULL;
    const char *connect_host = NULL;    
    const char *connect_port = NULL;

    static struct option long_options[] = {
        {"listen", required_argument, 0, 'l'},
        {"connect", required_argument, 0, 'c'},
        {0, 0, 0, 0}
    };

    int option;
    while ((option = getopt_long(argc, argv, "l:c:", long_options, NULL)) != -1) {
        switch (option) {
            case 'l':
                listen_port = optarg;
                break;
            case 'c': {
                char *colon_ptr = strchr(optarg, ':');
                if (!colon_ptr) { 
                    fprintf(stderr, "--connect <host:port> required\n");
                    exit(1);
                }
                *colon_ptr = '\0';
                connect_host = optarg;
                connect_port = colon_ptr + 1;
                break;
            } 
            default: 
                fprintf(stderr, "Usage: %s [--listen <port>] [--connect <host:port>]\n", argv[0]);
                exit(1);
        }
    }
    if (sodium_init()<0) {
        fprintf(stderr, "Initialization failed\n");
        return 1;
    }

    int sockfd, initiatior; 

    if (connect_host) {
        //send request
        sockfd = tcp_connect(connect_host, connect_port);
        if (sockfd <0) {
            perror("connect");
            return 1;
        }
        initiatior = 1;
    }else if (listen_port) {
        //waiting for response
        int listener = tcp_listen(listen_port);
        if (listener<0) {
            perror("listen");
            return 1;
        }
        sockfd = tcp_accept(listener);
        close(listener);
        initiatior=0;
    } else {
        fprintf(stderr, "Missing argument flag: use --listen <port> or --connect <host:port>\n");
        return 1;
    }

    peer_t peer = { .fd = sockfd };
    if (tapped_in(sockfd, initiatior, peer.k_rx, peer.k_tx) != 0) {
        close(sockfd);
        return 1;
    }

    pthread_t receive_thread, send_thread;
    pthread_create(&receive_thread, NULL, receive_loop, &peer);
    pthread_create(&send_thread, NULL, send_loop, &peer);

    pthread_join(send_thread, NULL);
    pthread_cancel(receive_thread);
    
    close(sockfd);
    return 0;
}

static void *receive_loop(void *arg) {
    peer_t *peer = (peer_t *)arg;
     for (;;) {
        uint16_t net_clen;
        if (read_all(peer->fd, &net_clen, sizeof(net_clen)) <= 0)
            break;

        size_t clen = ntohs(net_clen);
        unsigned char nonce[NONCE_SIZE];

        if (read_all(peer->fd, nonce, NONCE_SIZE) <= 0)
            break;

        unsigned char *ciphertext = malloc(clen);
        if (!ciphertext || read_all(peer->fd, ciphertext, clen) <= 0) {
            free(ciphertext);
            break;
        }

        unsigned char *plaintext = malloc(clen - crypto_secretbox_MACBYTES);
        if (plaintext && crypto_decrypt(plaintext, ciphertext, clen, nonce, peer->k_rx) == 0) {
            fwrite(plaintext, 1, clen - crypto_secretbox_MACBYTES, stdout);
            fflush(stdout);
        }

        free(ciphertext);
        free(plaintext);
    }
    return NULL;
}

static void *send_loop(void *arg) {
    peer_t *peer = (peer_t *)arg;
    char *line = NULL;
    size_t cap = 0;

    while (getline(&line, &cap, stdin) != -1) {
        size_t mlen = strlen(line);
        size_t clen = mlen + crypto_secretbox_MACBYTES;

        unsigned char *ciphertext = malloc(clen);
        unsigned char nonce[NONCE_SIZE];

        randombytes_buf(nonce, NONCE_SIZE);
        crypto_encrypt(ciphertext, (unsigned char *)line, mlen, nonce, peer->k_tx);

        uint16_t net_clen = htons(clen);
        write_all(peer->fd, &net_clen, sizeof(net_clen));
        write_all(peer->fd, nonce, NONCE_SIZE);
        write_all(peer->fd, ciphertext, clen);

        free(ciphertext);
    }
    free(line);
    return NULL;
}