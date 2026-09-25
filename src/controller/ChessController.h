#ifndef SRC_CONTROLLER_CHESS_CONTROLLER_H
#define SRC_CONTROLLER_CHESS_CONTROLLER_H

#include "MatrixController.h"

using namespace chess;
class ChessController{
    private:
        chess::Square attackField;
        chess::Board board;
        bool isAttack;
        chess::Movelist relevantSquares;
        bool capturing;
        chess::Square capturedField;
        chess::Move capturingMove;
        bool reset;
        bool getMovesForSquare(chess::Square position);
    public:
        bool handleAction(chess::Square position);
        bool isLegalDestination(chess::Square position);
        void init();
        ChessController();
        chess::Movelist getRelevantSquares();
        bool getReset();
        void setReset(bool set);
        bool isGameOver();
        chess::Square getCapturedField();
        bool isCapturing();
        void finish();
};
#endif