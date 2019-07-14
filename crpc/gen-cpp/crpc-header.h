#ifndef _crpc_header_1428064119_H_
#define _crpc_header_1428064119_H_

#include <string>
#include <bitset>
#include <stdint.h>
#include <unordered_map>
#include "proto/soce-proto.h"
#include "proto/type-tree.h"

using std::string;
using std::bitset;
using std::unordered_map;

namespace soce{ namespace crpc{ 

typedef enum{
    kRpcTypeReq,
    kRpcTypeResp,
    kRpcTypeCondCast,
    kRpcTypeUncondCast,
}RpcType;

typedef enum{
    kRpcStatusOK,
    kRpcStatusProtoError,
    kRpcStatusNoService,
    kRpcStatusAOPFailed,
}RpcStatus;

class CrpcReqHeaderTypeTree{
public:
    CrpcReqHeaderTypeTree(soce::proto::TypeTree& type_tree);
};

class CrpcReqHeader{
public:
    void set_type(const RpcType& _type){
        type = _type;
        status_[0] = true;
    }
    void set_type(RpcType&& _type){
        type = std::move(_type);
        status_[0] = true;
    }
    const RpcType& get_type() const {
        return type;
    }
    RpcType& mutable_type(){
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

    void set_method(const string& _method){
        method = _method;
        status_[2] = true;
    }
    void set_method(string&& _method){
        method = std::move(_method);
        status_[2] = true;
    }
    const string& get_method() const {
        return method;
    }
    string& mutable_method(){
        status_[2] = true;
        return method;
    }
    bool has_method() const{
        return status_[2];
    }

    void set_req_id(const int64_t& _req_id){
        req_id = _req_id;
        status_[3] = true;
    }
    void set_req_id(int64_t&& _req_id){
        req_id = std::move(_req_id);
        status_[3] = true;
    }
    const int64_t& get_req_id() const {
        return req_id;
    }
    int64_t& mutable_req_id(){
        status_[3] = true;
        return req_id;
    }
    bool has_req_id() const{
        return status_[3];
    }

    void set_tid(const int32_t& _tid){
        tid = _tid;
        status_[4] = true;
    }
    void set_tid(int32_t&& _tid){
        tid = std::move(_tid);
        status_[4] = true;
    }
    const int32_t& get_tid() const {
        return tid;
    }
    int32_t& mutable_tid(){
        status_[4] = true;
        return tid;
    }
    bool has_tid() const{
        return status_[4];
    }

    void set_target_addr(const string& _target_addr){
        target_addr = _target_addr;
        status_[5] = true;
    }
    void set_target_addr(string&& _target_addr){
        target_addr = std::move(_target_addr);
        status_[5] = true;
    }
    const string& get_target_addr() const {
        return target_addr;
    }
    string& mutable_target_addr(){
        status_[5] = true;
        return target_addr;
    }
    bool has_target_addr() const{
        return status_[5];
    }

    void set_ext(const unordered_map<string, string>& _ext){
        ext = _ext;
        status_[6] = true;
    }
    void set_ext(unordered_map<string, string>&& _ext){
        ext = std::move(_ext);
        status_[6] = true;
    }
    const unordered_map<string, string>& get_ext() const {
        return ext;
    }
    unordered_map<string, string>& mutable_ext(){
        status_[6] = true;
        return ext;
    }
    bool has_ext() const{
        return status_[6];
    }

public:
    /* operators */
    CrpcReqHeader() = default;
    CrpcReqHeader(const CrpcReqHeader&) = default;
    CrpcReqHeader& operator=(const CrpcReqHeader& rhs) = default;
    bool operator==(const CrpcReqHeader& rhs) const{
        if (type != rhs.type)
            return false;
        if (service != rhs.service)
            return false;
        if (method != rhs.method)
            return false;
        if (req_id != rhs.req_id)
            return false;
        if (tid != rhs.tid)
            return false;
        if (target_addr != rhs.target_addr)
            return false;
        if (ext != rhs.ext)
            return false;
        return true;
    }

    bool operator!=(const CrpcReqHeader& rhs) const{
        return !(*this == rhs);
    }

    size_t operator()(const CrpcReqHeader& rhs) const{
        size_t hash_code = 0;
        hash_code = 0
            ^ std::hash<int32_t>()(type)
            ^ std::hash<string>()(service)
            ^ std::hash<string>()(method)
            ^ std::hash<int64_t>()(req_id)
            ^ std::hash<int32_t>()(tid)
            ^ std::hash<string>()(target_addr)
            ^ [&, this]{
                size_t code = 0;
                for (auto& _i : ext){
                    code ^= std::hash<string>()(_i.first)^ std::hash<string>()(_i.second);
                }
                return code;
            }();
        return hash_code;
    }

    size_t read(soce::proto::ProtoIf* proto, bool required, bool with_type);
    size_t write(soce::proto::ProtoIf* proto, bool required, bool has_set, bool with_type = true) const;
    size_t deserialize(soce::proto::ProtoIf* proto);
    size_t serialize(soce::proto::ProtoIf* proto) const;
    static soce::proto::TypeTree* get_type_tree();

private:
    /* class members */
    RpcType type;
    string service;
    string method;
    int64_t req_id;
    int32_t tid;
    string target_addr;
    unordered_map<string, string> ext;

    /* for optional */
    bitset<7> attrs_ = 31;
    bitset<7> status_;

public:
    static CrpcReqHeaderTypeTree s_type_tree;
};

struct hash_CrpcReqHeader
{
    size_t operator()(const CrpcReqHeader& rhs) const{
        return rhs(rhs);
    }
};

class CrpcRespHeaderTypeTree{
public:
    CrpcRespHeaderTypeTree(soce::proto::TypeTree& type_tree);
};

class CrpcRespHeader{
public:
    void set_type(const RpcType& _type){
        type = _type;
        status_[0] = true;
    }
    void set_type(RpcType&& _type){
        type = std::move(_type);
        status_[0] = true;
    }
    const RpcType& get_type() const {
        return type;
    }
    RpcType& mutable_type(){
        status_[0] = true;
        return type;
    }
    bool has_type() const{
        return status_[0];
    }

