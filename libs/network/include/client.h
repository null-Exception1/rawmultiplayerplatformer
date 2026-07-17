#ifndef CLIENT_H
#define CLIENT_H
#include "config.h"
void init(int *socketfd);
void sendtoserver(int socketfd, Player playerinfo);
void recvfromserver(int socketfd, Player *playerinfo);
#endif