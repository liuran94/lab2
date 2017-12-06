#ifndef BLOOM_H_INCLUDED
#define BLOOM_H_INCLUDED

#define MAX_URLPATH_NUM	10000000	/*url path数目的上限(必须是1000的整倍数) */

int bloomFilter(char *path);

#endif
