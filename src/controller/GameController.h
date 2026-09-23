#ifndef SRC_CONTROLLER_GAME_CONTROLLER_H
#define SRC_CONTROLLER_GAME_CONTROLLER_H

#include "ChessController.h"
#include "MatrixController.h"

class GameController{
    private:
        void handleGame();
        MatrixController MatrixController;
        ChessController ChessController;
    public:
        GameController();
        void init();
};

#endif