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

#ifndef _SOCE_PROTO_DYNAMIC_NODE_wcLZJnKdms_H_
#define _SOCE_PROTO_DYNAMIC_NODE_wcLZJnKdms_H_

#include <memory>
#include <string>
#include <vector>
#include <unordered_set>
#include "proto/proto-if.h"
#include "proto/binary-proto.h"

namespace soce {
namespace proto {

    class DynamicNode
    {
    public:
        static std::string get_type_str(SoceDataType type);
        static std::shared_ptr<DynamicNode> create(SoceDataType type);

    public:
        DynamicNode(SoceDataType type);
        DynamicNode(SoceDataType type, const std::string& name);
        virtual ~DynamicNode();
        inline void set_type(SoceDataType type) {type_ = type;}
        inline SoceDataType get_type() const {return type_;}
        inline void set_name(const std::string& name){name_ = name;}
        inline const std::string& get_name() const {return name_;}
        inline void set_required(bool required) {required_ = required;}
        inline bool get_required() const {return required_;}
        inline void set_setted(bool setted) {setted_ = setted;}
        inline bool get_setted() const {return setted_;}
        std::string to_json();
        virtual std::string get_value() = 0;
        virtual uint32_t serialize(ProtoIf* proto) = 0;
        virtual uint32_t deserialize(ProtoIf* proto, bool with_schema) = 0;
        virtual void reset() = 0;
        virtual std::shared_ptr<DynamicNode> clone() = 0;

    protected:
        SoceDataType type_;
        std::string name_;
        bool required_ = false;
        bool setted_ = false;
    };

    class DynamicNodeNull : public DynamicNode
    {
    public:
        DynamicNodeNull();
        DynamicNodeNull(const std::string& name);
        virtual std::string get_value();
        virtual uint32_t serialize(ProtoIf* proto);
        virtual uint32_t deserialize(ProtoIf* proto, bool with_schema);
        virtual void reset();
        virtual std::shared_ptr<DynamicNode> clone();
    };

    class DynamicNodeBool : public DynamicNode
    {
    public:
        DynamicNodeBool();
        DynamicNodeBool(bool val);
        DynamicNodeBool(const std::string& name, bool val);
        virtual std::string get_value();
        virtual uint32_t serialize(ProtoIf* proto);
        virtual uint32_t deserialize(ProtoIf* proto, bool with_schema);
        virtual void reset();
        virtual std::shared_ptr<DynamicNode> clone();
        inline void set_val(int32_t val){val_ = val;}

    private:
        bool val_ = false;
    };

    class DynamicNodeInt32 : public DynamicNode
    {
    public:
        DynamicNodeInt32();
        DynamicNodeInt32(int32_t val);
        DynamicNodeInt32(const std::string& name, int32_t val);
        virtual std::string get_value();
        virtual uint32_t serialize(ProtoIf* proto);
        virtual uint32_t deserialize(ProtoIf* proto, bool with_schema);
        virtual void reset();
        virtual std::shared_ptr<DynamicNode> clone();
        inline void set_val(int32_t val){val_ = val;}

    private:
        int32_t val_ = 0;
    };

    class DynamicNodeInt64 : public DynamicNode
    {
    public:
        DynamicNodeInt64();
        DynamicNodeInt64(int64_t val);
        DynamicNodeInt64(const std::string& name, int64_t val);
        virtual std::string get_value();
        virtual uint32_t serialize(ProtoIf* proto);
        virtual uint32_t deserialize(ProtoIf* proto, bool with_schema);
        virtual void reset();
        virtual std::shared_ptr<DynamicNode> clone();
        inline void set_val(int64_t val){val_ = val;}

    private:
        int64_t val_ = 0;
    };

    class DynamicNodeFloat : public DynamicNode
    {
    public:
        DynamicNodeFloat();
        DynamicNodeFloat(float val);
        DynamicNodeFloat(const std::string& name, float val);
        virtual std::string get_value();
        virtual uint32_t serialize(ProtoIf* proto);
        virtual uint32_t deserialize(ProtoIf* proto, bool with_schema);
        virtual void reset();
        virtual std::shared_ptr<DynamicNode> clone();
        inline void set_val(float val){val_ = val;}

    private:
        float val_ = 0;
    };

    class DynamicNodeDouble : public DynamicNode
    {
    public:
        DynamicNodeDouble();
        DynamicNodeDouble(double val);
        DynamicNodeDouble(const std::string& name, double val);
        virtual std::string get_value();
        virtual uint32_t serialize(ProtoIf* proto);
        virtual uint32_t deserialize(ProtoIf* proto, bool with_schema);
        virtual void reset();
        virtual std::shared_ptr<DynamicNode> clone();
        inline void set_val(double val){val_ = val;}

    private:
        double val_ = 0;
    };

    class DynamicNodeString : public DynamicNode
    {
    public:
        DynamicNodeString();
        DynamicNodeString(const std::string& val);
        DynamicNodeString(const std::string& name, const std::string& val);
        virtual std::string get_value();
        virtual uint32_t serialize(ProtoIf* proto);
        virtual uint32_t deserialize(ProtoIf* proto, bool with_schema);
        virtual void reset();
        virtual std::shared_ptr<DynamicNode> clone();
        inline void set_val(const std::string& val){val_ = val;}
        inline void set_val(std::string&& val){val_ = std::move(val);}

