## Fadsdb
Fadsdb是一要使用fads作为序列化方案的KV数据库。与Redis类似，fadsdb也提供了丰富
的数据结构。与Redis不同的是，fadsdb具有表结构，在使用时需要根据schema创建表。
同时，fadsdb提供条件操作，包括条件查询和条件更新。

## 拟解决问题
Redis中基本是以KV的形式存在，V的类型主要就是字符串。当有多项时，需要使用多个
KV对，或者序列化后存入Redis。当遇到需要根据某个键的值来决定对另外一个键的操作
时，为保证原子性，只能使用Lua脚本。若序列化后再存在Redis，还需要一种序列化方案。
Fadsdb希望不借助外力自身实现相关功能。它具有表结构，允许原子性地操作表中的一个
或多个字段。拥有表结构的缺点是字段只能增加，不能减少。不过Redis由于不具有表结构，
当Redis中数据比较多时，依然需要借助外部文档来说明各个键的含义。

## 关键字
Fadsdb采用fads的语法，增加了关键字table用于表的创建。
```
struct Worker
{
    string name;
    i32 age;
}

table company
{
    string CEO;
    Worker worker;
}
```
以上schema定义了一张表，表名为company，包含两个成员。成员CEO类型为string类型，
成员worker类型为Worker。Worker是前面定义的一个struct类型，它又包含一个string
类型的成员name和一个i32类型的成员age。   
fads提供的require等限定词可用于限定哪些项是必须的，相当于SQL中的NOT NULL。

## 操作类型
Fadsdb支持创建、插入、删除、更新、选择、选择更新等操作。每种操作都定义了一个
fads类型的模式，在操作时只需创建一个相应的操作实例，设置对应字段的值，序列化，
最后将序列化后的数据交给DbCore的do_sql()函数处理即可。  
各种操作的定义见：*soce/fadsdb/internal-idl/dbcmd.fads*

## 线程模式
Fadsdb允许使用多线程以提高性能。当创建一个表时，会在每个线程都创建相应的表。
当有请求到来时，会针对key计算其散列值，再将消息发送到相应的线程执行。Fadsdb
会开启一个单独的线程来处理响应，默认只是打印。可以设置响应处理函数以更改行为。

## 初始化
Fadsdb目前还未提供网络功能，只能当成内容模块使用。Fadsdb在使用前需要创建一个
Dbcore，设置工作线程数，以及响应处理函数。
```
void RespHandler(const soce::fadsdb::DbCore::RespData& resp)
{
}
 
DbCore dbcore;
dbcore.init(4); // 使用4个线程
dbcore.set_resp_handler(std::bind(RespHandler, std::placeholders::_1));
```

## 创建表
创建一个FadsDbCreate实例，设置相应的字段，序列化后调用Dbcore的do_sql()方法即可。
```
FadsDbCreate crt;
crt.mutable_header().set_type(kCmdCreate);
crt.mutable_header().set_id(1);
// 设置表的schema
crt.set_schema("struct Worker{string name; i32 age;} table company{string CEO; Worker worker;}");
soce::proto::BinaryProto bp;
crt.serialize(&bp);
dbcore.do_sql(req_id++, bp.data(), bp.size());
```
上面可以看有两个id，FadsDbCreate的header中有一个ID，Docore的do_sql()中有一个
ID。这是因为前一个ID是在提供网络功能后，由客户端设置的。当有多个客户端时，它们
可能同时使用了相同的ID。do_sql()方法中使用的是服务端的ID，在提供网络功能时，会
在它们之间做一个映射。

## 插入数据
```
soce::proto::BinaryProto bp;
FadsDbInsert ist;
ist.mutable_header().set_type(kCmdInsert);
ist.mutable_header().set_id(2);
ist.set_table("company"); // 表名
ist.set_key("apple");     // 键
ist.set_data(data); // data为根据表结构序列化后的内容

ist.serialize(&bp);
dbcore.do_sql(req_id++, bp.data(), bp.size());
```

## 更新数据
更新时允许同时设置多个更新函数，以逗号隔开。同时也允许设置更新条件，只有满足
条件时才做更新操作。   
更新函数可以使用内置函数，也可以自定义更新函数。
```
soce::proto::BinaryProto bp;
FadsDbUpdate up;
up.mutable_header().set_type(kCmdUpdate);
up.mutable_header().set_id(3);
up.set_table("company");   // 表名
up.set_key("apple");       // 键
up.set_cmd("mod(worker.age, 4)"); // 更新函数
up.set_filters("CEO == jobs");    // 更新条件

up.serialize(&bp);
dbcore.do_sql(req_id++, bp.data(), bp.size());
```

## 选择
选择时可以选择所有域，此时使用*号。也可以选择部分域，当选择部分域时，域名用
逗号隔开。选择还支持对指定的域进行函数操作，比如返回某个域的最大值等。选择函数
也可以使用内置的或者自定义的。
```
soce::proto::BinaryProto bp;
FadsDbSelect sel;
sel.mutable_header().set_type(kCmdSelect);
sel.mutable_header().set_id(4);
sel.set_table("company");
sel.set_key("apple");
sel.set_fields("*"); // 选择所有域
sel.set_filters("CEO == jobs"); 设置过滤条件

sel.serialize(&bp);
dbcore.do_sql(req_id++, bp.data(), bp.size());
```

## 自定义更新/选择操作
Fadsdb允许用户自定义更新/选择函数，以扩充功能。更新操作的函数原型为：
```
int func(const std::vector<std::string>& params, NodeIf* node)
```
其中参数params用于记录参数，node表示执行当前操作的节点。比如更新函数为：
```
mod(worker.age, 4)
```
则参数列表中只包含一项，参数为4，"worker.age"将被用于查找执行此操作的内部
节点。Fadsdb也允许定义多参数的更新函数。   
Fadsdb提供了Plugin类来管理用户自定义的函数，对外的主要接口包括：
- *register_select_func()*  
注册选择函数。
- *unregister_select_func()*   
根据名称取消自定义选择函数。
- *register_update_func()*   
注册更新函数。
- *unregister_update_func()*   
根据名称取消自定义更新函数。
Fadsdb提供了宏SPlugin来以单例模式访问Plugin实例。以下语句为i32类型添加mod
方法，对应的执行函数为ModI32。
```
int ModI32(const std::vector<std::string>& params, NodeIf* node)
{
    // ...
}

SPlugin.register_update_func(soce::proto::SoceDataType::kTypeInt32,
                             "mod",
                             ModI32);
```

## 表结构查询
- *std::string get_tables();*  
- *std::string get_custom_type();*
- *std::string get_table_schema();*
- *std::string get_table_schema(const std::string& name);*
- *std::string get_custom_type_schema();*
- *std::string get_custom_type_schema(const std::string& name);*

## Roadmap
- 添加网络访问接口。
- 提供批量命令接口。
- 支持批量命令事务。
- 采用raft协议，添加集群功能。
- 添加持久化存储功能。
- 内置一些table，以兼容Redis协议。