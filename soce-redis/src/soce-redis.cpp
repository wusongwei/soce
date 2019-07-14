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

#include "soce-redis/soce-redis.h"
#include "soce-redis-impl.h"
#include "utils/factory.hpp"

using std::string;
using soce::utils::SoceFactory;

namespace soce{
namespace redis{

    SoceRedisIterator::SoceRedisIterator(std::shared_ptr<SoceRedisReply> reply)
        : reply_(reply)
    {
    }

    SoceRedisIterator::SoceRedisIterator(const SoceRedisIterator& rhs)
        : reply_(rhs.reply_)
    {
    }

    SoceRedisIterator& SoceRedisIterator::operator=(const SoceRedisIterator& rhs)
    {
        if (this != &rhs){
            reply_ = rhs.reply_;
        }

        return *this;
    }

    SoceRedisIterator& SoceRedisIterator::operator++()
    {
        reply_->operator++();
        return *this;
    }

    SoceRedisIterator SoceRedisIterator::operator++(int)
    {
        SoceRedisIterator tmp(*this);
        reply_->operator++();
        return tmp;
    }

    bool SoceRedisIterator::operator==(const SoceRedisIterator& rhs)
    {
        return (*reply_ == *rhs.reply_) ? true : false;
    }

    bool SoceRedisIterator::operator!=(const SoceRedisIterator& rhs)
    {
        return (*reply_ != *rhs.reply_) ? true : false;
    }

    SoceRedisReply& SoceRedisIterator::operator*()
    {
        return *reply_;
    }

    SoceRedisReply* SoceRedisIterator::operator->()
    {
        return reply_.get();
    }

    SoceRedisReply::SoceRedisReply(std::shared_ptr<SoceRedisReplyImpl> impl)
        : impl_(impl)
    {
    }

    SoceRedisReply::SoceRedisReply(const SoceRedisReply& rhs)
        : impl_(rhs.impl_)
    {
    }

    SoceRedisReply& SoceRedisReply::operator=(const SoceRedisReply& rhs)
    {
        if (this != &rhs){
            impl_ = rhs.impl_;
        }
        return *this;
    }

    SoceRedisReply& SoceRedisReply::operator++()
    {
        impl_->operator++();
        return *this;
    }

    SoceRedisReply SoceRedisReply::operator++(int)
    {
        SoceRedisReply tmp(*this);
        impl_->operator++(0);
        return tmp;
    }

    bool SoceRedisReply::operator==(const SoceRedisReply& rhs)
    {
        return (*impl_ == *rhs.impl_) ? true : false;
    }

    bool SoceRedisReply::operator!=(const SoceRedisReply& rhs)
    {
        return (*impl_ != *rhs.impl_) ? true : false;
    }

    int SoceRedisReply::get_type(SoceRedisType& type)
    {
        return impl_->get_type(type);
    }

    int SoceRedisReply::get_str(std::string& val)
    {
        return impl_->get_str(val);
    }

    int SoceRedisReply::get_array_size(int& size)
    {
        return impl_->get_array_size(size);
    }

    int SoceRedisReply::get_array_begin(SoceRedisIterator& iter)
    {
        return impl_->get_array_begin(iter);
    }

    int SoceRedisReply::get_array_end(SoceRedisIterator& iter)
    {
        return impl_->get_array_end(iter);
    }

    int SoceRedisReply::get_int(int& val)
    {
        return impl_->get_int(val);
    }

    int SoceRedisReply::get_status(std::string& val)
    {
        return impl_->get_status(val);
    }

    int SoceRedisReply::get_error(std::string& val)
    {
        return impl_->get_error(val);
    }

#if defined(SOCE_USE_COROUTINE)
    int SoceRedis::init(const string& conf, const SoceRedis::CortEnv& env)
    {
        impl_ = SoceFactory<SoceRedisImpl>::create();
        return impl_->init(conf, env);
    }

    void SoceRedis::keepalive()
    {
        impl_->keepalive();
    }
#else
    int SoceRedis::init(const std::string& conf)
    {
        impl_ = SoceFactory<SoceRedisImpl>::create();
        return impl_->init(conf);
    }
#endif

    int SoceRedis::exec(SoceRedis::Fmt fmt)
    {
        return impl_->exec(fmt);
    }

    void SoceRedis::clear()
    {
        impl_->clear();
    }

    int SoceRedis::append(SoceRedis::Fmt fmt)
    {
        return impl_->append(fmt);
    }

    int SoceRedis::pipeline()
    {
        return impl_->do_cmd();
    }

    SoceRedis::iterator SoceRedis::begin()
    {
        return impl_->begin();
    }

    SoceRedis::iterator SoceRedis::end()
    {
        return impl_->end();
    }

}} // end namespace
