#include "../Library/Client/client.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <arpa/inet.h>

int main(int argc, char *argv[]) {
  
    int sockfd;

    if (argc != 2) {
      fprintf(stderr, "usage: client hostname\n");
      exit(1);
    }
    const char *hostname = argv[1];

    sockfd = connection(hostname);
    if (sockfd == -1) { 
        exit(2);
    }

    close(sockfd);
    return 0;
}