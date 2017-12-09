#include "shape.h"
void shape::moveShapeLeft(int (&arr)[10][20] ){
    int i;
        for(i = 0; i < 4; i++){
            p[i]->prev_y = p[i]->y;
            p[i]->y--;
        }
}
void shape::moveShapeRight(int (&arr)[10][20] ){
    int i;
    for(i = 0; i < 4; i++){
        p[i]->prev_y = p[i]->y;
        p[i]->y--;
    }
};

void shape::moveShapeDown(int (&arr)[10][20] ){
    int i;
    for(i = 0;i < 4; i++) {
        p[i]->prev_y = p[i]->y;
        p[i]->x++;
    }
};
void shape::stopShape(int (&arr)[10][20] ) {
    
};

