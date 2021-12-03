#ifdef __cplusplus
#error "C CODE, NO C++ ALLOWED"
#else
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <stdint.h>
#include <stdbool.h>
// 结构定义，包含动态分配的关系矩阵和集合元素表
// 为加快元素查找效率，采用由字符到编号、编号到字符的双向数组映射
struct RelationSet {
    bool* relationMat;
    int8_t symToLbl[128];
    int8_t lblToSym[128];
    uint8_t mySize;
};
typedef struct RelationSet RelationSet;
// “公有”函数前置声明
RelationSet* initSet(RelationSet* this, const char* syms);
void addRelation(RelationSet* this, const char* from, const char* to);
void addRelationAlternate(RelationSet* this, const char* src);
void freeSet(RelationSet* this) {free(this->relationMat);}
void printRelation(RelationSet *this, int8_t row, int8_t col);
int32_t reflectiveClosure(const RelationSet* this, RelationSet* dest);
int32_t symmestricClosure(const RelationSet* this, RelationSet* dest);
int32_t transectiveClosure(const RelationSet* this, RelationSet* dest);
void printSet(const RelationSet* this);
RelationSet* copySet(const RelationSet* this, RelationSet* dest);
// 获取关系表this里行列对应下标
int16_t indexAt(const RelationSet* this, uint8_t row, uint8_t col) {
    // if (row > this->mySize || col > this->mySize)
    //     return -1;
    return row * this->mySize + col;
}
#pragma GCC diagnostic ignored "-Wchar-subscripts"
// 以syms里的字符作为集合元素初始化集合，分配关系矩阵
RelationSet* initSet(RelationSet* this, const char* syms) {
    this->mySize = 0;
    memset(this->symToLbl, -1, sizeof(int8_t) * 128);
    memset(this->lblToSym, -1, sizeof(int8_t) * 128);
    while (*syms) {
        if (this->symToLbl[*syms] == -1)
            this->symToLbl[*syms] = this->mySize++;
        else printf("Element repeatition: %c, ignoring\n", *syms);
        syms++;
    }
    memset(this->relationMat = malloc(this->mySize * this->mySize),
        false, sizeof(bool) * this->mySize * this->mySize);
    for (uint8_t i = 0; i < 128; i++)
        if (this->symToLbl[i] != -1)
            this->lblToSym[this->symToLbl[i]] = i;
    return this;
}
// 添加从from对应下标字符至to对应下标的字符的关系至this里，
// 如"ab" "bb"建立<a,b> <b,b>两个关系
void addRelation(RelationSet* this, const char* from, const char* to) {
    while (*from && *to) {
        if (this->symToLbl[*from] != -1 && this->symToLbl[*to] != -1)
            this->relationMat[indexAt(this, this->symToLbl[*from],
                this->symToLbl[*to])] = true;
        else printf("Tuple <%c, %c> contain unrecorded token\n", *from, *to);
        ++from, ++to;
    }
}
// src里元素交替作为二元组里的左、右至this里
// 如"abbb"建立<a,b> <b,b>两个关系
void addRelationAlternate(RelationSet *this, const char *src) {
    char from[128] = "", to[128] = ""; uint8_t i = 0;
    while (src[i])
        if (i & 1) to[i >> 1] = src[i], i++;
        else from[i >> 1] = src[i], i++;
    addRelation(this, from, to);
}
// 传入行列（可由上面的函数返回值获得）打印一对关系二元组及其存在情况
void printRelation(RelationSet *this, int8_t row, int8_t col) {
    printf("<%c, %c> ", this->lblToSym[row], this->lblToSym[col]);
    this->relationMat[indexAt(this, row, col)] 
        ? printf("exists") : printf("does not exist");
};
// 如果两者大小相等，把this的自反闭包放入dest里,否则只正常返回，不修改dest
// 返回一个以int32_t包装的int8_t数组，如果无自反性
// 下标为arr[0] arr[1]的元素不满足自反性 [2][3] 不满足反自反性
// 如果有，对应位置是-1
int32_t reflectiveClosure(const RelationSet *this, RelationSet *dest) {
    if (dest)
        dest = copySet(this, dest);
    int32_t ret = -1; int8_t* arr = (int8_t*)&ret;
    for (uint8_t i = 0; i < this->mySize; i++) {
        dest && (dest->relationMat[indexAt(dest, i, i)] = true);
        if (this->relationMat[indexAt(this, i, i)])
            arr[2] = arr[3] = i;
        else arr[0] = arr[1] = i;
    }
    return ret;
}
// 如果两者大小相等，把this的自反闭包放入dest里,否则只正常返回，不修改dest
// 返回一个以int32_t包装的int8_t数组，如果无对称性
// 下标为arr[0] arr[1]的元素不满足对称性 [2][3] 不满足反对称性
// 如果有，对应位置是-1
int32_t symmestricClosure(const RelationSet *this, RelationSet *dest) {
    if (dest)
        dest = copySet(this, dest);
    int32_t ret = -1; int8_t* arr = (int8_t*)&ret;
    for (uint8_t i = 1; i < this->mySize; i++) 
        for (uint8_t j = 0; j < i; j++) {
            bool up = this->relationMat[indexAt(this, i, j)], 
            down = this->relationMat[indexAt(this, j, i)];
            if (up != down) { // 对称位置不同，这一对位置不满足对称性
                arr[0] = i, arr[1] = j;
                dest && (dest->relationMat[indexAt(dest, i, j)] = true);
                dest && (dest->relationMat[indexAt(dest, j, i)] = true);
            } else if (up && down) // 对称位置全为1，这一对位置不满足反对称性
                arr[2] = i, arr[3] = j;
        }
    return ret;
}
// 如果两关系大小相等，把this的邻接矩阵复制给dest，返回dest
// 否则返回NULL，两者不变
RelationSet *copySet(const RelationSet *this, RelationSet *dest) {
    if (this && dest && this->mySize == dest->mySize) {
        memcpy(dest->relationMat, this->relationMat,
            sizeof(bool) * this->mySize * this->mySize);
        return dest;
    }
    return NULL;
}
// 打印由关系组成的集合
void printSet(const RelationSet *this) {
    putchar('{');
    for (uint8_t i = 0; i < this->mySize; i++)
        for (uint8_t j = 0; j < this->mySize; j++) 
            if (this->relationMat[indexAt(this, i, j)])
                printf("<%c, %c>, ", this->lblToSym[i], this->lblToSym[j]);
    puts("\b\b}\n");
}
// 如果两者大小相等，把this的传递闭包放入dest里，*否则什么都不做，返回INT32_MAX*
// 返回一个以int32_t包装的int8_t数组，如果无传递性
// 下标为arr[0] arr[1]的关系成立，[0][2]成立，但[1][2]不成立，可由此推出无传递性
// 如果有传递性，所有位置是-1。
int32_t transectiveClosure(const RelationSet *this, RelationSet *dest) {
    dest && (dest = copySet(this, dest)); // 建立新矩阵dest := this
    if (!dest) return INT32_MAX;
    int64_t ret = -1; int8_t* arr = (int8_t*)&ret;
    for (uint8_t i = 0; i < this->mySize; i++) {
        for (uint8_t j = 0; j < this->mySize; j++) { // Warshall: 对所有j
            if (dest->relationMat[indexAt(dest, j, i)]) { // 如果A[j,i] = 1,
                for (uint8_t k = 0; k < this->mySize; k++) { // 则对k = 1,2,...,n
                    bool nVal = dest->relationMat[indexAt(dest, j, k)] 
                        | dest->relationMat[indexAt(dest, i, k)]; // A[j,k] = A[j,k] + a[i,k]
                    if (nVal && !dest->relationMat[indexAt(dest, j, k)]) {
                        arr[0] = i, arr[1] = j, arr[2] = k; // 到这里出现矛盾，记录矛盾位置
                        dest->relationMat[indexAt(dest, j, k)] = true; // 并写入闭包
                    }
                }
            }
        }
    }
    return ret;
}

