#ifndef SERVER_H
#define SERVER_H

#include <netinet/in.h>
#include <poll.h>

#define PORT "8080" //user connection port
#define BACKLOG 10 //number of pending connections queue will hold


void *get_in_addr(struct sockaddr *sa);
int get_server_info(struct addrinfo *hints, struct addrinfo **servinfo);
int create_bind_socket(struct addrinfo *servinfo);
void add_to_pfds(struct pollfd **pfds, int newfd, int *fd_count, int *fd_size);
void del_from_pfds(struct pollfd *pfds, int i, int *fd_count);

#endif
