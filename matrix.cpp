//
// Created by liuran94 on 17-11-30.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "matrix.h"

int cooCol=0,cooTotal=16,ellTotal=1024,colTotal=0;
int **ellCol,**cooArray;

//a_cooIndex是记录coo矩阵的行号和列号的二维int型矩阵
int **a_ellCol,**a_cooIndex;

//a_cooValue是记录coo矩阵行列号指向的单元的值的一维double型数组
double **a_ellValue,*a_cooValue;

/*a_cooCol表示A矩阵的coo矩阵目前已记录的元素个数
 * a_cooTotal表示A矩阵的coo矩阵目前的列数（目前可以记录的元素个数）
 * a_ellTotal表示A矩阵的ell矩阵的总行数（总链接数）
 */
int a_cooCol=0,a_cooTotal=16,a_ellTotal=1024,a_colTotal=0;
double *pageRank;
double *pageRankTemp;

void printEllCoo(){
    //测试打印部分数组
    printf("ellCol:\n");
    for(int i=0;i<10;i++){
        for(int j=0;j<ELL_LEN+1;j++){
            printf("%d ",ellCol[i][j]);
        }
        printf("\n");
    }
//    printf("ellValue:\n");
//    for(int i=0;i<10;i++){
//        for(int j=0;j<10;j++){
//            printf("%d ",ellValue[i][j]);
//        }
//        printf("\n");
//    }
    printf("coo:\n");
    for(int i=0;i<2;i++){
        for(int j=0;j<10;j++){
            printf("%d ",cooArray[i][j]);
        }
        printf("\n");
    }
    printf("COO length:%d\n",cooTotal);
}

void printAEllCoo(){
    //测试打印部分数组
    printf("AellCol:\n");
    for(int i=0;i<10;i++){
        for(int j=0;j<ELL_LEN+1;j++){
            printf("%d ",a_ellCol[i][j]);
        }
        printf("\n");
    }
    printf("ellValue:\n");
    for(int i=0;i<10;i++){
        for(int j=0;j<10;j++){
            printf("%lf ",a_ellValue[i][j]);
        }
        printf("\n");
    }
}

void mallocEllCoo(){
    int i;
    ellCol=(int**)malloc(ellTotal*sizeof(int*)); //第一维
    for(i=0;i<ellTotal; i++)
    {
        ellCol[i]=(int*)malloc((ELL_LEN+1)* sizeof(int));//第二维
        memset(ellCol[i],-1, ELL_LEN* sizeof(int));
        ellCol[i][ELL_LEN]=0;
    }
    cooArray=(int**)malloc(3*sizeof(int*)); //第一维
    for(i=0;i<3; i++)
    {
        cooArray[i]=(int*)malloc(cooTotal* sizeof(int));//第二维
        memset(cooArray[i],-1,cooTotal* sizeof(int));
    }

    return;
}

void reallocEll(int row){
    ellCol=(int**)realloc(ellCol,(ellTotal+(row-ellTotal+1))*sizeof(int*)); //第一维
    for(int i=0;i<(row-ellTotal+1);i++) {
        ellCol[ellTotal+i] = (int *) malloc((ELL_LEN+1) * sizeof(int));//第二维
        memset(ellCol[ellTotal+i], -1, ELL_LEN * sizeof(int));
        ellCol[ellTotal+i][ELL_LEN]=0;
    }
    ellTotal=row+1;
}

void freeEllCoo(){
    for(int i=0;i<ellTotal;i++){
        free(ellCol[i]);
    }
    free(ellCol);
    for(int i=0;i<3;i++){
        free(cooArray[i]);
    }
    free(cooArray);
}

