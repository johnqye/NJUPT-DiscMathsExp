#pragma once
#ifdef __cplusplus
#error C CODE, NO CXX PERMITTED
#else
#include <stdbool.h>
#include <stdint.h>
// 用于实现节点名称和对应下标的O(n)转换
typedef struct AscMapper {
    int8_t ascToLbl[128];
    char lblToAsc[128];
} AscMapper;
// 初始化映射器
AscMapper* initMapper(AscMapper* this, const char* src);
// 存储无向图基本信息
typedef struct UndirGraphInfo {
    bool **adjMat; // 临接矩阵
    uint8_t *degrees; // 每个节点的度数
    unsigned edgeSize, nodeSize; // 边数、节点数
} GraphInfo;
// 存储一个无向图所有欧拉路径的信息
typedef struct EularData {
    unsigned resSize, resMax, stepSize; // 结果数目与最大结果数目
    uint8_t* stepStk; //临时数组，存储获取欧拉路径用到的栈的数据
    uint8_t** results; // 存储所有结果
} EularData;
// 存储图信息函数的相关声明
GraphInfo* initGraph(GraphInfo* this, unsigned size);
void addLinks(GraphInfo* this, const AscMapper* map,
    const char* src1, const char* src2);
void addLinksAlternate(GraphInfo *this, const AscMapper *map, const char *src);
bool toggleLink(GraphInfo* this, uint8_t pos1, uint8_t pos2);
bool hasLink(const GraphInfo *this, uint8_t pos1, uint8_t pos2);
void freeGraph(GraphInfo *this);
// 获取欧拉回路函数的相关声明
EularData* getEular(const GraphInfo* this, unsigned resMax);
void freeEular(EularData *data);
void printGraph(const GraphInfo *this, const AscMapper* map,
    const EularData* data);
#endif