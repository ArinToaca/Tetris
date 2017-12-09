#include "shape.h"


void swap(int *xp, int *yp)
{
    int temp = *xp;
    *xp = *yp;
    *yp = temp;
}

// A function to implement bubble sort
void bubbleSort(int arr[], int n)
{
    int i, j;
    for (i = 0; i < n-1; i++)

        // Last i elements are already in place
        for (j = 0; j < n-i-1; j++)
            if (arr[j] > arr[j+1])
                swap(&arr[j], &arr[j+1]);
}

void shape::drawPixel(point* p)
{
    draw->clearBlock(p->prev_x,p->prev_y);
    draw->drawBlock(p->x,p->y,color);
}

void shape::moveShapeLeft(){
    int i;
    stopRightMovement = false;
        for(i = 0; i < 4; i++){
            p[i]->prev_x = p[i]->x;
            p[i]->x--;
            drawPixel(p[i]);
        }
   // checkCollision();
}

void shape::moveShapeRight(){
    stopLeftMovement = false;
    for(int i = 0; i < 4; i++){
        p[i]->prev_x = p[i]->x;
        p[i]->x++;
        drawPixel(p[i]);
    }
    //checkCollision();
};

void shape::moveShapeDown(){
    int i;
    for(i = 0;i < 4; i++) {
        p[i]->prev_y = p[i]->y;
        p[i]->y--;
        drawPixel(p[i]);
    }
    //checkCollision();
};
void shape::checkPointCollision(int x, int y) {
    //side collision
    if( x > 0 && arr[x-1][y]) { //left collision
        stopLeftMovement = true;
    }
    if( x > 0 && arr[x-1][y]){ //right collision
        stopRightMovement = true;
    }
    //down collision, need to drop
    if( y > 0 ||  arr[x][y-1] ){
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
void shape::delLines(int index[4]){
    int i,j;


    for(i = 3; i > 0;i--){
        if(index[i] > 0) { //e valid, exista linie stearsa
            for (j = 0; j < 3; j++) {
                arr[index[i]][j] = arr[index[i] - 1][j];
                point p(index[i],j);
                p.prev_x = index[i]-1;
                p.prev_y = j;
                drawPixel(&p);
            }
        }
    }
}
void shape::stopShape() { //mark cells as occupied, destroy object
    //ALSO CHECK IF COLLISION TRIGGERED A WIN!
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

    int indexes[3];
        for(i = 0;i < 3;i++){
            indexes[i] = -1;
        }
    int counter = 0;
    for(i = 0;i < 4;i++){
        if (accum[i])
            indexes[counter++] = p[i]->x;
    }
    bubbleSort(indexes,4);
    delLines(indexes);


    delete this;

};


