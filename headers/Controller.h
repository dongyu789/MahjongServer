#ifndef CONTROLLER_H
#define CONTROLLER_H


#include "Mahjong.h"    
#include "MySocket.h"
#include "Desk.h"
#include <unordered_map>

//OPTION操作
#define CLIENT_BREAK 1  //用户断开read error
#define CLIENT_CLOSED 2 //用户主动关闭，用户退出
#define SELECT_DESK 10  //选择桌子
#define SELECT_DESK_SUCCESS 11
#define SELECT_DESK_FAIL 12
#define DISCARD 13
#define DEAL 14 //发牌
#define DRAW 15 //摸牌
#define GAME_OVER 16
#define GAME_BEGIN 17
#define JUDGE 18 //每个人看牌是否需要处理
#define NORMAL 19
//碰杠吃胡
#define CHOW 20
#define PONG 21
#define KONG 22
#define WIN 23
#define ANKONG 24//暗杠
#define ANWIN 25//自摸

//玩家打印位置标识
#define LEFT_PLAYER 1
#define RIGHT_PLAYER 2
#define OPPOSITE_PLAYER 3
#define MYSELF_PLAYER 4



using namespace std;
typedef int deskId;

class Controller {
public:
    Controller();
    
    void start();//游戏启动，准备开始，准备数据
    void playGame();//正式开始游戏
    void openConnect();



    int acceptClient();//接收一个客户端加入 +
    int handleClientMsg(int clientFd);//处理客户端信息
    int clientQuit(int clientFd);//处理客户退出


    int handle(Message &message, int clientFd);//正常数据处理
    int selectDesk(Message &message, int clientFd);//选择桌子
    void fillMsg(Message &message, int clientFd, int ifName = 0);

    int sendMsg(Message &message, int clientFd);//发送处理信息OPTION是对应操作

    unordered_map<deskId, Desk*> allDesk;//每一个桌子号对应的桌子

    unordered_map<int, ClientMsg*> allClient;//每一个客户对应的信息


    void test();

    int buffSize = 0;

};

#endif