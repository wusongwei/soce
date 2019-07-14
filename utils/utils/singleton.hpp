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

#ifndef _SOCE_SINGLETON_H_97Uenza4KM_H_
#define _SOCE_SINGLETON_H_97Uenza4KM_H_

namespace soce{
namespace utils{

    template <typename T>
    class SoceSingleton
    {
    private:
        struct ObjectCreator
        {
            ObjectCreator() { SoceSingleton<T>::get_instance(); }
            inline void do_nothing() const { }
        };
        static ObjectCreator create_object;

    public:
        SoceSingleton() = delete;
        SoceSingleton(const SoceSingleton&) = delete;
        SoceSingleton& operator = (const SoceSingleton&) = delete;

        static T& get_instance()
            {
                static T instance;
                create_object.do_nothing();
                return instance;
            }
    };

    template <typename T>
    class SoceSingletonThreadLocal
    {
    private:
        struct ObjectCreator
        {
            ObjectCreator() { SoceSingletonThreadLocal<T>::get_instance(); }
            inline void do_nothing() const { }
        };
        static ObjectCreator create_object;

    public:
        SoceSingletonThreadLocal() = delete;
        SoceSingletonThreadLocal(const SoceSingletonThreadLocal&) = delete;
        SoceSingletonThreadLocal& operator = (const SoceSingletonThreadLocal&) = delete;

        static T& get_instance()
            {
                static thread_local T instance;
                create_object.do_nothing();
                return instance;
            }
    };

template <typename T>
typename SoceSingleton<T>::ObjectCreator
SoceSingleton<T>::create_object;

template <typename T>
typename SoceSingletonThreadLocal<T>::ObjectCreator
SoceSingletonThreadLocal<T>::create_object;

}} //namespace

#endif
