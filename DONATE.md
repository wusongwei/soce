# 欢迎参加SOCE开源项目捐赠持股与捐赠出书计划

SOCE项目基于对于商业应用友好的Apache Licence协议开源。如果你感觉它能给你带来帮助，希望你能进行捐赠，你的捐赠将是这个项目持续改进的保障。与单纯地捐赠不同，本项目的捐赠包括捐赠持股计划、代码持股计划和捐赠出书计划。

## 捐赠持股计划：
你的捐赠是本项目持续改进的保障。捐赠后你将获得与捐赠额相等股份（参与捐赠出书部分列外）。如果有人愿意以至少每股两块的价格购买此软件且我也愿意出售时，你将获得相应的回报。

## 代码持股计划:
通过贡献代码，你将获得相应的股份。代码持股总量不超过100万股。当持股总量超过100万股时，通过代码获得的股份将相应“贬值”。（暂时还未想好积分规则）

## 捐赠出书计划：
开源软件最大的问题是缺乏文档，为此作者发起捐赠出书计划，捐赠超过70块的捐赠者视为参与捐赠出书计划。如果有超过500人参与捐赠出书计划，作者将自费出书。对于参与捐赠出书计划的捐赠者，前500名将获赠图书一本，同时获得相同捐赠额度的股份。500名以后的捐赠者将获赠图书一本，同时获得捐赠额超过70块的差额的股份。（比如捐赠10块，获得10股；捐赠100，前500名获赠图书一本外加100股，500名以后的获赠图书一本外加30股。）

# 捐赠二维码
![SoceDonate](./img/zhifubao.jpg "zhifubao")&emsp;&emsp;&emsp;&emsp;![SoceDonate](./img/weixin.jpg "weixin")
参与捐赠出书计划请在留言中注明收件地址和联系方式。

# 书籍拟定目录（非定稿）
本书拟首先给出期望中的日志库、序列化方案、协程库、网络库、RPC库、缓存库等所需要具备的特性，然后介绍阅读本项目所需要的一些基础知识以及项目中大量用到的一些辅助类，最后介绍各模块的实现。介绍各模块实现时，首先调研当前的一些开源实现，再分析其是否能满足期望的需求，最后给出解决方案。

一、SOCE简介
1.1 何谓SOCE
1.2 SOCE的目标
1.2.1 日志库
&emsp;&emsp;1.2.2 序列化方案
&emsp;&emsp;1.2.3 协程库
&emsp;&emsp;1.2.5 Redis客户端
&emsp;&emsp;1.2.6 网络库
&emsp;&emsp;1.2.7 zookeeper客户端
&emsp;&emsp;1.2.8 RPC库
&emsp;&emsp;1.2.9 缓存库

二、基础知识
2.1 C++11特性
&emsp;&emsp;2.1.1 智能指针
&emsp;&emsp;2.1.2类成员内部初始化
&emsp;&emsp;2.1.3右值引用
&emsp;&emsp;2.1.4 auto关键字
&emsp;&emsp;2.1.5以范围为基础的for循环
&emsp;&emsp;2.1.6 function和bind
&emsp;&emsp;2.1.7 lambda表达式
2.2 常见设计模式
&emsp;&emsp;2.2.1 单例模式
&emsp;&emsp;2.2.2 工厂模式
&emsp;&emsp;2.2.3 模板模式
&emsp;&emsp;2.2.4 组合模式
&emsp;&emsp;2.2.5 迭代器模式
2.3 辅助类
&emsp;&emsp;2.3.1 索引列表
&emsp;&emsp;2.3.2 组合列表
&emsp;&emsp;2.3.3 条件表达式
&emsp;&emsp;2.3.4 分发队列
&emsp;&emsp;2.3.5 阻塞队列
&emsp;&emsp;2.3.6 双缓冲队列
&emsp;&emsp;2.3.7 snowflake算法
&emsp;&emsp;2.3.8 读写锁

