#ifndef SRC_CONTROLLER_MATRIX_CONTROLLER_H
#define SRC_CONTROLLER_MATRIX_CONTROLLER_H

#include <cstdint>
class MatrixController {
  private:
    int count;
    uint64_t readMatrix();
    bool isAttack;
    void init();
    uint64_t state; 
    void checkState();
    void setOutput(int pin, bool output);
    void handleStateChange(uint64_t position);
    bool capturing;

  public:
   MatrixController(int count, bool isAttack);
};

#endif