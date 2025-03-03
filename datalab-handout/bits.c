/*
 * CS:APP Data Lab
 *
 * <Please put your name and userid here>
 *
 * bits.c - Source file with your solutions to the Lab.
 *          This is the file you will hand in to your instructor.
 *
 * WARNING: Do not include the <stdio.h> header; it confuses the dlc
 * compiler. You can still use printf for debugging without including
 * <stdio.h>, although you might get a compiler warning. In general,
 * it's not good practice to ignore compiler warnings, but in this
 * case it's OK.
 */

#if 0
/*
 * Instructions to Students:
 *
 * STEP 1: Read the following instructions carefully.
 */

You will provide your solution to the Data Lab by
editing the collection of functions in this source file.

INTEGER CODING RULES:
 
  Replace the "return" statement in each function with one
  or more lines of C code that implements the function. Your code 
  must conform to the following style:
 
  int Funct(arg1, arg2, ...) {
      /* brief description of how your implementation works */
      int var1 = Expr1;
      ...
      int varM = ExprM;

      varJ = ExprJ;
      ...
      varN = ExprN;
      return ExprR;
  }

  Each "Expr" is an expression using ONLY the following:
  1. Integer constants 0 through 255 (0xFF), inclusive. You are
      not allowed to use big constants such as 0xffffffff.
  2. Function arguments and local variables (no global variables).
  3. Unary integer operations ! ~
  4. Binary integer operations & ^ | + << >>
    
  Some of the problems restrict the set of allowed operators even further.
  Each "Expr" may consist of multiple operators. You are not restricted to
  one operator per line.

  You are expressly forbidden to:
  1. Use any control constructs such as if, do, while, for, switch, etc.
  2. Define or use any macros.
  3. Define any additional functions in this file.
  4. Call any functions.
  5. Use any other operations, such as &&, ||, -, or ?:
  6. Use any form of casting.
  7. Use any data type other than int.  This implies that you
     cannot use arrays, structs, or unions.

 
  You may assume that your machine:
  1. Uses 2s complement, 32-bit representations of integers.
  2. Performs right shifts arithmetically.
  3. Has unpredictable behavior when shifting if the shift amount
     is less than 0 or greater than 31.


EXAMPLES OF ACCEPTABLE CODING STYLE:
  /*
   * pow2plus1 - returns 2^x + 1, where 0 <= x <= 31
   */
  int pow2plus1(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     return (1 << x) + 1;
  }

  /*
   * pow2plus4 - returns 2^x + 4, where 0 <= x <= 31
   */
  int pow2plus4(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     int result = (1 << x);
     result += 4;
     return result;
  }

FLOATING POINT CODING RULES

For the problems that require you to implement floating-point operations,
the coding rules are less strict.  You are allowed to use looping and
conditional control.  You are allowed to use both ints and unsigneds.
You can use arbitrary integer and unsigned constants. You can use any arithmetic,
logical, or comparison operations on int or unsigned data.

You are expressly forbidden to:
  1. Define or use any macros.
  2. Define any additional functions in this file.
  3. Call any functions.
  4. Use any form of casting.
  5. Use any data type other than int or unsigned.  This means that you
     cannot use arrays, structs, or unions.
  6. Use any floating point data types, operations, or constants.


NOTES:
  1. Use the dlc (data lab checker) compiler (described in the handout) to 
     check the legality of your solutions.
  2. Each function has a maximum number of operations (integer, logical,
     or comparison) that you are allowed to use for your implementation
     of the function.  The max operator count is checked by dlc.
     Note that assignment ('=') is not counted; you may use as many of
     these as you want without penalty.
  3. Use the btest test harness to check your functions for correctness.
  4. Use the BDD checker to formally verify your functions
  5. The maximum number of ops for each function is given in the
     header comment for each function. If there are any inconsistencies 
     between the maximum ops in the writeup and in this file, consider
     this file the authoritative source.

/*
 * STEP 2: Modify the following functions according the coding rules.
 * 
 *   IMPORTANT. TO AVOID GRADING SURPRISES:
 *   1. Use the dlc compiler to check that your solutions conform
 *      to the coding rules.
 *   2. Use the BDD checker to formally verify that your solutions produce 
 *      the correct answers.
 */

#endif
// 1
/*
 * bitXor - x^y using only ~ and &
 *   Example: bitXor(4, 5) = 1
 *   Legal ops: ~ &
 *   Max ops: 14
 *   Rating: 1
 */
