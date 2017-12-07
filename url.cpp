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
#include "matrix.h"
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

int searchURL(char* filename,FILE *link,Queue* q,int id){

    int state;
    int i,j=0,n;

    char urlbuf[MAX_PATH_LENGTH];
    char searchedurl[MAX_PATH_LENGTH];
    char writeBuf[MAX_PATH_LENGTH];
    char urltemp[MAX_PATH_LENGTH];
    char host[MAX_PATH_LENGTH];
    char urlhttp[13]="http://news.";
    FILE * file;
    if((file = fopen(filename, "r")) == NULL){
        printf("searchURL: Failed to open %s.\n",filename);
        return -1;
    }
    state = 0;
    char currentchar=fgetc(file);
    while(currentchar!=EOF){
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
                n=0;
                while(urlbuf[n]=='\n'||urlbuf[n]=='\r'||urlbuf[n]==' '){//去掉前面的空格和回车
                    n++;
                }
                if(n!=0){
                    for(int k=0;k<strlen(urlbuf);k++){
                        urlbuf[k]=urlbuf[k+n];
                    }
                }
                for(n=0;n <= 11;n++){
                    if(urlbuf[n] != urlhttp[n]){
                        break;
                    }
                }
                if(urlbuf[0]=='/'){
                    memset(urltemp,0,sizeof(urltemp));
                    strcpy(urltemp,urlbuf);
                    memset(urlbuf,0,sizeof(urlbuf));
                    strcpy(urlbuf,"http://news.sohu.com");
                    strcpy(urlbuf+20,urltemp);
                    n=12;
                }
                if(n==12){
                    j=strlen(urlbuf);
                    while(urlbuf[j-1]=='\r'||urlbuf[j-1]=='\n'||urlbuf[j-1]=='/'||urlbuf[j-1]==' '){//去末尾的回车或者/或者空格
                        urlbuf[j-1]='\0';
                        j--;
                    }
                    memset(searchedurl,0,MAX_PATH_LENGTH);
                    memset(writeBuf,0,MAX_PATH_LENGTH);
                    strcpy(searchedurl,urlbuf);

                    sprintf(writeBuf,"%d %s\n",id,searchedurl);
                    fputs(writeBuf,link);

                    if(!bloomFilter(searchedurl) && (strlen(searchedurl) < MAX_PATH_LENGTH)) {
                        enQueue(q,searchedurl);
                    }
                    state=0;
                    j=0;
                }
                break;
        }
        currentchar=fgetc(file);
    }
    fclose(file);
    return 0;
}

int url2host(char url[],char host[]) {
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