#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>
// 五个基本命题变元 32位整数的每一位对应一种真假组合
const unsigned baseProp[5] = {
    0xaaaaaaaa, 0xcccccccc, 0xf0f0f0f0,
    0xff00ff00, 0xffff0000
};
// 所有要用的运算符
const char oprands[] = {
    '&', '|', '>', '<', '~',
    94, 118, 26, 29, 170, '(', ')', '\0'
};
// 前置声明
unsigned evalExpr(const char* src, const char** end, const char* letter);
// 在str里寻找c所在的下标 未找到返回-1
int charAt(char c, const char* str) {
    int res = 0;
    while (str[res] != c && str[res])
        res++;
    return str[res] == c ? res : -1;
}
// 提取src里的命题变元所使用字母放进dest里面，返回个数
int getLetter(const char* src, char* dest) {
    char* const begin = dest;
    *begin = '\0';
    while (*src) {
        // 不是运算符且还没有被写进字母表， 即在运算符和字母表里都找不到
        if (charAt(*src, oprands) == -1 && charAt(*src, begin) == -1)
            *dest++ = *src;
        src++;
    }
    *dest = '\0';
    return dest - begin;
}
// 计算两个操作数的运算结果
unsigned calcOne(unsigned lhs, unsigned rhs, char op) {
    switch (charAt(op, oprands)) {
    case 0: case 5: // 合取&
        return lhs & rhs;
    case 1: case 6: // 析取|
        return lhs | rhs;
    case 2: case 7: // 条件->
        return (~lhs) | rhs;
    case 3: case 8: // 双条件<->
        return ~(lhs ^ rhs);
    default:
        printf("ERROR: %c is not an operator\n", op);
        exit(EXIT_FAILURE);
    }
    return 0x44f8a1ef; //不会走到这里的
}
// 从src里获取一个运算数的值，获取后的位置放进end里，letter为命题变元的字母 
unsigned getNum(const char* src, const char** end, const char* letter) {
    bool notTag = 0; unsigned num;
    while (*src == oprands[4] || *src == oprands[9])
        notTag = !notTag, ++src;
    if (*src == '(')
        num = evalExpr(src + 1, &src, letter); //遇到括号当作完整表达式求出运算数的值
    else if (charAt(*src, letter) == -1) {
        printf("ERROR: found %c when expecting a proposition\n", *src);
        exit(EXIT_FAILURE);
    } else num = baseProp[charAt(*src++, letter)];
    *end = src;
    return notTag ? ~num : num;
}
// 求表达式src的值，求取后的位置放在end里，letter为命题变元的字母
unsigned evalExpr(const char* src, const char** end, const char* letter) {
    unsigned lhs, rhs; char op;
    lhs = getNum(src, &src, letter);
    while (*src != ')' && *src) {
        op = *src++;
        rhs = getNum(src, &src, letter);
        lhs = calcOne(lhs, rhs, op);
    }
    if (end)
        *end = (*src == ')' ? src + 1 : src);
    return lhs;
}

// 把获取的值打印为真值表
char* trueValTable(unsigned res, const char* letters, int letterNum, char* dest) {
    for (int i = 0; i < letterNum; i++)
        *dest++ = letters[i], *dest++ = '\t';
    *dest++ = '*', *dest++ = '\n';
    for (int i = 0; i < (1 << letterNum); i++) {
        for (int j = 0; j < letterNum; j++)
            *dest++ = (baseProp[j] & (1 << i)) ? 'T' : 'F', *dest++ = '\t';
        *dest++ = (res & (1 << i) ? 'T' : 'F'), *dest++ = '\n';
    }
    return dest;
}
// 根据获取的值（真值表），把主析取范式写入dest里
char* orFormula(unsigned res, const char* letters, int letterNum, char* dest) {
    for (int i = 0; i < (1 << letterNum); i++) {
        if (!(res & (1 << i))) continue;
        *dest++ = '(';
        for (int j = 0; j < letterNum; j++) {
            if (!(baseProp[j] & (1 << i)))
                *dest++ = oprands[4];
            *dest++ = letters[j]; *dest++ = oprands[5];
        }
        *(dest - 1) = ')'; *dest++ = oprands[6];
    }
    *(dest - 1) = '\n'; return dest;
}
// 根据获取的值（真值表），把主合取范式写入dest里
char* andFormula(unsigned res, const char* letters, int letterNum, char* dest) {
    for (int i = 0; i < (1 << letterNum); i++) {
        if (res & (1 << i)) continue;
        *dest++ = '(';
        for (int j = 0; j < letterNum; j++) {
            if (baseProp[j] & (1 << i))
                *dest++ = oprands[4];
            *dest++ = letters[j]; *dest++ = oprands[6];
        }
        *(dest - 1) = ')'; *dest++ = oprands[5];
    }
    *(dest - 1) = '\n'; return dest;
}
// 判断输入的命题是否合法，并去除空格
bool initProposition(char* prop) {
    static const char lBraces[] = "([{", rBraces[] = ")]}";
    const char* inIter = prop; char* outIter = prop;
    int braceDepth = 0;
    while (*inIter) {
        if (isspace(*inIter)) {
            inIter++; continue;
        }
        if (charAt(*inIter, lBraces) != -1)
            ++braceDepth;
        else if (charAt(*inIter, rBraces) != -1)
            --braceDepth;
        if (braceDepth < 0) // 右括号大于左括号数目
            return false;
        *outIter++ = *inIter++;
    }
    *outIter = '\0';
    return braceDepth == 0; // 左括号大于右括号数目
}
// 完整显示命题表达式expr的主合取析取范式
void display(char* expr) {
    char letters[6] = "", buf[1024] = "";
    if (!initProposition(expr))
        puts("Brackets Do Not Match"), exit(EXIT_FAILURE);
    int letLen = getLetter(expr, letters);
    if (letLen > 5)
        puts("Too Much Base Propositions"), exit(EXIT_FAILURE);
    unsigned res = evalExpr(expr, NULL, letters);
    andFormula(res, letters, letLen,  orFormula(res, letters, letLen,
        trueValTable(res, letters, letLen, buf)));
    puts(buf);
}

int main(int argc, char** argv) {
    if (argc < 2) {
        char exprIn[100];
        scanf("%s", exprIn);
        display(exprIn);
    } else for (int i = 1; i < argc; i++)
        display(argv[i]);
}
