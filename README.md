# 南邮离散数学实验代码收集

~~男生没电~~南京邮电大学离散数学实验代码  
目前只有代码没有实验报告😭因为作者（们）会有可能被判抄袭  
22年1月（2021年第一学期报告上交后）会有的  
更新于**2021.12.3**

1. 利用真值表法求取主析取范式以及主合取范式的实现
2. 集合上二元关系性质的判定（**含各种闭包**）
3. 整除关系中盖住关系的求取及格论中有补格的判定
4. 图的生成及欧拉回路的确定

注意这些代码是*博采众长*得来的。  
全部采用C语言是因为~~巧了~~考虑到大多数学生C语言是最熟悉的语言。  
实验3目前没有注释，1是简洁版，没有栈。目前在尝试搜刮更好的实验3。

## 实验4注意

实验4的运行结果十分绚丽，以至于代码十分困难。  
但很遗憾，这是现在为止搜刮到的唯一一个:  

1. 他人（指收集者）能看懂（指没有满天全局变量、瞎取名称）
2. 完全满足要求（输出**所有**欧拉路）
3. 作者同意公开
4. 效率还不错（80个欧拉回路1）

的代码了

### Makefile

除了实验4外，其他3个实验都是单文件，  
实验4的批量生成工具是*GNU Make*，如果没听说这个东西，  
比较现实的办法是把`Exp4`里的c和h文件下下来，按照习惯的方式处理。  
比较科幻的方法是安装Linux虚拟机，编译运行。  

### GraphViz

除了变态的`Makefile`以外，实验4还使用了一款叫做`GraphViz`的图可视化工具。  
比较现实的方法是把`getEular.c`里的`printGraph`里的`puts printf`
改为你喜欢的方式输出，  
还能减少借鉴程度。科幻的方法是安装。  
Deb: `apt install graphviz`  
Rpm: `dnf install graphviz`  
上面两行看不懂：采用现实的方法😭

## 我觉得这些代码太垃圾了！！！

欢迎提出Pull Request，帮助更多破邮人。  
注意我们的目的是帮扶困难群众**不是追求极致效率**  
可读性第一，百分百正确第二，效率第三  
全局变量超过3个（`static`不算），或  
命名出现类似`void fun(int sum1, int sum2)`直接过  
（作用域小，如`for`循环里的i j等不算）