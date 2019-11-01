#ifndef _server_attr_62132910_H_
#define _server_attr_62132910_H_

#include <string>
#include <bitset>
#include <stdint.h>
#include <unordered_map>
#include <vector>
#include "proto/fads-message.h"
#include "proto/fads-message.h"
#include "proto/soce-proto.h"
#include "proto/type-tree.h"

using std::string;
using std::bitset;
using std::unordered_map;
using std::vector;

namespace soce{ namespace crpc{ namespace attr{ 

class MethodFiltersTypeTree{
public:
    MethodFiltersTypeTree(soce::proto::TypeTree& type_tree);
};

class MethodFilters : public soce::fads::FadsMessage{
public:
    void set_filter(const string& _filter){
        filter = _filter;
        status_[0] = true;
    }
    void set_filter(string&& _filter){
        filter = std::move(_filter);
        status_[0] = true;
    }
    const string& get_filter() const {
        return filter;
    }
    string& mutable_filter(){
        status_[0] = true;
        return filter;
    }
    bool has_filter() const{
        return status_[0];
    }

    void set_val_type(const unordered_map<string, int32_t>& _val_type){
        val_type = _val_type;
        status_[1] = true;
    }
    void set_val_type(unordered_map<string, int32_t>&& _val_type){
        val_type = std::move(_val_type);
        status_[1] = true;
    }
    const unordered_map<string, int32_t>& get_val_type() const {
        return val_type;
    }
    unordered_map<string, int32_t>& mutable_val_type(){
        status_[1] = true;
        return val_type;
    }
    bool has_val_type() const{
        return status_[1];
    }

public:
    /* operators */
    MethodFilters() = default;
    MethodFilters(const MethodFilters&) = default;
    MethodFilters& operator=(const MethodFilters& rhs) = default;
    bool operator==(const MethodFilters& rhs) const{
        if (filter != rhs.filter)
            return false;
        if (val_type != rhs.val_type)
            return false;
        return true;
    }

    bool operator!=(const MethodFilters& rhs) const{
        return !(*this == rhs);
    }

    size_t operator()(const MethodFilters& rhs) const{
        size_t hash_code = 0;
        hash_code = 0
            ^ std::hash<string>()(filter)
            ^ [&, this]{
                size_t code = 0;
                for (auto& _i : val_type){
                    code ^= std::hash<string>()(_i.first)^ std::hash<int32_t>()(_i.second);
                }
                return code;
            }();
        return hash_code;
    }

    size_t read(soce::proto::ProtoIf* proto, bool required, bool with_type);
    size_t write(soce::proto::ProtoIf* proto, bool required, bool has_set, bool with_type = true) const;
    virtual size_t deserialize(soce::proto::ProtoIf* proto);
    virtual size_t serialize(soce::proto::ProtoIf* proto) const;
    static soce::proto::TypeTree* get_type_tree();

private:
    /* class members */
    string filter;
    unordered_map<string, int32_t> val_type;

    /* for optional */
    bitset<2> attrs_ = 0;
    bitset<2> status_;

public:
    static MethodFiltersTypeTree s_type_tree;
};

struct hash_MethodFilters
{
    size_t operator()(const MethodFilters& rhs) const{
        return rhs(rhs);
    }
};

class ServerAttrsTypeTree{
public:
    ServerAttrsTypeTree(soce::proto::TypeTree& type_tree);
};

class ServerAttrs : public soce::fads::FadsMessage{
public:
    void set_name(const string& _name){
        name = _name;
        status_[0] = true;
    }
    void set_name(string&& _name){
        name = std::move(_name);
        status_[0] = true;
    }
    const string& get_name() const {
        return name;
    }
    string& mutable_name(){
        status_[0] = true;
        return name;
    }
    bool has_name() const{
        return status_[0];
    }

