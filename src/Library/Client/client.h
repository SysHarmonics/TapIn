#ifndef CLIENT_H
#define CLIENT_H

#include <netinet/in.h>

#define PORT "5000"
#define MAXDATASIZE 100 //max number of bytes to obtain at once

//prototypes

void *get_in_addr(struct sockaddr *sa);
int connection(const char *hostname);
void send_data(int sockfd);
void receive_data(int sockfd);

#endif