#ifndef SRC_CONTROLLER_MATRIX_CONTROLLER_H
#define SRC_CONTROLLER_MATRIX_CONTROLLER_H

class MatrixController {
  private:
    int count;
    void readMatrix();
    bool isAttack;
    void init();
    int state[8][8]; 

  public:
   MatrixController(int count, bool isAttack);
};

#endif