# Utils
Utils提供一些常用的辅助工具。

## 条件表达式CondExpr
条件形如“a > 1 && ( c != 2 || d < 4)”，其中的"a > 1"等将被看成一个条件，它
包含三部分：键、逻辑运算符和值。用户只需要提供一个可根据这三者计算出的布尔结果
的函数即可求出整个条件表达式的值。它主要包括两个接口：
- *int parse(const char\* data, size_t len)*   
解析条件表达式。

- *bool calc(ConcreteExpr expr)*   
ConcreteExpr的定义如下：
```
using ConcreteExpr = std::function<bool(const std::string&,
                                        const std::string&,
                                        const std::string&)>;
```
示例：
```
static bool compare(const std::string& lvalue,
                    const std::string& oper,
                    const std::string& rvalue)
{
    (void) lvalue;
    (void) rvalue;

    return (oper == "true") ? true : false;
}

void sample_cond_expr()
{
    std::string str = "(a false 1) || ( ((b true 1)) && (c false 1 || d true 1)) && e true 1";

    soce::utils::CondExpr expr;
    expr.parse(str.c_str(), str.size());
    bool rc = expr.calc(std::bind(compare,
                                  std::placeholders::_1,
                                  std::placeholders::_2,
                                  std::placeholders::_3));
}

```

## 双缓冲队列DoubleCacheQueue
DoubleCacheQueue漏斗型队列，即线程安全地支持多个生产者和一个消费者。
它内部采用两个队列，生产者和消费者分别对应不同的队列。生产者之间通过
CAS无锁编程实现同步。生产者与消费者之间通过读写锁实现同步，当消费者消费
后，交换内部的两个队列，以做到允许同时生产和消费。

## FunnelBuffer
继承自DoubleCacheQueue，允许多线程写，一个线程读，用于日志库。

## FunnelQueue
继承自DoubleCacheQueue，与FunnelBuffer的区别在于此类是一个模板类，内部存储
的是类实例。它要求存储的类必须提供无参数的构造函数。

## 分发队列DispatchQueue
继承自FunnelQueue。
支持M:N的消费模型，即M个生产者和N个消费者。消费者的数量在初始化阶段就已经确定，
不适用于消费者数量变更的情形。一般用于固定线程数，每个线程为一个消费者。
生产者和消费都在消费时需要指定ID，在初始化时可以指定选择器，用以将特定消息
发送给特定的消费者，选择器返回的即为消费者ID。当返回的消费者ID为-1时，表示均衡
发送给所有消费者。   

主要API定义如下：
- *DispatchQueue(Selector selector = [](const T&, size_t){return -1;})*   
默认1:1的消费模型。选择器用于确定生产的数据应该被哪个消费者消费。

- *DispatchQueue(size_t producers, size_t consumers,Selector selector = [](const T&, size_t){return -1;})*    
指定生产者消费者数量的消费模型。

- *void produce(const T& val)*   
生产数据，另外还有带move语义的相关接口。

- *int consume(size_t cid, FQVector<T>& out)*   
消费数据。

- *int try_consume_for(size_t cid, FQVector<T>& out, uint64_t timeout_ms)*   
消费数据，在指定时间后超时返回。

- *int try_consume(size_t cid, FQVector<T>& out)*  
尝试消费数据，如果没有数据，立即返回。

- *int get_consumer_fd(size_t cid)*   
获取指定消费者的文件描述符，以用于网络库监控。

## 读写锁
read_lock()   
try_read_lock_for()   
read_unlock()   
write_lock()   
write_unlock()    

## 索引列表
提供顺序访问和随机访问两种接口。

## 分组列表

## Snowflake
Twitter开源的分布式全局ID生成算法。

## 协程Executor
在线程池中执行任务，挂起当前协程，直到任务执行完毕或者超时。
用以解决长耗时任务在协程中运行时，其它协程得不到执行的问题。