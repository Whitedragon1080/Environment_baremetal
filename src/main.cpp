
#include "GameController.h"
using namespace chess;


int main () {
    GameController::GameController Controller = GameController();
    Controller.init();
    while(1){
        Controller.handleGame();
        if(Controller.isGameOver()){
            return 1;
        }
    }
    return 0;
}