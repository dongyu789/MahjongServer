#include "Desk.h"
#include "MySocket.h"
#include <unordered_map>
#include <vector>
#include "Controller.h"
#include <time.h>
#include <stdlib.h>
using namespace std;

Desk::Desk(){
    
}

/**
 * @brief 给用户分配座次，用0123代替
 * 
 * @return int 成功返回0-3的编号，失败返回-1
 */
int Desk::giveClientNumInDesk() {
    int num = -1;
    for(int i = 0; i < 4; i++) {
        if(clientNumInDesk.find(i) == clientNumInDesk.end()) {
            clientNumInDesk.insert(i);
            num = i;
            break;
        }
    }
    return num;
}

void Desk::delClientNumInDesk(int clientFd) {
    int num = client[clientFd]->num;
    clientNumInDesk.erase(num);
    numToClient.erase(num);
    printf("删除座次%d\n", num);
}

void Desk::flashLocation() {
    East = lastWin;
    North = (East + 1) %4;
    West = (North + 1) %4;
    Sourth = (West +1 ) %4;
}

int Desk::deskOnlineNum() {
    return client.size();
}

void Desk::deal() {
    Desk *desk = this;
    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 4; j++) {
            Message message;
            drawOne(desk->numToClient[desk->East], DEAL);
        }
        for(int j = 0; j < 4; j++) {
            Message message;
            drawOne(desk->numToClient[desk->North], DEAL);
        }
        for(int j = 0; j < 4; j++) {
            Message message;
            drawOne(desk->numToClient[desk->West], DEAL);
        }
        for(int j = 0; j < 4; j++) {
            Message message;
            drawOne(desk->numToClient[desk->Sourth], DEAL);
        }
    }

    drawOne(desk->numToClient[desk->East], DEAL);
    drawOne(desk->numToClient[desk->East], DEAL);
    drawOne(desk->numToClient[desk->North], DEAL);
    drawOne(desk->numToClient[desk->West], DEAL);
    drawOne(desk->numToClient[desk->Sourth]);

    tellDiscard(desk->numToClient[desk->East]);
    
}

//输入上一盘赢家，清空桌子
void Desk::cleanDesk(int lastWinFd) {
    //清空所有麻将  ，填入新麻将
    mahjong.clear();
    //重新配置属性，但是用户不受影响
    for(int i = 0; i< 4; i++) {
        mahjong.push_back(ONEDOT);
        mahjong.push_back(TWODOT);
        mahjong.push_back(THREEDOT);
        mahjong.push_back(FOURDOT);
        mahjong.push_back(FIVEDOT);
        mahjong.push_back(SIXDOT);
        mahjong.push_back(SEVENDOT);
        mahjong.push_back(EIGHTDOT);
        mahjong.push_back(NINEDOT);
        mahjong.push_back(ONEBAMBOO);
        mahjong.push_back(TWOBAMBOO);
        mahjong.push_back(THREEBAMBOO);
        mahjong.push_back(FOURBAMBOO);
        mahjong.push_back(FIVEBAMBOO);
        mahjong.push_back(SIXBAMBOO);
        mahjong.push_back(SEVENBAMBOO);
        mahjong.push_back(EIGHTBAMBOO);
        mahjong.push_back(NINEBAMBOO);
        mahjong.push_back(ONECHARACTER);
        mahjong.push_back(TWOCHARACTER);
        mahjong.push_back(THREECHARACTER);
        mahjong.push_back(FOURCHARACTER);
        mahjong.push_back(FIVECHARACTER);
        mahjong.push_back(SIXCHARACTER);
        mahjong.push_back(SEVENCHARACTER);
        mahjong.push_back(EIGHTCHARACTER);
        mahjong.push_back(NINECHARACTER);
        mahjong.push_back(EAST);
        mahjong.push_back(SOUTH);
        mahjong.push_back(WEST);
        mahjong.push_back(NORTH);
        mahjong.push_back(RED);
        mahjong.push_back(GREEN);
        mahjong.push_back(WHITEDRAGON);
    }
    remaindMahjong = mahjong.size();

    srand(time(0));

    for(int i = 0; i < 1000; i++) {
        int indexA = rand() % remaindMahjong;
        int indexB = rand() % remaindMahjong;
        switchNum(indexA, indexB);
    }

    if(lastWinFd == 0) {
        lastWin = 0;
    } else {
        lastWin = client[lastWinFd]->num;
    }

    flashLocation();

    //发牌
    deal();
}

