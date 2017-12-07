//
// Created by liuran94 on 17-11-30.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "matrix.h"

int outLinkIndex[500000];
int outLinkIndexTemp[500000];

int cooCol=0,cooTotal=16,ellTotal=0;
int **ellCol,**cooArray;

//a_cooIndex是记录coo矩阵的行号和列号的二维int型矩阵
int **a_ellCol,**a_cooIndex;

//a_cooValue是记录coo矩阵行列号指向的单元的值的一维double型数组
double **a_ellValue,*a_cooValue;

/*a_cooCol表示A矩阵的coo矩阵目前已记录的元素个数
 * a_cooTotal表示A矩阵的coo矩阵目前的列数（目前可以记录的元素个数）
 * a_ellTotal表示A矩阵的ell矩阵的总行数（总链接数）
 */
int a_cooCol=0,a_cooTotal=16,a_ellTotal=1024;
double *pageRank;
double *pageRankTemp;

void mallocEllCoo(int urlId){
    ellTotal=urlId+1;
    int i;
    ellCol=(int**)malloc(ellTotal*sizeof(int*)); //第一维
    for(i=0;i<ellTotal; i++)
    {
        ellCol[i]=(int*)malloc((ELL_LEN+1)* sizeof(int));//第二维
        memset(ellCol[i],-1, ELL_LEN* sizeof(int));
        ellCol[i][ELL_LEN]=0;
    }
    cooArray=(int**)malloc(2*sizeof(int*)); //第一维
    for(i=0;i<2; i++)
    {
        cooArray[i]=(int*)malloc(cooTotal* sizeof(int));//第二维
        memset(cooArray[i],-1,cooTotal* sizeof(int));
    }

}

void fileToEllCoo(AC_STRUCT *tree,char *linkTxtDir){
    FILE *fp = fopen(linkTxtDir, "r");
    char buffer[MAXSIZE];
    char writebuffer[MAXSIZE];
    int index=0,ibuffer,i,id;
    if(NULL == fp)
    {
        printf("failed to open link.txt\n");
        exit(1);
    }
    while(fgets(buffer,MAXSIZE,fp)!=NULL){
        if(strcmp(buffer,"\n")==0){
            memset(buffer,0,sizeof(buffer));
            memset(writebuffer,0,sizeof(writebuffer));
            index=0;
            continue;
        }
        while(buffer[index]!=' '&&index<MAXSIZE){
            writebuffer[index]=buffer[index];
            index++;
        }
        if(index==MAXSIZE){
            memset(buffer,0,sizeof(buffer));
            memset(writebuffer,0,sizeof(writebuffer));
            index=0;
            continue;
        }
        writebuffer[index]='\0';
        ibuffer=atoi(writebuffer);
        memset(writebuffer,0,sizeof(writebuffer));
        i=0;
        index++;
        while(buffer[index]!='\n'&&index<MAXSIZE){
            writebuffer[i]=buffer[index];
            index++;
            i++;
        }
        writebuffer[i]='\0';
        //printf("ibuffer:%d",ibuffer);
        id=ac_search_string(tree,writebuffer,strlen(writebuffer));
        if(id!=-1){
            addInEllCoo(id,ibuffer);
        }
        memset(buffer,0,sizeof(buffer));
        memset(writebuffer,0,sizeof(writebuffer));
        index=0;
    }
    fclose(fp);
    //remove("./link.txt");
}

void freeEllCoo(){
    for(int i=0;i<ellTotal;i++){
        free(ellCol[i]);
    }
    free(ellCol);
    for(int i=0;i<2;i++){
        free(cooArray[i]);
    }
    free(cooArray);
}

