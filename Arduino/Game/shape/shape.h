 #ifndef __SHAPE_H_INCLUDED__
 #define __SHAPE_H_INCLUDED__
#include <Arduino.h>
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
    bool stopRightMovement;
    bool stopLeftMovement;
    bool stopObject;
    int rotationPhase;

    void moveShapeLeft();

    void moveShapeRight();

    void moveShapeDown();

    void stopShape();

    void checkCollision();

    void checkPointCollision(int x, int y);

    void delLines(int x[3]);
};
  
  #endif

