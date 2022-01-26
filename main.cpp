#include "Controller.h"
#include "MySocket.h"
#include "Desk.h"
#include "Mahjong.h"

using namespace std;

int main() {
    Controller *controller = new Controller();
    controller->start();
    int num = 0;
    while (1) {
        controller->playGame();
    }
    


    getchar();
    return 0;
}