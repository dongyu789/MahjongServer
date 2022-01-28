#include "Controller.h"
#include "MySocket.h"
#include "Desk.h"
#include "Mahjong.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <signal.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>


using namespace std;

int main() {

    //开启守护进程
    printf("开启守护进程执行，请到./logs/文件夹下查看日志\n");

    // 创建子进程
    pid_t pid = fork();
    if(pid > 0) {
        exit(0);//父进程退出
    }
    printf("守护进程已启动，进程号:%d\n", getpid());
    
    // 子进程重新创建会话，脱离当前控制终端
    setsid();

    //设置掩码
    umask(022);

    //更改工作目录
    chdir("./log/");

    //关闭或者重定向文件描述符
    // //关闭
    // int fd = open("/dev/null", O_RDWR);
    // dup2(fd, STDIN_FILENO);
    // dup2(fd, STDOUT_FILENO);
    // dup2(fd, STDERR_FILENO);

    //重定向,打印输出到日志文件中
    int fd = open("./logs/mylog.log", O_RDWR | O_APPEND | O_CREAT);
    dup2(fd, STDIN_FILENO);
    dup2(fd, STDOUT_FILENO);
    dup2(fd, STDERR_FILENO);

    //启动服务器逻辑

    Controller *controller = new Controller();
    controller->start();
    int num = 0;
    while (1) {
        controller->playGame();
    }
    


    getchar();
    close(fd);
    return 0;
}
