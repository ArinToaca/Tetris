#include "tzeny-game.h"

tzeny_game::tzeny_game(tzeny_draw *d)
{
    Serial.println("New TzenyGAME");
    draw = d;
    m = new Matrix(20,10);
    gameStarted = false;
    pinMode(SPEAKER_PIN, OUTPUT);
    
    Serial.println("DONE");
}

void tzeny_game::start_game()
{
    Serial.println("Game Started!");
    system_state=1;
    draw->drawPlayArea();

    score = 0;
    level = 1;
    draw->drawScore(score,level);

    make_shape();

    gameStarted = true;

    currentTicks=0;    
}

void tzeny_game::make_shape()
{
    color_index = random(0,6);

    int nr_of_shapes = 3;
    int next_shape = random(0,nr_of_shapes);

    switch(next_shape)
    {
        case 0:
            s = new shapeMirrorL(colors[color_index], draw, m);
            break;

        case 1:
            s = new shapeL(colors[color_index], draw, m);
            break;

        case 2:
            s = new shapeO(colors[color_index], draw, m);
            break;

        default:
            s = new shapeO(colors[color_index], draw, m);
            break;
    }
    
    s->startShape();
}

void tzeny_game::display_title_menu()
{
    draw->intro(ST7735_BLUE, ST7735_GREEN);
    Serial.println("Displaying menu!");
    draw->drawTitleMenu();
    current_menu = 1;
    current_menu_items = 2;
    assign_buttons(3,2);
    system_state = 0;
    current_menu_arrow_position=0;
    Serial.println("Done menu!");
}

void tzeny_game::stop_game()
{
    draw->intro(ST7735_RED, ST7735_CYAN);
    system_state=2;
    gameStarted=false;

    draw->addToHighscore("ALA",score,level);

    draw->drawEndMenu(score,level);
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
        //tone(SPEAKER_PIN,NOTE_B5,50);
        bool shapeOk = s->moveShapeDown();
        if(!shapeOk)
        {
            int i, max_y=0;
            for(i=0;i<4;i++)
                if(s->p[i]->y > max_y)
                    max_y=s->p[i]->y;

            if(max_y>15)
            {
                stop_game();
                return;
            }

            delete s;

            int x,y;
            int lines_completed = 0;
            for(y=0;y<20;y++)
            {
                bool rowComplete = true;
                for(x=0;x<10;x++)
                {
                    rowComplete = rowComplete && m->getValue(x,y);
                }
                if(rowComplete)
                {
                    shiftMatrixDown(y);
                    y--;
                    lines_completed ++;
                    redrawMatrix();
                }
            }
            
            uint32_t scoreImprovment = 0;
            switch(lines_completed)
            {
                case 0:
                break;

                case 1:
                scoreImprovment = 40 * level;
                tone(SPEAKER_PIN,NOTE_GS4,50);
                tone(SPEAKER_PIN,NOTE_GS5,50);
                tone(SPEAKER_PIN,NOTE_GS6,50);
                break;

                case 2:
                scoreImprovment = 100 * level;
                tone(SPEAKER_PIN,NOTE_DS4,50);
                tone(SPEAKER_PIN,NOTE_DS5,50);
                tone(SPEAKER_PIN,NOTE_DS6,50);
                break;

                case 3:
                scoreImprovment = 300 * level;
                tone(SPEAKER_PIN,NOTE_FS4,50);
                tone(SPEAKER_PIN,NOTE_FS5,50);
                tone(SPEAKER_PIN,NOTE_FS6,50);
                break;

                default:
                scoreImprovment = 1200 * level;
                tone(SPEAKER_PIN,NOTE_AS4,50);
                tone(SPEAKER_PIN,NOTE_AS5,50);
                tone(SPEAKER_PIN,NOTE_AS6,50);
                break;
            }

            score += scoreImprovment;
            if(score > 100)
                level++;
            draw->drawScore(score,level);
            make_shape();
        }
        currentTicks=0;
    }
}

void tzeny_game::shiftMatrixDown(uint8_t rowStart)
{
    int x,y;
    for(y=rowStart;y<18;y++)
    {
        for(x=0;x<10;x++)
        {
            m->setValue(x,y, m->getValue(x,y+1));
        }
    }
}

void tzeny_game::redrawMatrix()
{
    int x,y;
    for(y=0;y<19;y++)
    {
        redrawMatrixLine(y);
    }
}

void tzeny_game::redrawMatrixLine(uint8_t y)
{
    int x;
    for(x=0;x<10;x++)
    {
        uint16_t color = m->getValue(x,y);
        draw->drawBlock(x,y,color);
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
        if(current_menu_items!=0)
        {
            if(current_menu_arrow_position<(current_menu_items-1)*3)
            {
                draw->drawMenuArrow(current_menu_arrow_position, ST7735_BLACK); //delete arrow
                current_menu_arrow_position+=3;
                draw->drawMenuArrow(current_menu_arrow_position, ST7735_WHITE); //draw arrow
            }
        }
        tone(SPEAKER_PIN,NOTE_GS6,50);
        break;

    case 1:
        ticksToMove /= 2;
        break;

    case 2:
    draw->drawHighscoreMenu();
    current_menu_items = 0;
    current_menu = 2;
    system_state=0;
        break;
  }
}

void tzeny_game::back_button_released()
{
    switch(system_state)
    {
      case 1:
          ticksToMove *= 2;
          break;
    }
}

void tzeny_game::left_button_pressed() 
{
    //Serial.println("LEFT State: " + String(system_state) + "; items: "+ String(current_menu_items) + ";arrow: " + String(current_menu_arrow_position));
    switch(system_state)
    {
        case 0:
            if(current_menu!=-1)
                menu_go_back();
            
            tone(SPEAKER_PIN,NOTE_FS5,50);
            break;
        case 1:
            tone(SPEAKER_PIN,NOTE_CS5,50);
            s->moveShapeLeft();
            break;

        case 2:
        draw->drawHighscoreMenu();
        current_menu_items = 0;
        current_menu = 2;
        system_state=0;
            break;
    }

}

void tzeny_game::right_button_pressed() 
{
    //Serial.println("RIGHT State: " + String(system_state) + "; items: "+ String(current_menu_items) + ";arrow: " + String(current_menu_arrow_position));
    switch(system_state)
    {
        case 0:
            if(current_menu!=-1)
                menu_go_forward();
            tone(SPEAKER_PIN,NOTE_GS5,50);
            break;
        case 1:
            tone(SPEAKER_PIN,NOTE_CS5,50);
            s->moveShapeRight();
            break;

        case 2:
        draw->drawHighscoreMenu();
        current_menu_items = 0;
        current_menu = 2;
        system_state=0;
            break;
    }
  
}

void tzeny_game::top_button_pressed() 
{
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
            tone(SPEAKER_PIN,NOTE_AS5,50);
            break;

        case 1:
            tone(SPEAKER_PIN,NOTE_CS5,50);
            s->rotateShape();
            break;

        case 2:
        draw->drawHighscoreMenu();
        current_menu_items = 0;
        current_menu = 2;
        system_state=0;
            break;
    }

}