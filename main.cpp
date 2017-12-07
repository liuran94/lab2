#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <queue>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <sys/epoll.h>
#include "url.h"
#include "matrix.h"
#include "bloom.h"
#include "queue.h"
#include <dirent.h>
#include <sys/stat.h>
#define DEFAULT_PAGE_BUF_SIZE 1024 * 1024
#define MAX_PATH_LENGTH 1024
#define MAX_CONNECT_NUM 60

using namespace std;

typedef struct {
    char url[MAX_PATH_LENGTH];
    bool haveRecv;
    int sock_c;
} Ev_arg;

char tempDir[MAX_PATH_LENGTH];
char currentURL[MAX_PATH_LENGTH];
char request[MAX_PATH_LENGTH];
char host[MAX_PATH_LENGTH];
char path[MAX_PATH_LENGTH];
int epfd;
struct epoll_event ev;
struct epoll_event events[MAX_CONNECT_NUM];

int urlId=0;
void List(char *path,FILE *link,Queue* q);
void sendRequest(int isIndex,int *socket_client);
int revResponse(int socket_client,int ContentLength,FILE *out,FILE *link,char *url,AC_STRUCT *tree,Queue* q);
void setnoblocking(int socket_client);

void setnoblocking(int socket_client){
    int opts;
    opts = fcntl(socket_client, F_GETFL);	//获取文件标志和访问模式
    if (opts < 0) {
        perror("fcntl(sockfd,GETFL)");
        exit(1);
    }
    opts |= O_NONBLOCK;	//非阻塞
    if (fcntl(socket_client, F_SETFL, opts) < 0) {
        perror("fcntl(sockfd,SETFL)");
        exit(1);
    }
}
void sendRequest(int isIndex,int *socket_client){
    memset(request, 0, MAX_PATH_LENGTH);
    strcat(request,"GET ");
    if(isIndex == 0){
        strcat(request, "/ HTTP/1.1\r\nHost:");
        strcat(request, host);
    } else {
        strcat(request, path);
        strcat(request, " HTTP/1.1\r\nHost:");
        strcat(request, host);
    }
    strcat(request, "\r\n");
    strcat(request, "Connection:Keep-Alive\r\n\r\n");//keep-alive
    //printf("%s%s\n",host,path);

    send(*socket_client,request,strlen(request),MSG_NOSIGNAL);
    return;
}
int revResponse(int socket_client,int ContentLength,FILE *out,FILE *link,char *url,AC_STRUCT *tree,Queue* q){
    //Download Page
    int urlid;
    bool flag;
    char *PageBuf=(char *)malloc(ContentLength);
    char *endPattern=(char *)malloc(sizeof(char)*8);
    char writeUrl[MAX_PATH_LENGTH];
    FILE *file;
    char filename[MAX_PATH_LENGTH];
    memset(PageBuf, 0, ContentLength);

    int byteread = 0;
    int ret;
    ret = recv(socket_client, PageBuf + byteread, ContentLength - byteread, 0);
    if(ret==0){
        int scode;
        urlid=ac_add_string(tree,url,strlen(url),&urlId,&flag);
        sprintf(filename,"%s/%d.txt",tempDir,urlid);
        scode=searchURL(filename,link,q,urlid);
        if(scode==0)
            remove(filename);
        free(PageBuf);
        free(endPattern);
        return 2;
    }
    if(ret<0&&errno == EAGAIN){
        free(PageBuf);
        free(endPattern);
        return 1;
    }
    if(ret<0&&errno != EAGAIN){
        free(PageBuf);
        free(endPattern);
        return -1;
    }
    if(ret > 0) {
        byteread = byteread + ret;
    }
    PageBuf[byteread] = '\0';

    if(strstr(PageBuf,"404 Not Found")!=NULL||
       strstr(PageBuf,"400 Bad Request")!=NULL||
       strstr(PageBuf,"403 Forbidden")!=NULL||
       strstr(PageBuf,"301 Moved Permanently")!=NULL||
       strstr(PageBuf,"302 Moved")!=NULL||
       strstr(PageBuf,"304 Not Modified")!=NULL||
       strstr(url,".jpg")!=NULL){
        free(PageBuf);
        free(endPattern);
        return 0;
    }

    int i=0,j=0;
    while(j<8){
        if(PageBuf[byteread-i]=='\n'||PageBuf[byteread-i]=='\r'||PageBuf[byteread-i]=='\f'){
            i++;
            continue;
        }
        endPattern[7-j]=PageBuf[byteread-i];
        i++;j++;
    }

    flag=false;
    urlid=ac_add_string(tree,url,strlen(url),&urlId,&flag);
    if(flag){
        sprintf(writeUrl,"%s %d\n",url,urlid);
        fputs(writeUrl,out);
    }

    sprintf(filename,"%s/%d.txt",tempDir,urlid);
    if((file = fopen(filename, "a")) == NULL){
        printf("Recv: Failed to open %s.\n",filename);
        exit(1);
    }
    fputs(PageBuf,file);
    fclose(file);

    memset(currentURL, 0, MAX_PATH_LENGTH);

    if(strcmp(endPattern,"</html>")==0||strcmp(endPattern,"</HTML>")==0
       ||strcmp(endPattern,"script>")==0||strcmp(endPattern,"SCRIPT>")==0
       ||strcmp(endPattern,"nclude>")==0||strcmp(endPattern,"NCLUDE>")==0
       ||strcmp(endPattern,"adcode>")==0){
        //analysis url
        searchURL(filename,link,q,urlid);
        free(endPattern);
        free(PageBuf);
        remove(filename);
        return 0;
    }
    free(PageBuf);
    free(endPattern);
    return 1;
}

