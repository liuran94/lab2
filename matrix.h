//
// Created by liuran94 on 17-11-30.
//

#ifndef LAB2_MATRIX_H
#define LAB2_MATRIX_H
#include "ac.h"
#define MAXSIZE 1024
#define ELL_LEN 200

#define ROW 0
#define COL 1
//#define VALUE 2
//阻尼系数
#define CAMPING_COEFFICIENT 0.15
//计算特征向量的误差值
#define LIMIT 0.01
#define MAX_PATH_LENGTH 1024


void mallocEllCoo(int urlId);
void fileToEllCoo(AC_STRUCT *tree);

void quickSort(int* arr,int startPos, int endPos);//快排
int duplicate(int* arr,int* temp);//去重
void addInEllCoo(int col,int row);//存入稀疏矩阵ELL+COO
void freeEllCoo();
void a_mallocEllCoo();

void generateA(char *urlTxtDir);
void initPageRank();
void generatePageRank();
void printPageRank(char *urlTxtDir,char *resultTxtDir);

int getMaxFromPageRank(double lastMax);

#endif //LAB2_MATRIX_H
