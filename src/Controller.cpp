#include "Controller.h"
#include "MySocket.h"
#include "Desk.h"
#include "Mahjong.h"
#include <iostream>
#include <stdio.h>
using namespace std;

void Controller::start() {
    printf("服务器已启动\n");
    MySocket::mySocketInit();
}

//操作时进行循环函数
void Controller::playGame() {

    while(1) {
        openConnect();
    }
}


void Controller::openConnect() {
    int ret = MySocket::mySocketWait();
    printf("ret = %d\n", ret);
    for(int i = 0; i < ret; i++) {
        if(MySocket::epevs[i].data.fd == MySocket::listenFd) {
            //新用户接入
            acceptClient();
        } else {
            int clientFd = MySocket::epevs[i].data.fd;
            //用户发消息
            handleClientMsg(clientFd);

            //根据处理结果给对应的用户发消息
        }
    }
}

int Controller::acceptClient() {
    sockaddr_in clientAddr;
    int len = sizeof(clientAddr);
    int clientFd = accept(MySocket::listenFd, (sockaddr*)&clientAddr, (socklen_t*)&len);
    if(clientFd == -1) {
        perror("accpet");
        return -1;
    }

    ClientMsg *clientMsg = new ClientMsg();
    clientMsg->Port = ntohs(clientAddr.sin_port);
    inet_ntop(AF_INET, &clientAddr.sin_addr.s_addr, clientMsg->IP, sizeof(clientMsg->IP));
    //给用户fd和用户信息捆绑
    allClient[clientFd] = clientMsg;

    MySocket::epev.events = EPOLLIN;
    MySocket::epev.data.fd = clientFd;
    epoll_ctl(MySocket::epfd, EPOLL_CTL_ADD, clientFd, &(MySocket::epev));
    printf("新用户接入: clientFd : %d IP : %s Port : %d\n", clientFd, clientMsg->IP, clientMsg->Port);
    
    return 0;
}

int Controller::handleClientMsg(int clientFd) {
    
    Message message;
    char buff[4096] = {0};
    int len = recv(clientFd, buff, sizeof(buff), 0);
    memset(&message, 0, sizeof(message));
    memcpy(&message, buff, sizeof(message));
    printf("len : %d\n", len);
    if(len == -1) {
        printf("client %d recv error\n", clientFd);
        clientQuit(clientFd);
    } else if(len == 0) {
        printf("client %d quit..\n", clientFd);
        clientQuit(clientFd);
    } else if(len > 0){
        //正常数据处理
        handle(message,clientFd);
    }
    return 0;
}

//处理用户退出
int Controller::clientQuit(int clientFd) {
    //给桌子上的其他人发消息说这人退出了
    if(allClient[clientFd]->deskNum != -1) {
        //如果在游戏中，则给其他人发消息说此人退出
        int deskNum = allClient[clientFd]->deskNum;
        //遍历整张桌子,给其他人发消息
        for(auto iter = allDesk[deskNum]->client.begin(); iter != allDesk[deskNum]->client.end(); iter++) {
            if(iter->first != clientFd) {
                Message message;
                message.OPTION = CLIENT_CLOSED;
                message.nowNum = allClient[clientFd]->num;
                sendMsg(message, iter->first);
            }
        }
        //从桌子里退出,把桌子里存储的消息删除
        
        printf("删除座次:%d\n",allClient[clientFd]->num);
        //这两的顺序不能返
        allDesk[deskNum]->delClientNumInDesk(clientFd);
        allDesk[deskNum]->client.erase(clientFd);
        if(allDesk[deskNum]->client.size() == 0) {
            //桌子没人了
            delete(allDesk[deskNum]);
            allDesk.erase(deskNum);
            printf("%d号桌解散\n",deskNum);
        }
    }
    //处理此用户数据，删除fd对应的msg，关闭fd
    delete allClient[clientFd];
    allClient.erase(clientFd);
    epoll_ctl(MySocket::epfd, EPOLL_CTL_DEL, clientFd ,NULL);
    close(clientFd);
}


int Controller::sendMsg(Message &message, int clientFd) {
    
    char buff[4096] = {0};
    memcpy(buff, &message, sizeof(message));
    send(clientFd, buff, sizeof(buff), 0);

    return 0;
}

