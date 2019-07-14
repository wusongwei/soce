# soce-zk
当Zookeeper被用于服务注册时，一般是在Zookeeper中创建一个临时节点来实现。但是
原生的Zookeeper C语言客户端在两种情况时会存在问题：
- 服务发生coredump后，被watchdog快速重新拉起。由于客户端与服务端连接发生断开时，
注册的临时节点不会立即删除，服务被watchdog重新拉起来再次注册时，如果前后两次注册
的节点名相同，则会产生节点已经存在的错误，从而注册失败。
- 客户端与服务端之间发生网络故障。这种情况下，客户端会一直尝试重新连接服务端，
当网络恢复后，客户端重新连接上服务端后，只会重新注册watch，不会重新创建临时节点。   
        
除此之外，Zookeeper的watch事件回调会在一个单独的线程中，而soce期望能在指定的
线程中获取这些事件，从而避免锁。soce-zk对Zookeeper原生C语言客户端进行了封装，
以解决上述问题。

## 接口
Zookeeper允许监控节点变更事件，soce-zk对这类接口提供了WatchMode参数，可选值为:
   1. kWatchModeNone : 不监控，此时相当于仅同步访问一次。
   2. kWatchModeOnce ： 监控一次，当事件发生后，若想继续监控则需要重新注册监控事件。
   3. kWatchModeRepeat : 持续监控，只要有变更则通知。

- *int init(const std::string& addr, uint32_t timeout)*   
以同步的方式连接服务端。

- *int create(const std::string& path, const std::string& value, CreateMode mode)*   
创建节点，对于临时性节点，soce-zk会创建一个watch来监听节点的状态，一旦发现节点消息就会自动重建。
节点模式分为四种：
    1. kCrtModePresistent ：持久性节点。
    2. kCrtModeEphemeral ：临时节点。
    3. kCrtModeSequence ： 顺序节点。
    4. kCrtModeEphSeq ： 临时顺序节点。
      
- *int exist(const std::string& path, WatchMode mode, bool& exist)*   
检查节点是否存在。

- *int get(const std::string& path, WatchMode mode, std::string& value)*   
获取节点内容。

- *int get_children(const std::string& path, WatchMode mode, std::vector< std::string >& value) *
获取孩子节点路径。

- int get_children_content(const std::string& path, WatchMode mode, std::vector<std::pair<std::string, std::string>>& value)*   
获取孩子节点路径及内容。当孩子节点内容变更时，不会触发watch事件。

- *int get_notify_evtfd()*   
获取事件通知的文件描述符，可使用网络库来监听此文件描述符，以判断是否有事件发生。

- *void get_notifies(std::vector<NotifyInfo>& notifies)*   
用于当有事件发生时，获取具体的事件内容。

## 使用示例
参见：soce/soce-zk/sample/sample.cpp


