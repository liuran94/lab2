//
// Created by muki on 11/16/17.
//
#include "queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void initQueue(Queue *queue){
    queue->head=(queueNode*)malloc(sizeof(queueNode));
    queue->tail=queue->head;
    queue->head->next=NULL;
    queue->size=0;
}
bool isEmpty(Queue queue){
    return queue.head == queue.tail;
}
void enQueue(Queue *queue,char* nodeData){
    queueNode *newNode=(queueNode*)malloc(sizeof(queueNode));
    strcpy(newNode->data,nodeData);
    newNode->next=NULL;
    queue->tail->next=newNode;
    queue->tail=newNode;
    queue->size++;
}
bool deQueue(Queue *queue, char*nodeData){
    if(queue->tail == queue->head)
        return false;
    queueNode *q=queue->head->next;
    strcpy(nodeData,q->data);
    queue->head->next=q->next;
    if(queue->tail==q)
        queue->tail=(*queue).head;
    free(q);
    queue->size--;
    return true;
}