#ifndef URL_H_INCLUDED
#define URL_H_INCLUDED

#include "queue.h"
#include "ac.h"
#define MAX_PATH_LENGTH 1024

int getPath(char currenturl[],char* path);
int searchURL(char* currentpage,char *url,FILE *out,AC_STRUCT *tree,Queue* q,int* id);
int url2host(char url[],char host[]);

#endif // URL_H_INCLUDED
