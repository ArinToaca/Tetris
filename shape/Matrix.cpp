//
// Created by arin on 10.12.2017.
//

#include "Matrix.h"
Matrix::Matrix(int rows, int cols) {
    matrix = (uint16_t**)malloc(rows * sizeof(uint16_t*) );
    int i;
    this->rows = rows;
    this->columns = cols;
    for(i = 0;i < cols;i++){
        matrix[i] = (uint16_t*)malloc(cols * sizeof(uint16_t));
    }
}
Matrix::~Matrix(){
    int i;
    for(i = 0;i < columns;i++){
        free(matrix[i]);
    }
    free(matrix);
}
uint16_t Matrix::getValue(int x, int y) {
    return matrix[x][y];
}
void Matrix::setValue(int x, int y, uint16_t color) {
    matrix[x][y] = color;
}