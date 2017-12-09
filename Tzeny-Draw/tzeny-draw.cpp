#include "tzeny-draw.h"

#include <Arduino.h>
#include <EEPROM.h>

tzeny_draw::tzeny_draw()
{
  Serial.begin(9600);
  tft = new tzeny_tft(TFT_CS,  TFT_DC, TFT_RST);

  tft->initialize();
  
  hei = tft->height();
  wid = tft->width();

  Serial.println("Initialized display");

}

void tzeny_draw::test()
{
  highscores_number = 3;
  highscores[0] = (highscore_record){"ARINEL",666969,13};
  highscores[1] = (highscore_record){"BAIATU",1245,3};
  highscores[2] = (highscore_record){"TZENY ",12634,6};
  sortHighscores();
  writeHighscores();

  highscores_number = 0;
}

//------------------------SHAPES------------------
void tzeny_draw::drawText(uint8_t xIndex, uint8_t yIndex, uint8_t textSize, char *msg, uint16_t color)
{
  //Serial.print("Drawing text at position: ");Serial.print(xIndex);Serial.print("; ");Serial.print(yIndex);Serial.println("");
  tft->setCursor(blockSize*xIndex, blockSize*yIndex);
  tft->setTextColor(color);
  tft->setTextSize(textSize);
  tft->print(msg);
}

void tzeny_draw::intro(uint16_t color1, uint16_t color2) 
{
  tft->fillScreen(ST7735_BLACK);
  for (int16_t x=0; x < tft->width(); x+=6) {
    uint16_t color = color1;
    if(x/6%2==0)
      color = color2;
    tft->drawRect(tft->width()/2 -x/2, tft->height()/2 -x/2 , x, x, color);
    delay(50);
  }
  
  for (int16_t x=0; x < tft->width(); x+=6) {
    tft->drawRect(tft->width()/2 -x/2, tft->height()/2 -x/2 , x, x, ST7735_BLACK);
    delay(50);
  }
}

void tzeny_draw::drawPlayArea() {
    tft->fillScreen(ST7735_BLACK);

    Serial.println(blockSize*6);
    Serial.println(blockSize*7);

    Serial.println(wid);
    Serial.println(hei);

    tft->fillRect(blockSize*6, 0, blockSize, hei-1, ST7735_WHITE);
    tft->fillRect(blockSize*17, 0, blockSize, hei-1, ST7735_WHITE);
}

void tzeny_draw::startGame()
{
  drawPlayArea();
}

void tzeny_draw::drawMenuArrow(int yPos, uint16_t color)
{
    tft->drawTriangle(blockSize, (yPos + 7) * blockSize, blockSize, (yPos + 7 + 2) * blockSize, blockSize*2, (yPos + 7 + 1) * blockSize, color);
}

void tzeny_draw::drawBlock(int xIndex, int yIndex, uint16_t color)
{
    tft->fillRect(blockSize*7 + blockSize*xIndex, hei - blockSize*(yIndex+1), blockSize, blockSize, color);
}

void tzeny_draw::clearBlock(int xIndex, int yIndex)
{
    drawBlock(xIndex, yIndex, ST7735_BLACK);
}

void tzeny_draw::drawRightSmallBlock(int xIndex, int yIndex, uint16_t color)
{
    uint8_t side = blockSize / 2;
    tft->fillRect(blockSize*18 + side*(xIndex+1), blockSize + side*(yIndex+1), side, side, color);
}

//-----------------------MENUS--------------------
void tzeny_draw::initMenu()
{
  tft->fillScreen(ST7735_BLACK);

  if(current_menu_items!=0)
    drawMenuArrow(0, ST7735_WHITE);
}

void tzeny_draw::drawTitleMenu()
{
  current_menu_items = 2;

  initMenu();

  drawText(1,2,2,"AIA TETRIS");

  drawText(7,7,2,"Play");
  drawText(7,10,2,"Top");
}

void tzeny_draw::drawPlayMenu() 
{
  current_menu_items = 3;

  initMenu();

  drawText(2,2,2,"GAME TYPE");

  drawText(4,7,2,"Marathon");
  drawText(4,10,2,"Sprint");
  drawText(4,13,2,"Ultra");
}

void tzeny_draw::drawHighscoreMenu()
{
  current_menu_items = 0;

  readHighscores();

  initMenu();

  drawText(1,1,2,"HIGHSCORE");

  drawText(3, 6, 1, "NAME   SCORE  LV", colors[5]);

  uint8_t i,j;
  uint16_t color;
  char display_buffer[20];

  Serial.print("I got this many highscores to display: ");
  Serial.println(highscores_number);
  
  for(i=0;i<highscores_number;i++)
  {
    color = colors[i%(colors_number-1)];
    drawText(3, i*2+8, 1, highscores[i].player, color);

    //itoa(highscores[i].score, display_buffer, 10);
    sprintf(display_buffer, "%lu", highscores[i].score);

    drawText(10, i*2+8, 1, display_buffer, color);

    //itoa(highscores[i].level, display_buffer, 10);
    sprintf(display_buffer, "%d", highscores[i].level);

    drawText(17, i*2+8, 1, display_buffer, color);
    Serial.println(i);
  }
}

//---------------------------HIGHSCORE-------------
void tzeny_draw::readHighscores()
{
  int eeprom_addr = 0;
  uint8_t number_of_entries=0;

  Serial.println("Reading highscores from EEPROM");

  EEPROM.get(eeprom_addr, number_of_entries);
  eeprom_addr += sizeof(uint8_t);

  if(number_of_entries > max_highscores_number)
    number_of_entries = max_highscores_number;
  highscores_number = number_of_entries;

  Serial.print("Number of entries: ");
  Serial.println(number_of_entries);

  int i;
  for(i=0;i<number_of_entries;i++)
  {
    EEPROM.get(eeprom_addr, highscores[i]);
    eeprom_addr += sizeof(highscore_record);

    Serial.print("Read entry for player: ");
    Serial.println(highscores[i].player);
  }
  
  Serial.println("Read all highscores!");
}

void tzeny_draw::sortHighscores()
{
  uint8_t sw = 1;
  while(sw)
  {
    sw = 0;
    uint8_t i;
    highscore_record temp;
    for(i=0;i<highscores_number-1;i++)
    {
      if(highscores[i].score<highscores[i+1].score)
      {
        temp = highscores[i];
        highscores[i] = highscores[i+1];
        highscores[i+1] = temp;

        sw = 1;
      }
    }
  }
}

void tzeny_draw::writeHighscores()
{
  Serial.println("Writing highscores to EEPROM");
  
  uint8_t eeprom_addr = 0;

  //Print length of data to run CRC on.
  Serial.print("EEPROM length: ");
  Serial.println(EEPROM.length());

  EEPROM.put(eeprom_addr, highscores_number);
  eeprom_addr = sizeof(uint8_t);

  delay(200);
  
  Serial.print("Number of entries: ");
  Serial.println(highscores_number);

  int i;
  for(i=0;i<highscores_number;i++)
  {
    EEPROM.put(eeprom_addr, highscores[i]);
    eeprom_addr += sizeof(highscore_record);

    Serial.print("Wrote entry for player: ");
    Serial.println(highscores[i].player);
  }

  Serial.println("Wrote all highscores!");

  //Print length of data to run CRC on.
  Serial.print("EEPROM length: ");
  Serial.println(EEPROM.length());
}



