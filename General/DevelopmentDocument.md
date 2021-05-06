# TicketSystem:Automata Development Document

## 文件结构

//todo:此处应有树形图



## include

### algorithm.h

| 函数        | 功能                                  |
| ----------- | ------------------------------------- |
| sort        | 实现stl启发式算法的sort               |
| lower_bound | 同std::lower_bound，供BPlusTree类使用 |
| upper_bound | 同std::upper_bound，供BPlusTree类使用 |



### HashMap.h

//todo



### LRUCacheMemoryPool.h

//todo



### exceptions.h

//todo



### vector.h

//todo



### BPlusTree.h

//todo



### TokenScanner.h

//todo



### Parser.h

//todo



### string_t.h

长度为模板类参数，定长，可写入文件的string



### Clock.h

包含两个类`Time`与`Date`



## src

### UserManager.h/.cpp

负责所有用户相关的操作

内嵌类：`user_t`

| 变量类型     | 变量名称  | 备注                       |
| ------------ | --------- | -------------------------- |
| string_t<20> | username  |                            |
| string_t<30> | password  |                            |
| string_t<10> | name      | 一个汉字占据两个char的空间 |
| string_t<30> | mailAddr  |                            |
| int          | privilege |                            |



成员变量：

| 成员变量类型                  | 成员变量名称 | 作用               |
| ----------------------------- | ------------ | ------------------ |
| HashMap<username, bool>       | loginPool    | 存储用户登录态     |
| BPlusTree<username, int>      | indexPool    | username的索引文件 |
| LRUCacheMemoryPool\<user_t> | storagePool  | 存储用户信息       |
| std::ostream&                 | defaultOut   | 输出信息           |



成员函数：

```c++
UserManager(const string &indexPath, const string &storagePath, std::ostream& dft = std::cout);

void addUser(const Parser &parser);//构造一个userType类对象，写入storagePool与indexPool

void login(const Parser &parser);//更新loginPool

void logout(const Parser &parser);//更新loginPool

void queryProfile(const Parser &parser);//查找即可

void modifyProfile(const Parser &parser);//修改即可

void clear();//清空信息
```





### TrainManager.h/.cpp

负责所有火车相关的操作

内嵌类：`trainType`

| 变量类型          | 变量名称      | 备注                       |
| ----------------- | ------------- | -------------------------- |
| string_t<20>      | trainID       |                            |
| int               | stationNum    |                            |
| string_t<20>[100] | stations      | 一个汉字占据两个char的空间 |
| int               | seatNum       |                            |
| int[100]          | prices        |                            |
| Time              | startTime     |                            |
| int[100]          | travelTimes   |                            |
| int[100]          | stopoverTimes |                            |
| Date[2]           | saleDate      |                            |
| char              | type          |                            |
| bool              | released      |                            |
| int[70]\[100]     | remainSeats   | 存储每天每站剩余的类       |



成员变量：

| 成员变量类型                   | 成员变量名称 | 作用                     |
| ------------------------------ | ------------ | ------------------------ |
| BPlusTree<trainID, int>        | indexPool    | trainID的索引文件        |
| LRUCacheMemoryPool\<trainType> | storagePool  | 存储火车信息             |
| BPlusTree<station, trainID>    | stationPool  | 存储经过某站的所有车的ID |
| TokenScanner                   | splitTool    | 用于分割被\|分开的信息   |
| std::ostream&                  | defaultOut   | 输出信息                 |



成员函数：

```c++
TrainManager(const string &indexPath, const string &storagePath, const string &stationPath, std::ostream& dft = std::cout);

void addTrain(const Parser &parser);//构造一个trainType类对象，写入storagePool与indexPool

void releaseTrain(const Parser &parser);//更新trainType类中released变量，对于经过的每一站在stationPool中添加元素

void queryTrain(const Parser &parser);//按照trainID查询，检查-d是否在saleDate内

void deleteTrain(const Parser &parser);//检查是否release

void queryTicket(const Parser &parser);//找到所有经过-s（筛选出-d为出发日期的车），-t的车，暴力搜索其中的重复元素

void queryTransfer(const Parser &parser);//找到所有经过-s（筛选出-d为出发日期的车），-t的车，分别称为t1，t2，对于t1的每个元素，遍历它的每个-s后的站，看它在t2中是否出现

void clear();//清空信息
```





### OrderManager.h/.cpp

负责所有订单相关的操作

内嵌类：`orderType`

| 变量类型     | 变量名称     | 备注                     |
| ------------ | ------------ | ------------------------ |
| enum         | status       |                          |
| string_t<20> | trainID      |                          |
| string_t<20> | from         |                          |
| Time         | leavingTime  |                          |
| string_t<20> | to           |                          |
| Time         | arrivingTime | 需在构造时读取火车的数据 |
| int          | price        |                          |
| int          | num          |                          |



成员变量：

| 成员变量类型                   | 成员变量名称 | 作用                                    |
| ------------------------------ | ------------ | --------------------------------------- |
| BPlusTree<username, int>       | indexPool    | 存储每个用户的所有order对应的索引       |
| BPlusTree<trainID, int>        | pendingPool  | 对于每辆车，存储pending的订单对应的索引 |
| LRUCacheMemoryPool\<orderType> | storagePool  | 存储订单信息                            |



成员函数：

```c++
OrderManager(const string &indexPath, const string &storagePath, const string &pendingPath, std::ostream& dft = std::cout);

void buyTicket(const Parser &parser);//直接买，票如果不够的话加入pendingList

void queryOrder(const Parser &parser);//直接在indexPool内查询即可

void refundTicket(const Parser &parser);//在indexPool内查询找到该订单，到pendingList中按顺序解决所有pending的order，更新订单信息、火车信息

void clear();//清空信息
```





### Administrator.h/.cpp

负责整个程序的执行



成员变量：

| 变量类型     | 作用               |
| ------------ | ------------------ |
| string[]     | 存储各个文件的路径 |
| Parser       | 处理指令           |
| UserManager  | 处理用户操作       |
| TrainManager | 处理火车操作       |
| OrderManager | 处理订单操作       |



成员函数：

```c++
Administrator(const string &path...);

void initialize();//初始化

void clean();//清空所有信息

void exit();//退出程序，注意需要执行析构函数，不可直接std::exit(0);

void runProgramme();//处理指令，分解成子任务交给成员变量/自身去做
```















