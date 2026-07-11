#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <network.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

static int yes = 1;

int main(int argc, char *argv[]) {
  struct addrinfo hints, *res, *p;
  int sockfd;
  int status;
  struct sockaddr *addr;
  socklen_t addrlength;
  char buff[500];

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  status = getaddrinfo("localhost", "3490", &hints, &res);

  sockfd = find_addr_client(res, &p, &addr, &addrlength);

  printf("%d\n", status);

  int bytelen = recv(sockfd, buff, sizeof(buff) - 1, 0);

  if (bytelen < 0) {
    perror("recv");
  } else if (bytelen == 0) {
    printf("Connection closed by peer.\n");
  } else {
    buff[bytelen] = '\0';
    printf("Received string: %s\nBytes received: %d\n", buff, bytelen);
  }
}