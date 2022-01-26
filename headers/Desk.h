#ifndef DESK_H
#define DESK_H
#include "MySocket.h"
#include "Mahjong.h"
#include <unordered_map>
#include <unordered_set>
#include <vector>
using namespace std;



struct ClientMsg {
//存储客户的信息，
    char IP[16];
    int Port;
    string name;
    int deskNum = -1;//用户对应的桌子号
    int num;//用户对应的座次号，0123
};

struct JudgeInDesk{
    int clientFd;
    int OPTION;
};
class Desk {
public:
    Desk();
    int beginNum = 0;//给这个桌子的用户座次,废弃
    
    int deskNum;//桌子号
    int remaindMahjong;//此桌剩余麻将
    unordered_map<int, ClientMsg*> client; //客户fd对应的客户信息
    unordered_set<int> clientNumInDesk;//分配东南西北，标记已经分配的编号
    unordered_map<int, int> numToClient;//座次对应的用户
    
    //返回当前桌子的在线人数
    int deskOnlineNum();
    int giveClientNumInDesk(); //给用户分配座次
    void delClientNumInDesk(int clientFd); //删除用户座次，腾空此位置
    void deal();//发牌
    int drawOne(int clientFd, int OPTION);
    int drawOne(int clientFd);//摸一个牌，就发一张牌给用户
    int drawOneToWhichLocation(int clientFd, int location);//给某个位置发牌，上家下家对家
    void fillMsg(Message &message, int clientFd);
    int tellDiscard(int clientFd);//告知用户出牌
    int forwardWhoAction(Message &recvMessage,int clientFd, int OPTION);//转发某人的动作

    int sendMsg(Message &message, int clientFd);

    void cleanDesk(int lastWinFd = 0);//清理桌子

    void cleanDeskTest(int lastWinFd = 0);//测试版本


    vector<int> mahjong;//桌子上的麻将

    int East = 0;//东
    int North = 1;
    int West = 2;
    int Sourth = 3;

    int lastWin = 0;//上一盘赢家
    int nomalCount = 0;//不做反应的操作
    vector<JudgeInDesk*> otherJudge;

    int whoDiscardLastFd = 0;//最后一个出牌的人
    int whichDiscardMahjong = 0;//最后打出的一个麻将

    void flashLocation();

    int handleJudge(Message &recvMessage,int clientFd, int OPTION);//处理操作

    static bool cmp(JudgeInDesk* a, JudgeInDesk *b);//sort的cmp参数，按照OPTION从大到小排序

private:
    void switchNum(int indexA, int indexB);

};


#endif