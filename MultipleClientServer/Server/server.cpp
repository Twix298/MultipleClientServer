#include "server.h"

Server::Server()
{
    initServer();
}

int Server::set_nonblock(int fd) {
    int flags;
#if defined(O_NONBLOCK)
    if (-1 == (flags == fcntl(fd, F_GETFL, 0)))
        flags = 0;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
#else
    flags = 1;
    return ioctl(fd, FIOBIO, &flags);
#endif
}

void Server::initServer() {
    int masterSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    std::set<int> slaveSocket;

    struct sockaddr_in local;
    local.sin_family = AF_INET;
    local.sin_port = htons(12345);
    local.sin_addr.s_addr = htonl(INADDR_ANY);

    bind(masterSocket, (struct sockaddr *)&local, sizeof(local));
    set_nonblock(masterSocket); //неблокирующий сокет

    listen(masterSocket, SOMAXCONN);

    while(true) {
        fd_set set;
        FD_ZERO(&set);
        FD_SET(masterSocket, &set);
        for(auto it = slaveSocket.begin(); it != slaveSocket.end(); ++it) {
            FD_SET(*it, &set);
        }

        int max = std::max(masterSocket, *std::max_element(slaveSocket.begin(), slaveSocket.end()));
        select(max+1, &set, NULL, NULL, NULL);

        for(auto it = slaveSocket.begin(); it != slaveSocket.end(); ++it) {
            if(FD_ISSET(*it, &set)) {
                static char buf[1024];
                int recvSize = recv(*it, buf, 1024, MSG_NOSIGNAL);
                if(recvSize == 0 && errno != EAGAIN) {
                    shutdown(*it, SHUT_RDWR);
                    close(*it);
                    slaveSocket.erase(it);
                }
                else if( recvSize != 0) {
                    std::cout << buf << std::endl;
                    send(*it, buf, recvSize, MSG_NOSIGNAL);
                }
            }
        }
        if(FD_ISSET(masterSocket, &set)) {
            int socket = accept(masterSocket, 0,0);
            set_nonblock(socket);
            slaveSocket.insert(socket);
        }
    }
}
