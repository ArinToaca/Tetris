#include "tetris.h"

  void Tetris::setPins(){
    pinMode(BUTTON_LEFT_PIN, INPUT);
    pinMode(BUTTON_RIGHT_PIN, INPUT);
  }

  void Tetris::readPins(){
    this->buttonLeftState= digitalRead(BUTTON_LEFT_PIN);
    this->buttonRightState = digitalRead(BUTTON_RIGHT_PIN);
  }

  void Tetris::setGame(){
    int i,j;
    for(i=0;i<game_size;i++)
      for(j=0;j<game_size;j++)
        this->disp[i][j]=' ';

    for(j=0;j<game_size;j++){
      this->disp[j][6]='|';
      this->disp[j][16]='|';
    }

    this->posX = 2;
    this->posY = 9;
    //this->prev_posX = this->posX;
    //this->prev_posY = this->posY;

    Serial.println("Game ready!");
  }

  void Tetris::checkLeftButton(){
    if (this->buttonLeftState == HIGH){
      if (millis() - Tetris::Timer > BUTTON_DELAY){
        this->posY--;
        this->Timer = millis();
        this->display_tick();
          if(this->posY < POS_Y_MIN)
            this->posY = POS_Y_MIN;
        this->display_tick();
      }
      
    }
  }
  void Tetris::checkRightButton(){
    if (this->buttonRightState == HIGH){
      if(millis() - this->Timer >= BUTTON_DELAY){
          //prev_posY = posY;
          this->posY++;
          this->Timer = millis();
          if(this->posY > POS_Y_MAX)
            this->posY = POS_Y_MAX;
          this->display_tick();
      }
      
    }
  }
  void Tetris::goDown(){

    if (millis() - Tetris::speedTimer >= Tetris::speedTime){
        this->posX++;
        this->speedTimer = millis();
        if(this->posX > POS_X_MAX){
          this->posX = POS_X_MAX;
        }
        this->display_tick();

    }
  }

  void Tetris::update(){
    //this->prev_posY = this->posY;
    //this->prev_posX= this->posX;
    this->readPins();
    this->checkLeftButton();
    this->checkRightButton();
    this->goDown();
    //this->disp[this->prev_posX][this->prev_posY] = ' ';
    this->disp[this->posX][this->posY] = 'X';
  }

  void Tetris::display_tick(){
    int i,j;
    Serial.println(" ");
    for(i=0;i<game_size;i++)
      Serial.print('-');
    Serial.println(" ");
    for(i=0;i<game_size;i++)
    {
      for(j=0;j<game_size;j++)
        Serial.print(this->disp[i][j]);
      Serial.println(" ");
    }
    for(i=0;i<game_size;i++)
      Serial.print('-');
  }
