# CRPC   
Crpc是一个RPC框架，它依赖于序列化工具fads。在同一个工程中，可同时对外提供多种
服务，每种服务允许提供多个方法。它最主要的特点是允许为方法设置条件，客户端在发起
RPC调用请求时只会发送给满足条件的服务端，从而可以方便地做灰度发布。   
Crpc除了支持常规的RPC调用外，还支持条件广播和无条件广播。不过由于广播是一对多的
形式，因此广播没有返回类型，无法校验是否成功，只能用于允许失败的场景，比如配置更新。
Crpc目前还未集成链路监控等特性，不过提供了拦截器功能，通过此功能，用户可以很方便
地实现这些特性。

# 拟解决问题
- *灰度发布*
新版本上线时，设置条件，仅让小部分用户尝试新版本。

- *共享测试环境*
系统通常由多个服务构成，每个服务都可能在同时开发与测试。一般情况下，
不可能为每个开发人员都搭建一套独立的测试环境，只能多人共享。多人共享
可能出现某个服务在调试时影响依赖它的服务的调试与开发。  
使用crpc可以在调试服务时设置过滤条件，仅过滤出自己发送的测试数据。
其它人员的测试数据将由其它的版本服务处理，从而互不影响，共享测试环境。

## Schema定义
Crpc在使用以前需要定义Schema，采用fads语法，定义额外的几个关键字。   
- *service*  
 用于服务定义。
- *void*  
 用于定义方法的返回类型，表示无返回值，但是调用端需要等待服务端响应。
- *null*   
用于定义方法的返回类型，表示无返回值，且调用端不会等待服务端响应。
- *condcast*   
用于定义方法的返回类型，表示条件广播。
- *uncondcast*   
用于定义方法的返回类型，表示无条件广播。   
   
```
service CortDemo
{
    i32 echo(i32 p);
    void test_void(i32 p);
    null test_null(i32 p);
    condcast test_condcast(i32 p);
    uncondcast test_uncondcast(i32 p);
}
```
示例定义了一个名为CortDemo的服务，它包含五个方法。   

- *echo()*   
普通的RPC方法，接受一个类型为整形参数，返回值也是一个整形。可以设置过滤条件。
- *test_void()*   
返回值类型为void的RPC，接受一个类型为整形的参数。可以设置过滤条件。
- *test_null()*   
无返回值的RPC，与void类型不同，null类型表示调用端不会等待服务端响应。
可以设置过滤条件。
- *test_condcast()*   
带条件的广播，可以设置过滤条件，所有请求成功才算成功。
- *test_uncondcast()*   
无条件广播，不可以设置过滤条件，所有请求成功才算成功。

## 代码生成
生成代码使用fads提供的编译器，需要首先进入fads/compiler目录，编译生成编译器，
然后执行：
```
fadsgen your_schema.fads
```
这将生成一个gen-cpp目录，包含类型定义及服务定义相关文件。  

## 条件过滤基本思路
服务可以为每个方法设置过滤条件，当服务启动时，会将服务地址及这些条件都注册到
命名服务器上。客户端会拉取其感兴趣的服务的相关信息。当客户端发起RPC调用时，
它会将请求内容与过滤条件相比对，只会将请求发送给满足过滤条件的服务。   
服务端在注册时，会将用户指定的参数索引地址转换为索引下标。fads支持根据索引下标
快速从序列后的二进制数据中访问到指定元素的能力，从而可以快速实现条件判断。

## 线程模型
Crpc会开一个单独的线程用于IO处理，具体的业务逻辑交由工作线程处理。工作线程的
数量由用户指定的模式确定。Crpc允许同时支持多个服务，每个服务可以选择共享线程
还是独占线程。共享线程的数量由用户指定，默认为CPU的个数 * 2 + 1。
对设置为共享线程模式 的服务，其请求将按照负载均衡算法均衡到每个线程中。
若多个服务都指定共享线程模式，则它们共享这些线程。对设置为独占线程模式的服务，
会由单独的线程来处理属于此服务的请求。即工作线程的总数为共享线程的数量
与独占线程模式服务的数量之和。

