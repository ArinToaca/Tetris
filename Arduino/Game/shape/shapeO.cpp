//
// Created by arin on 09.12.2017.
//

#include "shapeO.h"

void shapeO::rotateShape() {//oop LISKOV VIOLATION..., but O shape does not rotate
}
void shapeO::startShape() {
    p[0] = new point(4,19);
    p[1] = new point(5,19);
    p[2] = new point(4,18);
    p[3] = new point(5,18);
    this->rotationPhase = 0;
}