#include "tzeny-game.h"

tzeny_game::tzeny_game(tzeny_draw *d)
{
    draw = d;

    int i,j;
    for(i=0;i<MATRIX_HEIGHT;i++)
    {
        for(j=0;j<MATRIX_WIDTH;j++)
        {
            matrix[i][j] = false;
        }
    }
}

void tzeny_game::start_game()
{
    Serial.println("Game Started!");
}

void tzeny_game::game_tick()
{
    Serial.println(draw->system_state);
    if(draw->system_state==1)
    {
        gameStarted=1;
        start_game();
    }
}