void Desk::cleanDeskTest(int lastWinFd) {
     //清空所有麻将  ，填入新麻将
    mahjong.clear();
    //重新配置属性，但是用户不受影响
    
    for(int i = 0; i< 200; i++) {
        mahjong.push_back(EAST);
    }
    remaindMahjong = mahjong.size();

    srand(time(0));

    for(int i = 0; i < 1000; i++) {
        int indexA = rand() % remaindMahjong;
        int indexB = rand() % remaindMahjong;
        switchNum(indexA, indexB);
    }

    if(lastWinFd == 0) {
        lastWin = 0;
    } else {
        lastWin = client[lastWinFd]->num;
    }

    flashLocation();

    //发牌
    deal();
}

void Desk::switchNum(int indexA, int indexB) {
    //交换ab下标的值
    int c = mahjong[indexA];
    mahjong[indexA]= mahjong[indexB];
    mahjong[indexB] = c;
}

int Desk::tellDiscard(int clientFd) {
    int deskNum = client[clientFd]->deskNum;
    Message message;
    
    for(auto iter = client.begin(); iter != client.end(); iter++) {
        fillMsg(message, iter->first);
        message.OPTION = DISCARD;
        message.nowNum = client[clientFd]->num;
        sendMsg(message, iter->first);
    }
}

int Desk::forwardWhoAction(Message &recvMessage, int clientFd, int OPTION) {
    Message message;
    message.nowNum = client[clientFd]->num;

    switch(OPTION) {
        //如果是discard
        case DISCARD:
            message.OPTION = JUDGE;
            break;
        case WIN:
            message.OPTION = WIN;
            memcpy(message.winPlayerMahjong, recvMessage.winPlayerMahjong, sizeof(message.winPlayerMahjong));
            message.winPlayerMahjongSize = recvMessage.winPlayerMahjongSize;
            break;
        case KONG:
            message.OPTION = KONG;
            break;
        case PONG:
            message.OPTION = PONG;
            break;
        case CHOW:
            message.OPTION = CHOW;   
            break;
        case ANKONG:
            message.OPTION = ANKONG;
            break;
        case ANWIN:
            memcpy(message.winPlayerMahjong, recvMessage.winPlayerMahjong, sizeof(message.winPlayerMahjong));
            message.winPlayerMahjongSize = recvMessage.winPlayerMahjongSize;
            message.OPTION = ANWIN;
            break;

    }

    printf("转发给所有人，通知%d正在%d, mahjong：%d\n",client[clientFd]->num, OPTION, recvMessage.mahjong);
    for(auto iter = client.begin(); iter != client.end(); iter++) {
        //转发给所有人
        if(iter->first != clientFd) {
            fillMsg(message, iter->first);
            message.mahjong = recvMessage.mahjong;
            message.nowNum = client[clientFd]->num;
            sendMsg(message, iter->first);
        } else if(message.OPTION != JUDGE) {
            //给自己发一份,通知自己可以进行碰杠吃赢
            fillMsg(message, iter->first);
            message.mahjong = recvMessage.mahjong;
            message.nowNum = client[clientFd]->num;
            sendMsg(message, iter->first);
        }
    }
}

bool Desk::cmp(JudgeInDesk *a, JudgeInDesk *b) {
    //大OPTINO在前
    if(a->OPTION != b->OPTION) {
        return a->OPTION > b->OPTION;
    }
}

