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

static void *recv_loop(void *arg);
static void *send_loop(void *arg);

typedef struct {
    int fd;
    unsigned char k_rx[32];
    unsigned char k_tx[32];
} peer_t;

int main(int argc, char *argv[]) {
    int opt;
    const char *listen_port = NULL;
    const char *connect_host = NULL;    
    const char *connect_port = NULL;

    static struct option long_opts[] = {
        {"listen", required_argument, 0, 'l'},
        {"connect", required_argument, 0, 'c'},
        {0, 0, 0, 0}
    };
    while ((opt = getopt_long(argc, argv, "l:c:", long_opts, NULL)) != -1) {
        switch (opt) {
            case 'l': listen_port = optarg; break;
            case 'c': {
                char *p = strchr(optarg, ':');
                if (!p) { fprintf(stderr, "--connect <host:port> required\n"); exit(1);}
                *p = '\0';
                connect_host = optarg;
                connect_port = p + 1;
            } break;
            default: fprintf(stderr, "Usage: %s [--listen <port>] [--connect <host:port>]\n")
        }
    }
    if (sodium()<0) { fprintf(stderr, "Initialization failed\n"); return 1; }

    int sockfd, initiatior; 

    if (connect_host) {
        //send request
        sockfd = tcp_connect(connect_host, connect_port);
        if (sockfd <0) { perror("connect"); return 1; }
        initiatior = 1;
    }else if (listen_port) {
        //waiting for response
        int listener = tcp_listen(listen_port);
        if (listener<0) {perror("listen"); return 1; }
        sockfd = tcp_accept(listener);
        close(listener);
        initiatior=0;
    } else {
        fprintf(stderr, "Missing argument flag: use --listen <port> or --connect <host:port>\n");
        return 1;
    }

    peer_t peer; peer.fd =sockfd;
    if (TapIn(sockfd, initiatior, peer.k_rx, peer.k_tx) != 0) {
        close(sockfd); return 1;
    }

    pthread_t rx_thread, tx_thread;
    pthread_create(&rx_thread, NULL, recv_loop, &peer);
    pthread_create(&tx_thread, NULL, send_loop, &peer);

    pthread_join(tx_thread, NULL);
    pthread_cancel(rx_thread);
    close(sockfd);
    return 0;
}

static void *recv_loop(void *arg) {
    peer_t *p =arg;
    for (;;) {
        uint16_t nlen;
        if (read_all(p->fd, &nlen, 2) <= 0) break;
        size_t clen = ntohs(nlen);
        unsigned char nonce[24];
        if (read_all(p->fd, nonce, 24) <= 0) break;
        unsigned char *ciph = malloc(clen);
        if (read_all(p->fd, ciph, clen) <= 0) { free(ciph); break; }
        unsigned char *plain = malloc(clen - crypto_secretbox_MACBYTES);
        if (crypto_decrypt(plain, ciph, clen, nonce, p->k_rx) == 0) {
            fwrite(plain, 1, clen - crypto_secretbox_MACBYTES, stdout);
            fflush(stdout);
        }
        free(ciph); free(plain);
    }
    return NULL;
}

static void *send_loop(void *arg) {
    peer_t *p = arg;
    char *line = NULL; size_t cap = 0;
    while (getline(&line, &cap, stdin) != -1) {
        size_t mlen = strlen(line);
        size_t clen = mlen + crypto_secretbox_MACBYTES;
        unsigned char *ciph = malloc(clen);
        unsigned char nonce[24];
        randombytes_buf(nonce, 24);
        crypto_encrypt(ciph, (unsigned char*)line, mlen, nonce, p->k_tx);
        uint16_t nlen = htons(clen);
        write_all(p->fd, &nlen, 2);
        write_all(p->fd, nonce, 24);
        write_all(p->fd, ciph, clen);
        free(ciph);
    }
    free(line);
    return NULL;
}