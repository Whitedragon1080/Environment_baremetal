#ifndef SRC_CONTROLLER_CHESS_CONTROLLER_H
#define SRC_CONTROLLER_CHESS_CONTROLLER_H

#include "MatrixController.h"

using namespace chess;
class ChessController{
    private:
        chess::Square attackField;
        chess::Board board;
        bool isAttack;
        bool handleAction(chess::Square position);
        chess::Movelist relevantSquares;
        bool capturing;
        chess::Square capturedField;
        chess::Move capturingMove;
        bool reset;
    public:
        bool isLegalDestination(chess::Square position);
        void init();
        ChessController();
        chess::Movelist getRelevantSquares();
        bool getReset();
        
};
#endif