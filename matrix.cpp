//
// Created by liuran94 on 17-11-30.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "matrix.h"

int coocol=0,coototal=16,elltotal=20;
int **ellcol,**ellvalue,**cooarray;

void printEllCoo(){
    //测试打印部分数组
    printf("ellcol:\n");
    for(int i=0;i<10;i++){
        for(int j=0;j<ELL_LEN+1;j++){
            printf("%d ",ellcol[i][j]);
        }
        printf("\n");
    }
//    printf("ellvalue:\n");
//    for(int i=0;i<10;i++){
//        for(int j=0;j<10;j++){
//            printf("%d ",ellvalue[i][j]);
//        }
//        printf("\n");
//    }
    printf("coo:\n");
    for(int i=0;i<3;i++){
        for(int j=0;j<10;j++){
            printf("%d ",cooarray[i][j]);
        }
        printf("\n");
    }
    printf("COO length:%d\n",coototal);
}
void mallocEllCoo(){
    int i;
    ellcol=(int**)malloc(elltotal*sizeof(int*)); //第一维
    for(i=0;i<elltotal; i++)
    {
        ellcol[i]=(int*)malloc((ELL_LEN+1)* sizeof(int));//第二维
        memset(ellcol[i],-1, ELL_LEN* sizeof(int));
        ellcol[i][ELL_LEN]=0;
    }
    ellvalue=(int**)malloc(elltotal*sizeof(int*)); //第一维
    for(i=0;i<elltotal; i++)
    {
        ellvalue[i]=(int*)malloc(ELL_LEN* sizeof(int));//第二维
        memset(ellvalue[i],-1, ELL_LEN* sizeof(int));
    }
    cooarray=(int**)malloc(3*sizeof(int*)); //第一维
    for(i=0;i<3; i++)
    {
        cooarray[i]=(int*)malloc(coototal* sizeof(int));//第二维
        memset(cooarray[i],-1,coototal* sizeof(int));
    }

    return;
}
void reallocEll(){
    ellcol=(int**)realloc(ellcol,(elltotal+1)*sizeof(int*)); //第一维
    ellcol[elltotal]=(int*)malloc(ELL_LEN* sizeof(int));//第二维
    memset(ellcol[elltotal],-1, ELL_LEN* sizeof(int));

    ellvalue=(int**)realloc(ellvalue,(elltotal+1)*sizeof(int*)); //第一维
    ellvalue[elltotal]=(int*)malloc(ELL_LEN* sizeof(int));//第二维
    memset(ellvalue[elltotal],-1, ELL_LEN* sizeof(int));
    elltotal++;
}
void addInEllCoo(int* arr,int n,int row){
    int i,j;
    printf("row:%d\n",row);
    if(row>=elltotal) reallocEll();
    //ell矩阵的最后一列用于标识该行是否需要继续存到coo中
    ellcol[row][0]=row;
    //printf("Nellcol:%d\n",ellcol[row][ELL_LEN]);
//    if(ellcol[row][ELL_LEN]<ELL_LEN) {
//        for (i = 0; i < n; i++) {
//            if (i+ellcol[row][ELL_LEN] < ELL_LEN) {//ELL还可存
//                ellcol[row][i+ellcol[row][ELL_LEN]] = arr[i];
//                ellvalue[row][i+ellcol[row][ELL_LEN]] = 1;
//            } else {//存入COO
//                if (coocol >= coototal) {//COO矩阵溢出
//                    for (j = 0; j < 3; j++) {
//                        cooarray[j] = (int *) realloc(cooarray[j], 2 * coototal * sizeof(int));//COO长度翻倍
//                    }
//                    coototal *= 2;
//                    //printf("coototal:%d\n",coototal);
//                }
//                cooarray[ROW][coocol] = row;
//                cooarray[COL][coocol] = arr[i];
//                cooarray[VALUE][coocol] = 1;
//                coocol++;
//            }
//        }
//    }
//    else{
//        for (i = 0; i < n; i++) {
//                if (coocol >= coototal) {//COO矩阵溢出
//                    for (j = 0; j < 3; j++) {
//                        cooarray[j] = (int *) realloc(cooarray[j], 2 * coototal * sizeof(int));//COO长度翻倍
//                    }
//                    coototal *= 2;
//                    //printf("coototal:%d\n",coototal);
//                }
//                cooarray[ROW][coocol] = row;
//                cooarray[COL][coocol] = arr[i];
//                cooarray[VALUE][coocol] = 1;
//                coocol++;
//        }
//    }
//    ellcol[row][ELL_LEN]=ellcol[row][ELL_LEN]+n;
}
void quickSort(int* arr,int startPos, int endPos) {
    int i, j;
    int key;
    key = arr[startPos];
    i = startPos;
    j = endPos;
    while (i<j)
    {
        while (arr[j] >= key && i<j)--j; //————1 从后往去前扫，直到找到一个a[j]<key或遍历完
        arr[i] = arr[j];
        while (arr[i] <= key && i<j)++i; //————2 从后往去前扫，直到找到一个a[i]>key或遍历完
        arr[j] = arr[i];
    }
    arr[i] = key;
    if (i - 1>startPos) quickSort(arr, startPos, i - 1); //————1 如果key前还有两个及以上的数，排key前的数（有一个的话自然就不用排了）
    if (endPos>i + 1) quickSort(arr, i + 1, endPos);//————2 如果key后还有两个及以上的数，排key后的数
}
//将arr数组中的数据去重后存入temp中
int duplicate(int* arr,int* temp,int startPos,int endPos){
    int n,i=0,j=0,tindex=0;
    bool flag= false;

    while(arr[i]!=-1){
        n=arr[i];
        j=0;
        while(temp[j]!=-1){
            if(temp[j]==n){
                flag= true;
                break;
            }
            j++;
        }
        i++;
        if(flag){
            flag= false;
            continue;
        }
        temp[tindex]=n;
        tindex++;
        flag= false;
    }
    return tindex;//返回新数组的长度
}