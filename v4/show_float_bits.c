#include <stdio.h>
#include <stdint.h>

void printFloatBits(float f) {
    // 将浮点数转换为32位无符号整数，以便查看其二进制表示
    uint32_t bits = *(uint32_t*)&f;

    // 打印浮点数的二进制表示
    printf("浮点数 %f 的二进制表示为：\n", f);
    printf("符号位（Sign bit）：");
    printf("%d", (bits >> 31) & 1);  // 符号位是最高位（第31位）

    printf("\n指数位（Exponent bits）：");
    for (int i = 30; i >= 23; i--) {  // 指数位是第23到30位
        printf("%d", (bits >> i) & 1);
    }

    printf("\n尾数位（Mantissa bits）：");
    for (int i = 22; i >= 0; i--) {  // 尾数位是第0到22位
        printf("%d", (bits >> i) & 1);
    }
    printf("\n");
}

int main() {
    float number;
    printf("请输入一个浮点数：");
    scanf("%f", &number);

    printFloatBits(number);

    return 0;
}
