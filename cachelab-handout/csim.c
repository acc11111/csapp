#include "cachelab.h"
#include <getopt.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// 定义一个cache_line的结构体
typedef struct cache_line {
    int valid;     // 有效位
    int tag;       // 标记位
    int time_tamp; // 时间戳-->为了实现后续的替换策略
} Cache_line;

// 定义一个Cache的结构体
typedef struct cache_ {
    int S;
    int E;
    int B;
    Cache_line **line;
} Cache;

/*
    此处初始化全局数据
*/
// 记录冲突不命中、缓存不命中
int hit_count = 0, miss_count = 0, eviction_count = 0;
int verbose = 0; // 是否打印详细信息
char t[1000];
Cache *cache = NULL;

// 初始化Cache
void Init_Cache(int s, int E, int b) {
    int S = 1 << s;                         // 组数
    int B = 1 << b;                         // 块大小
    cache = (Cache *)malloc(sizeof(Cache)); // 分配Cache结构体的内存
    cache->S = S;
    cache->E = E;
    cache->B = B;
    // 二维数组的分配
    // S组，每组有E行
    cache->line = (Cache_line **)malloc(sizeof(Cache_line *) * S);
    for (int i = 0; i < S; i++) {
        cache->line[i] = (Cache_line *)malloc(sizeof(Cache_line) * E);
        for (int j = 0; j < E; j++) {
            // 全部初始化，此处将指针当作数组使用
            // i指向组，j指向行
            cache->line[i][j].valid = 0; // 初始时，高速缓存是空的
            cache->line[i][j].tag = -1;
            cache->line[i][j].time_tamp = 0;
        }
    }
}

// 释放资源，避免内存泄漏
void free_Cache() {
    int S = cache->S;
    for (int i = 0; i < S; i++) {
        free(cache->line[i]);
    }
    free(cache->line);
    free(cache);
}

// 更新时间戳
// i为行号，op_s为需要操作的组号，op_tag为标记位
// 遍历需要操作的组，找到所有有效位为1的行，时间戳+1，这里就是意味着这里的数据没有被访问，时间戳变大
// 然后将需要操作的行的时间戳置为0
// 这里的时间戳是为了实现LRU替换策略
void update(int i, int op_s, int op_tag) {
    cache->line[op_s][i].valid = 1;
    cache->line[op_s][i].tag = op_tag;
    for (int k = 0; k < cache->E; k++)
        if (cache->line[op_s][k].valid == 1)
            cache->line[op_s][k].time_tamp++;
    cache->line[op_s][i].time_tamp = 0;
}

// 根据上面的时间戳策略寻找需要替换的行
int find_LRU(int op_s) {
    int max_index = 0;
    int max_stamp = 0;
    for (int i = 0; i < cache->E; i++) {
        if (cache->line[op_s][i].time_tamp > max_stamp) {
            max_stamp = cache->line[op_s][i].time_tamp;
            max_index = i;
        }
    }
    return max_index; // 这个就是行号，也就是那个E里面的
}

// 通过这个操作的组号s和tag来寻找行号
int get_index(int op_s, int op_tag) {
    for (int i = 0; i < cache->E; i++) {
        if (cache->line[op_s][i].valid && cache->line[op_s][i].tag == op_tag)
            return i;
    }
    return -1; // 没有找到返回-1
}

// 检查是否满了
int is_full(int op_s) {
    for (int i = 0; i < cache->E; i++) {
        if (cache->line[op_s][i].valid == 0)
            return i; // 没有满的话直接返回行号用来后续的替换操作
    }
    return -1;
}

// 更新一开始定义的全局变量
// 这里就是如果查找成功就是hit++，否则查找失败就是miss++，同时需要考虑是使用空行替换还是使用LRU策略替换
void update_info(int op_tag, int op_s) {
    int index = get_index(op_s, op_tag);
    if (index == -1) {
        miss_count++; // 没有找到这个行就是miss++
        if (verbose)  // 是否打印详细的消息
            printf("miss ");
        int i = is_full(op_s);
        if (i == -1) {
            eviction_count++; // 如果满了的话就是返回-1，此时需要替换
            if (verbose)
                printf("eviction");
            i = find_LRU(op_s); // 这里找到需要替换的行号
        }
        update(i, op_s, op_tag);
    } else {
        hit_count++; // 这里命中之后直接hit++就完事了
        if (verbose)
            printf("hit");
        update(index, op_s, op_tag);
    }
}

// 就是从现有的文件里面读取然后解析需要操作的指令以及一些地址
void get_trace(int s, int E, int b) {
    FILE *pFile;
    pFile = fopen(t, "r");
    if (pFile == NULL) {
        exit(-1);
    }
    char identifier;
    unsigned address;
    int size;
    // Reading lines like " M 20,1" or "L 19,3"
    while (fscanf(pFile, " %c %x,%d", &identifier, &address, &size) >
           0) // I读不进来,忽略---size没啥用
    {
        // 想办法先得到标记位和组序号
        // 就是从现有的文件里面读取然后解析需要操作的指令以及一些地址
        int op_tag = address >> (s + b);
        int op_s =
            (address >> b) & ((unsigned)(-1) >> (8 * sizeof(unsigned) - s));
        switch (identifier) {
        case 'M': // 一次存储一次加载
            update_info(op_tag, op_s);
            update_info(op_tag, op_s);
            break;
        case 'L':
            update_info(op_tag, op_s);
            break;
        case 'S':
            update_info(op_tag, op_s);
            break;
        }
    }
    fclose(pFile);
}

void print_help() {
    printf("** A Cache Simulator by Deconx\n");
    printf("Usage: ./csim-ref [-hv] -s <num> -E <num> -b <num> -t <file>\n");
    printf("Options:\n");
    printf("-h         Print this help message.\n");
    printf("-v         Optional verbose flag.\n");
    printf("-s <num>   Number of set index bits.\n");
    printf("-E <num>   Number of lines per set.\n");
    printf("-b <num>   Number of block offset bits.\n");
    printf("-t <file>  Trace file.\n\n\n");
    printf("Examples:\n");
    printf("linux>  ./csim -s 4 -E 1 -b 4 -t traces/yi.trace\n");
    printf("linux>  ./csim -v -s 8 -E 2 -b 4 -t traces/yi.trace\n");
}

int main(int argc, char *argv[]) {
    char opt;
    int s, E, b;
    /*
     * s:S=2^s是组的个数
     * E:每组中有多少行
     * b:B=2^b每个缓冲块的字节数
     */
    while (-1 != (opt = getopt(argc, argv, "hvs:E:b:t:"))) {
        switch (opt) {
        case 'h':
            print_help();
            exit(0);
        case 'v':
            verbose = 1;
            break;
        case 's':
            s = atoi(optarg);
            break;
        case 'E':
            E = atoi(optarg);
            break;
        case 'b':
            b = atoi(optarg);
            break;
        case 't':
            strcpy(t, optarg);
            break;
        default:
            print_help();
            exit(-1);
        }
    }
    Init_Cache(s, E, b); // 初始化一个cache
    get_trace(s, E, b);
    free_Cache();
    // 这里打印出我们模拟出来的数据
    printSummary(hit_count, miss_count, eviction_count);
    return 0;
}