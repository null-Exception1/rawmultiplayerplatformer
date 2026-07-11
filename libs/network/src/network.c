#include "network.h"
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

static int yes = 1;

int find_addr_client(struct addrinfo *res, struct addrinfo **p,
                     struct sockaddr **addr, socklen_t *addrlength) {
  for (*p = res; *p != NULL; *p = (*p)->ai_next) {
    printf("Trying an address\n");
    int sockfd;
    if ((sockfd = socket((*p)->ai_family, (*p)->ai_socktype,
                         (*p)->ai_protocol)) == -1) {
      perror("server: socket");
      continue;
    }

    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
      perror("setsockopt");
      exit(1);
    }

    *addr = (*p)->ai_addr;
    *addrlength = (*p)->ai_addrlen;

    if (connect(sockfd, *addr, *addrlength) == -1) {

      close(sockfd);
      perror("client: connect");
      continue;
    }
    printf("We got an address!\n");
    return sockfd;
  }
}

int find_addr_server(struct addrinfo *res, struct addrinfo **p) {
  for (*p = res; *p != NULL; *p = (*p)->ai_next) {
    printf("Trying an address\n");
    int sockfd;
    if ((sockfd = socket((*p)->ai_family, (*p)->ai_socktype,
                         (*p)->ai_protocol)) == -1) {
      perror("server: socket");
      continue;
    }

    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
      perror("setsockopt");
      exit(1);
    }

    if (bind(sockfd, (*p)->ai_addr, (*p)->ai_addrlen) == -1) {
      close(sockfd);
      perror("server: bind");
      continue;
    }
    printf("We got an address!\n");
    return sockfd;
  }
}
