#include "eularPath.h"
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
// 打印图以及所有的欧拉回路（如果存在）
// 格式是Graphviz图格式，便于可视化输出 
void printGraph(const GraphInfo *this, const AscMapper* map,
    const EularData* data) {
    if (!data || data->resSize == 0) // 不是欧拉图，图下方显示“不是欧拉图”
        puts("graph original {\n\tlabel=\"Not an Eularian Graph!\"");
    // 否则显示“是欧拉图”
    else puts("graph original {\n\tlabel=\"Eularian Graph!\"");
    for (unsigned r = 0; r < this->nodeSize; r++)
        for (unsigned j = 0; j <= r; j++)
            if (hasLink(this, r, j)) // 如果有连接，就输出
                printf("\t%c -- %c;\n", map->lblToAsc[j], map->lblToAsc[r]);
    puts("}");
    if (!data || data->resSize == 0) return; // 不是欧拉图，到此为止
    for (unsigned i = 0; i < data->resSize; i++) {
        unsigned step = 0; // 用于标识当前步数，输出的图可以看到具体走法
        printf("digraph solution%d {\n\tlabel=\"Solution %d\"\n", i, i);
        for (unsigned j = 0; j < this->edgeSize; j++)
        // 按照每一个res里记录的点走的路径是欧拉回路
            printf("\t%c -> %c [label=\"%d\"];\n",  // 画有向图箭头
                map->lblToAsc[data->results[i][j]], // 标注步数编号
                map->lblToAsc[data->results[i][j + 1]], ++step);
        puts("}");
    }
}
#pragma GCC diagnostic push 
#pragma GCC diagnostic ignored "-Wdiscarded-qualifiers" 
// 获取欧拉路径的主体递归函数。
void getEularImpl(EularData *dat, const GraphInfo* g, unsigned nodeLbl) {
    for (unsigned i = 0; i < g->nodeSize && dat->resSize != dat->resMax; i++)
        /* 对于每个点，如果有连接，则走入这个点
         * 走入、走出一个点前后，图的内容是不变的，因此声明*g为常量 
         * 但是在过程中，需要改变图的内容，把尝试走的路径标为无连接
         * 走出去后，会把图的内容恢复，因此需要暂时去除常量标识符 */
        if (hasLink(g, i, nodeLbl)) {
            GraphInfo *constCast = g; // 把图中走过的边去掉
            toggleLink(constCast, i, nodeLbl); constCast->edgeSize++;
            dat->stepStk[dat->stepSize++] = i; // 当前正在走的点压入栈
            if (dat->stepSize == g->edgeSize + 1) // 判断是否所有边都走过
                // 入栈的点是行走顺序，把它复制到对应结果数组里
                dat->results[dat->resSize++] = memcpy(malloc(sizeof(uint8_t) * (
                g->edgeSize + 1)), dat->stepStk, sizeof(uint8_t) * (g->edgeSize + 1));
            else getEularImpl(dat, g, i); // 未完成，再以下一个点为起点，重复调用此函数继续下一步
            dat->stepSize--; // 这个点走完了，出栈，把去掉的边恢复
            toggleLink(constCast, i, nodeLbl); constCast->edgeSize--;
        }
}
#pragma GCC diagnostic pop
// 用户调用的获取欧拉回路的函数。图g在进出函数前后无变化，返回数据写入的指针。
// 如果不是（半）欧拉图，返回空指针。
EularData *getEular(const GraphInfo *this, unsigned int resMax) {
    unsigned oddDeg = 0;
    for (unsigned i = 0; i < this->nodeSize; i++)
        if (this->degrees[i] & 1)
            oddDeg++;
    if (oddDeg != 0 && oddDeg != 2) // 统计度为奇数节点个数
        return NULL;    // 度为奇数节点个数不是0或2,无欧拉路径
    EularData *dat = malloc(sizeof(EularData)); // 有欧拉路径才分配空间
    dat->stepStk = calloc(sizeof(uint8_t), this->edgeSize + 1);
    dat->results = calloc(sizeof(uint8_t*), resMax);
    dat->resMax = resMax, dat->resSize = 0, dat->stepSize = 1;
    if (oddDeg == 0) { // 全欧拉图，每一个点都有欧拉回路
        for (unsigned i = 0; i < this->nodeSize; i++)
            // 走入栈的栈底元素是第一个点i，提前赋值
            dat->stepStk[0] = i, getEularImpl(dat, this, i);
        return dat;
    }
    for (unsigned i = 0; i < this->nodeSize; i++)
        if (this->degrees[i] & 1) // 半欧拉图，只有度为鸡的点有欧拉路径
            dat->stepStk[0] = i, getEularImpl(dat, this, i);
    return dat;
}
// 释放欧拉路径记录的空间
void freeEular(EularData *data) {
    free(data->stepStk);
    for (unsigned i = 0; i < data->resSize; i++)
        free(data->results[i]);
    free(data->results);
    free(data);
}