#include <stdio.h>
#include <iostream>
#include <bits/stdc++.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include "MySocket.h"
#include "Controller.h"
#include "Desk.h"
using namespace std;

int MySocket::epfd;
sockaddr_in MySocket::listenAddr;
int MySocket::listenFd;
epoll_event MySocket::epev;
epoll_event MySocket::epevs[1024];
set<int> MySocket::allClient;

int MySocket::mySocketInit() {
    int ret;
    //socket
    listenFd = socket(AF_INET, SOCK_STREAM, 0);
    if(listenFd == -1) {
        perror("listren");
        return -1;
    }

    //端口复用技术，服务器端口9999也可以进行绑定
    int optval = 1;
    setsockopt(listenFd, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));

    //bind
    listenAddr.sin_family = AF_INET;
    listenAddr.sin_port = htons(9998);
    inet_pton(AF_INET, "0.0.0.0", &listenAddr.sin_addr.s_addr);
    ret = bind(listenFd, (sockaddr*)&listenAddr, sizeof(listenAddr));
    if(ret == -1) {
        perror("bind");
        return -1;
    }

    //加入epoll
    epfd = epoll_create(2000);
    epev.events = EPOLLIN;
    epev.data.fd = listenFd;
    epoll_ctl(epfd, EPOLL_CTL_ADD, listenFd, &epev);

    //listen
    ret = listen(listenFd, 8);
    if(ret == -1) {
        perror("listen");
        return -1;
    }

}

int MySocket::mySocketWait() {
    int ret = epoll_wait(epfd, epevs, 1024, -1);//阻塞通信
    if(ret == -1) {
        perror("epoll wait");
        return -1;
    }
    return ret;
}