//
// Created by liuran94 on 17-11-30.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "matrix.h"

int coocol=0,coototal=16,elltotal=1024;
int **ellcol,**ellvalue,**cooarray;

int **AEllCol,**ACooArray;
double **AEllValue,*ACooValue;
int ACooCol=0,ACooTotal=16,AEllTotal=1024;
double *pageRank;

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
void reallocEll(int row){
    ellcol=(int**)realloc(ellcol,(elltotal+(row-elltotal+1))*sizeof(int*)); //第一维
    for(int i=0;i<(row-elltotal+1);i++) {
        ellcol[elltotal+i] = (int *) malloc((ELL_LEN+1) * sizeof(int));//第二维
        memset(ellcol[elltotal+i], -1, ELL_LEN * sizeof(int));
        ellcol[elltotal+i][ELL_LEN]=0;
    }

    ellvalue=(int**)realloc(ellvalue,(elltotal+(row-elltotal+1))*sizeof(int*)); //第一维
    for(int i=0;i<(row-elltotal+1);i++) {
        ellvalue[elltotal+i] = (int *) malloc(ELL_LEN * sizeof(int));//第二维
        memset(ellvalue[elltotal+i], -1, ELL_LEN * sizeof(int));
    }
    elltotal=row+1;
}
void addInEllCoo(int* arr,int n,int row){
    int i,j;
    printf("row:%d\n",row);
    if(row>=elltotal) reallocEll(row);
    //ell矩阵的最后一列用于标识该行是否需要继续存到coo中
    //ellcol[row][0]=row;

    //printf("Nellcol:%d\n",ellcol[row][ELL_LEN]);
    if(ellcol[row][ELL_LEN]<ELL_LEN) {
        for (i = 0; i < n; i++) {
            if (i+ellcol[row][ELL_LEN] < ELL_LEN) {//ELL还可存
                ellcol[row][i+ellcol[row][ELL_LEN]] = arr[i];
                ellvalue[row][i+ellcol[row][ELL_LEN]] = 1;
            } else {//存入COO
                if (coocol >= coototal) {//COO矩阵溢出
                    for (j = 0; j < 3; j++) {
                        cooarray[j] = (int *) realloc(cooarray[j], 2 * coototal * sizeof(int));//COO长度翻倍
                    }
                    coototal *= 2;
                    //printf("coototal:%d\n",coototal);
                }
                cooarray[ROW][coocol] = row;
                cooarray[COL][coocol] = arr[i];
                cooarray[VALUE][coocol] = 1;
                coocol++;
            }
        }
    }
    else{
        for (i = 0; i < n; i++) {
                if (coocol >= coototal) {//COO矩阵溢出
                    for (j = 0; j < 3; j++) {
                        cooarray[j] = (int *) realloc(cooarray[j], 2 * coototal * sizeof(int));//COO长度翻倍
                    }
                    coototal *= 2;
                    //printf("coototal:%d\n",coototal);
                }
                cooarray[ROW][coocol] = row;
                cooarray[COL][coocol] = arr[i];
                cooarray[VALUE][coocol] = 1;
                coocol++;
        }
    }
    ellcol[row][ELL_LEN]=ellcol[row][ELL_LEN]+n;
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

//根据行列号在稀疏矩阵G中找到对应的元素
int getGValueByIndex(int row,int col){
    //取ell该行最后一个数字  表示该行总共的元素个数
    int total=ellcol[row][ELL_LEN];
    int i;
    for(i=0;i<total;i++){
        if(i<ELL_LEN){
            if(ellcol[row][i]==col)
                return 1;
        }
        else{
            for(i=0;i<coocol;i++){
                if(cooarray[ROW][i]==row&&cooarray[COL][i]==col)
                    return 1;
            }
        }
    }
    return 0;
}
void setAValueByIndex(int row,int col,double value){

}

//将矩阵每一列的非-1元素除以该列非-1元素的总和，得到GM矩阵
//根据修正公式得到A矩阵的值
void generateA(){
    int i,j,total;
    double value;
    //列优先遍历
    for(i=0;i<elltotal;i++){
        total=0;
        for (j = 0; j < elltotal; j++) {
            if(getGValueByIndex(j,i)==1)
                total++;
        }
        for (j = 0; j < elltotal; j++) {
            //计算GM矩阵该位置的值
            value=1/total;
            //计算A矩阵该位置的值
            value=(1-CAMPING_COEFFICIENT)*value+CAMPING_COEFFICIENT/elltotal;
            //写入A矩阵
            setAValueByIndex(j,i,value);
        }
    }
}

//pageRank向量初始化长度等于总共的链接数且值为1的数组
void initPageRank(){

}

void generatePageRank(){
    int i,j,k,col;
    bool successFlag= false;
    double value;
    while (!successFlag){
        successFlag=true;
        for(i=0;i<AEllTotal;i++){
            value=0;
            //int num=AEllCol[i][ELL_LEN+1]-ELL_LEN;
            for(j=0;j<ELL_LEN;j++){
                col=AEllCol[i][j];
                value+=pageRank[col]*AEllValue[i][j];
            }
            for(k=0;k<ACooCol;k++){
                if(ACooArray[ROW][k]==i){
                    col=ACooArray[COL][k];
                    value+=pageRank[col]*ACooValue[k];
                }
            }
            if(fabs(pageRank[i]-value)>LIMIT)
                successFlag= false;
            pageRank[i]=value;
        }
    }

}