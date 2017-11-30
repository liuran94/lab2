#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "trie_ac.h"
#define MAXSIZE 1024
#define ELL_LEN 40
#define ROW 0
#define COL 1
#define VALUE 2

int coocol=0,coototal=16;
void quickSort(int* arr,int startPos, int endPos);//快排
int duplicate(int* arr,int* temp,int startPos,int endPos);//去重
void addInEllCoo(int* arr,int n,int row,int** ellcol,int** ellvalue,int** coo);//存入稀疏矩阵ELL+COO

void addInEllCoo(int* arr,int n,int row,int** ellcol,int** ellvalue,int** coo){
    int i,j;
    for(i=0;i<n;i++){
        if(i<ELL_LEN) {//ELL还可存
            ellcol[row][i] = arr[i];
            ellvalue[row][i] = 1;
        }
        else{//存入COO
            if(coocol>=coototal){//COO矩阵溢出
                for(j=0;j<3; j++)
                {
                    coo[j]=(int*)realloc(coo[j],2*coototal* sizeof(int));//COO长度翻倍
                }
                coototal*=2;
                //printf("coototal:%d\n",coototal);
            }
            coo[ROW][coocol]=row;
            coo[COL][coocol]=arr[i];
            coo[VALUE][coocol]=1;
            coocol++;
        }
    }
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
int main()
{
    char buffer[MAXSIZE];
    memset(buffer, 0, sizeof(buffer));
    char writebuffer[MAXSIZE];
    memset(writebuffer, 0, sizeof(writebuffer));

    FILE *fp = fopen("./result.txt", "r");
    FILE *out = fopen("./url.txt", "w");
    if(NULL == fp)
    {
        printf("failed to open result.txt\n");
        return 1;
    }
    AC_STRUCT *tree= ac_alloc();
    int id=0;
    int firstid,secondid;
    int index=0;
    bool firstflag= false;
    char ch=fgetc(fp);
    while(ch!=EOF){
        if(ch=='\n'){
            ch=fgetc(fp);
            continue;
        }
        buffer[index]=ch;
        if(ch!=' '){
            ch=fgetc(fp);
            index++;
            continue;
        }
        buffer[index]='\0';
        //printf("%s\n",buffer);
        if(strcmp(buffer,"#")==0){
            firstflag=true;
            index=0;
        }
        else if(firstflag){
            firstflag= false;
            index=0;
            firstid=ac_add_string(tree, buffer, strlen(buffer), &id);
            memset(writebuffer, 0, sizeof(writebuffer));
            sprintf(writebuffer,"%d %s\n",firstid,buffer);
            fputs(writebuffer,out);
        }
        else{
            index=0;
            secondid=ac_add_string(tree, buffer, strlen(buffer), &id);
            memset(writebuffer, 0, sizeof(writebuffer));
            sprintf(writebuffer,"%d %d\n",firstid,secondid);
            fputs(writebuffer,out);
        }
        ch=fgetc(fp);
    }
    fclose(fp);
    fclose(out);

    out = fopen("./url.txt", "r");
    int n;
    int **ellcol,**ellvalue,**cooarray,i;
    int ibuffer,abuffer[MAXSIZE],temp[MAXSIZE],row=-1;
    int abindex=0;

    ellcol=(int**)malloc(firstid*sizeof(int*)); //第一维
    for(i=0;i<firstid; i++)
    {
        ellcol[i]=(int*)malloc(ELL_LEN* sizeof(int));//第二维
        memset(ellcol[i],-1, ELL_LEN* sizeof(int));
    }
    ellvalue=(int**)malloc(firstid*sizeof(int*)); //第一维
    for(i=0;i<firstid; i++)
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

    index=0;
    firstflag=true;
    memset(abuffer,-1, MAXSIZE* sizeof(int));
    memset(temp,-1, MAXSIZE* sizeof(int));
    memset(buffer, 0, sizeof(buffer));
    memset(writebuffer, 0, sizeof(writebuffer));

    while(fgets(buffer,MAXSIZE,out)!=NULL){
        while(buffer[index]!=' '){
            writebuffer[index]=buffer[index];
            index++;
        }
        writebuffer[index]='\0';
        ibuffer=atoi(writebuffer);
        if(ibuffer!=row){
            index=0;
            if(firstflag){
                firstflag= false;
            }
            else{
                quickSort(abuffer,0,abindex-1);
                n=duplicate(abuffer,temp,0,abindex-1);
                addInEllCoo(temp,n,row,ellcol,ellvalue,cooarray);
                abindex=0;
                memset(abuffer,-1, MAXSIZE* sizeof(int));
                memset(temp,-1, MAXSIZE* sizeof(int));
            }
            row=ibuffer;
        }
        else {
            i = 0;
            while (buffer[index] != '\n') {
                writebuffer[i] = buffer[index];
                index++;
                i++;
            }
            writebuffer[i] = '\0';
            abuffer[abindex] = atoi(writebuffer);
            index = 0;
            abindex++;
        }
    }
    //测试打印部分数组
    printf("ellcol:\n");
    for(int i=0;i<10;i++){
     for(int j=0;j<10;j++){
         printf("%d ",ellcol[i][j]);
     }
        printf("\n");
    }
    printf("ellvalue:\n");
    for(int i=0;i<10;i++){
        for(int j=0;j<10;j++){
            printf("%d ",ellvalue[i][j]);
        }
        printf("\n");
    }
    printf("coo:\n");
    for(int i=0;i<3;i++){
        for(int j=0;j<10;j++){
            printf("%d ",cooarray[i][j]);
        }
        printf("\n");
    }
//    for(i=0;i<n1;i++)
//    {
//        free(ellcol[i]);//释放第二维指针
//    }
//    free(ellcol);//释放第一维指针
    fclose(out);
    ac_free(tree);

    return 0;
}

AC_STRUCT *ac_alloc(void)
{
    AC_STRUCT *node;

    if ((node = (AC_STRUCT*)malloc(sizeof(AC_STRUCT))) == NULL)
        return NULL;
    memset(node, 0, sizeof(AC_STRUCT));

    if ((node->tree =(AC_TREE) malloc(sizeof(ACTREE_NODE))) == NULL) {
        free(node);
        return NULL;
    }
    memset(node->tree, 0, sizeof(ACTREE_NODE));
    node->tree->matchid=-1;

    return node;
}

int ac_add_string(AC_STRUCT *node, char *P, int M, int *id)
{
    int i, j;
    AC_TREE tnode, child, back, new_node, list, tail;

    P--;

    tnode = node->tree;
    for (i=1; i <= M; i++) {
        back = NULL;
        child = tnode->children;
        while (child != NULL && child->ch < P[i]) {//匹配一个字节
            back = child;
            child = child->sibling;
        }
        if (child == NULL || child->ch != P[i])//不匹配
            break;

        tnode = child;//匹配下一个字
    }
    if(i>M){
        if(tnode->matchid==-1){
            tnode->matchid = *id;
            *id=(*id)+1;
            return (*id)-1;
        }
        else{
            return tnode->matchid;
        }
    }
    else {
        list = tail = NULL;
        for (j=i; j <= M; j++) {
            if ((new_node = (AC_TREE)malloc(sizeof(ACTREE_NODE))) == NULL)
                break;
            memset(new_node, 0, sizeof(ACTREE_NODE));
            new_node->ch = P[j];
            new_node->matchid=-1;
            if (list == NULL)
                list = tail = new_node;
            else{
                tail->children=new_node;
                tail=tail->children;
            }
        }
        if (j <= M) {
            while (list != NULL) {
                tail = list->children;
                free(list);
                list = tail;
            }
            printf("Error: in AC tree.\n");
            return 0;
        }

        list->sibling = child;
        if (back == NULL)
            tnode->children = list;
        else
            back->sibling = list;

        tnode = tail;
        tnode->matchid = *id;
        *id=(*id)+1;
        return (*id)-1;
    }
}

void ac_free(AC_STRUCT *node)
{
    AC_TREE front, back, next;

    if (node == NULL)
        return;

    if (node->tree != NULL) {//������
        front = back = node->tree;
        while (front != NULL) {//������
            back->sibling = front->children;
            while (back->sibling != NULL)
                back = back->sibling;

            next = front->sibling;
            free(front);//�ͷ����ڵ�
            front = next;
        }
    }

    free(node);
}