#include "MatrixController.h"
#include "../startup/matrixinputStartup.cpp"

MatrixController::MatrixController(int count, bool isAttack) : count(count), isAttack(isAttack), state{{1,1,1,1,1,1,1,1} , {1,1,1,1,1,1,1,1}, {}, {}, {}, {}, {1,1,1,1,1,1,1,1}, {1,1,1,1,1,1,1,1} } { 

}

void MatrixController::init(){
    pinEN();
}

void MatrixController::readMatrix() {
    int tmp[8][8] = {};
    
}