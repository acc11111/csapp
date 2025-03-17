#include <stdio.h>
int func4(int num1, int b, int c, int num2) {
    int temp = c;
    temp = temp - b;
    num2 = num2 - temp;
    num2 >>= 31;
    temp = temp + num2;
    num2 = temp + b;
    if (num1 <= num2) {
        temp = 0;
        if (num1 >= num2) {
            temp = 1;
        } else {
            b = num2 + 1;
            temp = func4(num1, b, c, num2);
        }
    } else {
        num1 = num2 - 1;
        temp = func4(num1, b, c, num2);
        temp = temp + temp;
        return temp;
    }
}

int main() {
    int result = func4(14, 0, 2, 2);
    printf("Result: %d\n", result);
}