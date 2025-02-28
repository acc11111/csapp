#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main() {
    int **a, **b;

    // 动态分配内存
    a = (int **)malloc(2048 * sizeof(int *));
    b = (int **)malloc(2048 * sizeof(int *));
    for (int i = 0; i < 2048; i++) {
        a[i] = (int *)malloc(2048 * sizeof(int));
        b[i] = (int *)malloc(2048 * sizeof(int));
    }

    // 初始化数组 a
    for (int i = 0; i < 2048; i++) {
        for (int j = 0; j < 2048; j++) {
            a[i][j] = 1;
        }
    }

    // 测试第一种循环顺序
    clock_t start1 = clock();
    for (int i = 0; i < 2048; i++) {
        for (int j = 0; j < 2048; j++) {
            b[i][j] = a[i][j];
        }
    }
    clock_t end1 = clock();
    double time1 = ((double)(end1 - start1)) / CLOCKS_PER_SEC; // 获取执行时间1

    // 测试第二种循环顺序
    clock_t start2 = clock();
    for (int j = 0; j < 2048; j++) {
        for (int i = 0; i < 2048; i++) {
            b[i][j] = a[i][j];
        }
    }
    clock_t end2 = clock();
    double time2 = ((double)(end2 - start2)) / CLOCKS_PER_SEC; // 获取执行时间2

    // 输出结果
    printf("第一种循环顺序的执行时间: %f秒\n", time1);
    printf("第二种循环顺序的执行时间: %f秒\n", time2);

    // 释放内存
    for (int i = 0; i < 2048; i++) {
        free(a[i]);
        free(b[i]);
    }
    free(a);
    free(b);

    return 0;
}
