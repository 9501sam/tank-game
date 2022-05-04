#ifndef __NETWORK_H__
#define __NETWORK_H__

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "tank.h"

struct action {
    tank tk;
};

int connect_to_serv(char *, int);

#endif /*__NETWORK_H__*/
