#include "tzeny-game.h"

tzeny_game::tzeny_game(tzeny_draw *d)
{
    draw = d;

    int i,j;
    for(i=0;i<20;i++)
    {
        for(j=0;j<10;j++)
        {
            matrix[i][j] = 0;
        }
    }
    gameStarted = false;
    pinMode(SPEAKER_PIN, OUTPUT);
}

void tzeny_game::start_game()
{
    Serial.println("Game Started!");
    system_state=1;
    draw->drawPlayArea();

    s = new shapeL();
    s->setDraw(draw);
    s->startShape();

    gameStarted = true;

    currentTicks=0;    
}

void tzeny_game::display_title_menu()
{
    draw->drawTitleMenu();
    current_menu = 1;
    current_menu_items = 2;
    assign_buttons(3,2);
    system_state = 0;
    current_menu_arrow_position=0;
}

void tzeny_game::game_tick()
{
    if(!gameStarted)
        return;
    if(system_state!=1)
        return;
    currentTicks++;

    int dif = ticksToMove - currentTicks;

    if(dif <= 0)
    {
        //beep();
        tone(SPEAKER_PIN,NOTE_B5,50);
        bool shapeOk = s->moveShapeDown();
        if(!shapeOk)
        {
            delete s;
            s = new shapeL();
            s->setDraw(draw);
            s->startShape();
        }
        currentTicks=0;
    }
}

void tzeny_game::assign_buttons(uint8_t b1, uint8_t b2, uint8_t b3)
{
  current_menu_buttons[0] = b1;
  current_menu_buttons[1] = b2;
  current_menu_buttons[2] = b3;
}

void tzeny_game::menu_go_back()
{
    current_menu_arrow_position=0;
    switch(current_menu)
    {
        case 1:
        draw->drawTitleMenu();
        current_menu = 1;
        current_menu_items = 2;
        assign_buttons(3,2);
        break;

        case 2:
        draw->drawTitleMenu();
        current_menu = 1;
        current_menu_items = 2;
        assign_buttons(3,2);
        break;

        case 3:
        draw->drawTitleMenu();
        current_menu = 1;
        current_menu_items = 2;
        assign_buttons(3,2);
        break;
    }
}

void tzeny_game::menu_go_forward()
{
    if(current_menu_items==0)
        return;

    uint8_t nextMenu = current_menu_buttons[current_menu_arrow_position/3];
    current_menu_arrow_position=0;
    switch(nextMenu)
    {
        case 1:
            draw->drawTitleMenu();
            current_menu = 1;
            current_menu_items = 2;
            assign_buttons(3,2);
            break;

        case 2:
            draw->drawHighscoreMenu();
            current_menu_items = 0;
            current_menu = 2;
            break;

        case 3:
            draw->drawPlayMenu();
            current_menu = 3;
            current_menu_items = 3;
            assign_buttons(4,4,4);
            break;

        case 4:
        current_menu = -1;
        current_menu_items = 0;
        start_game();
        break;
    }
}
//-----------------------EVENTS-------------------
void tzeny_game::back_button_pressed()
{
  switch(system_state)
  {
    case 0:
      if(current_menu!=-1)
        menu_go_back();
      break;

    case 1:
        ticksToMove /= 2;
  }
}

void tzeny_game::back_button_released()
{
    switch(system_state)
    {
      case 1:
          ticksToMove *= 2;
    }
}

void tzeny_game::left_button_pressed() 
{
    //Serial.println("LEFT State: " + String(system_state) + "; items: "+ String(current_menu_items) + ";arrow: " + String(current_menu_arrow_position));
    switch(system_state)
    {
        case 0:
            if(current_menu_items!=0)
            {
                if(current_menu_arrow_position>0)
                {
                    draw->drawMenuArrow(current_menu_arrow_position, ST7735_BLACK); //delete arrow
                    current_menu_arrow_position-=3;
                    draw->drawMenuArrow(current_menu_arrow_position, ST7735_WHITE); //draw arrow
                }
            }
            break;
        case 1:
            tone(SPEAKER_PIN,NOTE_CS5,50);
            s->moveShapeLeft();
    }

}

void tzeny_game::right_button_pressed() 
{
    //Serial.println("RIGHT State: " + String(system_state) + "; items: "+ String(current_menu_items) + ";arrow: " + String(current_menu_arrow_position));
    switch(system_state)
    {
        case 0:
            if(current_menu_items!=0)
            {
                if(current_menu_arrow_position<(current_menu_items-1)*3)
                {
                    draw->drawMenuArrow(current_menu_arrow_position, ST7735_BLACK); //delete arrow
                    current_menu_arrow_position+=3;
                    draw->drawMenuArrow(current_menu_arrow_position, ST7735_WHITE); //draw arrow
                }
            }
            break;
        case 1:
            tone(SPEAKER_PIN,NOTE_CS5,50);
            s->moveShapeRight();
    }
  
}

void tzeny_game::top_button_pressed() 
{
    switch(system_state)
    {
        case 0:
            if(current_menu!=-1)
                menu_go_forward();
            break;

        case 1:
            tone(SPEAKER_PIN,NOTE_CS5,50);
            s->rotateShape();
    }

}