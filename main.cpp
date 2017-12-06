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

#define DEFAULT_PAGE_BUF_SIZE 1024 * 1024
#define MAX_PATH_LENGTH 1024
#define MAX_CONNECT_NUM 60

using namespace std;

typedef struct {
    char url[MAX_PATH_LENGTH];
    int sock_c;
} Ev_arg;

char currentURL[MAX_PATH_LENGTH];
char request[MAX_PATH_LENGTH];
char host[MAX_PATH_LENGTH];
char path[MAX_PATH_LENGTH];
int epfd;
struct epoll_event ev;
struct epoll_event events[MAX_CONNECT_NUM];

//queue<char*> q;
int urlId=0;

void sendRequest(int isIndex,int *socket_client);
int revResponse(int socket_client,int ContentLength,FILE *out,FILE *link,FILE *test,char *url,AC_STRUCT *tree,Queue* q);
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
int revResponse(int socket_client,int ContentLength,FILE *out,FILE *link,FILE *test,char *url,AC_STRUCT *tree,Queue* q){
    //Download Page
    int urlid;
    bool flag;
    char *PageBuf=(char *)malloc(ContentLength);
    char *endPattern=(char *)malloc(sizeof(char)*8);
    char writeUrl[MAX_PATH_LENGTH];
    memset(PageBuf, 0, ContentLength);

    int byteread = 0;
    int ret;

    ret = recv(socket_client, PageBuf + byteread, ContentLength - byteread, 0);
    //printf("ret:%d\n",ret);
    if(ret==0){
        free(PageBuf);
        free(endPattern);
        return 0;
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
//    if(ContentLength - byteread < 100) {
//        printf("\nRealloc memory...\n");
//        ContentLength = ContentLength * 2;
//        PageBuf = (char *)realloc(PageBuf, ContentLength);
//    }
    PageBuf[byteread] = '\0';

    if(strstr(PageBuf,"404 Not Found")!=NULL||
       strstr(PageBuf,"400 Bad Request")!=NULL||
       strstr(PageBuf,"403 Forbidden")!=NULL||
       strstr(PageBuf,"301 Moved Permanently")!=NULL||
       strstr(url,".jpg")!=NULL){
        //printf("Illegal Status Code.\n");
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
    if(!flag&&strstr(PageBuf,"HTTP/1.1")!=NULL){
//        free(PageBuf);
//        free(endPattern);
//        return 0;
        sprintf(writeUrl,"%s %d\n",url,urlid);
        fputs(writeUrl,test);
    }
//    FILE *file;
//    char filename[MAX_PATH_LENGTH];
//    sprintf(filename,"./download/%d.txt",urlid);
//    if((file = fopen(filename, "a")) == NULL){
//        exit(1);
//    }
//    fputs(PageBuf,file);
//    fclose(file);

    //analysis url
    searchURL(PageBuf,url,link,test,q,urlid);

    memset(currentURL, 0, MAX_PATH_LENGTH);

    free(PageBuf);

    if(strcmp(endPattern,"</html>")==0||strcmp(endPattern,"</HTML>")==0
       ||strcmp(endPattern,"script>")==0||strcmp(endPattern,"SCRIPT>")==0
       ||strcmp(endPattern,"nclude>")==0||strcmp(endPattern,"NCLUDE>")==0
       ||strcmp(endPattern,"adcode>")==0){
        free(endPattern);
        //remove(filename);
        return 0;
    }
    free(endPattern);
    return 1;
}

int main(int argc,char* argv[]){
    int socket_client;
    int isIndex;
    int outflag=0;
    int ContentLength = DEFAULT_PAGE_BUF_SIZE;
    struct sockaddr_in serveraddr;
    char ipaddress[]="10.108.86.80";
    //char ipaddress[]="127.0.0.1";
    FILE *out,*link,*test;
    if((out = fopen("./result.txt", "w")) == NULL){
        exit(1);
    }
    if((link = fopen("./link.txt", "w")) == NULL){
        exit(1);
    }
    if((test = fopen("./test.txt", "w")) == NULL){
        exit(1);
    }
    memset(&serveraddr,0,sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    //serveraddr.sin_port = htons(atoi(argv[2]));
    serveraddr.sin_port = htons(atoi("80"));
    serveraddr.sin_addr.s_addr = inet_addr(ipaddress);

    //strcpy(currentURL, argv[1]);
    strcpy(currentURL, "http://news.sohu.com");
    bloomFilter(currentURL);

    Queue q;
    initQueue(&q);
    while(!isEmpty(q)){//保证开始时队列为空，可删去
        deQueue(&q, request);
    }
    enQueue(&q,currentURL);

    AC_STRUCT *tree= ac_alloc();

    bool timingflag=false;
    int timing=0;

    int j,n,state,connectFlag;
    int connectNum=0;
    epfd = epoll_create(MAX_CONNECT_NUM);	//生成用于处理accept的epoll专用文件描述符，最多监听256个事件
    for(int i=0;i<MAX_CONNECT_NUM;i++) {
        events[i].data.fd=-1;
    }
    while(outflag!=4)
    {
        if(n==0&&q.size==0&&connectNum!=0&&!timingflag){
            timingflag=true;
            timing=clock();
        }
        else if((n!=0||q.size!=0)&&timingflag){
            timingflag=false;
            timing=0;
        }
        if(timingflag){
            if(clock()-timing>10000){
                printf("Main loop:timeout.\n");
                outflag=4;
            }
        }
        printf("queueNum %d\n",q.size);
        if(outflag==1&&q.size>0){//队列开始增加
            outflag=2;//等待队列收敛
        }
        else if(outflag==2&&q.size==0){//队列收敛结束
            outflag=3;//等待最后的接收完毕
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
                    ev.data.ptr = arg;//

                    ev.events = EPOLLOUT | EPOLLET;
                    epoll_ctl(epfd, EPOLL_CTL_ADD, socket_client, &ev);    //注册ev
                    connectNum++;
                    j++;
                }
        }
        n = epoll_wait(epfd,events,MAX_CONNECT_NUM,200);
        printf("wait:%d,%d\n",n,connectNum);
        for(int i=0;i<n;i++){
            if( events[i].events&EPOLLIN ) //接收到数据，读socket
            {
                Ev_arg *arg = (Ev_arg *) (events[i].data.ptr);
                state=revResponse(arg->sock_c, ContentLength,out,link,test,arg->url,tree,&q);
                if(state==0){//全部接收完成
                    close(arg->sock_c);
                    connectNum--;
                    ev.data.ptr = arg;
                    epoll_ctl(epfd,EPOLL_CTL_DEL,arg->sock_c,&ev);
                    if(outflag==3&&connectNum==0){//最后的接收完毕
                        outflag=4;
                    }

                }
                else if(state==1){//部分接收完成
                    ev.data.ptr = arg;
                    ev.events=EPOLLIN|EPOLLET;//下一次继续接收
                    epoll_ctl(epfd,EPOLL_CTL_MOD,arg->sock_c,&ev);//修改标识符，等待下一个循环时发送数据，异步处理的精髓
                }
                else{//错误
                    printf("Error: In revPesponse.\n");
                }
            }
            else if(events[i].events&EPOLLOUT) //有数据待发送，写socket
            {

                Ev_arg *arg = (Ev_arg *) (events[i].data.ptr);
                deQueue(&q, currentURL);

                if(outflag==0&&q.size==0){//第一次pop主页，队列为空
                    outflag=1;//等待第一次入队
                }
                memset(path, 0, MAX_PATH_LENGTH);
                isIndex = getPath(currentURL,path);

                memset(host, 0, MAX_PATH_LENGTH);
                url2host(currentURL, host);

                sendRequest(isIndex, &(arg->sock_c));
                memset(arg->url,0,MAX_PATH_LENGTH);
                strcpy(arg->url,currentURL);

                ev.data.ptr = arg;
                //printf("2sk:%d\n",arg->sock_c);
                ev.events = EPOLLIN|EPOLLET ;    //设置要处理的事件类型。可读，边缘触发
                epoll_ctl(epfd,EPOLL_CTL_MOD,arg->sock_c,&ev); //修改标识符，等待下一个循环时接收数据
            }
            else{
                Ev_arg *arg = (Ev_arg *) (events[i].data.ptr);
                close(arg->sock_c);
                connectNum--;
                ev.data.ptr = arg;
                epoll_ctl(epfd,EPOLL_CTL_DEL,arg->sock_c,&ev);
                if(outflag==3&&connectNum==0){//最后的接收完毕
                    outflag=4;
                }
            }
        }

    }
    close(epfd);
    fclose(out);
    fclose(link);
    fclose(test);
    printf("\n*****total:%d\n",urlId);
    printf("mallocEllCoo\n");
    mallocEllCoo(urlId);
    printf("fileToEllCoo\n");
    fileToEllCoo(tree);

    ac_free(tree);
    //printEllCoo();
    printf("a_mallocEllCoo\n");
    a_mallocEllCoo();
    printf("generateA\n");
    generateA();
    printf("initPageRank\n");
    initPageRank();
    printf("generatePageRank\n");
    generatePageRank();
    printPageRank();
    return 0;
}

/*
int main(){
    mallocEllCoo();
    int arr0[3]={1,2};
    addInEllCoo(arr0,2,0);

    int arr1[3]={2};
    addInEllCoo(arr1,1,1);

    int arr3[3]={0};
    addInEllCoo(arr3,1,2);

    printEllCoo();
    a_mallocEllCoo();
    generateA();
    initPageRank();
    generatePageRank();
    printPageRank();
}*/
