#ifndef _TZENY_DRAW_
#define _TZENY_DRAW_

#include <tzeny-tft.h> // Hardware-specific library
#include <SPI.h>
#include <EEPROM.h>
#include <Arduino.h>

// For the breakout, you can use any 2 or 3 pins
// These pins will also work for the 1.8" TFT shield
#define TFT_CS     10
#define TFT_RST    30  // you can also connect this to the Arduino reset
// in which case, set this #define pin to -1!
#define TFT_DC     36

#define blockSize 6

#define max_highscores_number 8

typedef struct _highscore_record
{
    char player[7];
    uint32_t score;
    uint8_t level;
}highscore_record;

class tzeny_draw {

    public:
        tzeny_draw();
        void    drawText(uint8_t xIndex, uint8_t yIndex, uint8_t textSize, char *msg, uint16_t color=ST7735_WHITE),
                drawPlayArea(),
                drawMenuArrow(int yPos, uint16_t color),
                drawTitleMenu(),
                drawPlayMenu(),
                drawScore(uint32_t score, uint8_t level),
                drawEndMenu(uint32_t score, uint8_t level),
                drawHighscoreMenu(),
                drawBlock(int xIndex, int yIndex, uint16_t color),
                clearBlock(int xIndex, int yIndex),
                drawRightSmallBlock(int xIndex, int yIndex, uint16_t color),
                readHighscores(),
                writeHighscores(),
                sortHighscores(),
                addToHighscore(char * p, uint32_t score, uint8_t level),
                intro(uint16_t color1, uint16_t color2),
                test(),
                initMenu(),
                startGame();   
    private:
        tzeny_tft *tft;
        int hei,wid;
        highscore_record highscores[8];
        uint8_t current_menu_items = 1;
        uint8_t highscores_number = 0;
        
        //tzeny_tft tft (TFT_CS,  TFT_DC, TFT_RST);
        
        uint16_t colors[6] = {ST7735_BLUE, ST7735_RED, ST7735_GREEN, ST7735_CYAN, ST7735_MAGENTA, ST7735_YELLOW};
        uint8_t colors_number = 6;
};

#endif