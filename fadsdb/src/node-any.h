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

#ifndef _SOCE_NODE_ANY_K8diw6U7pb_H_
#define _SOCE_NODE_ANY_K8diw6U7pb_H_

#include <memory>
#include "node-if.h"

#include "proto/binary-proto.h"

namespace soce{
namespace fadsdb{

    class NodeAny : public NodeIf
    {
    public:
        static size_t sbuff_size;
        static char* sbuff;

        ~NodeAny();

        virtual std::shared_ptr<NodeIf> clone();
        virtual void reset();
        virtual SoceDataType get_type();
        virtual uint32_t serialize(soce::proto::ProtoIf* proto, bool with_type);
        virtual uint32_t deserialize(soce::proto::ProtoIf* proto, bool with_type);

    private:
        void reset(uint32_t cap);
        uint32_t copyInt32(soce::proto::ProtoIf* proto, soce::proto::BinaryProto& bp);
        uint32_t copyInt64(soce::proto::ProtoIf* proto, soce::proto::BinaryProto& bp);
        uint32_t copyFloat(soce::proto::ProtoIf* proto, soce::proto::BinaryProto& bp);
        uint32_t copyDouble(soce::proto::ProtoIf* proto, soce::proto::BinaryProto& bp);
        uint32_t copyString(soce::proto::ProtoIf* proto, soce::proto::BinaryProto& bp);

    private:
        size_t len_ = 0;
        size_t cap_ = 0;
        SoceDataType type_;
        char* data_ = NULL;
    };

}
}

#endif
