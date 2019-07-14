/*
* Licensed to the Apache Software Foundation (ASF) under one
* or more contributor license agreements. See the NOTICE file
* distributed with this work for additional information
* regarding copyright ownership. The ASF licenses this file
* to you under the Apache License, Version 2.0 (the
* "License"); you may not use this file except in compliance
* with the License. You may obtain a copy of the License at
*
*   http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing,
* software distributed under the License is distributed on an
* "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
* KIND, either express or implied. See the License for the
* specific language governing permissions and limitations
* under the License.
*/

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include "cort-engine/cort-engine.h"

using namespace soce::cortengine;
using namespace std;

static void SampleWithoutYield()
{
    cout << "---------- SampleWithoutYield ----------" << endl;
    cout << "Test case in" << endl;
    CortEngine::CortId id = SCortEngine.create([]{
            cout << "Exec coroutine" << endl;
        });
    cout << "Create coroutine, id = " << id << endl;
    SCortEngine.resume(id);
    cout << "Test case out" << endl << endl;;
}

static void SampleWithYield()
{
    cout << "---------- SampleWithYield ----------" << endl;
    cout << "Test case in" << endl;
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
}

static void SampleWithYieldTimeout()
{
    cout << "---------- SampleWithYield ----------" << endl;
    cout << "Test case in" << endl;
    CortEngine::CortId id = SCortEngine.create([]{
            cout << "Exec coroutine, before yield" << endl;
            SCortEngine.yield(1000);
            if (SCortEngine.is_timeout()){
                cout << "Exec coroutine, after yield, timeout" << endl;
            }
            else{
                cout << "Exec coroutine, after yield, not timeout" << endl;
            }
        });
    cout << "Create Coroutine, id = " << id << endl;
    SCortEngine.resume(id);
    for (size_t i =0; i<2; ++i){
        cout << "Sleep 1 sec!" << endl;
        sleep(1);
        cout << "Check timeout" << endl;
        SCortEngine.check_timeout();
    }
    cout << "Test case out" << endl << endl;;
}

static void SampleCallStack()
{
    cout << "---------- SampleCallStack ----------" << endl;
    cout << "Test case in" << endl;
    CortEngine::CortId cid;
    CortEngine::CortId id = SCortEngine.create([&]{
            cout << "Exec caller, before call" << endl;
            cid = SCortEngine.create([&]{
                    cout << "Exec callee, before yield" << endl;
                    SCortEngine.yield();
                    cout << "Exec callee, after yield" << endl;
                });
            SCortEngine.resume(cid);
            cout << "Exec caller, after call" << endl;
        });
    cout << "Create Coroutine, id = " << id << endl;
    SCortEngine.resume(id);
    cout << "Resume callee, id = " << cid << endl;
    SCortEngine.resume(cid);
    cout << "Test case out" << endl << endl;;
}

static void SampleChannelBlockRead()
{
    cout << "---------- SampleChannel ----------" << endl;
    cout << "Test case in" << endl;
    Channel<int> chan;
    CortEngine::CortId id = SCortEngine.create([&chan]{
            cout << "Exec coroutine 1, before wait on channel" << endl;
            int msg = 0;
            chan.pop(msg);
            cout << "Exec coroutine 1, after wait on channel, msg = " << msg << endl;
        });
    cout << "Create Coroutine, id = " << id << endl;
    SCortEngine.resume(id);

    id = SCortEngine.create([&chan]{
            cout << "Exec coroutine 2, before wait on channel" << endl;
            int msg = 0;
            chan.pop(msg);
            cout << "Exec coroutine 2, after wait on channel, msg = " << msg << endl;
        });
    cout << "Create Coroutine, id = " << id << endl;
    SCortEngine.resume(id);
    int msg = 1;
    cout << "Push msg, msg = " << msg << endl;
    chan.push(msg);

    ++msg;
    cout << "Push msg, msg = " << msg << endl;
    chan.push(msg);
    cout << "Test case out" << endl << endl;;
}

static void SampleChannelBlockWrite()
{
    cout << "---------- SampleChannelBlock ----------" << endl;
    cout << "Test case in" << endl;
    Channel<int> chan(0, 0, 1);
    CortEngine::CortId id = SCortEngine.create([&chan]{
            cout << "Exec coroutine 1, before push 1" << endl;
            chan.push(1);
            cout << "Exec coroutine 1, after push 1" << endl;
        });
    cout << "Create Coroutine, id = " << id << endl;
    SCortEngine.resume(id);

    id = SCortEngine.create([&chan]{
            cout << "Exec coroutine 1, before push 2" << endl;
            chan.push(2);
            cout << "Exec coroutine 1, after push 2" << endl;
        });
    cout << "Create Coroutine, id = " << id << endl;
    SCortEngine.resume(id);

    int msg = 0;
    cout << "Try Pop msg" << endl;
    chan.pop(msg);
    cout << "Pop msg = " << msg << endl;
    cout << "Try Pop msg" << endl;
    chan.pop(msg);
    cout << "Pop msg = " << msg << endl;

    cout << "Test case out " << id << endl << endl;;
}

static void SampleWaitOnFd()
{
    cout << "---------- SampleWaitOnFd ----------" << endl;
    int fd = socket(AF_INET, SOCK_STREAM, 0);

    cout << "Test case in" << endl;
    CortEngine::CortId id = SCortEngine.create([&fd]{
            cout << "Exec Coroutine, before wait on fd" << endl;
            SCortEngine.wait(fd);
            cout << "Exec Coroutine, after wait on fd" << endl;
        });
    cout << "Create Coroutine, id = " << id << endl;
    SCortEngine.resume(id);
    cout << "Wakeup" << endl;
    SCortEngine.wakeup(fd);
    cout << "Test case out" << endl << endl;
}

static void SampleHook()
{
    cout << "---------- SampleHook ----------" << endl;
    int fd[2];
    pipe(fd);

    cout << "Test case in" << endl;
    int hook_fd = 0;
    SCortEngine.set_hook_callback([&](int fd){
            hook_fd = fd;
        });
    CortEngine::CortId id = SCortEngine.create([&fd]{
            SCortEngine.enable_hook(1000);
            cout << "Exec Coroutine, before read" << endl;
            char buff[10] = {0};
            read(fd[0], buff, 10);
            cout << "Exec Coroutine, after read, msg = " << buff << endl;
        });
    cout << "Create Coroutine, id = " << id << endl;
    SCortEngine.resume(id);
    string msg("abcd");
    cout << "Write msg = " << msg << endl;
    write(fd[1], msg.c_str(), msg.size());
    cout << "WakeUp" << endl;
    SCortEngine.wakeup(hook_fd);
    cout << "Test case out" << endl << endl;;
}

int main()
{
    SampleWithoutYield();
    SampleWithYield();
    SampleWithYieldTimeout();
    SampleCallStack();
    SampleChannelBlockRead();
    SampleChannelBlockWrite();
    SampleWaitOnFd();
    SampleHook();
    return 0;
}
