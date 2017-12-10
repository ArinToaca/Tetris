//
// Created by arin on 09.12.2017.
//
#ifndef LIBRARIES_SHAPET_H
#define LIBRARIES_SHAPET_H

#include "shape.h"
class shapeT : public shape {
public:
    shapeT(uint16_t color);
    void startShape();
    void rotateShape();
};

#endif //LIBRARIES_SHAPET_H
