#ifndef __TZENY_GAME__
#define __TZENY_GAME__
#include <tzeny-draw.h>
#include <shape.h>
#include <shapeL.h>
#include <pitches.h>

#define MATRIX_WIDTH        10
#define MATRIX_HEIGHT       20

#define SPEAKER_PIN         8

class tzeny_game {
    public:
        tzeny_game(tzeny_draw *d);
        void    game_tick(),
                start_game(),
                back_button_pressed(),
                back_button_released(),
                left_button_pressed(),
                right_button_pressed(),
                top_button_pressed(),
                menu_go_back(),
                menu_go_forward(),
                display_title_menu(),
                assign_buttons(uint8_t b1=1, uint8_t b2=1, uint8_t b3=1);

    private:
        uint8_t current_menu_items = 1;
        uint8_t current_menu = 0;
        uint8_t current_menu_arrow_position = 0;
        uint8_t current_menu_buttons[10];
        uint8_t system_state = 0;//0 - menu; 1 - game 
        bool gameStarted=false;
        uint16_t matrix[20][10];
        int ticksToMove=10;
        int currentTicks=0;
        shapeL *s;
        tzeny_draw *draw;
        int ticks_per_move_down=10;

};

#endif