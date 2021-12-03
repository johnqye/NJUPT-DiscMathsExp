#include "eularPath.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
// 用于解析命令行参数选择合适模式初始化图和相关结构
int eulArg(int argc, char** argv, AscMapper* map, GraphInfo* g,
    unsigned* maxRes) {
    *maxRes = 50;
    switch (argc) {
    case 3: *maxRes = atoi(argv[2]); //[[fallthrough]]
    case 2:
        initMapper(map, argv[1]);
        initGraph(g, strlen(map->lblToAsc));
        addLinksAlternate(g, map, argv[1]);
        break;
    case 5: *maxRes = atoi(argv[4]); //[[fallthrough]]
    case 4: 
        initMapper(map, argv[1]);
        initGraph(g, strlen(argv[1]));
        addLinks(g, map, argv[2], argv[3]);
        break;
    default:
        printf("Usage: %s ALLNODES FROMNODES TONODES [MAXRES]\n", argv[0]);
        printf("Or: %s LINKPAIRS [MAXRES]", argv[0]);
        return 1;   //命令行输入错误返回1
    }
    return 0;   //正确返回0
}

int main(int argc, char** argv) {
    unsigned maxSize; AscMapper myMap; GraphInfo myGraph;
    if (eulArg(argc, argv, &myMap, &myGraph, &maxSize))
        return 1; // 输入错误，退出程序
    EularData* dat = getEular(&myGraph, maxSize); //获取欧拉图信息
    printGraph(&myGraph, &myMap, dat);  // 输出原图与欧拉图
    freeGraph(&myGraph);
    if (dat) freeEular(dat);    //若非欧拉图信息结构为空，需检验空值
    return 0;
}
