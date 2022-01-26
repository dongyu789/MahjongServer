/**
 * @file MySocket.h
 * @author 胡飞宇
 * @brief 服务端socket
 * @version 0.1
 * @date 2022-01-22
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef MYSOCKET_H
#define MYSOCKET_H
#include <stdio.h>
#include <iostream>
#include <bits/stdc++.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
using namespace std;
//用来和服务器传输的消息
struct Message{
    int mahjong;//麻将编号
    int remainMahjong;//剩余的麻将
    int diceNum;//色子大小
    int myNum;//自己的编号
    int leftNum;//上家编号
    int rightNum;//下家编号
    int oppositeNum;//对家编号
    char myName[20] = {0};
    char leftName[20] = {0};
    char rightName[20] = {0};
    char oppositeName[20] = {0};
    bool Pong; //废弃
    bool Kong;  //废弃
    bool Chow;  //废弃
    bool Win;   //废弃
    int nowNum = -1;//这次出牌人的编号
    int allOnlineNum;//当前在线人数
    int deskNum;//玩家的桌号
    int deskOnlineNum;//当前桌子在线人数
    int OPTION;//此次通信进行的操作

    //赢字段
    int winPlayerMahjongSize = 0;
    int winPlayerMahjong[15];
    
    
};

class MySocket {
public:
    static int epfd;
    static int listenFd;
    static set<int> allClient;
    static sockaddr_in listenAddr;
    static epoll_event epev;
    static epoll_event epevs[1024];

    static int mySocketInit();//链接前准备,到监听端口

    static int mySocketWait();//开始等待链接






};




#endif