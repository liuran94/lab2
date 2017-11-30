//
// Created by muki on 11/15/17.
//

#ifndef QUEUE_H
#define QUEUE_H

#define MAX_NUM 1024
typedef struct node{
    char data[MAX_NUM];
    struct node *next;
}queueNode;


typedef struct {
    queueNode *head;
    queueNode *tail;
    int size;
}Queue;


void initQueue(Queue *queue);
bool isEmpty(Queue queue);
void enQueue(Queue *queue,char* nodeData);

bool deQueue(Queue *queue,char*nodeData);

/*
Queue *queue_init(){
    Queue *q;
    q = malloc(sizeof(Queue))
    if(q == NULL){
        return NULL;
    }
    memset(q, 0, sizeof(Queue));
    q->head = 0;
    q->tail = 0;
    return q;
}

int isEmpty(Queue *q){
    return ((q->head - q->tail) == 0) ? 1 : 0;
}

int isFull(Queue *q){
    return ((q->head - q->tail + MAXN)%MAXN == 1);
}

int enQueue(Queue *q, AC_TREE node){
    if(isFull(q)){
        return 0;
    }
    else{
        q->queue[q->tail] = node;
        q->tail = (q->tail + 1) % MAXN;
        return 1;
    }
}

AC_TREE deQueue(Queue *q){
    if(isEmpty(q)){
        return NULL;
    }
    else{
        int temp = q->head;
        q->head = (q->head + 1) % MAXN;
        return q->queue[temp];
    }
}
*/

#endif //QUEUE_H
