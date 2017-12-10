//
// Created by arin on 10.12.2017.
//

#ifndef SHAPE_SHAPEZ_H
#define SHAPE_SHAPEZ_H
#include "shape.h"
class shapeZ : public shape {
public:
    shapeZ(uint16_t color, tzeny_draw* d, Matrix *m);
    void startShape();
    void rotateShape();
};
#endif //SHAPE_SHAPEZ_H
