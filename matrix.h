//
// Created by liuran94 on 17-11-30.
//

#ifndef LAB2_MATRIX_H
#define LAB2_MATRIX_H
#include "ac.h"
#define MAXSIZE 1024
#define ELL_LEN 50

#define ROW 0
#define COL 1
#define VALUE 2
//阻尼系数
#define CAMPING_COEFFICIENT 0.15
//计算特征向量的误差值
#define LIMIT 0.0001

void printEllCoo();
void mallocEllCoo();
void reallocEll(int row);
void quickSort(int* arr,int startPos, int endPos);//快排
int duplicate(int* arr,int* temp,int startPos,int endPos);//去重
void addInEllCoo(int* arr,int n,int row);//存入稀疏矩阵ELL+COO

void a_mallocEllCoo();
void a_reallocEll(int row);
void generateA();
void initPageRank();
void generatePageRank();

#endif //LAB2_MATRIX_H
