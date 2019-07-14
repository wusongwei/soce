#   cort-engine
cort-engine是利用linux提供的makecontext/getcontext/getcontext/swapcontext 
接口簇实现的一个协程库。由于SOCE提倡无锁编程、线程与线程之间通过分发队列来通信，
因此cort-engine提供了宏SCortEngine来访问CortEngine实例，它实际上是一个
thread_local类型的单例。   

##   基本功能
- *CortId create(CortFunc func)*   
创建协程，只创建并未执行。
- *CortId create(CortFunc func, uint32_t timeout)*   
创建协程，打开Hook功能，并设置Hook后的超时时间。
- *int resume(CortId id)*   
启动/恢复协程。
- *int yield(uint32_t timeout)*   
放弃执行，可以再通过 *resume()* 方法调度回来。超时时间以毫秒为单位，
如果时间设置为0，则表示永不超时。
- *bool is_timeout()*   
用于协程重新被调度后，检查是否是因为超时返回。
- *CortId get_cur_cort_id() const*   
返回当前协程ID。
- *void check_timeout()*   
用于协程超时检查，需要在外部周期性调用。可利用transport的定时器来周期调用。

##   Hook
cort-engine提供对*read()*方法和*close()*方法的Hook功能，若需要Hook其它系统
函数，只需要依照这两个方法自行添加即可。
cort-engine默认创建的协程不打开此功能。当打开此功能后，应用程序在调用*read()*
时就会被切换出去。一般应该在对应在文件描述符上有读事件时再将此协程调度回来。
cort-engine不提供网络库，而这些工作都是网络库所负责的，所以cort-engine提供了
接口用于设置Hook事件发生时的回调方法，用户可在此回调中执行与网络库关联的工作。
- *int enable_hook(uint32_t timeout)*   
当前协程打开Hook功能。
- *int disable_hook()*   
关闭当前协程的Hook功能。
- *bool is_enable_hook() const*   
检查当前协程是否允许Hook。
- *void set_hook_callback(HookCallback hook_cb)*
设置Hook发生时的回调方法，回调原型为*void(int)*，参数为正在操作的文件描述符。
用户可在此回调中使用网络库来监听读事件，然后调用*wakeup()*方法唤醒阻塞于此
文件描述符上的协程。

## Channel
Channel用于协程间通信，同样它也只支持单线程中的协程通信。Channel在初始化时可
指定缓存大小，当缓存为空时，消费者消费时会被yield，有生产者生产数据后被resume；
当缓存满时，生产者生产时会被yield，当消费者消费数据缓存未满时被resume。  
- *int push(const T& mail)*   
- *int push(T&& mail)*   
用于生产消息，当缓存满时，当前协程被yield。
- *int pop(T& mail)*   
消费消息，当缓存为空时，当前协程被yield。
- *size_t size()*   
缓存中消息条数。
- *void reset()*   
清空缓存，唤醒所有被阻塞的协程。

## 示例   

``` 
    // 演示创建协程，在执行过程中yield，之后再被resume。
    CortEngine::CortId id = SCortEngine.create([]{
            cout << "Exec coroutine, before yield" << endl;
            SCortEngine.yield();
            cout << "Exec coroutine, after yield" << endl;
        });
    cout << "Create Coroutine, id = " << id << endl;
    SCortEngine.resume(id);
    cout << "Resume again, id = " << id << endl;
    SCortEngine.resume(id);
    cout << "Test case out" << endl << endl;;
```
```
    // 演示创建协程，执行过程中期望从Channel获取消息。
    Channel<int> chan;
    CortEngine::CortId id = SCortEngine.create([&chan]{
            cout << "Exec coroutine 1, before wait on channel" << endl;
            int msg = 0;
            chan.pop(msg);
            cout << "Exec coroutine 1, after wait on channel, msg = " << msg << endl;
        });
    cout << "Create Coroutine, id = " << id << endl;
    SCortEngine.resume(id);

    int msg = 1;
    cout << "Push msg, msg = " << msg << endl;
    chan.push(msg);
```
更多示例参见：soce/cort-engine/sample/sample.cpp

## Roadmap
- 支持boost协程库。
- 添加对其它系统函数的Hook。