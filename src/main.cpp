
#include "GameController.h"
using namespace chess;
Board board = Board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

Movelist moves;
movegen::legalmoves(moves, board);
Movelist pieceLegalMoves;
Square position = Square(Square::underlying::SQ_A2);
for (const auto &move : moves) {
    if(move.from() == position) {
        pieceLegalMoves.add(move);
        std::cout << uci::moveToUci(move) << std::endl;
    }
}

return 0;

int main () {
    GameController::GameController Controller = GameController();
    Controller.init();
    while(1){
        Controller.handleGame();
        if(Controller.isGameOver()){
            return 1;
        }
    }
    return 0;
}