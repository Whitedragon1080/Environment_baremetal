#include "GameController.h"

GameController::GameController() : MatrixController(0), ChessController(){}

void GameController::init(){
    ChessController.init();
    MatrixController.init();
}
void GameController::handleGame(){
    while(!MatrixController.checkState());
    
    if(!ChessController.handleAction(MatrixController.getChange())){
        MatrixController.activateBuzzer();
        MatrixController.revert();
        ChessController.setReset(false);
    }
    chess::Movelist tmp = ChessController.getRelevantSquares();
    if(tmp.empty()){
        MatrixController.clearLED();
        ChessController.setReset(false);
        return;
    }
    MatrixController.setLEDs(tmp, 0);
}
