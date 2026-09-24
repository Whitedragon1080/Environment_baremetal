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
    }
    chess::Movelist tmp = ChessController.getRelevantSquares();
    if(ChessController.getReset()){
        MatrixController.clearLED();
        ChessController.setReset(false);
    }
}