int Desk::handleJudge(Message &recvMessage, int clientFd, int OPTION) {

    JudgeInDesk *judgeInDesk = new JudgeInDesk();
    judgeInDesk->clientFd = clientFd;
    judgeInDesk->OPTION = OPTION;

    if(OPTION == ANKONG || OPTION == ANWIN) {
        //某人暗杠或者自摸，不用等其他人，直接发送
        Message message;
        message.OPTION = OPTION;
        message.nowNum = client[clientFd]->num;
        message.mahjong = recvMessage.mahjong;
        if(OPTION == ANWIN) {
            memcpy(message.winPlayerMahjong, recvMessage.winPlayerMahjong, sizeof(message.winPlayerMahjong));
            message.winPlayerMahjongSize = recvMessage.winPlayerMahjongSize;
            printf("%d 号桌， num = %d， 自摸， 手牌数:%d 手牌是:", deskNum, message.nowNum, message.winPlayerMahjongSize);
            for(auto i :message.winPlayerMahjong) {
                printf("%d ",i);
            }
            printf("\n");
            
        }
        printf("%d号桌, num = %d, 暗杠/自摸 %d\n", deskNum, message.nowNum, message.mahjong);
        forwardWhoAction(message, clientFd, OPTION);
        if(OPTION == ANKONG) {
            //给这个发一张牌并且通知他继续出
            drawOne(clientFd);
            tellDiscard(clientFd);
        }
        return 0;
    }
    
    otherJudge.push_back(judgeInDesk);
    if(otherJudge.size() < 3) {
        return 0;
    }

    printf("其他三家回复完毕，执行下一步动作,开始处理回复\n");

    sort(otherJudge.begin(), otherJudge.end(), Desk::cmp);

    //取出优先级最高的，胡、杠、碰
    //处理所有的胡
    if(otherJudge.front()->OPTION == WIN) {
        
        for(auto i : otherJudge) {
            if(i->OPTION == WIN) {
                //胡了，转发给所有人
                Message message;
                memcpy(message.winPlayerMahjong, recvMessage.winPlayerMahjong, sizeof(message.winPlayerMahjong));
                message.winPlayerMahjongSize = recvMessage.winPlayerMahjongSize;
                message.OPTION = WIN;
                int clientFd = i->clientFd;//谁赢了
                message.nowNum = client[clientFd]->num;
                message.mahjong = whichDiscardMahjong;//赢了哪个牌
                printf("%d号桌，num = %d, 胡牌 %d\n", deskNum, message.nowNum, message.mahjong);
                forwardWhoAction(message, clientFd, WIN);
                //有人胡了，直接结束游戏
            }
        }
        printf("通知完毕，清除缓存otherJudge\n");
        otherJudge.clear();
        return WIN;
    }

    //处理所有的杠
    if(otherJudge.front()->OPTION == KONG) {
        for(auto i : otherJudge) {
            if(i->OPTION == KONG) {
                //杠了，发送给所有人
                Message message;
                message.OPTION = KONG;
                int clientFd = i->clientFd;
                message.nowNum = client[clientFd]->num;
                message.mahjong = whichDiscardMahjong;
                printf("%d号桌，num = %d, 杠牌 %d\n", deskNum, message.nowNum, message.mahjong);
                forwardWhoAction(message, clientFd, KONG);
                //杠的人起一张牌后出牌
                printf("杠结束，发给%d位置一张牌%d", message.nowNum, mahjong.back());
                printf("通知%d位置出牌\n", message.nowNum);
                drawOne(clientFd);
                tellDiscard(clientFd);
            }
        }
        printf("通知完毕，清除缓存otherJudge\n");
        otherJudge.clear();
        return KONG;
    }

    //处理所有的碰
    if(otherJudge.front()->OPTION == PONG) {
        for(auto i : otherJudge) {
            if(i->OPTION == PONG) {
                //杠了，发送给所有人
                Message message;
                message.OPTION = KONG;
                int clientFd = i->clientFd;
                message.nowNum = client[clientFd]->num;
                message.mahjong = whichDiscardMahjong;
                printf("%d号桌，num = %d, 碰牌 %d\n", deskNum, message.nowNum, message.mahjong);
                forwardWhoAction(message, clientFd, PONG);
                //碰的人不起直接出牌
                printf("通知%d位置出牌\n", message.nowNum);
                tellDiscard(clientFd);
            }
        }
        printf("通知完毕，清除缓存otherJudge\n");
        otherJudge.clear();
        return PONG;
    }

    //处理所有的吃（未实现）

    //全是Normal,给最后一个出牌的下家发牌
    printf("通知完毕，清除缓存otherJudge\n");
    otherJudge.clear();
    drawOneToWhichLocation(whoDiscardLastFd, RIGHT_PLAYER);
    

}

