#ifndef _proxy_msg_1944482654_H_
#define _proxy_msg_1944482654_H_

#include <string>
#include <bitset>
#include <stdint.h>
#include "proto/fads-message.h"
#include "proto/fads-message.h"
#include "proto/soce-proto.h"
#include "proto/type-tree.h"

using std::string;
using std::bitset;

namespace soce{ namespace crpc{ namespace proxy{ 

typedef enum{
    kProxyMsgTypeServerInfo,
    kProxyMsgTypeRegister,
    kProxyMsgTypeWatchService,
}ProxyMsgType;

class ProxyMsgServerInfoTypeTree{
public:
    ProxyMsgServerInfoTypeTree(soce::proto::TypeTree& type_tree);
};

class ProxyMsgServerInfo : public soce::fads::FadsMessage{
public:
    void set_type(const ProxyMsgType& _type){
        type = _type;
        status_[0] = true;
    }
    void set_type(ProxyMsgType&& _type){
        type = std::move(_type);
        status_[0] = true;
    }
    const ProxyMsgType& get_type() const {
        return type;
    }
    ProxyMsgType& mutable_type(){
        status_[0] = true;
        return type;
    }
    bool has_type() const{
        return status_[0];
    }

    void set_server_addr(const string& _server_addr){
        server_addr = _server_addr;
        status_[1] = true;
    }
    void set_server_addr(string&& _server_addr){
        server_addr = std::move(_server_addr);
        status_[1] = true;
    }
    const string& get_server_addr() const {
        return server_addr;
    }
    string& mutable_server_addr(){
        status_[1] = true;
        return server_addr;
    }
    bool has_server_addr() const{
        return status_[1];
    }

public:
    /* operators */
    ProxyMsgServerInfo() = default;
    ProxyMsgServerInfo(const ProxyMsgServerInfo&) = default;
    ProxyMsgServerInfo& operator=(const ProxyMsgServerInfo& rhs) = default;
    bool operator==(const ProxyMsgServerInfo& rhs) const{
        if (type != rhs.type)
            return false;
        if (server_addr != rhs.server_addr)
            return false;
        return true;
    }

    bool operator!=(const ProxyMsgServerInfo& rhs) const{
        return !(*this == rhs);
    }

    size_t operator()(const ProxyMsgServerInfo& rhs) const{
        size_t hash_code = 0;
        hash_code = 0
            ^ std::hash<int32_t>()(type)
            ^ std::hash<string>()(server_addr);
        return hash_code;
    }

    size_t read(soce::proto::ProtoIf* proto, bool required, bool with_type);
    size_t write(soce::proto::ProtoIf* proto, bool required, bool has_set, bool with_type = true) const;
    virtual size_t deserialize(soce::proto::ProtoIf* proto);
    virtual size_t serialize(soce::proto::ProtoIf* proto) const;
    static soce::proto::TypeTree* get_type_tree();

private:
    /* class members */
    ProxyMsgType type;
    string server_addr;

    /* for optional */
    bitset<2> attrs_ = 0;
    bitset<2> status_;

public:
    static ProxyMsgServerInfoTypeTree s_type_tree;
};

struct hash_ProxyMsgServerInfo
{
    size_t operator()(const ProxyMsgServerInfo& rhs) const{
        return rhs(rhs);
    }
};

class ProxyMsgRegisterInfoTypeTree{
public:
    ProxyMsgRegisterInfoTypeTree(soce::proto::TypeTree& type_tree);
};

class ProxyMsgRegisterInfo : public soce::fads::FadsMessage{
public:
    void set_type(const ProxyMsgType& _type){
        type = _type;
        status_[0] = true;
    }
    void set_type(ProxyMsgType&& _type){
        type = std::move(_type);
        status_[0] = true;
    }
    const ProxyMsgType& get_type() const {
        return type;
    }
    ProxyMsgType& mutable_type(){
        status_[0] = true;
        return type;
    }
    bool has_type() const{
        return status_[0];
    }

    void set_service(const string& _service){
        service = _service;
        status_[1] = true;
    }
    void set_service(string&& _service){
        service = std::move(_service);
        status_[1] = true;
    }
    const string& get_service() const {
        return service;
    }
    string& mutable_service(){
        status_[1] = true;
        return service;
    }
    bool has_service() const{
        return status_[1];
    }

