//definitions
#ifndef __MYCLASS_H_INCLUDED__
#define __MYCLASS_H_INCLUDED__

#include "Arduino.h"
#define game_size 21
#define BUTTON_LEFT_PIN  2
#define BUTTON_RIGHT_PIN 3
#define BUTTON_DELAY 500
#define POS_Y_MAX 15
#define POS_Y_MIN 7
#define POS_X_MAX 20

class tetris{  

  
public:
  void setPins();
  void readPins();
  void setGame();
  void checkLeftButton();
  void checkRightButton();  
  void update();
  void display_tick();
  void goDown();
  char disp[game_size][game_size];
  unsigned long Timer;
  unsigned long speedTimer;
  uint8_t buttonLeftState;
  uint8_t buttonRightState;
  int speedTime;
  uint8_t i,j;
  uint8_t posX,posY;
  uint8_t prev_posX,prev_posY;

};
#endif
