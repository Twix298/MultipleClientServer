#ifndef SERVER_H
#define SERVER_H

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <set>
#include <algorithm>
#include <sys/epoll.h>

#define MAX_EVENTS 32

class Server
{
public:
    Server();
    void initServer();
    int set_nonblock(int fd);
};

#endif // SERVER_H
