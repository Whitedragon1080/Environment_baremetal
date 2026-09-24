#ifndef SRC_CONTROLLER_MATRIX_CONTROLLER_H
#define SRC_CONTROLLER_MATRIX_CONTROLLER_H

#include <cstdint>
#include <string>
#include "../../include/chess.hpp"

using namespace chess;
class MatrixController {
  private:
    int count;
    uint64_t readMatrix();
    uint64_t state; 
    uint64_t oldState;
    void setOutput(int pin, bool output);
    void handleStateChange(uint64_t position);
    bool capturing;
    bool getAttackState();
    chess::Square change;
    chess::Square square;
    Square posToSquare(uint64_t position);
    void turnOffLED(chess::Square field);
    
    public:
    void turnOnLED(chess::Square field);
    void toggleBuzzer();
    void clearLED();
    chess::Square getChange();
    MatrixController(int count);
    void turnFinished();
    void revert();
    bool checkState();
    void init();
    void setLEDs(chess::Movelist squares, int color);
    void activateBuzzer();
    void deactivateBuzzer();
    void setAllLEDs();
};

#endif