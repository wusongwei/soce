# soce-redis
soce-redis是一个redis客户端，它提供以统一的接口访问单点、哨兵、集群模式的Redis。
同时，它对外只提供了少量接口，简单易用。

## 初始化
客户端需要初始化一个SoceRedis实例，它接受一个字符串类型的参数。连接不同模式的Redis
服务，参数类型略有不同。
- 单机模式   
*passwd@ip:port?timeout=1000*
- 哨兵模式   
*passwd@ip:port,ip:port?master=[master-name]&timeout=1000*
- 集群模式   
*passwd@ip:port,ip:port?cluster=true&timeout=1000*   
若服务器未配置密码，则passwd设置为none。timeout可选，默认永不超时。目前连接、
读写超时都使用同一个超时时间，后续可能分离。
```
string conf = "none@127.0.0.1:6379";
SoceRedis clt;
clt.init(conf);
```

## SoceRedis API
soce-redis支持执行单条命令和批量命令，无论哪种方式，都是通过迭代器来获取执行结果。
- *int init(const std::string& conf)*   
初始化连接，conf见初始化相关说明。
- *int exec(Fmt fmt)*   
执行单个命令。
- *void clear()*   
清除缓存的批量命令。
- *int append(Fmt fmt)*   
添加批量命令。 
- *int pipeline()*   
执行批量任务。
- *iterator begin()*   
返回执行结果的起始迭代器。
- *iterator end()*   
返回执行结果的结束迭代器。

## 迭代器
迭代器用于迭代访问执行结果，它指向一个SoceRedisReply的实例。SoceRedisReply
提供了根据各种返回类型的访问接口。当命令返回类型与调用接口类型不符时，这些接口
将返回非零值，表示访问非法，比如使用get_status()接口来获取get命令的结果。
主要接口如下所示：
- *int get_type(SoceRedisType& type)*   
- *int get_type(SoceRedisType& type)*   
- *int get_str(std::string& val)*   
- *int get_array_size(int& size)*   
- *int get_array_begin(SoceRedisIterator& iter)*   
- *int get_array_end(SoceRedisIterator& iter)*   
- *int get_int(int& val)*   
- *int get_status(std::string& val)*   
- *int get_error(std::string& val)*   

## 辅助宏
exec()方法接受参数类型为Fmt，相关定义如下：
```
using Fmt = std::function<void(Param&)>;
int exec(Fmt fmt);
```
为简化使用，soce-redis提供了宏soce_redis_docmd和soce_redis_append来执行
命令和添加命令，以屏蔽内部使用的Fmt以及Param类型。

## 执行单条命令
```
soce_redis_docmd(clt, "set" << "a" << "b");
string val;
clt.begin()->get_status(val); // 获取响应
```

## 执行批量命令
```
soce_redis_append(clt, "set" << "a" << "b");
soce_redis_append(clt, "get" << "a");
clt.pipeline();
for (auto& result : clt){
    // 处理结果
}
```

## 异步访问
soce支持协程，可以很方便地协程访问方案改造成异步访问方案。
为了使soce-redis不依赖于协程以及网络库等，目前异步访问的代码仅存放
于sample目录。示例中只开启了一个异步访问线程，修改DispatchQueue及
初始化时的线程数量，可方便地将其改造成多线程异步方案。

更多示例参见：soce-redis/sample目录。
