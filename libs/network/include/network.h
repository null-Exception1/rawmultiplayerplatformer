#ifndef NETWORK_H
#define NETWORK_H

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int find_addr_client(struct addrinfo *res, struct addrinfo **p,
                     struct sockaddr **addr, socklen_t *addrlength);
int find_addr_server(struct addrinfo *res, struct addrinfo **p);

#endif