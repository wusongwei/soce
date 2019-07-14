# Transport
Transport是对网络库的封装，以屏蔽底层网络库，从而可以很方便地更换网络库。Transport
根据soce框架的需求抽象出具体的接口，主要包括网络部分和定时器部分。   
网络部分接口
除了用于服务端监听、用于客户端连接以及常用的收发接口外，还添加了watch()接口，用
于监听指定文件描述符上是否有读写事件，从而可以方便地集成一些外部组件，比如soce-zk
的事件，分发队列中有新数据可消费等。   

## Processor
对一般的协议而言，通常会将请求分成一个一个的数据包，每个包都会有一个包头，里面
包含了包的长度等信息。Processor用于对数据包进行拆分和处理。Transport中定义
ProcessorIf接口，用户需要实现其中的虚函数。这些函数用于包的拆分及处理，主要包括：
- *size_t get_max_header_size()*   
指定包头的最大长度。

- *size_t get_full_pkt_size(const char* data, size_t len)*   
参数为目前已经收到的包头数据，用户解析包头数据，返回整个包的长度。当包头不完整
时，返回IMCOMPLETE_PACKET_SIZE。

- *void process(uint64_t conn_id, std::string&& data)*   
处理数据包。conn_id为连接的文件描述符。

- * bool auto_read() *
默认为true，允许自动读取。当设置为false后，每次读取完后要再调用
transport的enable_read()方法后才会继续读取消息。

- *void on_accept(uint64_t conn_id)*    
有新的连接时的回调，用户可以此时做一些额外的事情，比如为连接创建Session等。

- *void on_connected(uint64_t conn_id)*   
作为客户端时时，connect()方法返回后并不代表连接已经建立。
当方法被调用时才表示真正建议成功。

- *void on_read(uint64_t conn_id)*   
有可读事件时的回调。

- *void on_error(uint64_t conn_id)*   
连接断开。

## TransportIf接口
- int listen(const std::string& addr, std::shared_ptr<ProcessorIf> processor, int* sock = nullptr)*   
用于服务端监听指定地址。addr格式为IP:Port，当IP或者Port为零时，采用随机地址，
此时可通过最后一个参数sock获取实际的文件描述符，再通过文件描述符获取真实的IP和端口。

- *int listen(const SSLCert& cert, const std::string& addr, std::shared_ptr<ProcessorIf> processor,int* sock = nullptr)*   
用于服务端以加密的方式监听指定地址。

- *int listen(const std::string& addr,struct lws_protocols* protocols,struct lws_extension* exts, int* sock = nullptr)*   
用于服务端以websocket的方式监听指定地址。

- *int connect(const std::string& addr, std::shared_ptr<ProcessorIf> processor, uint64_t& cli_id)*   
用于客户端连接服务端，并使用processor来处理收到的数据。

- *int connect(const SSLCert& cert,const std::string& addr,std::shared_ptr<ProcessorIf> processor, uint64_t&& cli_id)*    
用于客户端以加密的方式连接服务端，并使用processor来处理收到的数据。

- *int watch(int fd, std::shared_ptr<ProcessorIf> processor)*   
监听指定的文件描述符，使用processor来处理收到的数据。

- *int unwatch(int fd)*
取消监听。

- *int send(uint64_t conn_id, const char* data, size_t len)*
向指定的文件描述符发送数据。
 
- *int signal(int signum, SignalHandler handler)*    
指定信号处理函数。

- *int close(uint64_t conn_id)*   
关闭连接。

- *int enable_read(uint64_t conn_id)*   
允许接收数据，可用于速率控制。

- *int disable_read(uint64_t conn_id)*   
暂停接收数据，可用于速率控制。

- *int set_timeout(uint64_t conn_id, uint32_t timeout_ms)*    
设置连接的keepalive时间。

- *tmid_t add_timer(uint32_t gap_ms,TimerMode mode,TimerCallback tc)*   
创建一个定时器，定时器的模式分为三种：
   1. kTimerModeOnceAndDel : 触发一次并自动删除。
   2. kTimerModeOnceAndRetain ： 触发一次但不自动删除，后续可通过restart()方法重新启用。
   3. kTimerModePresist : 周期性触发。
   
- *int restart_timer(tmid_t tid, uint32_t gap)*    
以新的时间间隔重启定时器。

- *int del_timer(tmid_t tid)*   
删除定时器

## 内容Processor
在soce框架内容，经常使用eventfd来进行事件传播，这种情况下，并不会有真实的数据
传输，只是通知事件发生。为此提供了内置的EvtfdProcessor来处理这种场景。除此之外，
还提供了BypassProcessor和RawDataProcessor。
- *EvtfdProcessor*   
构造函数为*EvtfdProcessor(std::function<void(int fd)> callback)*，当有
事件发生时，callback会被调用，参数会发生事件的文件描述符。

- *BypassProcessor*   
同样用于通知事件发生，与EvtfdProcessor不同的是当事件发生时，它完全不读取文件
描述符中的数据，而eventfd中有一个8字节的缓冲区，每当有事件发生时，会读取8字节。
可用于监听第三方库的事件。其构造函数为*BypassProcessor(std::function<void(int fd)> callback)*。

- *RawDataProcessor*   
用于将收到的数据原封不动地传递用户处理，使用它时用户还需要重载process()方法，
默认buffer大小为1024字节。

## 使用示例
目前底层网络库只支持libevent，封装类为TransportLibevent。   
服务端使用示例如下:
```
// 初始化TransportLibevent
TransportLibevent tle; 
tle.init();   

//创建Processor。
std::shared_ptr<BaseProcessor> svr_prc(new ServerProcessor); 
svr_prc->set_transport(&tle);
 
//开始监听。
tle.listen("127.0.0.1:8988", svr_prc); 
```
客户端使用示例如下：
```
// 初始化TransportLibevent
TransportLibevent tle; 
tle.init();  

// 连接服务器
uint64_t conn_non_tls = 0;
tle.connect("127.0.0.1:8988",
            std::shared_ptr<ProcessorIf>(new ClientProcessor),
            conn_non_tls);

// 发送数据
string msg("hello, not tls on 8988!");
int len = msg.size();
tle.send(conn_non_tls, (char*)&len, 4);
tle.send(conn_non_tls, msg.c_str(), len);
```
定时器使用示例：
```
voi timer_entry(){}
tle.add_timer(10, TransportIf::kTimerModePresist, timer_entry);
```
更多示例参见：soce/transport/sample/sample.cpp
