#include <arpa/inet.h>
#include <client.h>
#include <config.h>
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
  int socketfd;
  init(&socketfd);
  Player playerinfo = {12, 100.0f, 100.0f, RIGHT_WALK, 1};
  sendtoserver(socketfd, playerinfo);

  while (1) {
    Player newplayerinfo;
    recvfromserver(socketfd, &newplayerinfo);

    printf("player info : %d %f %f %d %d \n", playerinfo.id, playerinfo.x,
           playerinfo.y, playerinfo.animframe, playerinfo.newfd);
  }
}