//处理正常的数据
int Controller::handle(Message &message, int clientFd) {
    int option = message.OPTION;
    printf("desk : %d, clientNum : %d, mahjong : %d \n", allClient[clientFd]->deskNum, allClient[clientFd]->num, message.mahjong);
    printf("OPTION :%d\n", message.OPTION);
    int deskNum;
    Desk *desk;
    deskNum = allClient[clientFd]->deskNum;
    desk = allDesk[deskNum];

    switch(option) {
        case SELECT_DESK:
            selectDesk(message, clientFd);
            break;
        case DRAW:
            //无
            break;
        case DISCARD:
            //1 转发给所有用户，等待回复判断其他人是否碰杠吃胡
            desk->whoDiscardLastFd = clientFd;
            desk->whichDiscardMahjong = message.mahjong;
            desk->forwardWhoAction(message, clientFd, DISCARD);
            break;
        case NORMAL:
            //2 如果无操作
            desk->handleJudge(message, clientFd, NORMAL);
            break;
        case PONG:
            //3 如果有碰，
            desk->handleJudge(message, clientFd, PONG);
            break;
        case KONG:
            //4 如果有杠
            desk->handleJudge(message, clientFd, KONG);
            break;
        case CHOW:
            //5 如果有吃
            desk->handleJudge(message, clientFd, CHOW);
            break;
        case WIN:
            //6 如果有胡
            desk->handleJudge(message, clientFd, WIN);
            break;
        case ANKONG:
            //7 如果暗杠
            desk->handleJudge(message, clientFd, ANKONG);
            break;
        case ANWIN:
            //8 如果自摸
            desk->handleJudge(message, clientFd, ANWIN);
            break;


    }
    return 0;
}




int Controller::selectDesk(Message &message, int clientFd) {
    int deskNum = message.deskNum;
    if(allDesk.find(deskNum) == allDesk.end()) {
        //没有桌子就创建,并且绑定
        printf("创建桌子,deskNum = %d\n",deskNum);
        Desk *desk = new Desk();
        desk->deskNum = deskNum;
        allDesk[deskNum] = desk;
        desk->remaindMahjong = 100000;//初始麻将
    } 

    Desk *desk = allDesk[deskNum];

    if(desk->client.size() < 4) {
        printf("加入前桌子有%ld人，client = %d 正在加入桌子%d\n",desk->client.size(), clientFd, deskNum);
        //桌子不满可以加入
        desk->client[clientFd] = allClient[clientFd];
        allClient[clientFd]->name = message.myName;
        allClient[clientFd]->deskNum = deskNum;
        allClient[clientFd]->num = desk->giveClientNumInDesk();
        desk->numToClient[allClient[clientFd]->num] = clientFd;
        if(allClient[clientFd]->num == -1) {
            printf("给用户分配座次失败\n");
            return -1;
        }
        Message message;
        //通知桌子上的所有人，加入成功
        printf("通知桌子上的所有人，有人加入桌子\n");
        for (auto iter = desk->client.begin(); iter != desk->client.end(); iter++){
            message.OPTION = SELECT_DESK_SUCCESS;
            fillMsg(message, iter->first);
            message.nowNum = allClient[clientFd]->num;
            strcpy(message.myName, allClient[clientFd]->name.c_str());
            printf("准备发送信息\n");
            sendMsg(message, iter->first);
        }
        printf("加入桌子成功,通知完毕, clientFd :%d ,座次为 : %d 姓名为 : %s\n", clientFd, allClient[clientFd]->num, allClient[clientFd]->name.c_str());
        sleep(1);
        if(desk->deskOnlineNum() == 4) {
            //填充姓名信息
            Message message;
            for(auto iter = desk->client.begin(); iter != desk->client.end(); iter++) {
                message.OPTION = 0;
                fillMsg(message, iter->first, 1);
                sendMsg(message, iter->first);
            }

            printf("%d号桌已满，准备发牌\n", desk->deskNum);
            sleep(3);

            //测试点位
            desk->cleanDeskTest(0);
            //desk->cleanDesk(0);
        }
    } else {
        //桌子满了，不可以加入
        printf("%d号桌已满，clientFd %d 加入失败\n", deskNum, clientFd);
        Message message;
        fillMsg(message, clientFd);
        message.OPTION = SELECT_DESK_FAIL;
        sendMsg(message, clientFd);
    }
    return 0;
}



//打牌时便于传参
void Controller::fillMsg(Message &message, int clientFd, int ifName) {
    message.myNum = allClient[clientFd]->num % 4;
    message.rightNum = (message.myNum +1) % 4;
    message.oppositeNum = (message.rightNum +1 ) %4;
    message.leftNum = (message.oppositeNum +1) %4;
    message.deskNum = allClient[clientFd]->deskNum;
    message.deskOnlineNum = allDesk[message.deskNum]->client.size();
    message.remainMahjong = allDesk[message.deskNum]->remaindMahjong;
    if(ifName == 0) {
        return;
    }
    Desk *desk = allDesk[allClient[clientFd]->deskNum];
    int fd;
    //上家
    fd = desk->numToClient[message.leftNum];
    strcpy(message.leftName, desk->client[fd]->name.c_str());
    //下家
    fd = desk->numToClient[message.rightNum];
    strcpy(message.rightName, desk->client[fd]->name.c_str());
    //对家
    fd = desk->numToClient[message.oppositeNum];
    strcpy(message.oppositeName, desk->client[fd]->name.c_str());
    //自己
    strcpy(message.myName, desk->client[clientFd]->name.c_str());

    printf("姓名转发：num1 : %d %s num2 : %d %s , num3 %d %s , num4 %d %s\n",message.myNum, message.myName, message.leftNum, message.leftName, message.rightNum, message.rightName, message.oppositeNum, message.oppositeName);

}


Controller::Controller() {
    buffSize = sizeof(Message);
}
