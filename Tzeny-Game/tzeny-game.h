#ifndef __TZENY_GAME__
#define __TZENY_GAME__
#include <tzeny-draw.h>
#include <shape.h>

#define MATRIX_WIDTH        10
#define MATRIX_HEIGHT       20

class tzeny_game {
    public:
        tzeny_game(tzeny_draw *d);
        void    game_tick(),
                start_game();

    private:
        bool gameStarted=false;
        bool matrix[MATRIX_WIDTH][MATRIX_HEIGHT];
        tzeny_draw *draw;
        int ticks_per_move_down=10;

};

#endif