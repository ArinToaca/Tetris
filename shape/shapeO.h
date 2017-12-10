//
// Created by arin on 09.12.2017.
//

#ifndef LIBRARIES_SHAPEO_H
#define LIBRARIES_SHAPEO_H

#include "shape.h"
class shapeO : public shape {
public:
    shapeO(uint16_t color);
    void startShape();
    void rotateShape();
};
#endif //LIBRARIES_SHAPEO_H
