//
// Created by liuran94 on 17-11-30.
//
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "queue.h"
#include "ac.h"
#include "url.h"
#include "bloom.h"
int getPath(char currenturl[],char* path){
    int i,j=0;

    if(currenturl[4]==':') { //http
        i=7;
    } else {  //https
        return 0;
    }
    while(currenturl[i]!='\0'&&currenturl[i]!='/'){
        i++;
    }

    if(currenturl[i]=='\0') return 0;

    while(currenturl[i]!='\0'){
        path[j++]=currenturl[i++];
    }
    if(path[j-1]=='/'||path[j-1]=='\n'){
        path[j-1]='\0';
    }
    else{
        path[j]='\0';
    }

    return 1;
}

int searchURL(char* currentpage,char *url,FILE *out,AC_STRUCT *tree,Queue *q,int *id){

    int state;
    int i,j=0,n,urlid;
    char currentchar;
    char urlbuf[MAX_PATH_LENGTH];
    char *searchedurl;
    char *writeUrl=(char *)malloc(sizeof(char)*strlen(url)+20);
    char urlhttp[13]="http://news.";
    bool flag=true;
    state = 0;


    for(i=0; currentpage[i] != '\0'; i++){
        currentchar=currentpage[i];
        switch(state){
            case 0:if(currentchar == '<'){
                    state=1; break;
                } else {
                    state=0; j=0; break;
                }
            case 1:if(currentchar == 'a'){
                    state=2; break;
                } else {
                    state=0; j=0; break;
                }
            case 2:if(currentchar == 'h'){
                    state=3; break;
                } else if(currentchar == '>'){
                    state=0; j=0; break;
                } else {
                    state=2; break;
                }
            case 3:if(currentchar == 'r'){
                    state=4; break;
                } else if(currentchar == '>') {
                    state=0; j=0; break;
                } else {
                    state=2; break;
                }
            case 4:if(currentchar == 'e'){
                    state=5; break;
                }
                else if(currentchar == '>'){
                    state=0; j=0; break;
                } else {
                    state=2; break;
                }
            case 5:if(currentchar == 'f') {
                    state=6; break;
                } else if(currentchar == '>'){
                    state=0; j=0; break;
                } else {
                    state=2; break;
                }
            case 6:if(currentchar == '='){
                    state=7; break;
                }
                else if(currentchar == '>'){
                    state=0; j=0; break;
                } else {
                    state=2; break;
                }
            case 7:if(currentchar == '"') {
                    state=10; break;
                } else if(currentchar == ' ') {
                    state=7; break;
                } else {
                    state=0; j=0; break;
                }
            case 10:if((currentchar=='"')||(currentchar=='||')||(currentchar=='>')||(currentchar=='#')) {
                    state=0; j=0; break;
                } else if(currentchar == '/') {
                    state=8;
                    urlbuf[j++]=currentchar;
                    break;
                } else {
                    state=10;
                    urlbuf[j++]=currentchar;
                    break;
                }
            case 8:if(currentchar == '"'){
                    state=9; break;
                } else if(currentchar=='>') {
                    state=0; j=0; break;
                } else {
                    state=8;
                    urlbuf[j++]=currentchar;
                    break;
                }
            case 9:urlbuf[j] = '\0';      //char urlhttp[13]="http://news.";
                state=0;
                for(n = 0;n <= 11;n++){
                    if(urlbuf[n] != urlhttp[n]){
                        break;
                    }
                }
                if(n==12){
                    searchedurl=(char*)malloc(sizeof(urlbuf));
                    if(urlbuf[j-1]=='\n'||urlbuf[j-1]=='/'){//去末尾的回车或者/
                        urlbuf[j-1]='\0';
                    }
                    strcpy(searchedurl,urlbuf);


                    flag=false;
                    urlid=ac_add_string(tree,searchedurl,strlen(searchedurl),id,&flag);
                    if(flag){
                        sprintf(writeUrl,"%s %d\n",searchedurl,urlid);
                        fputs(writeUrl,out);
                    }

                    if(!bloomFilter(searchedurl) && (strlen(searchedurl) < MAX_PATH_LENGTH)) {
                        enQueue(q,searchedurl);
                    }
                    state=0;
                    j=0;
                }
                break;
        }
    }

    return 0;
}

int url2host(char url[],char host[]) {//判断是否是https, 将url中的hosts提取出来
    int i,j=0;
    int length;

    if(url[4]==':') { //http
        i=7;
    } else {  //https
        return 0;
    }

    while(url[i]!='/'&&url[i]!='\0'){
        host[j++] = url[i++];
    }
    host[j]='\0';

    length=strlen(host);
    if(host[length-1]=='/'){
        host[length-1]='\0';
    }

    return 1;
}