//
// Created by arin on 10.12.2017.
//

#ifndef SHAPE_MATRIX_H
#define SHAPE_MATRIX_H
#include<Arduino.h>
class Matrix{
public:
    Matrix(int rows,int cols);
    //~Matrix();
    int rows;
    int columns;
    void setValue(int x,int y, uint16_t color);
    uint16_t getValue(int x,int y);

private:
    uint16_t **matrix;
};


#endif //SHAPE_MATRIX_H
