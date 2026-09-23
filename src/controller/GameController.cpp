#include "GameController.h"

GameController::GameController() : MatrixController(0), ChessController(){}

void GameController::init(){
    ChessController.init();
    MatrixController.init();
}
void GameController::handleGame(){
    while(!MatrixController.checkState());
    
    if(!ChessController.handleAction(MatrixController.getChange())){
        MatrixController.soundBuzzer();
        MatrixController.revert();
    }
    chess::Movelist tmp = ChessController.getRelevantSquares();
    if(ChessController.getReset()){
        MatrixController.clearLED();
    }
}
