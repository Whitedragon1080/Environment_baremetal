#include "GameController.h"

GameController::GameController() :gameOver(false), matrix(0), chess(){}

void GameController::init(){
    chess.init();
    matrix.init();
}

//the main function to use
void GameController::handleGame(){
    //reads the field until a change is detected
    while(!matrix.checkState());
    //hands the change to ChessController, only returns false when the change is not legal
    if(!chess.handleAction(matrix.getChange())){
        //error state, buzzer sounds and wait until the oldstate is restored
        matrix.activateBuzzer();
        matrix.revert();
        chess.setReset(false);
    }
    if(chess.isCapturing()){
        matrix.clearLED();
        matrix.turnOnLED(chess.getCapturedField());
    }
    //get the fields that can be used
    chess::Movelist legalFields = chess.getRelevantSquares();
    //only empty in this point if Chesscontroller flushed the list, so the turn has to be over
    if(legalFields.empty()){
        //turn is over, turn off all LEDs and signal as much to the other controllers
        matrix.clearLED();
        chess.setReset(false);
        if(chess.isGameOver()){
            endGame();
        }
        return;
    }
    //otherwise, light up all possible squares, the figure can move to
    matrix.setLEDs(legalFields, 0);
}

void GameController::endGame(){
    matrix.setAllLEDs();
    gameOver = true;
}

bool GameController::isGameOver(){
    return gameOver;
}