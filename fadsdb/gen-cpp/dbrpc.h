#ifndef _dbrpc_23148924_H_
#define _dbrpc_23148924_H_

#include <string>
#include <bitset>
#include <stdint.h>
#include "gen-cpp/crpc-header.h"
#include "proto/fads-message.h"
#include "proto/fads-message.h"
#include "proto/soce-proto.h"
#include "proto/type-tree.h"

using std::string;
using std::bitset;

namespace soce{ namespace fadsdb{ 

class FadsDbServiceexecRespTypeTree{
public:
    FadsDbServiceexecRespTypeTree(soce::proto::TypeTree& type_tree);
};

class FadsDbServiceexecResp : public soce::fads::FadsMessage{
public:
    void set_header(const soce::crpc::CrpcRespHeader& _header){
        header = _header;
        status_[0] = true;
    }
    void set_header(soce::crpc::CrpcRespHeader&& _header){
        header = std::move(_header);
        status_[0] = true;
    }
    const soce::crpc::CrpcRespHeader& get_header() const {
        return header;
    }
    soce::crpc::CrpcRespHeader& mutable_header(){
        status_[0] = true;
        return header;
    }
    bool has_header() const{
        return status_[0];
    }

    void set_result(const string& _result){
        result = _result;
        status_[1] = true;
    }
    void set_result(string&& _result){
        result = std::move(_result);
        status_[1] = true;
    }
    const string& get_result() const {
        return result;
    }
    string& mutable_result(){
        status_[1] = true;
        return result;
    }
    bool has_result() const{
        return status_[1];
    }

public:
    /* operators */
    FadsDbServiceexecResp() = default;
    FadsDbServiceexecResp(const FadsDbServiceexecResp&) = default;
    FadsDbServiceexecResp& operator=(const FadsDbServiceexecResp& rhs) = default;
    bool operator==(const FadsDbServiceexecResp& rhs) const{
        if (header != rhs.header)
            return false;
        if (result != rhs.result)
            return false;
        return true;
    }

    bool operator!=(const FadsDbServiceexecResp& rhs) const{
        return !(*this == rhs);
    }

    size_t operator()(const FadsDbServiceexecResp& rhs) const{
        size_t hash_code = 0;
        hash_code = 0
            ^ header(header)
            ^ std::hash<string>()(result);
        return hash_code;
    }

    size_t read(soce::proto::ProtoIf* proto, bool required, bool with_type);
    size_t write(soce::proto::ProtoIf* proto, bool required, bool has_set, bool with_type = true) const;
    virtual size_t deserialize(soce::proto::ProtoIf* proto);
    virtual size_t serialize(soce::proto::ProtoIf* proto) const;
    static soce::proto::TypeTree* get_type_tree();

private:
    /* class members */
    soce::crpc::CrpcRespHeader header;
    string result;

    /* for optional */
    bitset<2> attrs_ = 0;
    bitset<2> status_;

public:
    static FadsDbServiceexecRespTypeTree s_type_tree;
};

struct hash_FadsDbServiceexecResp
{
    size_t operator()(const FadsDbServiceexecResp& rhs) const{
        return rhs(rhs);
    }
};

class FadsDbServiceexecReqTypeTree{
public:
    FadsDbServiceexecReqTypeTree(soce::proto::TypeTree& type_tree);
};

class FadsDbServiceexecReq : public soce::fads::FadsMessage{
public:
    void set_header(const soce::crpc::CrpcReqHeader& _header){
        header = _header;
        status_[0] = true;
    }
    void set_header(soce::crpc::CrpcReqHeader&& _header){
        header = std::move(_header);
        status_[0] = true;
    }
    const soce::crpc::CrpcReqHeader& get_header() const {
        return header;
    }
    soce::crpc::CrpcReqHeader& mutable_header(){
        status_[0] = true;
        return header;
    }
    bool has_header() const{
        return status_[0];
    }

    void set_id(const int64_t& _id){
        id = _id;
        status_[1] = true;
    }
    void set_id(int64_t&& _id){
        id = std::move(_id);
        status_[1] = true;
    }
    const int64_t& get_id() const {
        return id;
    }
    int64_t& mutable_id(){
        status_[1] = true;
        return id;
    }
    bool has_id() const{
        return status_[1];
    }

    void set_req(const string& _req){
        req = _req;
        status_[2] = true;
    }
    void set_req(string&& _req){
        req = std::move(_req);
        status_[2] = true;
    }
    const string& get_req() const {
        return req;
    }
    string& mutable_req(){
        status_[2] = true;
        return req;
    }
    bool has_req() const{
        return status_[2];
    }

public:
    /* operators */
    FadsDbServiceexecReq() = default;
    FadsDbServiceexecReq(const FadsDbServiceexecReq&) = default;
    FadsDbServiceexecReq& operator=(const FadsDbServiceexecReq& rhs) = default;
    bool operator==(const FadsDbServiceexecReq& rhs) const{
        if (header != rhs.header)
            return false;
        if (id != rhs.id)
            return false;
        if (req != rhs.req)
            return false;
        return true;
    }

    bool operator!=(const FadsDbServiceexecReq& rhs) const{
        return !(*this == rhs);
    }

    size_t operator()(const FadsDbServiceexecReq& rhs) const{
        size_t hash_code = 0;
        hash_code = 0
            ^ header(header)
            ^ std::hash<int64_t>()(id)
            ^ std::hash<string>()(req);
        return hash_code;
    }

    size_t read(soce::proto::ProtoIf* proto, bool required, bool with_type);
    size_t write(soce::proto::ProtoIf* proto, bool required, bool has_set, bool with_type = true) const;
    virtual size_t deserialize(soce::proto::ProtoIf* proto);
    virtual size_t serialize(soce::proto::ProtoIf* proto) const;
    static soce::proto::TypeTree* get_type_tree();

private:
    /* class members */
    soce::crpc::CrpcReqHeader header;
    int64_t id;
    string req;

    /* for optional */
    bitset<3> attrs_ = 0;
    bitset<3> status_;

public:
    static FadsDbServiceexecReqTypeTree s_type_tree;
};

struct hash_FadsDbServiceexecReq
{
    size_t operator()(const FadsDbServiceexecReq& rhs) const{
        return rhs(rhs);
    }
};

}}//end namespace

#endif