int main(int argc, char** argv) {
    if (argc <= 2 || argc >= 5)
        return printf("Usage: %s SET RELATION [RELATIONTO]\n", argv[0]);
    RelationSet* set = initSet(malloc(sizeof(RelationSet)), argv[1]),
        *closure = initSet(malloc(sizeof(RelationSet)), argv[1]);
    if (argc == 3)
        addRelationAlternate(set, argv[2]);
    else addRelation(set, argv[2], argv[3]);
    puts("Your Relation: "); printSet(set);
    int64_t res = reflectiveClosure(set, closure);
    int8_t* symInfo = (int8_t*)&res; // 强制指针转换获得使性质矛盾的元素下标 
    if (symInfo[0] == -1)
        puts("Relation is reflective.");
    else printRelation(set, symInfo[0], symInfo[1]), puts(", therefore not reflective");
    if (symInfo[2] == -1)
        puts("Relation is irreflective.");
    // （反）自反性只需打印对角线上的一个不满足即可推出
    else printRelation(set, symInfo[2], symInfo[3]), puts(", therefore not irreflective");
    puts("Reflective Closure: "); printSet(closure);
    res = symmestricClosure(set, closure);
    if (symInfo[0] == -1)
        puts("Relation is symmestric.");
    else { // （反）对称性需要打印对称的两个元素推出不成立
        printRelation(set, symInfo[1], symInfo[0]); putchar(',');
        printRelation(set, symInfo[0], symInfo[1]); puts(", therefore not symmestric");
    }
    if (symInfo[2] == -1)
        puts("Relation is assymmestric.");
    else {
        printRelation(set, symInfo[3], symInfo[2]); putchar(',');
        printRelation(set, symInfo[2], symInfo[3]); puts(", therefore not assymmestric");
    }
    puts("Symmestric Closure: "); printSet(closure);
    res = transectiveClosure(set, closure);
    if (symInfo[0] == -1)
        puts("Relation is transective.");
    else  { // 传递性需要三个元素才可推出不成立
        printRelation(set, symInfo[1], symInfo[0]); putchar(',');
        printRelation(set, symInfo[0], symInfo[2]); putchar(',');
        printRelation(set, symInfo[1], symInfo[2]); puts(", therefore not transective");
    }
    puts("Transective Closure: "); printSet(closure);
    freeSet(set); freeSet(closure); free(set); free(closure);
    return 0;
}
#endif