三、日志库log4rel
3.1 常见的C++日志库
&emsp;&emsp;3.1.1 Boost.Log
&emsp;&emsp;3.1.2 glog
&emsp;&emsp;3.1.3 log4cplus
3.2 log4rel的思路
3.3 log4rel的使用
&emsp;&emsp;3.3.1 简单示例
&emsp;&emsp;3.1.2 logger与sink
&emsp;&emsp;3.1.3 域控制
&emsp;&emsp;3.1.4 JSON格式
&emsp;&emsp;3.1.5 可扩展后端
&emsp;&emsp;3.1.6 日志动态过滤
3.4 log4rel的基本实现
&emsp;&emsp;3.4.1 LoggerMgr
&emsp;&emsp;3.4.2 Logger
&emsp;&emsp;3.4.3 关键字过滤
3.5 log4rel的过滤方案

四、序列化方案fads
4.1 常见的序列化方案
&emsp;&emsp;4.1.1 ProtocolBuffer
&emsp;&emsp;4.1.2 Thrift
&emsp;&emsp;4.1.3 Avro
4.2 fads的思路
4.3 fads的IDL定义规则
4.4 fads的使用
4.5 zigzag压缩算法
4.6 fads的二进制序列化方案
4.7 fads解析器的实现
4.8 DynamicGetter
4.9 DynamicSetter
4.10 fads的未来

五、协程库cort-engine
5.1 进程、线程与协程
5.2 Hook技术
5.3常见的协程库
&emsp;&emsp;5.3.1 libco
&emsp;&emsp;5.3.2 libgo
5.4 cort-engine的思路
5.5 cort-engine的使用
5.6 cort-engine的实现

六、redis客户端soce-redis
6.1 Redis简介
6.2 常见的C/C++ Redis客户端
6.3 soce-redis的思路
6.4 soce-redis的API
6.5 soce-redis的使用
&emsp;&emsp;6.5.1 非协程客户端
&emsp;&emsp;6.5.2 协程客户端
6.6 soce-redis的实现
&emsp;&emsp;6.6.1 非协程客户端
&emsp;&emsp;6.6.2 协程客户端

七、网络库
7.1 基础套接字编程
&emsp;&emsp;7.1.1套接字编程简介
&emsp;&emsp;7.1.2 echo服务器
&emsp;&emsp;7.1.3 echo客户端
7.2 SSL/TSL
7.3 libevent简介
7.4 transport类
&emsp;&emsp;7.4.1 transport的接口
&emsp;&emsp;7.4.2 transport的使用
&emsp;&emsp;7.4.3 transport的实现

八、zookeeper客户端soce-zk
8.1 zookeeper简介
8.2 zookeeper C语言API的简介
8.3 Zookeeper C语言API的缺陷
8.4 soce-zk的思路
8.5 soce-zk的API
8.6 soce-zk的实现

九、RPC库crpc
9.1 常见的RPC库
&emsp;&emsp;9.1.1 Thrift
&emsp;&emsp;9.1.2 gRPC
9.2 crpc的特点
9.3 crpc的IDL定义
9.4 crpc的使用
9.5 crpc的思路
9.6 crpc的实现
&emsp;&emsp;9.6.1 TypeTree
&emsp;&emsp;9.6.2 代码生成
&emsp;&emsp;9.6.3 CRPC的初始化
&emsp;&emsp;9.6.4 CondTree
&emsp;&emsp;9.6.5 命名服务
&emsp;&emsp;9.6.6 拦截器
&emsp;&emsp;9.6.7 crpc的调用过程
&emsp;&emsp;9.6.8 任务窃取
&emsp;&emsp;9.6.9 ServiceMesh方案
9.7 crpc的未来

十、内存数据库fadsdb
10.1 常见的内存数据库
&emsp;&emsp;10.1.1 redis
&emsp;&emsp;10.1.2 memcached
&emsp;&emsp;10.1.3 sqlite
10.2 fadsdb的特点
10.3 fadsdb的IDL定义
10.4 fadsdb的使用
10.5 fadsdb的思路
10.6 fadsdb的实现
10.7 fadsdb的未来