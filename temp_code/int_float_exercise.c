#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <limits.h>

#define EPSILON 1e-6

void testExpression(int x, float f, double d) {
    // x == (int)(float)x
    bool expr1 = x == (int)(float)x;
    printf("Expression 1 (x == (int)(float)x): %s\n", expr1 ? "True" : "False");

    // x == (int)(double)x
    bool expr2 = x == (int)(double)x;
    printf("Expression 2 (x == (int)(double)x): %s\n", expr2 ? "True" : "False");

    // f == (float)(double)f
    bool expr3 = fabs(f - (float)(double)f) < EPSILON;
    printf("Expression 3 (f == (float)(double)f): %s\n", expr3 ? "True" : "False");

    // d == (double)(float)d
    bool expr4 = fabs(d - (double)(float)d) < EPSILON;
    printf("Expression 4 (d == (double)(float)d): %s\n", expr4 ? "True" : "False");

    // f == -(-f)
    bool expr5 = f == -(-f);
    printf("Expression 5 (f == -(-f)): %s\n", expr5 ? "True" : "False");

    // 2/3 == 2/3.0
    bool expr6 = fabs(2.0/3.0 - (double)(2/3)) < EPSILON;
    printf("Expression 6 (2/3 == 2/3.0): %s\n", expr6 ? "True" : "False");

    // d < 0.0 => ((d*2) < 0.0)
    bool expr7 = (d < 0.0) == (d*2 < 0.0);
    printf("Expression 7 (d < 0.0 => ((d*2) < 0.0)): %s\n", expr7 ? "True" : "False");

    // d > f => -f > -d
    bool expr8 = (d > f) == (-f > -d);
    printf("Expression 8 (d > f => -f > -d): %s\n", expr8 ? "True" : "False");

    // d * d >= 0.0
    bool expr9 = d * d >= 0.0;
    printf("Expression 9 (d * d >= 0.0): %s\n", expr9 ? "True" : "False");

    // (d+f) - d == f
    bool expr10 = fabs((d+f) - d - f) < EPSILON;
    printf("Expression 10 ((d+f) - d == f): %s\n", expr10 ? "True" : "False");
}

int main() {
    int x;
    float f;
    double d;

    // Test with different values
    printf("Testing with x = 5, f = 3.5, d = 4.5:\n");
    x = 5; f = 3.5; d = 4.5;
    testExpression(x, f, d);

    printf("\nTesting with x = -5, f = -3.5, d = -4.5:\n");
    x = -5; f = -3.5; d = -4.5;
    testExpression(x, f, d);

    printf("\nTesting with x = 0, f = 0.0, d = 0.0:\n");
    x = INT_MAX; f = 0.0; d = 0.0;
    testExpression(x, f, d);

    printf("\nTesting with x = INT_MAX, f = 2.0/3, d = 2.0/3.0:\n");
    x = INT_MAX; f = 2.0/3; d = 2.0/3.0;
    testExpression(x, f, d);

    return 0;
}
