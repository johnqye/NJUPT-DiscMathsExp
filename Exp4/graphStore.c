#include "eularPath.h"
#include <stdio.h>
#include <memory.h>
#include <stdlib.h>
#include <assert.h>
#pragma GCC diagnostic ignored "-Wchar-subscripts"
// 初始化字符编号映射，src的字符作为节点名称
AscMapper *initMapper(AscMapper *this, const char *src) {
    unsigned sz = 0;
    memset(this->lblToAsc, -1, 128);
    memset(this->ascToLbl, -1, 128);
    while (*src) {
        if (this->ascToLbl[*src] == -1) {
            this->ascToLbl[*src] = sz;
            this->lblToAsc[sz++] = *src;
        }
        src++;
    }
    this->lblToAsc[sz] = '\0'; // lblToAsc按顺序为整个图的节点
    return this;
}
// 初始化大小为size * size的图
GraphInfo *initGraph(GraphInfo *this, unsigned size) {
    this->nodeSize = size; this->edgeSize = 0;
    this->degrees = calloc(this->nodeSize, sizeof(uint8_t));
    this->adjMat = calloc(this->nodeSize, sizeof(bool*));
    for (int i = 0; i < this->nodeSize; i++)
    // 由于是无向图，只存储下三角与对角线，第i行动态数组长度i+1
        this->adjMat[i] = calloc(i + 1, sizeof(bool));
    return this;
}
// 改变pos1行pos2列的连接状态
bool toggleLink(GraphInfo *this, uint8_t pos1, uint8_t pos2) {
    // 先判断是否越界
    assert(pos1 < this->nodeSize && pos2 < this->nodeSize);
    // 由于只有下三角，若行大于列交换行列下标
    bool* item = pos1 < pos2 ?
        &(this->adjMat[pos2][pos1]) : &(this->adjMat[pos1][pos2]);
    if (*item) // 未连接则连上，有连接则断开
        this->degrees[pos1]--, this->degrees[pos2]--, this->edgeSize--;
        // 改变对应节点度数，加减图的边数
    else this->degrees[pos1]++, this->degrees[pos2]++, this->edgeSize++;
    return *item = !(*item);
}
// 获取节点pos1 pos2之间是否有连接
bool hasLink(const GraphInfo *this, uint8_t pos1, uint8_t pos2) {
    assert(pos1 < this->nodeSize && pos2 < this->nodeSize); // 先判断是否越界
    // 由于只有下三角，若行大于列交换行列下标
    return pos1 < pos2 ? this->adjMat[pos2][pos1] : this->adjMat[pos1][pos2];
}
// 为一个图添加多个边，节点为src1 src2里的每一个字符
void addLinks(GraphInfo *this, const AscMapper *map,
    const char *src1, const char *src2) {
    while (*src1 && *src2) 
        toggleLink(this, map->ascToLbl[*src1++], map->ascToLbl[*src2++]);
}
// 交替添加src里的字符
void addLinksAlternate(GraphInfo *this, const AscMapper *map, const char *src) {
    char from[256] = "", to[256] = ""; uint8_t i = 0;
    while (src[i])
        if (i & 1) to[i >> 1] = src[i], i++;
        else from[i >> 1] = src[i], i++;
    addLinks(this, map, from, to);
}
// 释放图的内存
void freeGraph(GraphInfo *this) {
    free(this->degrees);
    for (unsigned i = 0; i < this->nodeSize; i++)
        free(this->adjMat[i]); // 先释放矩阵
    free(this->adjMat);        // 再释放指向矩阵的指针
}
