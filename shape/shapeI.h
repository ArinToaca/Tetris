//
// Created by arin on 10.12.2017.
//

#ifndef SHAPE_SHAPEI_H
#define SHAPE_SHAPEI_H
#include "shape.h"
class shapeI : public shape {
public:
    shapeI(uint16_t color);
    void startShape();
    void rotateShape();
};

#endif //SHAPE_SHAPEI_H
