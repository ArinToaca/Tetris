//definitions
#ifndef __MYCLASS_H_INCLUDED__
#define __MYCLASS_H_INCLUDED__

#include <Arduino.h>
#define game_size 21
#define BUTTON_LEFT_PIN  2
#define BUTTON_RIGHT_PIN 3
#define BUTTON_DELAY 500
#define POS_Y_MAX 15
#define POS_Y_MIN 7
#define POS_X_MAX 20

class tetris{  
  public:
    tetris(){
      
    }
    void setPins();
    int posX;

};
#endif
