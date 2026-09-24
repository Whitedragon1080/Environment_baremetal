#ifndef SRC_STARTUP_MATRIX_INPUT_STARTUP_CPP
#define SRC_STARTUP_MATRIX_INPUT_STARTUP_CPP
#include <wiringPi.h> 
//#include "stm32f0xx.h"

void pinEN(){
    /* deprecated, CMSIS not compatible with Pi alone, kept in case someone wants to experiment later down the line
    //Enable peripheral clock of pins C,D and F
    RCC->AHBENR |= RCC_AHBENR_GPIODEN;
    RCC -> AHBENR |= RCC_AHBENR_GPIOCEN;
    RCC -> AHBENR |= RCC_AHBENR_GPIOFEN;
    //set pins 0 to 9 of D into output mode
    GPIOD -> MODER = (GPIOA -> MODER & ~(GPIO_MODER_MODER0 | GPIO_MODER_MODER1 | GPIO_MODER_MODER2 | GPIO_MODER_MODER3 |GPIO_MODER_MODER4 | GPIO_MODER_MODER5 |GPIO_MODER_MODER6 | GPIO_MODER_MODER7 | GPIO_MODER_MODER8 | GPIO_MODER_MODER9));
    GPIOD -> MODER |= (GPIO_MODER_MODER0_0 | GPIO_MODER_MODER1_0 | GPIO_MODER_MODER2_0 | GPIO_MODER_MODER3_0 |GPIO_MODER_MODER4_0 | GPIO_MODER_MODER5_0 | GPIO_MODER_MODER6_0 |GPIO_MODER_MODER7_0 | GPIO_MODER_MODER8_0 | GPIO_MODER_MODER9_0);
    //set pins 12 to 15 of D into analogue mode
    GPIOD->MODER |= (GPIO_MODER_MODER12 | GPIO_MODER_MODER13 | GPIO_MODER_MODER14 | GPIO_MODER_MODER15); */
    wiringPiSetupPinType(WPI_PIN_PHYS);
    pinMode(11, OUTPUT);
    pinMode(13, OUTPUT);
    pinMode(15, OUTPUT);
    pinMode(16, OUTPUT);
    pinMode(31, OUTPUT);
    pinMode(29, OUTPUT);
    pinMode(22, OUTPUT);
}



#endif