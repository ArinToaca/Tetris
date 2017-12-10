//
// Created by arin on 10.12.2017.
//
#include <Arduino.h>
#include "Matrix.h"
Matrix::Matrix(int rows, int cols) {
    Serial.println("Matrix init!");
    matrix = (uint16_t**)calloc(rows,sizeof(uint16_t*) );
    int i,j;
    this->rows = rows;
    this->columns = cols;
    for(i = 0;i < rows;i++){
        matrix[i] = (uint16_t*)calloc(cols,sizeof(uint16_t));
    }
    Serial.println("Matrix done!");
}
/*Matrix::~Matrix(){
    int i;
    for(i = 0;i < columns;i++){
        free(matrix[i]);
    }
    free(matrix);
}*/
uint16_t Matrix::getValue(int x, int y) {
    return matrix[y][x];
}
void Matrix::setValue(int x, int y, uint16_t color) {
    matrix[y][x] = color;
}