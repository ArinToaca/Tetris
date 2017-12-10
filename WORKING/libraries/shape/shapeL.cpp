#include "shapeL.h"
void shapeL::startShape(){
    this->rotationPhase = 2;
    p[0] = new point(4,19);
    p[1] = new point(5,19);
    p[2] = new point(4,18);
    p[3] = new point(4,17);

    int i,j;
    for(i=0;i<20;i++)
    {
        for(j=0;j<10;j++)
            arr[j][i]=0;
    }

    eraseShape();
    drawShape();
}
void shapeL::rotateShape() {
    eraseShape();
    int i;
    int interm_x[4];
    int interm_y[4];
    for(i = 0;i < 4; i++) {
        interm_x[i] = p[i]->x;
        interm_y[i] = p[i]->y;
    }
    switch(rotationPhase){
        case 1: {
            p[1]->x = p[0]->x + 1;
            p[1]->y = p[0]->y;

            p[2]->x = p[0]->x;
            p[2]->y = p[0]->y - 1;

            p[3]->x = p[0]->x;
            p[3]->y = p[0]->y - 2;



            break;
        }
        case 2: {
            p[1]->x = p[0]->x;
            p[1]->y = p[0]->y - 1;

            p[2]->x = p[0]->x - 1;
            p[2]->y = p[0]->y;

            p[3]->x = p[0]->x - 2;
            p[3]->y = p[0]->y;
            break;
        }
        case 3:{
            p[1]->x = p[0]->x - 1;
            p[1]->y = p[0]->y;

            p[2]->x = p[0]->x;
            p[2]->y = p[0]->y + 1;

            p[3]->x = p[0]->x;
            p[3]->y = p[0]->y + 2;
            break;
        }
        case 4:{
            p[1]->x = p[0]->x;
            p[1]->y = p[0]->y + 1;

            p[2]->x = p[0]->x + 1;
            p[2]->y = p[0]->y;

            p[3]->x = p[0]->x + 2;
            p[3]->y = p[0]->y;
            break;
        }
    }
    bool collides = checkCollision(); //rotation does not make collision
    Serial.println(String(collides));
    if (collides)
    { //if it does collide, then revert, do not validate rotation.
        for(i = 0;i < 4; i++) {
            p[i]->y = interm_y[i];
            p[i]->x = interm_x[i];
        }
    }
    else
    {
        rotationPhase = rotationPhase  % 4 + 1;
        for(i = 0;i < 4; i++) {
            p[i]->prev_y = interm_y[i];
            p[i]->prev_x = interm_x[i];
        }
    }
    eraseShape();
    drawShape();

}
