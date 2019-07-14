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

#ifndef _TYPE_FACTORY_20161226_wVQ3Z5naXz_H_
#define _TYPE_FACTORY_20161226_wVQ3Z5naXz_H_

#include <memory>
#include "utils/factory.hpp"
#include "fads.h"
#include "types-def-cpp.h"
#include "service-creater-cpp.h"

namespace soce{
namespace compiler{

#define DEF_FACTORY(TYPE, SCOPE)                                    \
    class Factory##TYPE                                             \
    {                                                               \
    public:                                                         \
        template<typename... Args>                                  \
            static std::shared_ptr<SCOPE::Type##TYPE>               \
            create(Args&&... args)                                  \
        {                                                           \
            std::shared_ptr<SCOPE::Type##TYPE> elem;                \
                                                                    \
            if (SOpMgr.get_lang() == "cpp"){                         \
                elem = soce::utils::SoceFactory<Type##TYPE####Cpp>:: \
                    create(std::forward<Args>(args)...);             \
            }                                                       \
                                                                    \
            return elem;                                            \
        }                                                           \
    };

    DEF_FACTORY(Enum, soce::compiler)
    DEF_FACTORY(List, soce::compiler)
    DEF_FACTORY(Set, soce::compiler)
    DEF_FACTORY(Map, soce::compiler)
    DEF_FACTORY(Struct, soce::compiler)

    DEF_FACTORY(StructElemPrime, TypeStruct)
    DEF_FACTORY(StructElemCustom, TypeStruct)
    DEF_FACTORY(StructElemList, TypeStruct)
    DEF_FACTORY(StructElemMap, TypeStruct)
    DEF_FACTORY(StructElemSet, TypeStruct)
    DEF_FACTORY(StructElemByte, TypeStruct)

#undef DEF_FACTORY

    class FactoryRef
    {
    public:
        static std::shared_ptr<ReferenceIf> create()
            {
                std::shared_ptr<ReferenceIf> instance;
                if (SOpMgr.get_lang() == "cpp"){
                    instance = soce::utils::SoceFactory<ReferenceCpp>::create();
                }

                return instance;
            }
    };

    class FactoryService
    {
    public:
        static std::shared_ptr<IServiceCreater> create()
            {
                std::shared_ptr<IServiceCreater> instance;
                if (SOpMgr.get_lang() == "cpp"){
                    instance = soce::utils::SoceFactory<ServiceCreaterCpp>::create();
                }

                return instance;
            }
    };

} // namespace compiler
} // namespace soce
#endif
