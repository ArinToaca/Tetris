 #ifndef __SHAPE_L_H_INCLUDED__
 #define __SHAPE_L_H_INCLUDED__
#include "shape.h"
class shapeL : public shape {
  public:
     shapeL(uint16_t color);
     void startShape();
     void rotateShape();
};
#endif
