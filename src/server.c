#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <network.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

static int yes = 1;

int main() {
  int sockfd;
  int status;
  static struct addrinfo hints, *res, *p;
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  status = getaddrinfo(NULL, "3490", &hints, &res);

  printf("%d\n", status);

  sockfd = find_addr_server(res, &p);

  char readable[INET6_ADDRSTRLEN];
  inet_ntop(AF_INET, &(((struct sockaddr_in *)p->ai_addr)->sin_addr), readable,
            sizeof readable);
  printf("%s\n", readable);

  printf("listening\n");

  if (listen(sockfd, 1) == -1) {
    perror("listen");
    exit(1);
  }

  struct sockaddr_storage client_addr;
  socklen_t addrlen = sizeof client_addr;

  int newfd = accept(sockfd, (struct sockaddr *)&client_addr, &addrlen);

  printf("conn made");

  char *msg = "wsp gamer";

  send(newfd, msg, strlen(msg), 0);
}