    void set_req_id(const int64_t& _req_id){
        req_id = _req_id;
        status_[1] = true;
    }
    void set_req_id(int64_t&& _req_id){
        req_id = std::move(_req_id);
        status_[1] = true;
    }
    const int64_t& get_req_id() const {
        return req_id;
    }
    int64_t& mutable_req_id(){
        status_[1] = true;
        return req_id;
    }
    bool has_req_id() const{
        return status_[1];
    }

    void set_tid(const int32_t& _tid){
        tid = _tid;
        status_[2] = true;
    }
    void set_tid(int32_t&& _tid){
        tid = std::move(_tid);
        status_[2] = true;
    }
    const int32_t& get_tid() const {
        return tid;
    }
    int32_t& mutable_tid(){
        status_[2] = true;
        return tid;
    }
    bool has_tid() const{
        return status_[2];
    }

    void set_status(const RpcStatus& _status){
        status = _status;
        status_[3] = true;
    }
    void set_status(RpcStatus&& _status){
        status = std::move(_status);
        status_[3] = true;
    }
    const RpcStatus& get_status() const {
        return status;
    }
    RpcStatus& mutable_status(){
        status_[3] = true;
        return status;
    }
    bool has_status() const{
        return status_[3];
    }

    void set_ext(const unordered_map<string, string>& _ext){
        ext = _ext;
        status_[4] = true;
    }
    void set_ext(unordered_map<string, string>&& _ext){
        ext = std::move(_ext);
        status_[4] = true;
    }
    const unordered_map<string, string>& get_ext() const {
        return ext;
    }
    unordered_map<string, string>& mutable_ext(){
        status_[4] = true;
        return ext;
    }
    bool has_ext() const{
        return status_[4];
    }

public:
    /* operators */
    CrpcRespHeader() = default;
    CrpcRespHeader(const CrpcRespHeader&) = default;
    CrpcRespHeader& operator=(const CrpcRespHeader& rhs) = default;
    bool operator==(const CrpcRespHeader& rhs) const{
        if (type != rhs.type)
            return false;
        if (req_id != rhs.req_id)
            return false;
        if (tid != rhs.tid)
            return false;
        if (status != rhs.status)
            return false;
        if (ext != rhs.ext)
            return false;
        return true;
    }