    private:
        std::string val_;
    };

    class DynamicNodeBytes : public DynamicNode
    {
    public:
        DynamicNodeBytes();
        DynamicNodeBytes(const char* data, size_t len);
        DynamicNodeBytes(const std::string& name, const char* data, size_t len);
        ~DynamicNodeBytes();
        virtual std::string get_value();
        virtual uint32_t serialize(ProtoIf* proto);
        virtual uint32_t deserialize(ProtoIf* proto, bool with_schema);
        virtual void reset();
        virtual std::shared_ptr<DynamicNode> clone();
        void set_val(const char* data, size_t len);

    private:
        char* val_ = NULL;
        uint32_t len_ = 0;
    };

    class DynamicNodeEnum : public DynamicNode
    {
    public:
        DynamicNodeEnum();
        DynamicNodeEnum(int32_t val);
        DynamicNodeEnum(const std::string& name, int32_t val);
        virtual std::string get_value();
        virtual uint32_t serialize(ProtoIf* proto);
        virtual uint32_t deserialize(ProtoIf* proto, bool with_schema);
        virtual void reset();
        virtual std::shared_ptr<DynamicNode> clone();
        inline void set_val(int32_t val){val_ = val;}

    private:
        int32_t val_ = 0;
    };

    class DynamicNodeList : public DynamicNode
    {
    public:
        DynamicNodeList();
        DynamicNodeList(SoceDataType type);
        DynamicNodeList(const std::string& name, SoceDataType type);
        virtual std::string get_value();
        virtual uint32_t serialize(ProtoIf* proto);
        virtual uint32_t deserialize(ProtoIf* proto, bool with_schema);
        virtual void reset();
        virtual std::shared_ptr<DynamicNode> clone();
        inline SoceDataType get_elem_type(){return elem_type_;}
        inline std::vector<std::shared_ptr<DynamicNode>>& get_elems(){return elems_;}

    private:
        SoceDataType elem_type_;
        std::vector<std::shared_ptr<DynamicNode>> elems_;
    };

    class DynamicNodeSet : public DynamicNode
    {
    public:
        DynamicNodeSet();
        DynamicNodeSet(SoceDataType type);
        DynamicNodeSet(const std::string& name, SoceDataType type);
        virtual std::string get_value();
        virtual uint32_t serialize(ProtoIf* proto);
        virtual uint32_t deserialize(ProtoIf* proto, bool with_schema);
        virtual void reset();
        virtual std::shared_ptr<DynamicNode> clone();
        inline SoceDataType get_elem_type(){return elem_type_;}
        inline std::vector<std::shared_ptr<DynamicNode>>& get_elems(){return elems_;}

    private:
        SoceDataType elem_type_;
        std::vector<std::shared_ptr<DynamicNode>> elems_;
    };

    class DynamicNodeMap : public DynamicNode
    {
    public:
        using DynamicNodePtr = std::shared_ptr<DynamicNode>;
        using Elements = std::vector<std::pair<DynamicNodePtr, DynamicNodePtr>>;

    public:
        DynamicNodeMap();
        DynamicNodeMap(SoceDataType key_type, SoceDataType val_type);
        DynamicNodeMap(const std::string& name,
                       SoceDataType key_type,
                       SoceDataType val_type);
        virtual std::string get_value();
        virtual uint32_t serialize(ProtoIf* proto);
        virtual uint32_t deserialize(ProtoIf* proto, bool with_schema);
        virtual void reset();
        virtual std::shared_ptr<DynamicNode> clone();
        inline SoceDataType get_key_type(){return key_type_;}
        inline SoceDataType get_val_type(){return val_type_;}
        inline Elements& get_elems(){return elems_;}

    private:
        SoceDataType key_type_;
        SoceDataType val_type_;
        Elements elems_;
    };

    class DynamicNodeStruct : public DynamicNode
    {
    public:
        DynamicNodeStruct();
        DynamicNodeStruct(const std::string& name);
        int from_bytes(const char* data, size_t len);
        virtual std::string get_value();
        virtual uint32_t serialize(ProtoIf* proto);
        virtual uint32_t deserialize(ProtoIf* proto, bool with_schema);
        virtual void reset();
        virtual std::shared_ptr<DynamicNode> clone();
        int add(std::shared_ptr<DynamicNode> node);
        int replace(std::shared_ptr<DynamicNode> node);
        int insert(const std::string& node_name, std::shared_ptr<DynamicNode> node);
        std::shared_ptr<DynamicNode> get(const std::string& path);
        std::vector<std::shared_ptr<DynamicNode>>& get_nodes();

    private:
        std::shared_ptr<DynamicNode> get(const std::vector<std::string>& pathes, size_t index);
        uint32_t deserialize_with_schema(ProtoIf* proto);
        uint32_t deserialize_without_schema(ProtoIf* proto);

    private:
        std::vector<std::shared_ptr<DynamicNode>> nodes_;
        std::unordered_set<std::string> names_;
        size_t auto_index_ = 0;
    };

} // namespace soce
} // namespace soce

#endif
