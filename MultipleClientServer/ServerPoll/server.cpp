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

    struct pollfd set[POLL_SIZE];
    set[0].fd = masterSocket;
    set[0].events = POLLIN;

    while(true) {
        unsigned int index = 1;
        for(auto it = slaveSocket.begin(); it != slaveSocket.end(); ++it) {
            set[index].fd = *it;
            set[index].events = POLLIN;
            index++;
        }

        unsigned int setSize = 1 + slaveSocket.size();
        poll(set, setSize, -1);

        for(unsigned int i = 0; i < setSize; ++i) {
            if(set[i].revents && POLLIN) {
                if(i) {
                    static char buf[1024];
                    int recvSize = recv(set[i].fd, buf, 1024, MSG_NOSIGNAL);
                    if(recvSize == 0 && errno != EAGAIN) {
                        shutdown(set[i].fd, SHUT_RDWR);
                        close(set[i].fd);
                        slaveSocket.erase(set[i].fd);
                    }
                    else if(recvSize > 0) {
                        std::cout << buf << std::endl;
                        send(set[i].fd, buf, recvSize, MSG_NOSIGNAL);
                    }
                }
                else {
                    int socket = accept(masterSocket, 0, 0);
                    set_nonblock(socket);
                    slaveSocket.insert(socket);
                }
            }
        }
    }
}
