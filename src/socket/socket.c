#include "socket.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <arpa/inet.h>

static void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) return &(((struct sockaddr_in*)sa)->sin_addr);
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int tcp_listen(const char *port) {
  struct addrinfo hints = {0}, *res, *p;
  int yes = 1, rv, listener;
  hints.ai_family = AF_UNSPEC; hints.ai_socktype = SOCK_STREAM; hints.ai_flags = AI_PASSIVE;

  if ((rv = getaddrinfo(NULL, port, &hints, &res)) != 0) return -1;
  for(p = res; p; p = p->ai_next) {
        listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (listener < 0) continue; 
        setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes);
        if (bind(listener, p->ai_addr, p->ai_addrlen) == 0) break;
        close(listener);
 }
    freeaddrinfo(res);
    if (!p) return -1;
    if (listen(listener, 10) < 0) return -1;
    return listener;
}

int tcp_accept(int listener) {
    struct sockaddr_storage their_addr; socklen_t addrlen = sizeof their_addr;
    return accept(listener, (struct sockaddr*)&their_addr, &addrlen);
}

int tcp_connect(const char *host, const char *port) {
    struct addrinfo hints = {0}, *res, *p;
    int rv, sockfd;
    hints.ai_family = AF_UNSPEC; hints.ai_socktype = SOCK_STREAM;
    if ((rv = getaddrinfo(host, port, &hints, &res)) != 0) return -1;
    for (p = res; p; p = p->ai_next) {
        sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (sockfd < 0) continue;
        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == 0) break;
        close(sockfd);
    }
    freeaddrinfo(res);
    return p ? sockfd : -1;
}

ssize_t read_all(int fd, void *buf, size_t len) {
    size_t off = 0; ssize_t n;
    while (off < len && (n = read(fd, (char*)buf + off, len - off)) > 0) off += n;
    return n < 0 ? n : off;
}
ssize_t write_all(int fd, const void *buf, size_t len) {
    size_t off = 0; ssize_t n;
    while (off < len && (n = write(fd, (const char*)buf + off, len - off)) > 0) off += n;
    return n < 0 ? n : off;
}