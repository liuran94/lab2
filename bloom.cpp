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
using namespace std;

int p_table[MAX_URLPATH_NUM] = { 0 };
int myEncrypt(char *str, char *key){
    assert(str != NULL);
    char *cipher = strdup(str);
    ecb_crypt(key, cipher, strlen(str), DES_ENCRYPT);	/*第一次映射函数采用ecb_crypt */
    int i;
    int var = 0;
    int len = strlen(cipher);
    for (i = 0; i < len; i++){
        var = (var * 7 + cipher[i]) % (int)INT_MAX;
    }
    free(cipher);
    return var;
}


/*判断path是否存在过，如果不存在就把它记入PathBloomTable中*/
int bloomFilter(char *path){

    int mod = 32 * MAX_URLPATH_NUM;
    int flag = 0;
    //printf("before\n");

    string salt[] = { "Dm", "VB", "ui", "LK", "uj", "RD", "we", "fc" };
    //printf("after\n");
    int f[8] = { 0 };
    int g[8] = { 0 };
    int i;
    for (i = 0; i < 8; i++) {
        char *key;
        //printf("bloompath:%s\n",path);
        key = strdup(salt[i].c_str());
        f[i] = myEncrypt(path, key);
        free(key);
        srand(f[i]);
        g[i] = rand() % mod;	/* rand() * 33 * MAX_URLPATH_NUM */
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