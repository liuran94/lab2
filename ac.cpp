//
// Created by liuran94 on 17-11-30.
//
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ac.h"
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

int ac_add_string(AC_STRUCT *node, char *P, int M, int *id,bool *firstflag)
{
    int i, j;
    AC_TREE tnode, child, back, new_node, list, tail;

    P--;
    *firstflag= false;
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
            *firstflag= true;
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
        *firstflag= true;
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