## 使用示例
目前Crpc还未支持配置文件，因此初始化参数需要靠用户手动指定。服务端依赖于Transport,
处理器以及命名服务器，而后续每种类型都可能提供多种实现，因此需要提供它们的工厂
类，并在此设置各种参数。启动服务大致需要以下几个步骤：
1. 继承服务接口，实现具体的服务处理逻辑。
2. 继承TransportFactoryIf接口，实现Transport的工厂类。
3. 继承ProcessorFactoryIf接口，实现处理器工厂类。
4. 继承NameServerFactoryIf接口，实现命令服务工厂类。
5. 利用创建的工厂类，初始化Server类型，再调用其run()方法启动服务。
详细示例参考 ：soce/crpc/sample/main.cpp

### 服务工厂类
用于创建具体的服务实例，同时可以为服务中的方法指定过滤条件。
```
class CortDemoFactory : public ServiceFactoryIf
{
public:
    virtual std::shared_ptr<ServiceIf> create()
    {
        CortDemo* p = new CortDemo();
        p->set_echo_filter("p <= 3 || p > 10");
        p->set_test_condcast_filter("p >= 3");
        return std::shared_ptr<ServiceIf>(p);
    }
};
```
示例中创建了CortDemo服务，并为echo()和test_condcast()方法添加了过滤条件。
对允许设置过滤条件的方法，在代码生成时都会生成一个设置过滤条件的方法，命名
规则为set_[method]_filter()。   
过滤条件是一个字符串，由||、&&以及小括号将多个规则串联起来。每个规则包括三部分，
第一部分是参数名称，第二部分为逻辑运算符，第三部分为值。每种类型的参数支持
不同的布尔运算，普通类型支持大于、小于、等于、不等于、小于等于、大于等于等运算，
字符串支持等于、不等于、以及正则表达式匹配运算。目前复合类型不支持任意布尔运算。   
Crpc目前支持一个特殊的内置变量_rand100，表示取【0，100）区间的随机数。可以设置
过滤条件为"_rand100 < 5 && p > 3"，表示条件为随机数小于5且参数p大于3。

默认情况下，所有请求会开一个协程来处理。如果服务不再依赖于其它服务，只是单纯的
CPU计算，则每个请求开一个协程会影响性能。Crpc允许用户重载get_svr_proc_type()
，返回服务处理类型。可选的类型包括：kSvrProcTypeSync，kSvrProcTypeAsync以及
kSvrProcTypeCort，其中kSvrProcTypeAsync还未实现。

### Transport工厂类
在crpc内部，会调用此工厂类为每个线程创建一个TransportIf对象，用于事件循环。
目前Transport只支持libevent，工厂类实现大致如下：
```
class LibeventTransportFactory : public TransportFactoryIf
{
public:
    virtual std::shared_ptr<soce::transport::TransportIf> create()
        {
            soce::transport::TransportLibevent* transport = new soce::transport::TransportLibevent;
            transport->init();
            return std::shared_ptr<soce::transport::TransportIf>(transport);
        }
};
```

### Processor工厂 类
在此工厂类中可以设置共享线程的数量，以及添加需要处理的服务类。
```
class ProcessorFactory : public ProcessorFactoryIf
{
public:
    virtual std::shared_ptr<Processor> create()
        {
            std::shared_ptr<Processor> processor(new Processor);
            // 设置共享线程数量
            processor->set_shared_thread_num(8);
            // 添加服务，最后一参数表示是否共享线程
            processor->add_service_factory(std::shared_ptr<ServiceFactoryIf>(new CortDemoFactory), true);
            return processor;
        }
};
```
### 命名服务器
目前命名服务器只实现了基于Zookeeper的命令服务，在初始化时需要指定Zookeeper
的地址。同时，在此服务中指定当前服务所依赖的服务。命令服务会从Zookeeper中拉
取所依赖的服务的地址，同时也会监控地址的变更。
```
class NameServerZkFactory : public NameServerFactoryIf
{
public:
    virtual std::shared_ptr<NameServerIf> create()
        {
            NameServerZk* nameserver = new NameServerZk;
            nameserver->init("127.0.0.1:2181", 4000);
            // 表示当前服务依赖于Echo服务
            nameserver->watch_service("CortDemo");
            return std::shared_ptr<NameServerIf>(nameserver);
        }
};
```