int bitXor(int x, int y) {
    // 使用离散数学表达式转换即可
    return ~((~(x & ~y)) & (~(~x & y)));
}
/*
 * tmin - return minimum two's complement integer
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 4
 *   Rating: 1
 */
int tmin(void) {
    // 1-->0000...001
    // 将0000... 001左移31位即可获得1000...000也就是tmin
    int x = 1;
    return x << 31;
}
// 2
/*
 * isTmax - returns 1 if x is the maximum, two's complement number,
 *     and 0 otherwise
 *   Legal ops: ! ~ & ^ | +
 *   Max ops: 10
 *   Rating: 1
 */
int isTmax(int x) {
    // tmax=0111...111
    // tmax+1=1000...000
    // 检查第一位为0，其余位为1
    // 左侧括号判断x加一是否为互补状态，这里筛选出来的只有1111...111和0111...111是符合的
    // 右侧括号判断x不是1111...111
    return ((!((x + 1) ^ (~x))) & (!!(~x)));
}
/*
 * allOddBits - return 1 if all odd-numbered bits in word set to 1
 *   where bits are numbered from 0 (least significant) to 31 (most significant)
 *   Examples allOddBits(0xFFFFFFFD) = 0, allOddBits(0xAAAAAAAA) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 2
 */
int allOddBits(int x) {
    // 构造的数字必须在0-255之间，也就是0000 0000 0000 0000 0000 0000 1111 1111
    // 因此通过移位和相加构造出目标1010101010...101010
    // 通过取与运算获得原数字x的全部奇数为为原来的数字，偶数位0
    // 在将这个过滤后仅剩奇数位数字的新数与奇数位全为1的进行取异或运算
    // 如果结果为0则说明x的奇数位全为1，否则只要原来x中的奇数位有一个0就会出现结果为1
    int mask = 0xAA;
    mask = (mask << 8) + 0xAA;
    mask = (mask << 8) + 0xAA;
    mask = (mask << 8) + 0xAA;
    return !((x & mask) ^ mask);
}
/*
 * negate - return -x
 *   Example: negate(1) = -1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 5
 *   Rating: 2
 */
int negate(int x) { return (~x + 1); }
// 3
/*
 * isAsciiDigit - return 1 if 0x30 <= x <= 0x39 (ASCII codes for characters '0'
 * to '9') Example: isAsciiDigit(0x35) = 1. isAsciiDigit(0x3a) = 0.
 *            isAsciiDigit(0x05) = 0.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 3
 */
int isAsciiDigit(int x) {
    // 0x30 <= x <= 0x39将表达式变形一下
    // 0x30 - x <= 0 <= 0x39 - x
    // 0x2f - x就应该是一个负数，也就是1.....
    // 0x39 - x就应该是一个正数，也就是0.....
    // 左侧检测出来是tmin和大于0x2f的数
    // 右侧检测出来的是小于等于0x39的数
    // 但是这里有个坑点，tmin转换之后还是tmin，导致tmin满足第一条
    // 同时第二条使用!导致检测出来的负数也满足非0所以使用!之后导致为真使得陷入了误区
    // 这里其实就是检测那个1和检测那个0，然后1可以使用&检测，0可以使用^检测
    return (((0x2f + (~x + 1)) >> 31 & 1) & (((0x39 + (~x + 1)) >> 31) ^ 1));
}
/*
 * conditional - same as x ? y : z
 *   Example: conditional(2,4,5) = 4
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 16
 *   Rating: 3
 */
int conditional(int x, int y, int z) {
    // x == 0 >> return z;
    // x != 0 >> return y;
    // 检查这个x是否为0
    // !这是一个好东西，可以将所有的非0数通过!!变成1，然后通过-1也就是(~1 +
    // 1)来实现为0 同时非0的数字可以使用一次!将其变成0，然后通过-1也就是(~1 +
    // 1)来实现为-1也就是全1，此时使用&操作即可获得原来的数因为已经是全1
    return ((!x + (~1 + 1)) & y) + ((!!x + (~1 + 1)) & z);
}
/*
 * isLessOrEqual - if x <= y  then return 1, else return 0
 *   Example: isLessOrEqual(4,5) = 1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 24
 *   Rating: 3
 */
