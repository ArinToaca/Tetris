#include <tzeny-tft.h> // Hardware-specific library
#include <SPI.h>


// For the breakout, you can use any 2 or 3 pins
// These pins will also work for the 1.8" TFT shield
#define TFT_CS     10
#define TFT_RST    8  // you can also connect this to the Arduino reset
                      // in which case, set this #define pin to -1!
#define TFT_DC     9

#define blockSize 6

tzeny_tft tft (TFT_CS,  TFT_DC, TFT_RST);

int hei,wid;
int current_menu_items = 1;

void drawPlayArea() {
    tft.fillScreen(ST7735_BLACK);

    Serial.println(blockSize*6);
    Serial.println(blockSize*7);

    Serial.println(wid);
    Serial.println(hei);

    tft.fillRect(blockSize*6, 0, blockSize, hei-1, ST7735_WHITE);
    tft.fillRect(blockSize*17, 0, blockSize, hei-1, ST7735_WHITE);
}

void drawMenuArror(int yPos)
{
  
}

void drawTitleMenu()
{
  tft.fillScreen(ST7735_BLACK);

  drawText(1,2,2,"AIA TETRIS");

  drawText(7,7,2,"Play");
  drawText(7,10,2,"Top");

  current_menu_items = 2;
}

void drawPlayMenu() 
{
  tft.fillScreen(ST7735_BLACK);

  drawText(2,2,2,"GAME TYPE");

  drawText(4,7,2,"Marathon");
  drawText(4,10,2,"Sprint");
  drawText(4,13,2,"Ultra");

  current_menu_items = 3;
}

void drawBlock(int xIndex, int yIndex, uint16_t color)
{
    tft.fillRect(blockSize*7 + blockSize*xIndex, hei - blockSize*(yIndex+1), blockSize, blockSize, color);
}

void drawRightSmallBlock(int xIndex, int yIndex, uint16_t color)
{
    uint8_t side = blockSize / 2;
    tft.fillRect(blockSize*18 + side*(xIndex+1), blockSize + side*(yIndex+1), side, side, color);
}

void setup(void) 
{
  
  Serial.begin(9600);
  Serial.print("Hello! ST7735 TFT Test");

  tft.initialize();

  hei = tft.height();
  wid = tft.width();

  Serial.println("Initialized");

  //intro(ST7735_BLUE, ST7735_GREEN);

  //delay(500);

  drawTitleMenu();

  /*drawPlayArea();

  drawBlock(0,0,ST7735_BLUE);
  drawBlock(1,0,ST7735_BLUE);
  drawBlock(2,0,ST7735_BLUE);
  drawBlock(1,1,ST7735_BLUE);

  drawBlock(5,5,ST7735_GREEN);
  drawBlock(5,6,ST7735_GREEN);
  drawBlock(6,5,ST7735_GREEN);
  drawBlock(6,6,ST7735_GREEN);

  drawRightSmallBlock(0,0,ST7735_CYAN);
  drawRightSmallBlock(0,1,ST7735_CYAN);
  drawRightSmallBlock(0,2,ST7735_CYAN);
  drawRightSmallBlock(0,3,ST7735_CYAN);

  drawRightSmallBlock(1,6,ST7735_BLUE);
  drawRightSmallBlock(0,7,ST7735_BLUE);
  drawRightSmallBlock(1,7,ST7735_BLUE);
  drawRightSmallBlock(2,7,ST7735_BLUE);

  drawText(0,0,1,"Score:");
  drawText(0,2,1,"666");
  drawText(0,6,1,"Level:");
  drawText(0,8,1,"10");

  drawText(0,12,1,"Timer:");
  drawText(0,14,1,"10");*/
}

void intro(uint16_t color1, uint16_t color2) 
{
  tft.fillScreen(ST7735_BLACK);
  for (int16_t x=0; x < tft.width(); x+=6) {
    uint16_t color = color1;
    if(x/6%2==0)
      color = color2;
    tft.drawRect(tft.width()/2 -x/2, tft.height()/2 -x/2 , x, x, color);
    delay(50);
  }
  
  for (int16_t x=0; x < tft.width(); x+=6) {
    tft.drawRect(tft.width()/2 -x/2, tft.height()/2 -x/2 , x, x, ST7735_BLACK);
    delay(50);
  }
}

void drawText(uint8_t xIndex, uint8_t yIndex, uint8_t textSize, char *msg)
{
  tft.setCursor(blockSize*xIndex, blockSize*yIndex);
  tft.setTextColor(ST7735_WHITE);
  tft.setTextSize(textSize);
  tft.print(msg);
}

void loop(){
  
}

