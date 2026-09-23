#include "../include/chess.hpp"

using namespace chess;

int main () {
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
}