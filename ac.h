#ifndef _AC_H_
#define _AC_H_

typedef struct actreenode {
    char ch;
    int matchid;
    struct actreenode *children, *sibling;
} ACTREE_NODE, *AC_TREE;


typedef struct {
    AC_TREE tree;
} AC_STRUCT;

AC_STRUCT *ac_alloc(void);
int ac_add_string(AC_STRUCT *node, char *P, int M, int *id,bool* firstflag);
void ac_free(AC_STRUCT *node);

#endif