void addInEllCoo(int* arr,int n,int row){
    if(row+1>colTotal) colTotal=row+1;
    int i,j;
//    printf("row:%d\n",row);
    if(row>=ellTotal) reallocEll(row);
    //ell矩阵的最后一列用于标识该行是否需要继续存到coo中
    //ellCol[row][0]=row;

    //printf("Nellcol:%d\n",ellCol[row][ELL_LEN]);
    if(ellCol[row][ELL_LEN]<ELL_LEN) {
        for (i = 0; i < n; i++) {
            if (i+ellCol[row][ELL_LEN] < ELL_LEN) {//ELL还可存
                ellCol[row][i+ellCol[row][ELL_LEN]] = arr[i];
            } else {//存入COO
                if (cooCol >= cooTotal) {//COO矩阵溢出
                    for (j = 0; j < 3; j++) {
                        cooArray[j] = (int *) realloc(cooArray[j], 2 * cooTotal * sizeof(int));//COO长度翻倍
                    }
                    cooTotal *= 2;
                    //printf("cooTotal:%d\n",cooTotal);
                }
                cooArray[ROW][cooCol] = row;
                cooArray[COL][cooCol] = arr[i];
                //cooArray[VALUE][cooCol] = 1;
                cooCol++;
            }
        }
    }
    else{
        for (i = 0; i < n; i++) {
                if (cooCol >= cooTotal) {//COO矩阵溢出
                    for (j = 0; j < 3; j++) {
                        cooArray[j] = (int *) realloc(cooArray[j], 2 * cooTotal * sizeof(int));//COO长度翻倍
                    }
                    cooTotal *= 2;
                    //printf("cooTotal:%d\n",cooTotal);
                }
                cooArray[ROW][cooCol] = row;
                cooArray[COL][cooCol] = arr[i];
                //cooArray[VALUE][cooCol] = 1;
                cooCol++;
        }
    }
    ellCol[row][ELL_LEN]=ellCol[row][ELL_LEN]+n;
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
    int total=ellCol[row][ELL_LEN];
    int i;
    for(i=0;i<total;i++){
        if(i<ELL_LEN){
            if(ellCol[row][i]==col)
                return 1;
        }
        else{
            for(i=0;i<cooCol;i++){
                if(cooArray[ROW][i]==row&&cooArray[COL][i]==col)
                    return 1;
            }
        }
    }
    return 0;
}

//初始化A矩阵的ell和coo矩阵
void a_mallocEllCoo(){
    int i;
    a_ellCol=(int**)malloc(a_ellTotal*sizeof(int*)); //第一维
    for(i=0;i<a_ellTotal; i++)
    {
        a_ellCol[i]=(int*)malloc((ELL_LEN+1)* sizeof(int));//第二维
        memset(a_ellCol[i],-1, ELL_LEN* sizeof(int));
        a_ellCol[i][ELL_LEN]=0;
    }
    a_ellValue=(double **)malloc(a_ellTotal*sizeof(double *)); //第一维
    for(i=0;i<a_ellTotal; i++)
    {
        a_ellValue[i]=(double*)malloc(ELL_LEN* sizeof(double));//第二维
        memset(a_ellValue[i],-1, ELL_LEN* sizeof(int));
    }
    a_cooIndex=(int**)malloc(2*sizeof(int*)); //第一维
    for(i=0;i<2; i++)
    {
        a_cooIndex[i]=(int*)malloc(a_cooTotal* sizeof(int));//第二维
        memset(a_cooIndex[i],-1,a_cooTotal* sizeof(int));
    }
    a_cooValue=(double*)malloc(a_cooTotal* sizeof(double));
}

//给A矩阵的ell矩阵增加到row行
void a_reallocEll(int row){
    a_ellCol=(int**)realloc(a_ellCol,(a_ellTotal+(row-a_ellTotal+1))*sizeof(int*)); //第一维
    for(int i=0;i<(row-a_ellTotal+1);i++) {
        a_ellCol[a_ellTotal+i] = (int *) malloc((ELL_LEN+1) * sizeof(int));//第二维
        memset(a_ellCol[a_ellTotal+i], -1, ELL_LEN * sizeof(int));
        a_ellCol[a_ellTotal+i][ELL_LEN]=0;
    }

    a_ellValue=(double **)realloc(a_ellValue,(a_ellTotal+(row-a_ellTotal+1))*sizeof(double *)); //第一维
    for(int i=0;i<(row-a_ellTotal+1);i++) {
        a_ellValue[a_ellTotal+i] = (double *) malloc(ELL_LEN * sizeof(double));//第二维
        memset(a_ellValue[a_ellTotal+i], -1, ELL_LEN * sizeof(int));
    }
    a_ellTotal=row+1;
}

