#include "server.h"

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

Server::Server()
{
    initServer();
}


void Server::initServer() {
    int masterSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    struct sockaddr_in local;
    local.sin_family = AF_INET;
    local.sin_port = htons(12345);
    local.sin_addr.s_addr = htonl(INADDR_ANY);

    bind(masterSocket, (struct sockaddr *)&local, sizeof(local));
    set_nonblock(masterSocket); //неблокирующий сокет

    listen(masterSocket, SOMAXCONN);

    int ePoll = epoll_create1(0);

    struct epoll_event event;
    event.data.fd = masterSocket;
    event.events = EPOLLIN;
    epoll_ctl(ePoll, EPOLL_CTL_ADD, masterSocket, &event);

    while(1) {
        struct epoll_event events[MAX_EVENTS];
        int n = epoll_wait(ePoll, events, MAX_EVENTS, -1);

        for( unsigned int i = 0; i < n; ++i) {
            if(events[i].data.fd == masterSocket) {
                int socket = accept(masterSocket, 0, 0);
                set_nonblock(socket);
                struct epoll_event event;
                event.data.fd = socket;
                event.events = EPOLLIN;
                epoll_ctl(ePoll, EPOLL_CTL_ADD, socket, &event);
            }
            else {
                static char buf[1024];
                int recvSize = recv(events[i].data.fd, buf, 1024, MSG_NOSIGNAL);
                if(recvSize == 0 && errno != EAGAIN) {
                    shutdown(events[i].data.fd, SHUT_RDWR);
                    close(events[i].data.fd);
                }
                else if(recvSize > 0) {
                    std::cout << buf << std::endl;
                    send(events[i].data.fd, buf, recvSize, MSG_NOSIGNAL);
                }
            }
        }
    }
}