    void set_methods(const unordered_map<string, MethodFilters>& _methods){
        methods = _methods;
        status_[1] = true;
    }
    void set_methods(unordered_map<string, MethodFilters>&& _methods){
        methods = std::move(_methods);
        status_[1] = true;
    }
    const unordered_map<string, MethodFilters>& get_methods() const {
        return methods;
    }
    unordered_map<string, MethodFilters>& mutable_methods(){
        status_[1] = true;
        return methods;
    }
    bool has_methods() const{
        return status_[1];
    }

public:
    /* operators */
    ServerAttrs() = default;
    ServerAttrs(const ServerAttrs&) = default;
    ServerAttrs& operator=(const ServerAttrs& rhs) = default;
    bool operator==(const ServerAttrs& rhs) const{
        if (name != rhs.name)
            return false;
        if (methods != rhs.methods)
            return false;
        return true;
    }

    bool operator!=(const ServerAttrs& rhs) const{
        return !(*this == rhs);
    }

    size_t operator()(const ServerAttrs& rhs) const{
        size_t hash_code = 0;
        hash_code = 0
            ^ std::hash<string>()(name)
            ^ [&, this]{
                size_t code = 0;
                for (auto& _i : methods){
                    code ^= std::hash<string>()(_i.first)^ _i.second(_i.second);
                }
                return code;
            }();
        return hash_code;
    }

    size_t read(soce::proto::ProtoIf* proto, bool required, bool with_type);
    size_t write(soce::proto::ProtoIf* proto, bool required, bool has_set, bool with_type = true) const;
    virtual size_t deserialize(soce::proto::ProtoIf* proto);
    virtual size_t serialize(soce::proto::ProtoIf* proto) const;
    static soce::proto::TypeTree* get_type_tree();

private:
    /* class members */
    string name;
    unordered_map<string, MethodFilters> methods;

    /* for optional */
    bitset<2> attrs_ = 3;
    bitset<2> status_;

public:
    static ServerAttrsTypeTree s_type_tree;
};

struct hash_ServerAttrs
{
    size_t operator()(const ServerAttrs& rhs) const{
        return rhs(rhs);
    }
};

class ProcessorAttrsTypeTree{
public:
    ProcessorAttrsTypeTree(soce::proto::TypeTree& type_tree);
};

class ProcessorAttrs : public soce::fads::FadsMessage{
public:
    void set_server_attrs(const vector<string>& _server_attrs){
        server_attrs = _server_attrs;
        status_[0] = true;
    }
    void set_server_attrs(vector<string>&& _server_attrs){
        server_attrs = std::move(_server_attrs);
        status_[0] = true;
    }
    const vector<string>& get_server_attrs() const {
        return server_attrs;
    }
    vector<string>& mutable_server_attrs(){
        status_[0] = true;
        return server_attrs;
    }
    bool has_server_attrs() const{
        return status_[0];
    }

public:
    /* operators */
    ProcessorAttrs() = default;
    ProcessorAttrs(const ProcessorAttrs&) = default;
    ProcessorAttrs& operator=(const ProcessorAttrs& rhs) = default;
    bool operator==(const ProcessorAttrs& rhs) const{
        if (server_attrs != rhs.server_attrs)
            return false;
        return true;
    }

    bool operator!=(const ProcessorAttrs& rhs) const{
        return !(*this == rhs);
    }

    size_t operator()(const ProcessorAttrs& rhs) const{
        size_t hash_code = 0;
        hash_code = 0
            ^ [&, this]{
                size_t code = 0;
                for (auto& _i : server_attrs){
                    code ^= std::hash<string>()(_i);
                }
                return code;
            }();
        return hash_code;
    }

    size_t read(soce::proto::ProtoIf* proto, bool required, bool with_type);
    size_t write(soce::proto::ProtoIf* proto, bool required, bool has_set, bool with_type = true) const;
    virtual size_t deserialize(soce::proto::ProtoIf* proto);
    virtual size_t serialize(soce::proto::ProtoIf* proto) const;
    static soce::proto::TypeTree* get_type_tree();

private:
    /* class members */
    vector<string> server_attrs;

    /* for optional */
    bitset<1> attrs_ = 1;
    bitset<1> status_;

public:
    static ProcessorAttrsTypeTree s_type_tree;
};

struct hash_ProcessorAttrs
{
    size_t operator()(const ProcessorAttrs& rhs) const{
        return rhs(rhs);
    }
};

}}}//end namespace

#endif

