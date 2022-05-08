#ifndef __NETWORK_H__
#define __NETWORK_H__

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

struct action {
};

void *network_thread(void *);

#endif /*__NETWORK_H__*/
