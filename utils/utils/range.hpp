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

#ifndef _SOCE_RANGE_meYVevL3NJ_H_
#define _SOCE_RANGE_meYVevL3NJ_H_

#include <type_traits>
#include <iterator>

namespace soce{
namespace utils{

    template<class T>
    class BaseRangeIterator : public std::iterator<std::forward_iterator_tag, T>
    {
    public:
        BaseRangeIterator(T val, T step)
            : val_(val), step_(step)
            {
            }

        BaseRangeIterator(const BaseRangeIterator& rhs)
            {
                val_ = rhs.val_;
            }

        BaseRangeIterator& operator=(const BaseRangeIterator& rhs)
            {
                if (this == &rhs){
                    return *this;
                }
                val_ = rhs.val_;
                step_ = rhs.step_;
                return *this;
            }

        BaseRangeIterator& operator++()
            {
                val_ += step_;
                return *this;
            }

        BaseRangeIterator operator++(int)
            {
                BaseRangeIterator tmp(val_, step_);
                operator++();
                return tmp;
            }

        bool operator==(const BaseRangeIterator& rhs)
            {
                return (val_ == rhs.val_) ? true : false;
            }

        bool operator!=(const BaseRangeIterator& rhs)
            {
                return (val_ != rhs.val_) ? true : false;
            }

        T& operator*()
            {
                return val_;
            }

    protected:
        T val_ = 0;
        T step_ = 1;
    };

    template <class T>
    class RangeIterator : public BaseRangeIterator<T>
    {
    public:
        RangeIterator(T val, T step)
            : BaseRangeIterator<T>(val, step)
            {
            }
    };

    template <>
    class RangeIterator<float> : public BaseRangeIterator<float>
    {
    public:
        RangeIterator<float>(float val, float step)
        : BaseRangeIterator<float>(val, step)
            {
            }

        bool operator!=(const RangeIterator& rhs)
            {
                return ((rhs.val_ - val_) > 0.01) ? true : false;
            }
    };

    template <>
    class RangeIterator<double> : public BaseRangeIterator<double>
    {
    public:
        RangeIterator<double>(double val, double step)
        : BaseRangeIterator<double>(val, step)
            {
            }

        bool operator!=(const RangeIterator& rhs)
            {
                return ((rhs.val_ - val_) > 0.01) ? true : false;
            }
    };

    template <class T>
    class Range
    {
    public:
        typedef RangeIterator<T> iterator;

    public:
        Range(T beg, T end, T step = 1)
            : beg_(beg), end_(end), step_(step)
            {
            }

        iterator begin()
            {
                return RangeIterator<T>(beg_, step_);
            }

        iterator end()
            {
                return RangeIterator<T>(end_, step_);
            }

    private:
        T beg_ = 0;
        T end_ = 0;
        T step_ = 1;
    };

} // namespace utils
} // namespace soce

#endif
