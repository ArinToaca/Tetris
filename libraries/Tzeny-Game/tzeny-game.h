#ifndef __TZENY_GAME__
#define __TZENY_GAME__
#include <tzeny-draw.h>
#include <shape.h>
#include <shapeL.h>
#include <shapeZ.h>
#include <shapeT.h>
#include <shapeO.h>
#include <shapeMirrorZ.h>
#include <shapeMirrorL.h>
#include <pitches.h>
#include <Matrix.h>

#define MATRIX_WIDTH        10
#define MATRIX_HEIGHT       20

#define SPEAKER_PIN         4

class tzeny_game {
    public:
        tzeny_game(tzeny_draw *d);
        void    game_tick(),
                start_game(),
                stop_game(),
                back_button_pressed(),
                back_button_released(),
                left_button_pressed(),
                right_button_pressed(),
                top_button_pressed(),
                menu_go_back(),
                menu_go_forward(),
                display_title_menu(),
                make_shape(),
                shiftMatrixDown(uint8_t rowStart),
                redrawMatrix(),
                redrawMatrixLine(uint8_t y),
                assign_buttons(uint8_t b1=1, uint8_t b2=1, uint8_t b3=1);

    private:
        uint16_t colors[6] = {ST7735_BLUE, ST7735_RED, ST7735_GREEN, ST7735_CYAN, ST7735_MAGENTA, ST7735_YELLOW};
        uint8_t color_index=0;
        uint8_t current_menu_items = 1;
        uint8_t current_menu = 0;
        uint8_t current_menu_arrow_position = 0;
        uint8_t current_menu_buttons[10];
        uint8_t system_state = 0;//0 - menu; 1 - game 
        uint32_t score=0;
        uint8_t level=1;
        bool gameStarted=false;
        Matrix *m;
        int ticksToMove=10;
        int currentTicks=0;
        shape *s;
        tzeny_draw *draw;
        int ticks_per_move_down=10;

};

#endif