#include "client.h"

Client::Client()
{
    initClient();
}

void Client::initClient(){
    struct sockaddr_in peer;
    int s;
    int rc;
    std::string str = "asdfasd";
    char buf[1024];
    strcpy(buf, str.c_str());
    int nPort = 5054;
    peer.sin_family = AF_INET;
    peer.sin_port = htons(12345);
    peer.sin_addr.s_addr = inet_addr("127.0.0.1");
    s = socket(AF_INET, SOCK_STREAM, 0);

    if(s < 0) {
        perror("socket");
        exit(1);
    }

    rc = connect(s, (struct sockaddr *)&peer, sizeof(peer));
    if(rc) {
        perror("connect");
        exit(1);
    }
    std::cout << "send" << std::endl;
    rc = send (s, str.c_str(), 1024, MSG_NOSIGNAL);
    if(rc <= 0) {
        perror("recv");
        exit(1);
    }
}
