 #ifndef __SHAPE_H_INCLUDED__
 #define __SHAPE_H_INCLUDED__
  
  
class shape{
  
  public:
      class point{
          public:
          int x,y,prev_x,prev_y;
          point(int x,int y){
              this->x = x; 
              this->y = y;
          };
          void drawPoint(){
              //IMPLEMENT TZENY! :D
          };
      };
      point *p[4];
      void moveShapeLeft(int (&arr)[10][20] );
      void moveShapeRight(int (&arr)[10][20] );
      void moveShapeDown(int (&arr)[10][20] );
      void stopShape(int (&arr)[10][20] );
 };
  
  #endif

