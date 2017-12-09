#ifndef __SHAPE_H_INCLUDED__
#define __SHAPE_H_INCLUDED__
#include <Arduino.h>

#define	ST7735_BLACK   0x0000
#define	ST7735_BLUE    0x001F
#define	ST7735_RED     0xF800
#define	ST7735_GREEN   0x07E0
#define ST7735_CYAN    0x07FF
#define ST7735_MAGENTA 0xF81F
#define ST7735_YELLOW  0xFFE0
#define ST7735_WHITE   0xFFFF

#include <tzeny-draw.h>

class shape {

public:
    class point {
    public:
        int x, y, prev_x, prev_y;

        point(int x, int y) {
            this->x = x;
            this->y = y;
            prev_x = x;
            prev_y = y;
        };

        void drawPoint() {
        };
    };

    point *p[4];
    bool arr[11][21];
    tzeny_draw *draw;
    bool stopRightMovement;
    bool stopLeftMovement;
    bool stopObject;
    uint16_t color=ST7735_BLUE;
    int rotationPhase;

    void setDraw(tzeny_draw* d){
        draw = d;
    }

    void moveShapeLeft();

    void moveShapeRight();

    void moveShapeDown();

    void stopShape();

    void redrawShape();

    void checkCollision();

    void checkPointCollision(int x, int y);

    void delLines(int x[3]);
};
  
  #endif

