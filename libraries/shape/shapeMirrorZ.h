//
// Created by arin on 10.12.2017.
//

#ifndef SHAPE_SHAPEMIRRORZ_H
#define SHAPE_SHAPEMIRRORZ_H
#include "shape.h"
class shapeMirrorZ : public shape {
public:
    shapeMirrorZ(uint16_t color, tzeny_draw* d, Matrix *m);
    void startShape();
    void rotateShape();
};
#endif //SHAPE_SHAPEMIRRORZ_H
