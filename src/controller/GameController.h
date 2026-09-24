#ifndef SRC_CONTROLLER_GAME_CONTROLLER_H
#define SRC_CONTROLLER_GAME_CONTROLLER_H

#include "ChessController.h"
#include "MatrixController.h"

class GameController{
    private:
    MatrixController MatrixController;
    ChessController ChessController;
    void endGame();
    bool gameOver;
    public:
    void handleGame();
        GameController();
        void init();
        bool isGameOver();
};

#endif