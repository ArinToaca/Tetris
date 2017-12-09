#include "tetris.h"

  void tetris::setPins(){
    pinMode(BUTTON_LEFT_PIN, INPUT);
    pinMode(BUTTON_RIGHT_PIN, INPUT);
  }

  void tetris::readPins(){
    buttonLeftState= digitalRead(BUTTON_LEFT_PIN);
    buttonRightState = digitalRead(BUTTON_RIGHT_PIN);
  }

  void tetris::setGame(){
    int i,j;
    for(i=0;i<game_size;i++)
      for(j=0;j<game_size;j++)
        disp[i][j]=' ';

    for(j=0;j<game_size;j++){
      disp[j][6]='|';
      disp[j][16]='|';
    }

    posX = 2;
    posY = 9;
    prev_posX = posX;
    prev_posY = posY;

    Serial.println("Game ready!");
  }

  void tetris::checkLeftButton(){
    if (buttonLeftState == HIGH){
      if (millis() - tetris::Timer > BUTTON_DELAY){
        posY--;
        Timer = millis();
        display_tick();
          if(posY < POS_Y_MIN)
            posY = POS_Y_MIN;
        display_tick();
      }
      
    }
  }
  void tetris::checkRightButton(){
    if (buttonRightState == HIGH){
      if(millis() - Timer >= BUTTON_DELAY){
          prev_posY = posY;
          posY++;
          Timer = millis();
          if(posY > POS_Y_MAX)
            posY = POS_Y_MAX;
          display_tick();
      }
      
    }
  }
  void tetris::goDown(){

    if (millis() - speedTimer >= speedTime){
        posX++;
        speedTimer = millis();
        if(posX > POS_X_MAX){
          posX = POS_X_MAX;
        }
        display_tick();

    }
  }

  void tetris::update(){
    prev_posY = posY;
    prev_posX= posX;
    readPins();
    checkLeftButton();
    checkRightButton();
    goDown();
    disp[prev_posX][prev_posY] = ' ';
    disp[posX][posY] = 'X';
  }

  void tetris::display_tick(){
    int i,j;
    Serial.println(" ");
    for(i=0;i<game_size;i++)
      Serial.print('-');
    Serial.println(" ");
    for(i=0;i<game_size;i++)
    {
      for(j=0;j<game_size;j++)
        Serial.print(disp[i][j]);
      Serial.println(" ");
    }
    for(i=0;i<game_size;i++)
      Serial.print('-');
  }
