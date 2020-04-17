# SOCE(Service Oriented Communication Environment)

# License
Licensed to the Apache Software Foundation (ASF) under one
or more contributor license agreements. See the NOTICE file
distributed with this work for additional information
regarding copyright ownership. The ASF licenses this file
to you under the Apache License, Version 2.0 (the
"License"); you may not use this file except in compliance
with the License. You may obtain a copy of the License at

  http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing,
software distributed under the License is distributed on an
"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
KIND, either express or implied. See the License for the
specific language governing permissions and limitations
under the License.

# 简介
SOCE是Service Oriented Communication Environment的简称，提供基于
基于微服务编程所需要的基本工具，目前主要提供了RPC框架crpc、KV数据库fadsdb、
协程库cort-engine、redis客户端、zookeeper客户端、日志库log4rel
以及一些位于utils目录中的辅助类。

# 层级结构
- ***soce/***   
   - ***cort-engine/***  
   使用的是linux提供的makecontext等接口封装的协程库，支持Hook、Channel及
   协程连接池等。通过协程连接池，可以方便地为一些只支持同步操作的库提供异步访问机制。
   这套接口性能不是很好，但比较简单。使用它们仅是为了方便，后续可能会更换。
   [详细文档](./cort-engine/README.md)

   - ***crpc/***  
   RPC框架，主要特点是允许指定条件，方便做灰度发布及共享测试环境。服务在启动时
   可以为方法指定条件，比如设置只接收参数p的值大于10的请求。还可设置流量，
   比如只导入5%的流量。crpc还支持添加全局、服务级别以及方法级别的拦截函数，
   以实现诸如链路监控等扩展特性。
   与Grpc和Thrift类似，crpc在使用时也需要先编写IDL文件。目前crpc支持同步客户端及
   协程客户端。可参照soce-redis的异步示例，使用协程连接池，可方便地将同步接口转为异步接口。
   crpc除了支持普通的调用方式外，还支持无条件广播和条件广播，允许同时将消息发送给多个服务端。
   目前支持服务之间直连，通过Proxy代理中转和ServiceMesh三种方案。
   [详细文档](./crpc/README.md)

   - ***fads/***  
    序列化方案，支持向后兼容、向前兼容以及允许根据索引路径直接访问成员内容等特点。
    crpc及fadsdb都使用fads 序列化方案。
    [详细文档](./fads/README.md)

   - ***fadsdb/***  
   KV数据库，与Reids相比，主要特点是操作时允许带条件，即只有满足条件时才进行操作。
   fadsdb需要指定数据的Schema。它支持丰富的数据类型，除基本类型外还包括list、set、map
   等复合类型，且这些复杂类型允许嵌套。它允许用户自定义方法，比如为整形添加取模的方法，
   以便扩展。在查询时，允许只返回整个Schema中的指定的域。
   [详细文档](./fadsdb/README.md)

   - ***log4rel/***   
   日志库，支持同步日志和异步日志，多种后端，允许按字符串格式打印和json格式打印。
   在使用时可以为每条日志设置一个key，key以点号分隔成多个域，比如module.key1.key2，
   允许设置黑白名单过滤器，只输出特定日志。过滤器的规则支持+和*两个通配符，
   只有满足条件的日志才会被序列化，从而可以实现在调试时只打印指定模块的日志等功能。
   同时还支持基于内容的过滤，基于内容的过滤通过正则表达式过滤， 过滤内容为序列化后的内容。
   基于key的过滤在序列化之前，具有更高的效率，用于粗过滤。
   基于内容的过滤在序列化之后，需要更大的开销，不过更精确和灵活。过滤条件可动态更改，
   当只能在线上环境打开日志时，可减少日志的输出，避免大量日志影响性能。
   [详细文档](./log4rel/README.md)

   - ***soce-redis/***   
   Redis客户端，已被Redis官网收录。实现以统一的接口访问单机、哨兵以及集群模式。
   支持批量和非批量操作， 对外只暴露出几个简单，简单易用。通过协程客户端，
   可以方便地参照示例改造成异步访问。
   [详细文档](./soce-redis/docs/README-cn.md)

   - ***soce-zk/***   
   对zookeeper c语言客户端的封装，使其更易使用，同时解决临时节点因网络波动而丢失的问题，
   以及立即重启服务可能无法创建同名临时节点等问题。
   [详细文档](./soce-zk/README.md)

   - ***transport/***   
   对网络库的封装，屏蔽网络库，从而可以随时更换底层网络库。
   [详细文档](./transport/README.md)

   - ***utils/***   
   一些其它模块可能用到的工具类，主要包括分发队列、阻塞队列、双缓充队列、条件表达式、snowflake算法、索引列表、分组列表以及读写锁等。
   [详细文档](./utils/README.md)
   
# 编译
soce的源码按模块组织，每个模块都可独立运行及测试。在每个模块下都有examples目录，
用以演示各模块的基本用法。编译时进入各模块的examples目录，执行以下命令：
```
mkdir build
cd build
cmake ..
make
```

# 捐赠持股、代码持股、捐赠出书计划
如果你觉得本项目对你有帮忙，欢迎进行捐赠，你的捐赠将是这个项目持续改进的保障。
如果捐赠达到一定的数量，作者将自费出书，并对满足捐赠条件的捐赠者回赠书籍。
[详细说明](./DONATE.md)

# 联系方式
socebook@163.com  
