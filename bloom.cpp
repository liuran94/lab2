//
// Created by liuran94 on 17-11-30.
//
#include<stdlib.h>
#include<assert.h>
#include<string.h>
#include<rpc/des_crypt.h>	/*for ecd_crypt */
#include<limits.h>		/*for INT_MAX */
#include<string>
#include "bloom.h"
#include "MurmurHasn2.h"
using namespace std;

unsigned int p_table[MAX_URLPATH_NUM] = { 0 };
unsigned int seed[8]={5, 7, 11, 13, 31, 37, 61 ,71};

/*判断path是否存在过，如果不存在就把它记入PathBloomTable中*/
int bloomFilter(char *path){
    unsigned int mod = 32 * MAX_URLPATH_NUM;//m空间
    int flag = 0;

    int g[8] = { 0 };
    unsigned int hashResult;
    int i;
    for (i = 0; i < 8; i++) {
        hashResult=MurmurHash2(path,strlen(path),seed[i]);
        g[i] = hashResult % mod;	/* rand() * 32 * MAX_URLPATH_NUM */
        int index = g[i] / 32;
        int pos = g[i] % 32;
        if (p_table[index] & (0x80000000 >> pos))//如果与p_table[index]相同
            flag++;
        else
            p_table[index] |= (0x80000000 >> pos);//不同，则添加这一位
    }
    if (flag == 8){
        return 1;
    }//已存在则返回true
    return 0;
}