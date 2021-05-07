#ifndef CLIENT_H
#define CLIENT_H

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <string.h>
#include <arpa/inet.h>


class Client
{
public:
    Client();
    void initClient();
};

#endif // CLIENT_H
