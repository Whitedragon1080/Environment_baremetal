#include "MatrixController.h"
#include "../startup/matrixinputStartup.cpp"
#include <cstdint>

//controller is initialized with the state how a chessboard should start
MatrixController::MatrixController(int count) : count(count), capturing(false), state(0b1111111111111111000000000000000000000000000000001111111111111111) { 

}
//activate the pins needed for communication
void MatrixController::init(){
    pinEN();
    oldState = state;
}
//set the bit of the respective Pin, if its a D pin
void MatrixController::setOutput(int pin, bool enabled){  
  if(enabled){
    if(pin == 0){
        digitalWrite(11, HIGH);
    }
    if(pin == 1){
        digitalWrite(13, HIGH);
    }
    if(pin == 2){
        digitalWrite(15, HIGH);
    }
    if(pin== 3){
        digitalWrite(16, HIGH);
    }
    if(pin == 4){
        digitalWrite(18, LOW);
    }
    if(pin == 5){
        digitalWrite(22, LOW);
    }
    if(pin == 6){
        digitalWrite(29, LOW);
    }
    if(pin == 7){
        digitalWrite(31, LOW);
    }
    //  GPIOD -> ODR |= (1 << pin);
  } else { 
    //  GPIOD -> ODR &= ~(1 << pin);
    if(pin == 0){
        digitalWrite(11, LOW);
    }
    if(pin == 1){
        digitalWrite(13, LOW);
    }
    if(pin == 2){
        digitalWrite(15, LOW);
    }
    if(pin== 3){
        digitalWrite(16, LOW);
    }
    if(pin == 4){
        digitalWrite(18, HIGH);
    }
    if(pin == 5){
        digitalWrite(22, HIGH);
    }
    if(pin == 6){
        digitalWrite(29, HIGH);
    }
    if(pin == 7){
        digitalWrite(31, HIGH);
    }
  }    
}
//read the matrix and notify if a change persists for 10 readings
//TODO: Change implementation to fit hardware
uint64_t MatrixController::readMatrix() {
  uint64_t output = 0;
  /* deprecated, hardware not compatible 
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
        */
    for(int square = 0; square <= 15; square++){
        bool setSquare0 = square == 1 || square == 3 || square == 5|| square == 7 ||square == 9 || square == 11 || square == 13|| square == 15; 
        bool setSquare1 = square == 2 || square == 3 ||square == 6 ||square == 7 || square == 10 || square == 11 || square == 14 || square == 15;
        bool setSquare2 = square == 4 || square == 5 || square == 6 || square == 7 || square == 12 ||square == 13 ||square == 14 || square == 15;
        bool setSquare3 = square >= 8;

        setOutput(0, setSquare0);
        setOutput(1, setSquare1);
        setOutput(2, setSquare2);
        setOutput(3, setSquare3);

        for(int mux = 4; mux <= 7; mux++){
            setOutput(mux, true);
            int index = 12 + (mux - 4);
            bool reading = ((1 << index) & GPIO -> IDR) >> index;
            int bitIndex = 63 - ((mux - 4) * 16 + square);
            output = (output & (~(uint64_t{1} << bitIndex))) | (uint64_t{reading} << bitIndex); // output[bitIndex] = reading;
            setOutput(mux, false);
        }
    }
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

void MatrixController::turnOnLED(chess::Square field){

}

void MatrixController::turnOffLED(chess::Square field){

}
void MatrixController::setLEDs(Movelist squares, int color){
    for(const auto& move : squares){
        chess:Square destination = move.to();
        turnOnLED(destination);
    }
}
//TODO:IMPLEMENT
void MatrixController::clearLED(){

}

void MatrixController::deactivateBuzzer(){
    setOutput(10, false);
}

void MatrixController::activateBuzzer(){
    setOutput( 10, true);
}

void MatrixController::setAllLEDs(){

}