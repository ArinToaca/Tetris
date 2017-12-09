#include "shape.h"

void shape::redrawShape()
{
    int i;
    for(i=0; i<4;i++)
    {   
        draw->clearBlock(p[i]->prev_x,p[i]->prev_y);
    }
    for(i=0; i<4;i++)
    {   
        draw->drawBlock(p[i]->x,p[i]->y,color);
    }
}

void shape::moveShapeLeft(){
    int i;
    if(stopLeftMovement == false){
        stopRightMovement = false;
        for(i = 0; i < 4; i++){
            p[i]->prev_x = p[i]->x;
            p[i]->x--;
        }
    }
    redrawShape();
    checkCollision();
}

void shape::moveShapeRight(){
    if(stopRightMovement == false){
       stopLeftMovement = false;
    for(int i = 0; i < 4; i++){
        p[i]->prev_x = p[i]->x;
        p[i]->x++;
    }
    }
    redrawShape();
    checkCollision();
};

void shape::moveShapeDown(){
    int i;
    for(i = 0;i < 4; i++) {
        p[i]->prev_y = p[i]->y;
        p[i]->y--;
    }
    redrawShape();
    checkCollision();
};
void shape::checkPointCollision(int x, int y) {
    //side collision
    if( x == 0 || arr[x-1][y] ) { //left collision
        stopLeftMovement = true;
    }
    if( x == 10 || arr[x+1][y]){ //right collision
        stopRightMovement = true;
    }
    //down collision, need to drop
    if( y == 0 ||  arr[x][y-1] ){
       stopObject = true;
    }
}
void shape::checkCollision() { //called every movement left,right,down
    stopObject = false;
    int i;
    for(i = 0; i < 4; i++){
      checkPointCollision(p[i]->x,p[i]->y);
        if(stopObject);
         break;
    }
    if(stopObject){
        this->stopShape();
    }
}
void shape::stopShape() { //mark cells as occupied, destroy object
    int i,j;
    for(i = 0; i < 4;i++){
        arr[p[i]->x][p[i]->y] = true;
    }

    bool accum[3];
    for(j = 0; j < 20;j++){
        accum[0] &= arr[p[0]->x][j];
        accum[1] &= arr[p[1]->x][j];
        accum[2] &= arr[p[2]->x][j];
        accum[3] &= arr[p[3]->x][j];
    }

    delete this;

};