int Desk::drawOneToWhichLocation(int clientFd, int location) {

    //考虑了断线情况
    int nextClientFd;
    int nextNum;
    int myNum = client[clientFd]->num;
    int playerInDesk = deskOnlineNum();
    switch(location) {
        case LEFT_PLAYER:
            //发给上家
            nextNum = (myNum -1 + playerInDesk) % playerInDesk;
            while(clientNumInDesk.find(nextNum) == clientNumInDesk.end()) {
                nextNum = (nextNum -1 + playerInDesk) % playerInDesk;
            }
            nextClientFd = numToClient[nextNum];
            break;
        case RIGHT_PLAYER:
           //发给下家
            nextNum = (myNum +1) % playerInDesk;
            while(clientNumInDesk.find(nextNum) == clientNumInDesk.end()) {
                nextNum = (nextNum +1) % playerInDesk;
            }
            nextClientFd = numToClient[nextNum];
            break;
    }
    printf("myNum = %d, 下家num = %d\n", myNum, nextNum);
    printf("给%d号桌的 num = %d的用户发牌，牌号是%d\n", deskNum, nextNum, mahjong.back());
    drawOne(nextClientFd);
    tellDiscard(nextClientFd);

}

int Desk::drawOne(int clientFd, int OPTION) {
    for(auto iter = client.begin(); iter != client.end(); iter++) {
        Message message;
        fillMsg(message, iter->first);
        message.nowNum = client[clientFd]->num;
        message.OPTION = OPTION;
        if(iter->first == clientFd) {
            //发给本人真实牌
            if(mahjong.size() != 0) {
                message.mahjong = mahjong.back();
                mahjong.pop_back();
            } else {
                //牌发完了，游戏结束
                message.OPTION = GAME_OVER;
            }
        } else {
            message.mahjong = -1;
        }
        sendMsg(message, iter->first);
    }
}

int Desk::drawOne(int clientFd) {
    for(auto iter = client.begin(); iter != client.end(); iter++) {
        Message message;
        fillMsg(message, iter->first);
        message.nowNum = client[clientFd]->num;
        message.OPTION = DRAW;
        if(iter->first == clientFd) {
            //发给本人真实牌
            if(mahjong.size() != 0) {
                message.mahjong = mahjong.back();
                mahjong.pop_back();
            } else {
                //牌发完了，游戏结束
                message.OPTION = GAME_OVER;
            }
        } else {
            message.mahjong = -1;
        }
        sendMsg(message, iter->first);
    }
}

void Desk::fillMsg(Message &message, int clientFd) {
    message.myNum = client[clientFd]->num % 4;
    message.rightNum = (message.myNum +1) % 4;
    message.oppositeNum = (message.rightNum +1 ) %4;
    message.leftNum = (message.oppositeNum +1) %4;
    message.deskNum = deskNum;
    message.deskOnlineNum = client.size();
    message.remainMahjong = remaindMahjong;
}

int Desk::sendMsg(Message &message, int clientFd) {
    char buff[4096] = {0};
    memcpy(buff, &message, sizeof(message));
    send(clientFd, buff, sizeof(buff), 0);

    return 0;
}