int main(int argc,char* argv[]){
    int beginSpiderTime=clock();//记录程序开始时间
    int socket_client;
    int isIndex;
    int ContentLength = DEFAULT_PAGE_BUF_SIZE;
    struct sockaddr_in serveraddr;
    char url_txtDir[MAX_PATH_LENGTH/2];
    char result_txtDir[MAX_PATH_LENGTH/2];
    char link_txtDir[MAX_PATH_LENGTH/2];
    char ipaddress[50];//"10.108.86.80";
    strcpy(ipaddress, argv[2]);

    strcpy(url_txtDir, argv[4]);
    strcpy(result_txtDir, argv[5]);
    strcpy(tempDir, argv[6]);
    sprintf(link_txtDir,"%s/link.txt",tempDir);
    FILE *out,*link;
    if((out = fopen(url_txtDir, "w")) == NULL){
        exit(1);
    }
    if((link = fopen(link_txtDir, "w")) == NULL){
        exit(1);
    }
    mkdir(tempDir,S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    memset(&serveraddr,0,sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(atoi(argv[3]));
    //serveraddr.sin_port = htons(atoi("80"));
    serveraddr.sin_addr.s_addr = inet_addr(ipaddress);

    strcpy(currentURL, argv[1]);
    //strcpy(currentURL, "http://news.sohu.com");
    bloomFilter(currentURL);

    Queue q;
    initQueue(&q);
    while(!isEmpty(q)){//保证开始时队列为空，可删去
        deQueue(&q, request);
    }
    enQueue(&q,currentURL);

    AC_STRUCT *tree= ac_alloc();

    int j,n,state,connectFlag,lastNum=0;
    int connectNum=0;
    epfd = epoll_create(MAX_CONNECT_NUM);	//生成用于处理accept的epoll专用文件描述符，最多监听256个事件
    for(int i=0;i<MAX_CONNECT_NUM;i++) {
        events[i].data.fd=-1;
    }
    while(q.size!=0||connectNum!=0) {
        if(abs(q.size-lastNum)>=5000){
            printf("Queue size: %d\n",q.size);
            lastNum=q.size;
        }
        j=0;
        while(j<q.size&&connectNum<MAX_CONNECT_NUM){

                socket_client = socket(AF_INET,SOCK_STREAM,0);
                connectFlag=connect(socket_client,(struct sockaddr*)&serveraddr,sizeof(serveraddr));
                if(connectFlag==0) {
                    setnoblocking(socket_client);

                    Ev_arg *arg = (Ev_arg *) malloc(sizeof(Ev_arg));

                    memset(arg->url, 0, MAX_PATH_LENGTH);
                    strcpy(arg->url, currentURL);
                    arg->sock_c = socket_client;
                    arg->haveRecv=false;
                    ev.data.ptr = arg;//

                    ev.events = EPOLLOUT | EPOLLET;
                    epoll_ctl(epfd, EPOLL_CTL_ADD, socket_client, &ev);    //注册ev
                    connectNum++;
                    j++;
                }
        }
        n = epoll_wait(epfd,events,MAX_CONNECT_NUM,200);
        //printf("wait:%d,%d\n",n,connectNum);
        for(int i=0;i<n;i++){
            if( events[i].events&EPOLLIN ) //接收到数据，读socket
            {
                Ev_arg *arg = (Ev_arg *) (events[i].data.ptr);
                state=revResponse(arg->sock_c, ContentLength,out,link,arg->url,tree,&q);
                if(state==0){//全部接收完成
                    ev.data.ptr = arg;
                    ev.events=EPOLLOUT|EPOLLET;
                    epoll_ctl(epfd,EPOLL_CTL_MOD,arg->sock_c,&ev);
                }
                else if(state==1){//部分接收完成
                    arg->haveRecv= true;
                    ev.data.ptr = arg;
                    ev.events=EPOLLIN|EPOLLET;//下一次继续接收
                    epoll_ctl(epfd,EPOLL_CTL_MOD,arg->sock_c,&ev);//修改标识符，等待下一个循环时发送数据，异步处理的精髓
                }
                else if(state==2){//服务器超时
                    if(!arg->haveRecv)
                        enQueue(&q,arg->url);
                    close(arg->sock_c);
                    connectNum--;
                    ev.data.ptr = arg;
                    epoll_ctl(epfd,EPOLL_CTL_DEL,arg->sock_c,&ev);
                }
                else{//错误
                    printf("Error: In revResponse.\n");
                }
            }
            else if(events[i].events&EPOLLOUT) //有数据待发送，写socket
            {
                Ev_arg *arg = (Ev_arg *) (events[i].data.ptr);
                if(q.size==0){
                    List(tempDir,link,&q);
                }
                if(q.size==0){
                    close(arg->sock_c);
                    connectNum--;
                    ev.data.ptr = arg;
                    epoll_ctl(epfd,EPOLL_CTL_DEL,arg->sock_c,&ev);
                    continue;
                }
                deQueue(&q, currentURL);

                memset(path, 0, MAX_PATH_LENGTH);
                isIndex = getPath(currentURL,path);

                memset(host, 0, MAX_PATH_LENGTH);
                url2host(currentURL, host);

                sendRequest(isIndex, &(arg->sock_c));
                memset(arg->url,0,MAX_PATH_LENGTH);
                strcpy(arg->url,currentURL);

                ev.data.ptr = arg;
                ev.events = EPOLLIN|EPOLLET ;
                epoll_ctl(epfd,EPOLL_CTL_MOD,arg->sock_c,&ev);
            }
            else{
                Ev_arg *arg = (Ev_arg *) (events[i].data.ptr);
                close(arg->sock_c);
                connectNum--;
                ev.data.ptr = arg;
                epoll_ctl(epfd,EPOLL_CTL_DEL,arg->sock_c,&ev);
            }
        }

    }
    int finishSpiderTime=clock();
    printf("the time to process spider:%dms\n",finishSpiderTime-beginSpiderTime);
    close(epfd);
    fclose(out);
    fclose(link);
    remove(tempDir);
    printf("\n***** Total:%d *****\n",urlId);
    printf("Malloc space for matrix G ...\n");
    mallocEllCoo(urlId);
    printf("Load file to matrix G ...\n");
    fileToEllCoo(tree,link_txtDir);
    ac_free(tree);
    printf("Malloc space for matrix A ...\n");
    a_mallocEllCoo();
    printf("Generate matrix A ...\n");
    generateA(url_txtDir);
    printf("Init PageRank ...\n");
    int finishGenerateMatrix=clock();
    printf("the time to generate Matrix:%dms\n",finishGenerateMatrix-finishSpiderTime);
    initPageRank();
    printf("Generate PageRank ...\n");
    generatePageRank();
    int finishGeneratePageRank=clock();
    printf("the time to generate Matrix:%dms\n",finishGeneratePageRank-finishGenerateMatrix);
    printf("the total time of this process:%dms\n",finishGeneratePageRank-beginSpiderTime);
    printPageRank(url_txtDir,result_txtDir);
    return 0;
}
void List(char *path,FILE *link,Queue* q) {
    struct dirent *ent = NULL;
    DIR *pDir;
    pDir = opendir(path);
    int urlid;
    char filename[MAX_PATH_LENGTH];
    //d_reclen：16表示子目录或以.开头的隐藏文件，24表示普通文本文件,28为二进制文件，还有其他……
    while (NULL != (ent = readdir(pDir))) {
        //printf("reclen=%d    type=%d\t", ent->d_reclen, ent->d_type);
        if (ent->d_reclen == 24) {
            //d_type：4表示为目录，8表示为文件
            if (ent->d_type == 8) {
                //printf("普通文件[%s]\n", ent->d_name);
            }
        } else if (ent->d_reclen == 16) {
            //printf("[.]开头的子目录或隐藏文件[%s]\n", ent->d_name);
        } else {
            printf("Loading file [%s]\n", ent->d_name);
            int scode,i;
            char numBuf[10];
            memset(numBuf,0,sizeof(numBuf));
            for(i=0;i<10&&ent->d_name[i]!='.';i++){
                numBuf[i]=ent->d_name[i];
            }
            numBuf[i]='\0';
            urlid=atoi(numBuf);
            sprintf(filename,"%s/%s",path,ent->d_name);
            scode=searchURL(filename,link,q,urlid);
            if(scode==0){
                remove(filename);
            }
        }
    }
}
