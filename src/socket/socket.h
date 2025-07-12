#ifndef TAPIN_SOCKET_H
#define TAPIN_SOCKET_H

int tcp_listen(const char *port);
int tcp_accept(int listener);
int tcp_connect(const char *host, const char *port);

ssize_t read_all(int fd, void *buf, size_t len);
ssize_t write_all(int fd, const void *buf, size_t len);

#endif