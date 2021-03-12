# BPlusTree

支持`BookStore`中重复key插入/查询/删除的`BPlusTree`

采用的是子节点数目比索引多一个的定义

还有了一些后续火车票大作业也许会用到的功能

`TokenScanner.h`：功能类似于`std::stringstream`，支持用`std::string`构造

`HashType.h`：将`std::string`进行哈希，用于`BPlusTree`的`key`

`Parser`：火车票大作业的命令处理装置，重载了`operator[]`用于直接访问对应参数

`algorithm.h`：包含了`BPlusTree`需要用到的三个`stl`函数：`lower_bound`，`upper_bound`，`find`，以及模仿`std::sort`中启发式算法的`sort`

本工程后续~~也许~~有相当大可能性会变成火车票大作业🤔