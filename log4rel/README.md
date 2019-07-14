# log4rel
允许按照键及内容进行过滤，以便进行调试及问题排查。

## 目标
当服务部署到线上后，假设某用户反馈遇到问题且此问题较难在测试环境出现。若此时打开
调试模式，则有可能出现大量日志影响性能。需要一种只过滤出特定用户的日志的方案。   

## 解决思路
一般的解决方案是采用正则表达式进行过滤，优点时可以做到很精细的控制，不过性能上
可能存在瓶颈。因为在使用正则表达式进行过滤前，需要将日志进行序列化，然后再使用
正则表达式进行过滤。   
log4rel认为大部分情况下不需要进行精确的过滤，因此可以为每个日志设置一个简单的key，
在过滤时首先针对key进行过滤，只有当key满足过滤条件时才对日志进行序列化。key的设计
可借鉴MQTT协议的topic匹配规则。key的组成为以点分隔的字符串，形如“module.user.key1”，
点号将key分隔成多级。在过滤时可以设置过滤条件，过滤条件支持+和*两个通配符，
+表示本级可为任意字符，*表示后续可为任意字符。   
比如"module.\*"表示匹配第一级为module的任意key，而“module.+.key1”表示匹配第一级
为“module”，第二级任意，第三级为“key1”的key。
同时log4rel也支持针对内容的过滤。

## log4rel特性
- 基于key的过滤。
- 基于内容的过滤，采用正则表表达式实现。
- 基于局部变量的过滤，使用lambda表达式捕获局部变量实现。
- 支持按普通模式和Json格式打印。
- 支持打印行号、日志等级、线程ID多个域。
- 支持同步和异步日志。

## 辅助宏
log4rel提供SOCE_XXX来打印各级别的日志，其中的XXX可为DEBUG,INFO,ERROR,FATAL等。   
log4rel提供了_S，_N和_D三个宏来控制格式。当按Json格式打印时，_S表示按字符串
打印，_N表示按字符打印，_D表示采用默认的键。

log4rel为每个线程都提供了一个LoggerMgr实例，用以管理当前线程创建的Logger实例，
可通过宏SOCE_LOGGER_MGR访问。同时还提供了宏SOCE_CUR_LOGGER用以访问当前线程
当前正在使用的Logger实例。   

## Logger
在线程中可通过宏SOCE_CUR_LOGGER访问当前的Logger实例，为其设置一些属性。相应接口如下：
- *void set_log_level(LogLevel level)*   
设置日志等级。

- *void reserve_field(LogField field, bool on)*   
设置日志中需要保存的域。

- *void set_fmt(LogFmt fmt)*   
设置日志格式，按普通文本格式或者Json格式打印。

- *int add_key_filter(LogLevel level, LogFilterType type, const std::string& filter)*   
添加针对key的过滤器。level表示过滤器作用的日志级别，type表示设置白名单还是黑名单，
filter为过滤条件。

- *int del_key_filter(LogLevel level, LogFilterType type, const std::string& filter)*   
删除过滤器。

- *int add_record_filter(LogLevel level, LogFilterType type, const std::string& filter)*   
添加针对内容的过滤，过滤条件为正则表达式。

- *int del_record_filter(LogLevel level, LogFilterType type, const std::string& filter)*   
删除针对内容的过滤。

- *void enable_key_filter()*  
开启key过滤。

- *void disable_key_filter()*  
关闭key过滤。

- *void enable_record_filter()*   
打开内容过滤。

- *void disable_record_filter()*  
关闭内容过滤。

## LoggerMgr
LoggerMgr用于Logger管理，主要接口为：
- *int32_t add_logger(const std::string& name, std::shared_ptr<Logger> logger)*   
添加logger。

- *int32_t switch_logger(const std::string& name)*   
切换logger。

- *std::vector<std::shared_ptr<Logger>> get_loggers()*   
获取所有日志，从而查看/修改它们的属性。

## 共享Logger
Logger是线程安全的，若要每个线程共享一个Logger，则可以在线程入口处使用LoggerMgr的
add_logger()和switch_logger()添加共享的Logger实例，并切换到此Logger。

## 多线程异步Logger
共享Logger比较简单，但是多个线程之间会抢锁，性能不太好。多线程异步Logger会由
每个线程都创建一个Logger，把日志写到本地，然后开启一个单独的线程来定时收取日志
，线程之间不会产生竞争，性能较高。   
log4rel提供了SAsyncLogger宏来简化异步logger的使用，它以单例的方式访问AsyncLogger
实例。   
使用AsyncLogger分为两个步骤：
- 主线程中初始化并启动日志
```
    // 设置日志后端为普通文本后端
    SAsyncLogger.set_sink(std::shared_ptr<PlainSink>(new PlainSink));
    // 在Creator中设置需要日志的属性
    SAsyncLogger.set_logger_creator([](){
            std::shared_ptr<soce::log4rel::Logger> logger(new soce::log4rel::Logger);
            logger->reserve_field(soce::log4rel::kLogFieldPos, true);
            logger->set_log_level(soce::log4rel::kLogLevelInfo);
            return logger;
        });

    SAsyncLogger.start();

```
- 在工作线程入口处调用以下方法
```
SAsyncLogger.init_thread_logger();
```

## 示例
```
SOCE_DEBUG("your.key") << _S("str", "hello")
                       << _N("num", 1)
                       << _D("default key 1");

string key1 = "your";
string key2 = "key2";
SOCE_DEBUG(key1 << "." << key2)
    << _S("str", "hello")
    << _N("num", 2)
    << _D("default key 2");
```
输出结果为：
```
2019-01-19 20:28:08.520, str = hello, num = 1, default key 1
2019-01-19 20:28:08.520, str = hello, num = 2, default key 2
```
通过set_fmt()方法可改变输出格式。
```
SOCE_CUR_LOGGER->set_fmt(soce::log4rel::kLogFmtJson);
```
输出结果为：
```
{"@timestamp" : "2019-01-19 20:32:07.120", "@fields" : { "str" : "hello", "num" : 1, "msg" : "default key 1"}}
{"@timestamp" : "2019-01-19 20:32:07.120", "@fields" : { "str" : "hello", "num" : 2, "msg" : "default key 2"}}
```
通过add_key_filter()方法设置过滤条件。
```
SOCE_CUR_LOGGER->add_key_filter(kLogLevelDebug, kLogFilterAllow, "your.*");
```
局部变量过滤
```
int x = 3;
// 条件为局部变量x>2
SOCE_INFO_IF("your.key3", x>2) << _N("x", x);
```
更多示例参考：soce/log4rel/sample/sample.cpp

## Roadmap
- 性能优化，取消strftime函数。
- 性能优化，缓存key的解析结果。
