 #ifndef __SHAPE_L_H_INCLUDED__
 #define __SHAPE_L_H_INCLUDED__
#include "shape.h"
class shapeL : public shape {
  public:
     
     void startShape(int (&arr)[10][20] );
     void rotateShape(int (&arr)[10][20] ); 
     void stopShape(int (&arr)[10][20] );
};
#endif
