# soce-redis
soce-redis is a C++ client, which can be used to access the sever
with the same interface, no matter what mode(single, sentinel, cluster)
 the server use. It also support pipeline and async.
 [中文版点这里](./docs/README-cn.md)
 
 # initialization
 A SoceRedis instance must be initialized with the configuration which
  is a string.
 - single mode   
 *passwd@ip:port?timeout=1000*
 
 - sentinel mode   
 *passwd@ip:port,ip:port?master=[master-name]&timeout=1000*
 
 - cluster mode   
 *passwd@ip:port,ip:port?cluster=true&timeout=1000*
 
 The password should be set to "none" if it is not necessary.
 timeout is optional, it is used for read/write/connect. 
 ```
 string conf = "none@127.0.0.1:6379";
 SoceRedis clt;
 clt.init(conf);
 ```

# SoceRedis API
 soce-redis support single command and pipeline。And you can access
 the result by iterator. There are only a few interfaces, it's easy to use.
 - *int init(const std::string& conf)*   
 @see initialization.
 
 - *int exec(Fmt fmt)*    
 Execute single command.
 
 - *void clear()*     
 Clear the cached command for pipeline.
 
 - *int append(Fmt fmt)*   
 Add pipeline command.
 
 - *int pipeline()*   
 Execute pipeline commands.
 
 - *iterator begin()*   
 Return the beginning iterator of the result.
 
 - *iterator end()*   
 Return the ending iterator of the result.
 
 
 ## iterator
 iterator is used to access the result. It point to a SoceRedisReply instance.
 You can used the interface of SoceRedisReply to access the result.
 - *int get_type(SoceRedisType& type)*
 - *int get_type(SoceRedisType& type)*
 - *int get_str(std::string& val)*
 - *int get_array_size(int& size)*
 - *int get_array_begin(SoceRedisIterator& iter)*
 - *int get_array_end(SoceRedisIterator& iter)*
 - *int get_int(int& val)*
 - *int get_status(std::string& val)*
 - *int get_error(std::string& val)*
 
 ## helper macros
 The type of parameter used by exec() is "Fmt" which is defined as below:
 ```
 using Fmt = std::function<void(Param&)>;
 int exec(Fmt fmt);
 ```
Macro soce_redis_docmd and soce_redis_append are supplied to simplify the 
usage of SoceRedis.
 
 ## sample : single command
 ```
 soce_redis_docmd(clt, "set" << "a" << "b");
 string val;
 clt.begin()->get_status(val); // get the result
 ```
 
 ## sample : pipeline command
 ```
 soce_redis_append(clt, "set" << "a" << "b");
 soce_redis_append(clt, "get" << "a");
 clt.pipeline();
 for (auto& result : clt){
     // handle the results
 }
 ```
 
 # async
 It's easy to implement async-access by coroutine.   
 @see soce-redis/sample/sample-async
 
 # More samples
 @see soce-redis/sample。