    bool operator!=(const CrpcRespHeader& rhs) const{
        return !(*this == rhs);
    }

    size_t operator()(const CrpcRespHeader& rhs) const{
        size_t hash_code = 0;
        hash_code = 0
            ^ std::hash<int32_t>()(type)
            ^ std::hash<int64_t>()(req_id)
            ^ std::hash<int32_t>()(tid)
            ^ std::hash<int32_t>()(status)
            ^ [&, this]{
                size_t code = 0;
                for (auto& _i : ext){
                    code ^= std::hash<string>()(_i.first)^ std::hash<string>()(_i.second);
                }
                return code;
            }();
        return hash_code;
    }

    size_t read(soce::proto::ProtoIf* proto, bool required, bool with_type);
    size_t write(soce::proto::ProtoIf* proto, bool required, bool has_set, bool with_type = true) const;
    size_t deserialize(soce::proto::ProtoIf* proto);
    size_t serialize(soce::proto::ProtoIf* proto) const;
    static soce::proto::TypeTree* get_type_tree();

private:
    /* class members */
    RpcType type;
    int64_t req_id;
    int32_t tid;
    RpcStatus status;
    unordered_map<string, string> ext;

    /* for optional */
    bitset<5> attrs_ = 15;
    bitset<5> status_;

public:
    static CrpcRespHeaderTypeTree s_type_tree;
};

struct hash_CrpcRespHeader
{
    size_t operator()(const CrpcRespHeader& rhs) const{
        return rhs(rhs);
    }
};

class CrpcErrorRespTypeTree{
public:
    CrpcErrorRespTypeTree(soce::proto::TypeTree& type_tree);
};

class CrpcErrorResp{
public:
    void set_header(const CrpcRespHeader& _header){
        header = _header;
        status_[0] = true;
    }
    void set_header(CrpcRespHeader&& _header){
        header = std::move(_header);
        status_[0] = true;
    }
    const CrpcRespHeader& get_header() const {
        return header;
    }
    CrpcRespHeader& mutable_header(){
        status_[0] = true;
        return header;
    }
    bool has_header() const{
        return status_[0];
    }

public:
    /* operators */
    CrpcErrorResp() = default;
    CrpcErrorResp(const CrpcErrorResp&) = default;
    CrpcErrorResp& operator=(const CrpcErrorResp& rhs) = default;
    bool operator==(const CrpcErrorResp& rhs) const{
        if (header != rhs.header)
            return false;
        return true;
    }

    bool operator!=(const CrpcErrorResp& rhs) const{
        return !(*this == rhs);
    }

    size_t operator()(const CrpcErrorResp& rhs) const{
        size_t hash_code = 0;
        hash_code = 0
            ^ header(header);
        return hash_code;
    }

    size_t read(soce::proto::ProtoIf* proto, bool required, bool with_type);
    size_t write(soce::proto::ProtoIf* proto, bool required, bool has_set, bool with_type = true) const;
    size_t deserialize(soce::proto::ProtoIf* proto);
    size_t serialize(soce::proto::ProtoIf* proto) const;
    static soce::proto::TypeTree* get_type_tree();

private:
    /* class members */
    CrpcRespHeader header;

    /* for optional */
    bitset<1> attrs_ = 0;
    bitset<1> status_;

public:
    static CrpcErrorRespTypeTree s_type_tree;
};

struct hash_CrpcErrorResp
{
    size_t operator()(const CrpcErrorResp& rhs) const{
        return rhs(rhs);
    }
};

}}//end namespace

#endif

