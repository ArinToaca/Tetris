#include "shape.h"

void shape::shape(int (&array)[10][20], uint16_t color) {
    this->arr = array;
    this->color = color;
}







void shape::eraseShape()
{
    int i;
    for(i=0; i<4;i++)
    {   
        draw->clearBlock(p[i]->x,p[i]->y);
    }
}

void shape::drawShape()
{
    int i;
    for(i=0; i<4;i++)
    {   
        draw->drawBlock(p[i]->x,p[i]->y,color);
    }
}

void shape::moveShapeLeft(){
    if(stopObject)
        return;
    eraseShape();
    int i;
    for(i = 0; i < 4; i++){
        p[i]->prev_x = p[i]->x;
        p[i]->x--;
    }
    checkCollision();
    if(stopLeftMovement){
        for(i = 0; i < 4; i++){
            p[i]->x = p[i]->prev_x;
        }
    }
    eraseShape();
    drawShape();
}

void shape::moveShapeRight(){
    if(stopObject)
        return;
    eraseShape();
    int i;
    for(i = 0; i < 4; i++){
        p[i]->prev_x = p[i]->x;
        p[i]->x++;
    }
    checkCollision();
    if(stopRightMovement){
        for(i = 0; i < 4; i++){
            p[i]->x = p[i]->prev_x;
        }
    }
    eraseShape();
    drawShape();
};

bool shape::moveShapeDown(){
    if(stopObject)
        return true;
    eraseShape();
    int i;
    for(i = 0;i < 4; i++) {
        p[i]->prev_y = p[i]->y;
        p[i]->y--;
    }
    checkCollision();
    if(stopObject){
        for(i = 0; i < 4; i++){
            p[i]->y = p[i]->prev_y;
        }
    }
    eraseShape();
    drawShape();
    if(stopObject){
        this->stopShape();
        return false;
    }

    return true;
};
bool shape::checkArray(int x, int y)
{
    if(x<0 || x>9)
        return false;
    if(y<0 || y>19)
        return false;

    return arr[x][y];
}

bool shape::checkPointCollision(int x, int y) {

    //side collision
    if( x == -1 || checkArray(x,y) ) { //left collision
        stopLeftMovement = true;
    }
    else
    {
        stopLeftMovement = false;
    }
    if( x == 10 ||  checkArray(x,y)){ //right collision
        stopRightMovement = true;
    }
    else
    {
        stopRightMovement = false;
    }
    //down collision, need to drop
    if( y == -1 || checkArray(x,y) ){
       stopObject = true;
    }
    else{
        stopObject = false;
    }

    Serial.println(String(stopObject)+String(stopLeftMovement)+String(stopRightMovement)+String(checkArray(x,y)));

    return stopObject || stopLeftMovement || stopRightMovement;
}
bool shape::checkCollision() { //called every movement left,right,down
    int i;
    bool ret=false;;
    for(i = 0; i < 4; i++){
      ret = ret || checkPointCollision(p[i]->x,p[i]->y);
    }
}
void shape::stopShape() { //mark cells as occupied, destroy object
    int i,j;
    for(i = 0; i < 4;i++){
        arr[p[i]->x][p[i]->y] = this->getColor();
    }
};


