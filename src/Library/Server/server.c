#include "server.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <poll.h>


void *get_in_addr(struct sockaddr *sa) 
{
    //get sockaddress, IPv4 or IPv6
    if  (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr); 
}

int get_server_info(struct addrinfo *hints, struct addrinfo **servinfo)
{

        int status;
        memset(hints, 0, sizeof *hints);
        hints->ai_family = AF_UNSPEC; //dont care IPv4 or IPv6
        hints->ai_socktype = SOCK_STREAM; //TCP socket streams
        hints->ai_flags = AI_PASSIVE; //auto fill my IP for me

        if ((status = getaddrinfo(NULL, PORT, hints, servinfo)) != 0) {
            fprintf(stderr, "selectserver: %s\n", gai_strerror(status));
            return -1;
        }

        return 0;
}

int create_bind_socket(struct addrinfo *servinfo)
{

        int listener, yes = 1;
        struct addrinfo *p;

        for(p = servinfo; p != NULL; p = p->ai_next) {
            listener = socket(p->ai_family, p ->ai_socktype, p->ai_protocol);
            if (listener < 0) {
                continue;
            }

            setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

            if (bind(listener, p->ai_addr, p->ai_addrlen) < 0) {
                close(listener);
                continue;
            }

            break; //successful conection if reached here
        }

        if (p == NULL) {
            fprintf(stderr, "server: failed to bind\n");
            return -1;
        }

        //Listen
        if (listen(listener, BACKLOG) == -1) {
            return -1;
        }

        return listener;
}

// Add a new file descriptor to the set
void add_to_pfds(struct pollfd **pfds, int newfd, int *fd_count, int *fd_size)
{
    // If we don't have room, add more space in the pfds array
    if (*fd_count == *fd_size) {
        *fd_size *= 2; // Double it

        struct pollfd *new_pfds = realloc(*pfds, sizeof(**pfds) * (*fd_size));
        if (!new_pfds) {
            perror("reallocating memory");
            return;
        }
        *pfds = new_pfds;
    }

    (*pfds)[*fd_count].fd = newfd;
    (*pfds)[*fd_count].events = POLLIN; // Check ready-to-read

    (*fd_count)++;
}

// Remove an index from the set
void del_from_pfds(struct pollfd pfds[], int i, int *fd_count)
{
    // Copy the one from the end over this one
    pfds[i] = pfds[*fd_count-1];

    (*fd_count)--;
}

