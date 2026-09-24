#include "MatrixController.h"
#include "../startup/matrixinputStartup.cpp"
#include <cstdint>

//controller is initialized with the state how a chessboard should start
MatrixController::MatrixController(int count, bool isAttack) : count(count), capturing(false), state(0b1111111111111111000000000000000000000000000000001111111111111111) { 

}
//activate the pins needed for communication
void MatrixController::init(){
    pinEN();
    oldState = state;
}
//set the bit of the respective Pin, if its a D pin
void MatrixController::setOutput(int pin, bool enabled){  
  if(enabled){
      GPIOD -> ODR |= (1 << pin);
  } else { 
      GPIOD -> ODR &= ~(1 << pin);
  }    
}
//read the matrix and notify if a change persists for 10 readings
//TODO: Change implementation to fit hardware
uint64_t MatrixController::readMatrix() {
  uint64_t output = 0;
  for(int row = 0; row <= 7; row++){
      bool isRowSelectAEnabled = row == 0 || row == 1 || row == 2 || row == 4; //results in: 1,1,1,0,1,0,0,0
      bool isRowSelectBEnabled = row == 1 || row == 2 || row == 5 || row == 6; //results in: 0,1,1,0,0,1,1,0
      bool isRowSelectCEnabled = row == 0 || row == 2 || row == 6 || row == 7; //results in: 1,0,1,0,0,0,1,1
      //relevant for Multiplexer, selects the row thats to be read
      setOutput(0, isRowSelectAEnabled);
      setOutput(1, isRowSelectBEnabled);
      setOutput(2, isRowSelectCEnabled);

      for(int col = 0; col <= 7; col++){
        bool isColSelectAEnabled = col == 0 || col == 2 || col == 4 || col == 7; //results in: 1,0,1,0,1,0,0,1
        bool isColSelectBEnabled = col == 2 || col == 3 || col == 6 || col == 7; //results in: 0,0,1,1,0,0,1,1
        bool isColSelectCEnabled = col >= 4;                                     //results in: 0,0,0,0,1,1,1,1
        //Multiplexer, selects Collumn thats to be read
        setOutput(3, isColSelectAEnabled);
        setOutput(4, isColSelectBEnabled);
        setOutput(5, isColSelectCEnabled);
        //read the value of Pin D15, the Multiplexer should set the read square on that pin
        bool reading = ((1 << 15) & GPIOD -> IDR) >> 15;
        //set the corresponding bit in the current State, 0 = no figure on it
        int bitIndex = 63 - (row * 8 + col);
        output = (output & (~(uint64_t{1} << bitIndex))) | (uint64_t{reading} << bitIndex); // output[bitIndex] = reading;
      }
    }
    
    GPIOA->ODR |= 1<<5; // Set the Pin PA5

    GPIOA->ODR &= ~(1<<5); // Reset the Pin PA5

    return output;
}
//this should be the function that gets looped to listen to the board
bool MatrixController::checkState(){
    uint64_t currentState = readMatrix();

    if(state != currentState){ // if it has changed
        count++;
        if(count >= 10){ //the state has changed 
            handleStateChange(state ^ currentState);
            state = currentState; //update internal state to the updated one after attack was handled
            count = 0;
            return true;
        }
    } else {
        count--;
        return false;
    }
}

//if there was no attack before the statechange, the attack state is in progress and the LED's with the pieces legal moves should be light up
//if attack was in progress before statechange, the attack should be resolved
void MatrixController::handleStateChange(uint64_t position){
    //Query, if more than one bit is changed -> position has more than one 1
    foo();
    //determine the position of the changed position
    change = posToSquare(position);
}

// prerequisite: position contains only a single "1"
Square MatrixController::posToSquare(uint64_t position) {
    //TODO may need additional arithmetic
    int onesPosition = std::log2(position);
    return onesPosition;
} 


chess::Square MatrixController::getChange(){
    return change;
}

void MatrixController::turnFinished(){
    oldState = state;
}

void MatrixController::revert(){
    while(!(readMatrix() == oldState));
    state = oldState;
    deactivateBuzzer();
}
//TODO:IMPLEMENT
void MatrixController::setLEDs(Movelist squares, int color){

}
//TODO:IMPLEMENT
void MatrixController::clearLED(){

}

void MatrixController::deactivateBuzzer(){

}

void MatrixController::activateBuzzer(){
    
}