void addInEllCoo(int col,int row){
    //if(row+1>colTotal) colTotal=row+1;
    int i,j;
//    printf("row:%d\n",row);
//    if(row>=ellTotal) reallocEll(row);
    //ell矩阵的最后一列用于标识该行是否需要继续存到coo中
    //ellCol[row][0]=row;

    //printf("Nellcol:%d\n",ellCol[row][ELL_LEN]);
    if(ellCol[row][ELL_LEN]<ELL_LEN) {
        ellCol[row][ellCol[row][ELL_LEN]] = col;
    }
    else{
        if (cooCol >= cooTotal) {//COO矩阵溢出
            for (j = 0; j < 2; j++) {
                cooArray[j] = (int *) realloc(cooArray[j], 2 * cooTotal * sizeof(int));//COO长度翻倍
            }
            cooTotal *= 2;
            //printf("cooTotal:%d\n",cooTotal);
        }
        cooArray[ROW][cooCol] = row;
        cooArray[COL][cooCol] = col;
        cooCol++;
    }
    ellCol[row][ELL_LEN]=ellCol[row][ELL_LEN]+1;
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
int duplicate(int* arr,int* temp){
    memset(temp, -1, 500000 * sizeof(int));
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

//初始化A矩阵的ell和coo矩阵
void a_mallocEllCoo(){
    int i;
    a_ellTotal=ellTotal;
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
        //memset(a_ellValue[i],-1, ELL_LEN* sizeof(int));
    }
    a_cooIndex=(int**)malloc(2*sizeof(int*)); //第一维
    for(i=0;i<2; i++)
    {
        a_cooIndex[i]=(int*)malloc(a_cooTotal* sizeof(int));//第二维
        memset(a_cooIndex[i],-1,a_cooTotal* sizeof(int));
    }
    a_cooValue=(double*)malloc(a_cooTotal* sizeof(double));
}

void setAValueByIndex(int row,int col,double value){
    if(col<0||col>a_ellTotal){//
        printf("error in set value of A :row:%d,col:%d\n",row,col);
        return;
    }
    int i;
    //printf("row:%d,col:%d\n",row,col);
    //if(row>=a_ellTotal) a_reallocEll(row);

    //ell矩阵的最后一列表示该行在ell和coo中总的元素个数
    //ELL还可存
    if(a_ellCol[col][ELL_LEN]<ELL_LEN) {
        a_ellCol[col][a_ellCol[col][ELL_LEN]] = row;
        a_ellValue[col][a_ellCol[col][ELL_LEN]] = value;
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
    a_ellCol[col][ELL_LEN]++;
}

void getOutLinkIndex(int row){
    memset(outLinkIndex, -1, 500000 * sizeof(int));
    //取ell该行最后一个数字  表示该行总共的元素个数
    int total=ellCol[row][ELL_LEN];
    int i;
    int index=0;
    for(i=0;i<total;i++){
        if(i<ELL_LEN&&ellCol[row][i]!=-1){
            outLinkIndex[index]=ellCol[row][i];
            index++;
        }
        else if(cooArray[ROW][i-ELL_LEN]==row&&cooArray[COL][i-ELL_LEN]!=-1){
            outLinkIndex[index]=cooArray[COL][i-ELL_LEN];
            index++;
        }
    }
}

//将矩阵每一列的非-1元素除以该列非-1元素的总和，得到GM矩阵
//根据修正公式得到A矩阵的值
void generateA(char *urlTxtDir){
    FILE *fp=NULL;
    char writeBuf[50];

    if((fp = fopen(urlTxtDir, "a")) == NULL){
        exit(1);
    }
    fputs("\n",fp);
    int i,j,total;
    double value;
    //行优先遍历G
    for(i=0;i<a_ellTotal;i++){
        getOutLinkIndex(i);
        total=duplicate(outLinkIndex,outLinkIndexTemp);
        //计算GM矩阵该位置的值
        if(total==0)
            value=-1;
        else{
            value=1/(double)total;
            //计算A矩阵该位置的值
            //value=(1-CAMPING_COEFFICIENT)*value+CAMPING_COEFFICIENT/(double)a_ellTotal;
            value=(1-CAMPING_COEFFICIENT)*value;
        }
        for(j=0;j<total;j++){
            memset(writeBuf,0,strlen(writeBuf));
            sprintf(writeBuf,"%d %d\n",i,outLinkIndexTemp[j]);
            fputs(writeBuf,fp);
            setAValueByIndex(i,outLinkIndexTemp[j],value);
        }
    }
    freeEllCoo();
    fclose(fp);
    /*for(i=0;i<a_ellTotal;i++){
        printf("col:%d num:%d\n",a_ellCol[i][ELL_LEN],i);
    }*/
}

//pageRank向量初始化长度等于总共的链接数且值为1的数组
void initPageRank(){
    pageRank=(double *)malloc((a_ellTotal+1)* sizeof(double));
    pageRankTemp=(double *)malloc(a_ellTotal* sizeof(double));
    for(int i=0;i<a_ellTotal;i++){
        pageRank[i]=1;
        pageRankTemp[i]=1;
    }
    //printPageRank();
}

void generatePageRank(){
    double correctValue=(CAMPING_COEFFICIENT/(double)a_ellTotal);
    int i,j,k,row;
    int pageRankNum=0;
    int num=0;
    bool successFlag= false;
    double value;
    while (!successFlag){
        successFlag=true;
        for(i=0;i<a_ellTotal;i++){
            num=0;
            value=0;

            for(j=0;j<a_ellTotal;j++){
                value+=pageRank[j];
            }
            value=value*correctValue;
            for(j=0;j<ELL_LEN&&a_ellCol[i][j]!=-1;j++){
                num++;
                row=a_ellCol[i][j];
                value+=(pageRank[row]*a_ellValue[i][j]);
            }
            for(k=0;k<a_cooCol&&num<a_ellCol[i][ELL_LEN];k++){
                if(a_cooIndex[COL][k]==i){
                    num++;
                    row=a_cooIndex[ROW][k];
                    value+=(pageRank[row]*a_cooValue[k]);
                }
            }
            if(fabs(pageRank[i]-value)>LIMIT)
                successFlag= false;
            pageRankTemp[i]=value;
        }
        for(i=0;i<a_ellTotal;i++)
            pageRank[i]=pageRankTemp[i];
        //printPageRank();
        pageRankNum++;
        //printf("pageRank pageRankNum:%d\n",pageRankNum);
    }
    printf("pageRank pageRankNum:%d\n",pageRankNum);
}

int getMaxFromPageRank(double lastMax) {
    int maxIndex=a_ellTotal;
    int q=0;
    while (q<a_ellTotal){
        if(pageRank[q]>pageRank[maxIndex]&&pageRank[q]<lastMax)
            maxIndex=q;
        q++;
    }
    return maxIndex;
}

void printPageRank(char *urlTxtDir,char *resultTxtDir){
    FILE *fp=NULL;
    char writeBuf[MAX_PATH_LENGTH+50];

    int i,j;
    pageRank[a_ellTotal]=-1;
    //printf("pageRank result:\n");
    int maxIndex;
    double lastMax=999;
    FILE *out;
    char buffer[1024];
    if((out = fopen(urlTxtDir, "r")) == NULL){
        exit(1);
    }
    if((fp = fopen(resultTxtDir, "w")) == NULL){
        exit(1);
    }
    for(i=0;i<10;i++){
        maxIndex=getMaxFromPageRank(lastMax);
        printf("***%d***  %d %f ",i+1,maxIndex,pageRank[maxIndex]);
        fseek(out, 0, SEEK_SET);
        for(j=0;j<=maxIndex;j++){
            memset(buffer,0,1024);
            fgets(buffer,1024,out);
        }
        printf("%s\n",buffer);
        j=strlen(buffer)-1;
        while (buffer[j]!=' '){
            buffer[j]='\0';
            j--;
        }
        memset(writeBuf,0,strlen(writeBuf));
        sprintf(writeBuf,"%s%f\n",buffer,pageRank[maxIndex]);
        fputs(writeBuf,fp);

        lastMax=pageRank[maxIndex];
    }
    fclose(out);
    fclose(fp);
}