    void set_filters(const string& _filters){
        filters = _filters;
        status_[2] = true;
    }
    void set_filters(string&& _filters){
        filters = std::move(_filters);
        status_[2] = true;
    }
    const string& get_filters() const {
        return filters;
    }
    string& mutable_filters(){
        status_[2] = true;
        return filters;
    }
    bool has_filters() const{
        return status_[2];
    }

public:
    /* operators */
    ProxyMsgRegisterInfo() = default;
    ProxyMsgRegisterInfo(const ProxyMsgRegisterInfo&) = default;
    ProxyMsgRegisterInfo& operator=(const ProxyMsgRegisterInfo& rhs) = default;
    bool operator==(const ProxyMsgRegisterInfo& rhs) const{
        if (type != rhs.type)
            return false;
        if (service != rhs.service)
            return false;
        if (filters != rhs.filters)
            return false;
        return true;
    }

    bool operator!=(const ProxyMsgRegisterInfo& rhs) const{
        return !(*this == rhs);
    }

    size_t operator()(const ProxyMsgRegisterInfo& rhs) const{
        size_t hash_code = 0;
        hash_code = 0
            ^ std::hash<int32_t>()(type)
            ^ std::hash<string>()(service)
            ^ std::hash<string>()(filters);
        return hash_code;
    }

    size_t read(soce::proto::ProtoIf* proto, bool required, bool with_type);
    size_t write(soce::proto::ProtoIf* proto, bool required, bool has_set, bool with_type = true) const;
    virtual size_t deserialize(soce::proto::ProtoIf* proto);
    virtual size_t serialize(soce::proto::ProtoIf* proto) const;
    static soce::proto::TypeTree* get_type_tree();

private:
    /* class members */
    ProxyMsgType type;
    string service;
    string filters;

    /* for optional */
    bitset<3> attrs_ = 0;
    bitset<3> status_;

public:
    static ProxyMsgRegisterInfoTypeTree s_type_tree;
};

struct hash_ProxyMsgRegisterInfo
{
    size_t operator()(const ProxyMsgRegisterInfo& rhs) const{
        return rhs(rhs);
    }
};

class ProxyMsgWatchServiceTypeTree{
public:
    ProxyMsgWatchServiceTypeTree(soce::proto::TypeTree& type_tree);
};

class ProxyMsgWatchService : public soce::fads::FadsMessage{
public:
    void set_type(const ProxyMsgType& _type){
        type = _type;
        status_[0] = true;
    }
    void set_type(ProxyMsgType&& _type){
        type = std::move(_type);
        status_[0] = true;
    }
    const ProxyMsgType& get_type() const {
        return type;
    }
    ProxyMsgType& mutable_type(){
        status_[0] = true;
        return type;
    }
    bool has_type() const{
        return status_[0];
    }

    void set_service(const string& _service){
        service = _service;
        status_[1] = true;
    }
    void set_service(string&& _service){
        service = std::move(_service);
        status_[1] = true;
    }
    const string& get_service() const {
        return service;
    }
    string& mutable_service(){
        status_[1] = true;
        return service;
    }
    bool has_service() const{
        return status_[1];
    }

public:
    /* operators */
    ProxyMsgWatchService() = default;
    ProxyMsgWatchService(const ProxyMsgWatchService&) = default;
    ProxyMsgWatchService& operator=(const ProxyMsgWatchService& rhs) = default;
    bool operator==(const ProxyMsgWatchService& rhs) const{
        if (type != rhs.type)
            return false;
        if (service != rhs.service)
            return false;
        return true;
    }

    bool operator!=(const ProxyMsgWatchService& rhs) const{
        return !(*this == rhs);
    }

    size_t operator()(const ProxyMsgWatchService& rhs) const{
        size_t hash_code = 0;
        hash_code = 0
            ^ std::hash<int32_t>()(type)
            ^ std::hash<string>()(service);
        return hash_code;
    }

    size_t read(soce::proto::ProtoIf* proto, bool required, bool with_type);
    size_t write(soce::proto::ProtoIf* proto, bool required, bool has_set, bool with_type = true) const;
    virtual size_t deserialize(soce::proto::ProtoIf* proto);
    virtual size_t serialize(soce::proto::ProtoIf* proto) const;
    static soce::proto::TypeTree* get_type_tree();

private:
    /* class members */
    ProxyMsgType type;
    string service;

    /* for optional */
    bitset<2> attrs_ = 0;
    bitset<2> status_;

public:
    static ProxyMsgWatchServiceTypeTree s_type_tree;
};

struct hash_ProxyMsgWatchService
{
    size_t operator()(const ProxyMsgWatchService& rhs) const{
        return rhs(rhs);
    }
};

}}}//end namespace

#endif

