#include "ChessController.h"

ChessController::ChessController() : reset(false), isAttack(false), capturing(false), relevantSquares(), board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1") {

}

void ChessController::init(){
    
}

bool ChessController::isLegalDestination(chess::Square position){
    for(const auto& move: relevantSquares){
        if(move.to() == position){
            capturingMove = move;
            return true;
        }
    }
    return false;
}

bool ChessController::handleAction(chess::Square position){
    Piece piece = board.at(position);
    //if no attack is in progress
    if(!isAttack){
        //is the square empty in the virtual board, then the change has to be illegal
        if(piece == Piece::NONE){
            return false;
        }
        //get all possible moves of the board
        board.legalMoves(relevantSquares, board);
        chess::Movelist tmp = chess::Movelist();
        //filter out all moves not from the Square // TODO separate function :-(
        for(const auto &move : relevantSquares){
            if(move.from() == position){
                tmp.add(move);
            }
        }
        //if no possible moves from this square, then false
        if(tmp.empty()){
            return false;
        }
        //give out the filtered list, set the current square as attackingField and initalize attack
        relevantSquares = tmp;
        attackField = position;
        isAttack = true;
        return true;
    }
    //if attack is already before in progress
    if(position == attackField){
        return true;
    }
    if(!isLegalDestination(position)){
        return false;
    }
    if(piece != Piece::NONE){
    if(!capturing){
        capturedField = position;
        capturing = true;
        return true;
    }
    if(position != capturedField){
        return false;
    }
    capturing = false; 
    }
    board.move(capturingMove);
    isAttack = false;
    reset = true;
    return true;
}

chess::Movelist ChessController::getRelevantSquares(){
    return relevantSquares;
}

bool ChessController::getReset(){
    return reset;
}

void ChessController::setReset(bool set){
    reset = set;
}