### 服务启动
在服务端启动时需要指定要监听的地址，如果IP或者端口指定为0，表示随机选择。
服务启动后，通过命名服务器的get_server_addr()方法，可以获取真实的分配的地址。
```
 Server server("0.0.0.0:0", //服务端地址，
                  std::shared_ptr<TransportFactoryIf>(new LibeventTransportFactory),
                  std::shared_ptr<ProcessorFactoryIf>(new ProcessorFactory),
                  std::shared_ptr<NameServerFactoryIf>(new NameServerZkFactory));
    server.run();
```

### 客户端调用
目前支持同步客户端和协程客户端。
同步客户端不具备负载均衡及条件功能，它需要与Proxy配合使用。
Proxy收到请求后会从请求中解析出服务名、方法名等信息，
如果它还没有监视此服务，则会从命名中心拉取相关信息，建立相应连接。
它会进行请求的分发以及响应的回传。
使用Proxy后，其它语言要实现同步客户端就非常方便了，只需要将请求序列化
后发送给Proxy即可。
同步客户端在使用以前需要调用init()方法为其指定Proxy地址。
Proxy启动示例：
```
CrpcProxyBuilder builer;
builer.set_service_addr("127.0.0.1:7890")
      .set_ns_addr("127.0.0.1:2181")
      .build()
      ->start();
```
同步客户端使用示例：
```
SyncDemoSyncClient client;
client.init("127.0.0.1", 7890);
string hello_resp;
soce::crpc::RpcStatus status = client.hello(hello_resp, "hello");
```

协程客户端只能在协程环境中使用。如果不更改服务处理类型，
默认情况下，所有的请求处理函数已经是在协程环境中。此时可按以下方式调用：
```
crpc::example::CortDemoCortClient clt;
int value = 0;
int rc = clt.echo(value, 100); // value用于接收返回结果
// 处理结果
```
客户端目前提供两个特性，设置超时和设置目的地。
- *clt.set_timeout()*   
设置本次调用的超时时间。
- *clt.set_target_addr("127.0.0.1:7890")*    
设置本次调用的目的地。当设置了目的地后，只会将请求发送给此服务，用于定向发送。
如果设置了目的地，而目的服务设置了过滤条件且当前请求不满足条件时，则会调用失败。


## 拦截器
一个完善的RPC框架会包括链路监控、熔断、限流等特性，Crpc提供了拦截器功能可用于
实现这些功能。   
拦截器有一定的优先级，当某个拦截器返回结果为非零时表示此次调用被拦截，此时将
直接返回，后续的拦截方法不会再被调用，服务端也不会再收到此请求。   
Crpc的拦截器提供三种类型的作用域，分别是全局拦截、服务级别的拦截
和方法级别的拦截。其中全局拦截优先级最高，其次是服务级别拦截，最低为方法级别拦截。
比如链路监控就可以采用全局拦截，它不会因为方法级别的拦截导致链路监控的方法不会
被调用到。   
针对每个级别还可以设置多个拦截器，每个拦截都有一个优先权，优先权值越大，越靠后执行。
一次完整的RPC调用包含请求链路和响应链路，拦截器也可以指定针对请求，还是针对响应。   
Crpc提供了宏SInterceptor来操作拦截器，它是一个非thread_local的单例，内部采用
了读写锁。主要接口如下：
- *add_before_itcptor()*   
添加请求链路的拦截器。
- *add_after_itcptor()*   
添加响应链路的拦截器。  
- *remove_itcptor()*
根据拦截器名称删除拦截器。     
除拦截函数原型外，它们的参数类型基本一样。   

| 参数 | 说明题 | 
| :------ |:------ |
| const std::string& name    | 拦截器名称 | 
| int priority               | 优先级 |
|BeforeFunc/AfterFunc func   |拦截方法|
|const std::string& service  |服务名|
|const std::string& method   | 方法名|
|bool client                 |用于客户端还是服务端|
当服务名和方法名都为空时表示全局拦截，当服务名不空，方法名为空时表示服务级别的拦截。
参数client表示当前拦截器用于客户端还是服务端，比如设置了一个用于客户端的前置
拦截器，则在客户端发起调用时就将进行拦截检查，如果检查不合法，则请求不会发送给
服务端。而如果设置的是一个用户服务端的前置拦截器，则会在服务端处理请求前被调用。
同样，后置拦截器用于拦截服务端发回的响应，以及客户端在收到响应前做一些处理。


## Roadmap
- 添加配置，通过配置减少初始化代码编写。
- 提供etcd注册中心。
- 提供链路监控、熔断、限流等特性。