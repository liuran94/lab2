//
// Created by liuran94 on 17-11-30.
//

#ifndef LAB2_MATRIX_H
#define LAB2_MATRIX_H
#include "ac.h"
#define MAXSIZE 1024
#define ELL_LEN 40
#define ROW 0
#define COL 1
#define VALUE 2

void printEllCoo();
void mallocEllCoo();
void reallocEll();
void quickSort(int* arr,int startPos, int endPos);//快排
int duplicate(int* arr,int* temp,int startPos,int endPos);//去重
void addInEllCoo(int* arr,int n,int row);//存入稀疏矩阵ELL+COO

#endif //LAB2_MATRIX_H
