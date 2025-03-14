#include <stdio.h>
int func4(int a, int b, int num1, int num2) {
    int temp = num1;
    temp = temp - b;
    num2 = num2 - temp;
    num2 >>= 31;
    temp = temp + num2;
    num2 = temp + b;
    if (a <= num2) {
        temp = 0;
        if (a >= num2) {
            temp = 1;
        } else {
            b = num2 + 1;
            temp = func4(a, b, num1, num2);
        }
    } else {
        a = num2 - 1;
        temp = func4(a, b, num1, num2);
        temp = temp + temp;
        return temp;
    }
}

int main() {
    int result = func4(14, 0, 2, 2);
    printf("Result: %d\n", result);
}