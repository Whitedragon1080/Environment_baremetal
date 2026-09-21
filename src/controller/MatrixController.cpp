#include "MatrixController.h"
#include "../startup/matrixinputStartup.cpp"
#include <cstdint>

MatrixController::MatrixController(int count, bool isAttack) : count(count), isAttack(isAttack), state(0b1111111111111111000000000000000000000000000000001111111111111111) { 

}

void MatrixController::init(){
    pinEN();
}

void MatrixController::setOutput(int pin, bool enabled){  
  if(enabled){
      GPIOD -> ODR |= (1 << pin);
  } else {
      GPIOD -> ODR &= ~(1 << pin);
  }    
}

uint64_t MatrixController::readMatrix() {
  uint64_t output = 0;
  for(int row = 0; row <= 7; row++){
      bool isRowSelectAEnabled = row == 0 || row == 1 || row == 2 || row == 4; //results in: 1,1,1,0,1,0,0,0
      bool isRowSelectBEnabled = row == 1 || row == 2 || row == 5 || row == 6; //results in: 0,1,1,0,0,1,1,0
      bool isRowSelectCEnabled = row == 0 || row == 2 || row == 6 || row == 7; //results in: 1,0,1,0,0,0,1,1
      
      setOutput(0, isRowSelectAEnabled);
      setOutput(1, isRowSelectBEnabled);
      setOutput(2, isRowSelectCEnabled);

      for(int col = 0; col <= 7; col++){
        bool isColSelectAEnabled = col == 0 || col == 2 || col == 4 || col == 7; //results in: 1,0,1,0,1,0,0,1
        bool isColSelectBEnabled = col == 2 || col == 3 || col == 6 || col == 7; //results in: 0,0,1,1,0,0,1,1
        bool isColSelectCEnabled = col >= 4;                                     //results in: 0,0,0,0,1,1,1,1
        
        setOutput(3, isColSelectAEnabled);
        setOutput(4, isColSelectBEnabled);
        setOutput(5, isColSelectCEnabled);

        bool reading = ((1 << 15) & GPIOD -> IDR) >> 15;
        int bitIndex = 63 - (row * 8 + col);
        output = (output & (~(uint64_t{1} << bitIndex))) | (uint64_t{reading} << bitIndex); // output[bitIndex] = reading;
      }
    }
    
    GPIOA->ODR |= 1<<5; // Set the Pin PA5

    GPIOA->ODR &= ~(1<<5); // Reset the Pin PA5

    return output;
}

void MatrixController::checkState(){
    uint64_t currentState = readMatrix();

    if(state != currentState){ // if it has changed
        count++;
        if(count >= 10 && !isAttack){
            isAttack = true;
            handleStateChange(state ^ currentState);
            state = currentState;
            count = 0;
        }
    } else {
        count--;
    }
}
void MatrixController::handleStateChange(uint64_t position){
    /* const position_int = concatZeroesUntilSizeMatches((num1 ^ num2).toString(2), 64).indexOf("1"); //because of the xor, theres a 1 only at the changed position
		const xPositionStr = String.fromCharCode("a".charCodeAt(0) + (position_int % 8));
		const yPositionStr = String.fromCharCode(position_int / 8 + 1);
		const positionInChessNotation = xPositionStr + yPositionStr; */ 
}