void setAValueByIndex(int row,int col,double value){
    int i;
    //printf("row:%d,col:%d\n",row,col);
    if(row>=a_ellTotal) a_reallocEll(row);
    //ell矩阵的最后一列表示该行在ell和coo中总的元素个数
    //ELL还可存
    if(a_ellCol[row][ELL_LEN]<ELL_LEN) {
        a_ellCol[row][a_ellCol[row][ELL_LEN]] = col;
        a_ellValue[row][a_ellCol[row][ELL_LEN]] = value;
    }
    //存入COO
    else {
        //COO矩阵溢出
        if (a_cooCol >= a_cooTotal) {
            //a_cooIndex长度翻倍
            for (i = 0; i < 2; i++)
                a_cooIndex[i] = (int *) realloc(a_cooIndex[i], 2 * a_cooTotal * sizeof(int));
            //a_cooValue长度翻倍
            a_cooValue=(double*)realloc(a_cooValue,2 * a_cooTotal * sizeof(double));
            a_cooTotal *= 2;
            //printf("cooTotal:%d\n",cooTotal);
        }
        a_cooIndex[ROW][a_cooCol] = row;
        a_cooIndex[COL][a_cooCol] = col;
        a_cooValue[a_cooCol] = value;
        a_cooCol++;
    }
    a_ellCol[row][ELL_LEN]++;
}

//将矩阵每一列的非-1元素除以该列非-1元素的总和，得到GM矩阵
//根据修正公式得到A矩阵的值
void generateA(){
    a_colTotal=colTotal;
    int i,j,total;
    double value;
    //行优先遍历
    for(i=0;i<colTotal;i++){
        total=0;
        for (j = 0; j < colTotal; j++) {
            if(getGValueByIndex(i,j)==1)
                total++;
        }
        //计算GM矩阵该位置的值
        value=1/(double)total;
        //计算A矩阵该位置的值
        value=(1-CAMPING_COEFFICIENT)*value+CAMPING_COEFFICIENT/(double)colTotal;
        double value1=CAMPING_COEFFICIENT/(double)colTotal;
        //printf("value1 %lf\n",value1);
        for (j = 0; j < colTotal; j++) {
            //写入A矩阵
            if(getGValueByIndex(i,j)==0)
                setAValueByIndex(j,i,CAMPING_COEFFICIENT/(double)colTotal);
            else
                setAValueByIndex(j,i,value);
        }
    }
    freeEllCoo();
    //printAEllCoo();
}

//pageRank向量初始化长度等于总共的链接数且值为1的数组
void initPageRank(){
    pageRank=(double *)malloc(a_colTotal* sizeof(double));
    pageRankTemp=(double *)malloc(a_colTotal* sizeof(double));
    for(int i=0;i<a_colTotal;i++){
        pageRank[i]=1;
        pageRankTemp[i]=1;
    }
    //printPageRank();
}

void generatePageRank(){
    int i,j,k,col;
    bool successFlag= false;
    double value;
    while (!successFlag){
        successFlag=true;
        for(i=0;i<a_colTotal;i++){
            value=0;
            //int num=a_ellCol[i][ELL_LEN+1]-ELL_LEN;
            for(j=0;j<ELL_LEN;j++){
                col=a_ellCol[i][j];
                if(col!=-1)
                    value+=(pageRank[col]*a_ellValue[i][j]);
            }
            for(k=0;k<a_cooCol;k++){
                if(a_cooIndex[ROW][k]==i){
                    col=a_cooIndex[COL][k];
                    value+=pageRank[col]*a_cooValue[k];
                }
            }
            if(fabs(pageRank[i]-value)>LIMIT)
                successFlag= false;
            pageRankTemp[i]=value;
        }
        for(i=0;i<a_colTotal;i++)
            pageRank[i]=pageRankTemp[i];
        //printPageRank();
    }
}

int getMaxFromPageRank(double lastMax) {
    int maxIndex=0;
    for(int i=1;i<a_colTotal;i++){
        if(pageRank[i]>pageRank[maxIndex]&&pageRank[i]<lastMax)
            maxIndex=i;
    }
    return maxIndex;
}
void printPageRank(){
    printf("pageRank:\n");
    int maxIndex;
    int lastMax=99999;
    FILE *out;
    char buffer[1024];
    if((out = fopen("./result.txt", "r")) == NULL){
        exit(1);
    }
    for(int i=0;i<10;i++){
        maxIndex=getMaxFromPageRank(lastMax);
        printf("***%d***  %f ",i+1,pageRank[maxIndex]);
        fseek(out, 0, SEEK_SET);
        for(int j=0;j<maxIndex;j++){
            memset(buffer,0,1024);
            fgets(buffer,1024,out);
        }
        printf("%s\n",buffer);
        lastMax=pageRank[maxIndex];
    }
    fclose(out);
}

