//
// Created by arin on 10.12.2017.
//

#ifndef SHAPE_SHAPEMIRRORL_H
#define SHAPE_SHAPEMIRRORL_H

#include "shape.h"
class shapeMirrorL : public shape {
public:
    shapeMirrorL(uint16_t **array, uint16_t color);
    void startShape();
    void rotateShape();
};
#endif //SHAPE_SHAPEMIRRORL_H
