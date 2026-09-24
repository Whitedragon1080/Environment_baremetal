#ifndef SRC_CONTROLLER_GAME_CONTROLLER_H
#define SRC_CONTROLLER_GAME_CONTROLLER_H

#include "ChessController.h"
#include "MatrixController.h"

class GameController{
    private:
        void handleGame();
        MatrixController MatrixController;
        ChessController ChessController;
        void endGame();
    public:
        GameController();
        void init();

};

#endif