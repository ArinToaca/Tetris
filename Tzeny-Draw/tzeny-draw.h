#ifndef _TZENY_DRAW_
#define _TZENY_DRAW_

#include <tzeny-tft.h> // Hardware-specific library
#include <SPI.h>
#include <EEPROM.h>
#include <Arduino.h>

// For the breakout, you can use any 2 or 3 pins
// These pins will also work for the 1.8" TFT shield
#define TFT_CS     10
#define TFT_RST    8  // you can also connect this to the Arduino reset
                      // in which case, set this #define pin to -1!
#define TFT_DC     9

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
                assign_buttons(uint8_t b1=1, uint8_t b2=1, uint8_t b3=1),
                drawPlayMenu(),
                drawHighscoreMenu(),
                drawBlock(int xIndex, int yIndex, uint16_t color),
                clearBlock(int xIndex, int yIndex),
                drawRightSmallBlock(int xIndex, int yIndex, uint16_t color),
                readHighscores(),
                writeHighscores(),
                sortHighscores(),
                intro(uint16_t color1, uint16_t color2),
                test(),
                menu_go_back(),
                menu_go_forward(),
                back_button_pressed(),
                left_button_pressed(),
                right_button_pressed(),
                top_button_pressed(),
                initMenu(),
                startGame();
        uint8_t system_state = 0;//0 - menu; 1 - game    
    private:
        tzeny_tft *tft;
        int hei,wid;
        uint8_t current_menu_items = 1;
        uint8_t current_menu = 0;
        uint8_t current_menu_arrow_position = 0;
        uint8_t current_menu_buttons[10];
        highscore_record highscores[8];
        uint8_t highscores_number = 0;
        
        //tzeny_tft tft (TFT_CS,  TFT_DC, TFT_RST);
        
        uint16_t colors[6] = {ST7735_BLUE, ST7735_RED, ST7735_GREEN, ST7735_CYAN, ST7735_MAGENTA, ST7735_YELLOW};
        uint8_t colors_number = 6;
};

#endif