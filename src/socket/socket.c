#define _POSIX_C_SOURCE 200809L
#include "socket.h"
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define BACKLOG 10 //max number of pending connections

__attribute__((unused)) static inline void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) { 
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int tcp_listen(const char *port) {
  struct addrinfo hints = {0}, *res, *p;
  int yes = 1, listener =-1;

  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  if (getaddrinfo(NULL, port, &hints, &res) != 0)
    return -1;

  for(p = res; p; p = p->ai_next) {
        listener = socket(
            p->ai_family,
            p->ai_socktype,
            p->ai_protocol);

        if (listener < 0) continue;

        setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes);
        if (bind(listener, p->ai_addr, p->ai_addrlen) == 0)
        break;

        close(listener);
        listener =-1;
    }
    freeaddrinfo(res);
    if (!p || listen(listener, BACKLOG) < 0) {
        if  (listener >= 0) close(listener);
        return -1;
    }
    return listener;
}

int tcp_accept(int listener) {
    struct sockaddr_storage their_addr;
    socklen_t addrlen = sizeof their_addr;
    return accept(listener, (struct sockaddr*)&their_addr, &addrlen);
}

int tcp_connect(const char *host, const char *port) {
    struct addrinfo hints = {0}, *res, *p;
    int sockfd =-1;

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(host, port, &hints, &res) != 0)
        return -1;

    for (p = res; p; p = p->ai_next) {
        sockfd = socket(p->ai_family,
                        p->ai_socktype,
                        p->ai_protocol
                        );
        if (sockfd < 0) continue;
        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == 0)
            break;
        close(sockfd);
        sockfd=-1;
    }
    freeaddrinfo(res);
    return sockfd;
}

ssize_t read_all(int fd, void *buf, size_t len) {
    struct pollfd fds[1];
    fds[0].fd = fd;
    fds[0].events = POLLIN;
    fds[0].revents = 0;
    
    size_t off = 0; 
    ssize_t n;
    const int VALID_FILE_DESCRIPTORS = 1;
    const int TIMEOUT = 0;
    const int TIMEOUT_MS = 10000; // 10 seconds
    const int ERROR = -1;
    int poll_status = poll(fds,1,TIMEOUT_MS);
   
    if (poll_status < VALID_FILE_DESCRIPTORS){
      fprintf(stderr, "File descriptors received no input. Errno value: %d\n", errno);
      return errno;
    } 
    
    if (poll_status == TIMEOUT) {
      fprintf(stderr, "Connection timedout. Errno value: %d\n", errno);
      return errno;
    }
    
    if (poll_status == ERROR) {
      fprintf(stderr, "Error occurred. Errno value: %d\n", errno);
      return errno;
    } 

    while (off < len) {
      n = read(fd, (char*)buf + off, len - off);
      if (n <= 0) return n;
      off += n;
    }
    return off; 
}
ssize_t write_all(int fd, const void *buf, size_t len) {
    size_t off = 0;
    ssize_t n;
    while (off < len) {
        n = write(fd, (const char*)buf + off, len - off);
        if (n <= 0) return n;
    off +=n;
    }
    return off;
}
