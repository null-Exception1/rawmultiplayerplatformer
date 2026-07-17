#include "client.h"
#include "network.h"
#include <netdb.h>
#include <string.h>
void init(int *socketfd) {
  struct addrinfo hints, *res, *p;
  int sockfd;
  int status;
  struct sockaddr *addr;
  socklen_t addrlength;
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  status = getaddrinfo("localhost", "3490", &hints, &res);
  *socketfd = find_addr_client(res, &p, &addr, &addrlength);
}

void sendtoserver(int socketfd, Player playerinfo) {
  send(socketfd, &playerinfo, sizeof(playerinfo), 0);
}
void recvfromserver(int socketfd, Player *playerinfo) {
  recv(socketfd, playerinfo, sizeof(*playerinfo), 0);
}
