#include "GameController.h"

GameController::GameController() : MatrixController(0), ChessController(){}

void GameController::init(){
    ChessController.init();
    MatrixController.init();
}

//the main function to use
void GameController::handleGame(){
    //reads the field until a change is detected
    while(!MatrixController.checkState());
    //hands the change to ChessController, only returns false when the change is not legal
    if(!ChessController.handleAction(MatrixController.getChange())){
        //error state, buzzer sounds and wait until the oldstate is restored
        MatrixController.activateBuzzer();
        MatrixController.revert();
        ChessController.setReset(false);
    }
    //get the fields that can be used
    chess::Movelist legalFields = ChessController.getRelevantSquares();
    //only empty in this point if Chesscontroller flushed the list, so the turn has to be over
    if(legalFields.empty()){
        //turn is over, turn off all LEDs and signal as much to the other controllers
        MatrixController.clearLED();
        ChessController.setReset(false);
        MatrixController.turnFinished();
        if(ChessController.isGameOver()){
            endGame();
        }
        return;
    }
    //otherwise, light up all possible squares, the figure can move to
    MatrixController.setLEDs(legalFields, 0);
}

void GameController::endGame(){
    MatrixController.setAllLEDs();
}