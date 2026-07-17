#include <arpa/inet.h>
#include <config.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <network.h>
#include <pthread.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <threads.h>
#include <unistd.h>

static int yes = 1;
static int sockfd;
static int lim_players = 10;
static int num_players = 0;
static Player *players;
static atomic_bool mutex = false;

void *accepting_clients() {
  while (1) {
    struct sockaddr_storage client_addr;
    socklen_t addrlen = sizeof client_addr;

    int newfd = accept(sockfd, (struct sockaddr *)&client_addr, &addrlen);
    if (newfd == -1)
      continue;

    printf("conn made\n");

    Player playerinfo;
    recv(newfd, &playerinfo, sizeof(Player), 0);
    playerinfo.newfd = newfd;
    printf("player info : %d %f %f %d %d \n", playerinfo.id, playerinfo.x,
           playerinfo.y, playerinfo.animframe, playerinfo.newfd);

    while (atomic_exchange(&mutex, true)) {
      usleep(1);
    }
    players[num_players] = playerinfo;
    num_players++;

    if (num_players >= lim_players) {
      Player *temp = realloc(players, sizeof(Player) * lim_players * 2);
      if (temp) {
        players = temp;
        lim_players *= 2;
      }
    }
    atomic_store(&mutex, false);
  }
  return NULL;
}

int main() {
  int status;
  static struct addrinfo hints, *res, *p;
  players = (Player *)malloc(sizeof(Player) * lim_players);

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

  if (listen(sockfd, 10) == -1) {
    perror("listen");
    exit(1);
  }

  pthread_t accepting_thread;
  pthread_create(&accepting_thread, NULL, accepting_clients, 0);

  while (1) {
    usleep(16666); // 60 Hz relay rate
    if (num_players >= 2) {
      for (int i = 0; i < num_players; i++) {
        while (atomic_exchange(&mutex, true)) {
          usleep(1);
        }

        int actual_fd = players[i].newfd;
        Player player;

        int bytes = recv(actual_fd, &player, sizeof(player), MSG_DONTWAIT);

        if (bytes > 0) {
          player.newfd = actual_fd;
          players[i] = player;

          for (int j = 0; j < num_players; j++) {
            if (i != j) {
              Player player2 = players[j];
              send(player2.newfd, &player, sizeof(player), 0);
            }
          }
        } else if (bytes == 0 ||
                   (bytes < 0 && errno != EAGAIN && errno != EWOULDBLOCK)) {
          printf("Player ID %d dropped from fd %d. Removing slot.\n",
                 players[i].id, actual_fd);
          close(actual_fd);
          for (int k = i; k < num_players - 1; k++) {
            players[k] = players[k + 1];
          }
          num_players--;
          i--;
        }

        atomic_store(&mutex, false);
      }
    }
  }
}
