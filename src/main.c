#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <getopt.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "socket/socket.h"
#include "crypto/crypto.h"
#include "synack/tapin.h"
#include "invite/invite.h"

#define NONCE_SIZE 24

static int get_local_ip(char *buffer, size_t buflen) {
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) return -1;

    struct sockaddr_in remote = {
        .sin_family = AF_INET,
        .sin_port = htons(53),
    };
    inet_pton(AF_INET, "8.8.8.8", &remote.sin_addr);

    if (connect(sock, (struct sockaddr *)&remote, sizeof(remote)) < 0) {
        close(sock);
        return -1;
    }

    struct sockaddr_in local;
    socklen_t len = sizeof(local);
    if (getsockname(sock, (struct sockaddr *)&local, &len) < 0) {
        close(sock);
        return -1;
    }

    inet_ntop(AF_INET, &local.sin_addr, buffer, buflen);
    close(sock);
    return 0;
}

typedef struct {
    int fd;
    unsigned char k_rx[SHARED_KEY_LEN];
    unsigned char k_tx[SHARED_KEY_LEN];
} peer_t;

static void *receive_loop(void *arg);
static void *send_loop(void *arg);

int main(int argc, char *argv[]) {
    const char *listen_port = NULL;
    const char *connect_host = NULL;
    const char *connect_port = NULL;
    const char *invite_code = NULL;
    const char *password = NULL;

    static struct option long_options[] = {
        {"listen",   required_argument, 0, 'l'},
        {"connect",  required_argument, 0, 'c'},
        {"invite",   required_argument, 0, 'i'},
        {"password", required_argument, 0, 'p'},
        {0, 0, 0, 0}
    };

    int option;
    while ((option = getopt_long(argc, argv, "l:c:i:p", long_options, NULL)) != -1) {
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
            case 'i':
                invite_code =optarg;
                break;
            case 'p':
                password =optarg;
                break;

            default: 
                fprintf(stderr, "Usage: %s [--listen <port>] [--connect <host:port>] [--invite <code>] [--password <secret>]\n", argv[0]);
                exit(1);
        }
    }
    if (sodium_init()<0) {
        fprintf(stderr, "Initialization failed\n");
        return 1;
    }

    int sockfd =-1;
    int initiator = 0; 

    if (invite_code && password) {
        char ip_str[INET_ADDRSTRLEN];
        char port_str[6];
        if (invite_parse(invite_code, password, ip_str, port_str) != 0) {
            fprintf(stderr, "Invalid invite code or password\n");
            return 1;
        }
        connect_host = ip_str;
        connect_port = port_str;

        sockfd = tcp_connect(connect_host, connect_port);
        if (sockfd < 0) {
            perror("connect");
            return 1;
        }
        initiator = 1;

    } else if (connect_host && connect_port) {
        sockfd = tcp_connect(connect_host, connect_port);
        if (sockfd < 0) {
            perror("connect");
            return 1;
        }
        initiator = 1;
    }else if (listen_port) {
        if (password) {
            char local_ip[INET_ADDRSTRLEN];
            if (get_local_ip(local_ip, sizeof local_ip) != 0) {
                strcpy(local_ip, "127.0.0.1");
            }

            char invite[INVITE_LEN];
            if (invite_generate(invite, sizeof invite, password, local_ip, listen_port) == 0) {
                printf("[+] Invite Code: %s\n", invite);
                printf("[+] Share with peer: --invite %s --password <secret>\n", invite);
            } else {
                fprintf(stderr, "[-] Failed to generate invite code\n");
            }
        }
        //waiting for response
        int listener = tcp_listen(listen_port);
        if (listener<0) {
            perror("listen");
            return 1;
        }
        sockfd = tcp_accept(listener);
        close(listener);
        initiator=0;
    } else {
        fprintf(stderr, "Missing argument flag: use --listen <port> or --connect <host:port>\n");
        return 1;
    }

    peer_t peer = { .fd = sockfd };
    if (tapped_in(sockfd, initiator, invite_code, password, peer.k_rx, peer.k_tx) != 0) {
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
        if (plaintext && decrypt(plaintext, ciphertext, clen, nonce, peer->k_rx) == 0) {
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
        encrypt(ciphertext, (unsigned char *)line, mlen, nonce, peer->k_tx);

        uint16_t net_clen = htons(clen);
        write_all(peer->fd, &net_clen, sizeof(net_clen));
        write_all(peer->fd, nonce, NONCE_SIZE);
        write_all(peer->fd, ciphertext, clen);

        free(ciphertext);
    }
    free(line);
    return NULL;
}