int isLessOrEqual(int x, int y) {
    // 根据之前的0x30-0x39的思路一下子就可以将这个写出来
    // 两者做差取符号位
    // 但是这里需要注意的是如果是<0才是稳定获得1,如果是<=0那就是可能为1也可能为0,所以做好使用非负的符号位为0
    return !((y + (~x + 1)) >> 31 & 1);
}
// 4
/*
 * logicalNeg - implement the ! operator, using all of
 *              the legal operators except !
 *   Examples: logicalNeg(3) = 0, logicalNeg(0) = 1
 *   Legal ops: ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 4
 */
int logicalNeg(int x) {
    //
    return ((x | (~x + 1)) >> 31) + 1;
}
/* howManyBits - return the minimum number of bits required to represent x in
 *             two's complement
 *  Examples: howManyBits(12) = 5
 *            howManyBits(298) = 10
 *            howManyBits(-5) = 4
 *            howManyBits(0)  = 1
 *            howManyBits(-1) = 1
 *            howManyBits(0x80000000) = 32
 *  Legal ops: ! ~ & ^ | + << >>
 *  Max ops: 90
 *  Rating: 4
 */
int howManyBits(int x) {
    int sign = x >> 31;
    x = x ^ sign; // 获取一个无符号的x

    int b16 = (!!(x >> 16)) << 4;
    x >>= b16;

    int b8 = (!!(x >> 8)) << 3;
    x >>= b8;

    int b4 = (!!(x >> 4)) << 2;
    x >>= b4;

    int b2 = (!!(x >> 2)) << 1;
    x >>= b2;

    int b1 = (!!(x >> 1)) << 0;
    x >>= b1;

    int b0 = x;

    return b16 + b8 + b4 + b2 + b1 + b0 + 1;
}
// float
/*
 * floatScale2 - Return bit-level equivalent of expression 2*f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representation of
 *   single-precision floating point values.
 *   When argument is NaN, return argument
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned floatScale2(unsigned uf) {
    unsigned sign = uf & 0x80000000;
    unsigned exp = (uf >> 23) & 0xff;
    unsigned frac = uf & 0x7fffff;

    if (exp == 255) {
        // 如果exp全为1则就是inf直接返回arg
        return uf;
    } else if (exp == 0) {
        // 如果exp全为0则就是denorm，frac左移一位
        frac <<= 1;
    } else {
        // 正常情况就是这样
        exp += 1;
        if (exp == 255) {
            return sign | (exp << 23);
        }
    }
    return sign | (exp << 23) | frac;
}
/*
 * floatFloat2Int - Return bit-level equivalent of expression (int) f
 *   for floating point argument f.
 *   Argument is passed as unsigned int, but
 *   it is to be interpreted as the bit-level representation of a
 *   single-precision floating point value.
 *   Anything out of range (including NaN and infinity) should return
 *   0x80000000u.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
int floatFloat2Int(unsigned uf) {
    unsigned sign = uf & 0x80000000;
    unsigned exp = (uf >> 23) & 0xff;
    unsigned frac = uf & 0x7fffff;

    int E = exp - 127;

    frac |= 0x800000;

    if (E >= 31) {
        // 数字太大了
        return 0x80000000u;
    }
    if (E < 0) {
        // 小数部分
        return 0;
    }

    if (E > 23) {
        // 现在已经默认是23位的整数，如果E大于23，那么就是需要左移来扩大数字
        frac <<= (E - 23);
    } else {
        frac >>= (23 - E);
    }
    if (sign) {
        return -frac;
    } else {
        return frac;
    }
}

/*
 * floatPower2 - Return bit-level equivalent of the expression 2.0^x
 *   (2.0 raised to the power x) for any 32-bit integer x.
 *
 *   The unsigned value that is returned should have the identical bit
 *   representation as the single-precision floating-point number 2.0^x.
 *   If the result is too small to be represented as a denorm, return
 *   0. If too large, return +INF.
 *
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. Also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned floatPower2(int x) {
    int E = x + 127;
    if (E >= 255) {
        return 0x7f800000;
    }
    if (E <= 0) {
        if (E < -149) {
            // 已经是最小的可以表示的次数了
            return 0;
        } else {
            // 这里之所以可以这样是因为已经限定了E<0所以这里的x是小于等于-127的
            return 1 << (x + 149);
        }
    }
    return E << 23;
}
