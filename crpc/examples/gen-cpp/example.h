#ifndef _example_1907375978_H_
#define _example_1907375978_H_

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

namespace crpc{ namespace example{ 

class SyncDemohelloRespTypeTree{
public:
    SyncDemohelloRespTypeTree(soce::proto::TypeTree& type_tree);
};

class SyncDemohelloResp : public soce::fads::FadsMessage{
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
    SyncDemohelloResp() = default;
    SyncDemohelloResp(const SyncDemohelloResp&) = default;
    SyncDemohelloResp& operator=(const SyncDemohelloResp& rhs) = default;
    bool operator==(const SyncDemohelloResp& rhs) const{
        if (header != rhs.header)
            return false;
        if (result != rhs.result)
            return false;
        return true;
    }

    bool operator!=(const SyncDemohelloResp& rhs) const{
        return !(*this == rhs);
    }

    size_t operator()(const SyncDemohelloResp& rhs) const{
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
    static SyncDemohelloRespTypeTree s_type_tree;
};

struct hash_SyncDemohelloResp
{
    size_t operator()(const SyncDemohelloResp& rhs) const{
        return rhs(rhs);
    }
};

class SyncDemohelloReqTypeTree{
public:
    SyncDemohelloReqTypeTree(soce::proto::TypeTree& type_tree);
};

class SyncDemohelloReq : public soce::fads::FadsMessage{
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

    void set_msg(const string& _msg){
        msg = _msg;
        status_[1] = true;
    }
    void set_msg(string&& _msg){
        msg = std::move(_msg);
        status_[1] = true;
    }
    const string& get_msg() const {
        return msg;
    }
    string& mutable_msg(){
        status_[1] = true;
        return msg;
    }
    bool has_msg() const{
        return status_[1];
    }

public:
    /* operators */
    SyncDemohelloReq() = default;
    SyncDemohelloReq(const SyncDemohelloReq&) = default;
    SyncDemohelloReq& operator=(const SyncDemohelloReq& rhs) = default;
    bool operator==(const SyncDemohelloReq& rhs) const{
        if (header != rhs.header)
            return false;
        if (msg != rhs.msg)
            return false;
        return true;
    }

    bool operator!=(const SyncDemohelloReq& rhs) const{
        return !(*this == rhs);
    }

    size_t operator()(const SyncDemohelloReq& rhs) const{
        size_t hash_code = 0;
        hash_code = 0
            ^ header(header)
            ^ std::hash<string>()(msg);
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
    string msg;

    /* for optional */
    bitset<2> attrs_ = 0;
    bitset<2> status_;

public:
    static SyncDemohelloReqTypeTree s_type_tree;
};

struct hash_SyncDemohelloReq
{
    size_t operator()(const SyncDemohelloReq& rhs) const{
        return rhs(rhs);
    }
};

class CortDemoechoRespTypeTree{
public:
    CortDemoechoRespTypeTree(soce::proto::TypeTree& type_tree);
};

class CortDemoechoResp : public soce::fads::FadsMessage{
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

    void set_result(const int32_t& _result){
        result = _result;
        status_[1] = true;
    }
    void set_result(int32_t&& _result){
        result = std::move(_result);
        status_[1] = true;
    }
    const int32_t& get_result() const {
        return result;
    }
    int32_t& mutable_result(){
        status_[1] = true;
        return result;
    }
    bool has_result() const{
        return status_[1];
    }

public:
    /* operators */
    CortDemoechoResp() = default;
    CortDemoechoResp(const CortDemoechoResp&) = default;
    CortDemoechoResp& operator=(const CortDemoechoResp& rhs) = default;
    bool operator==(const CortDemoechoResp& rhs) const{
        if (header != rhs.header)
            return false;
        if (result != rhs.result)
            return false;
        return true;
    }

    bool operator!=(const CortDemoechoResp& rhs) const{
        return !(*this == rhs);
    }

    size_t operator()(const CortDemoechoResp& rhs) const{
        size_t hash_code = 0;
        hash_code = 0
            ^ header(header)
            ^ std::hash<int32_t>()(result);
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
    int32_t result;

    /* for optional */
    bitset<2> attrs_ = 0;
    bitset<2> status_;

public:
    static CortDemoechoRespTypeTree s_type_tree;
};

struct hash_CortDemoechoResp
{
    size_t operator()(const CortDemoechoResp& rhs) const{
        return rhs(rhs);
    }
};

class CortDemoechoReqTypeTree{
public:
    CortDemoechoReqTypeTree(soce::proto::TypeTree& type_tree);
};

class CortDemoechoReq : public soce::fads::FadsMessage{
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

    void set_p(const int32_t& _p){
        p = _p;
        status_[1] = true;
    }
    void set_p(int32_t&& _p){
        p = std::move(_p);
        status_[1] = true;
    }
    const int32_t& get_p() const {
        return p;
    }
    int32_t& mutable_p(){
        status_[1] = true;
        return p;
    }
    bool has_p() const{
        return status_[1];
    }

public:
    /* operators */
    CortDemoechoReq() = default;
    CortDemoechoReq(const CortDemoechoReq&) = default;
    CortDemoechoReq& operator=(const CortDemoechoReq& rhs) = default;
    bool operator==(const CortDemoechoReq& rhs) const{
        if (header != rhs.header)
            return false;
        if (p != rhs.p)
            return false;
        return true;
    }

    bool operator!=(const CortDemoechoReq& rhs) const{
        return !(*this == rhs);
    }

    size_t operator()(const CortDemoechoReq& rhs) const{
        size_t hash_code = 0;
        hash_code = 0
            ^ header(header)
            ^ std::hash<int32_t>()(p);
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
    int32_t p;

    /* for optional */
    bitset<2> attrs_ = 0;
    bitset<2> status_;

public:
    static CortDemoechoReqTypeTree s_type_tree;
};

struct hash_CortDemoechoReq
{
    size_t operator()(const CortDemoechoReq& rhs) const{
        return rhs(rhs);
    }
};

class CortDemotest_voidRespTypeTree{
public:
    CortDemotest_voidRespTypeTree(soce::proto::TypeTree& type_tree);
};

class CortDemotest_voidResp : public soce::fads::FadsMessage{
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

public:
    /* operators */
    CortDemotest_voidResp() = default;
    CortDemotest_voidResp(const CortDemotest_voidResp&) = default;
    CortDemotest_voidResp& operator=(const CortDemotest_voidResp& rhs) = default;
    bool operator==(const CortDemotest_voidResp& rhs) const{
        if (header != rhs.header)
            return false;
        return true;
    }

    bool operator!=(const CortDemotest_voidResp& rhs) const{
        return !(*this == rhs);
    }

    size_t operator()(const CortDemotest_voidResp& rhs) const{
        size_t hash_code = 0;
        hash_code = 0
            ^ header(header);
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

    /* for optional */
    bitset<1> attrs_ = 0;
    bitset<1> status_;

public:
    static CortDemotest_voidRespTypeTree s_type_tree;
};

struct hash_CortDemotest_voidResp
{
    size_t operator()(const CortDemotest_voidResp& rhs) const{
        return rhs(rhs);
    }
};

class CortDemotest_voidReqTypeTree{
public:
    CortDemotest_voidReqTypeTree(soce::proto::TypeTree& type_tree);
};

class CortDemotest_voidReq : public soce::fads::FadsMessage{
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

    void set_p(const int32_t& _p){
        p = _p;
        status_[1] = true;
    }
    void set_p(int32_t&& _p){
        p = std::move(_p);
        status_[1] = true;
    }
    const int32_t& get_p() const {
        return p;
    }
    int32_t& mutable_p(){
        status_[1] = true;
        return p;
    }
    bool has_p() const{
        return status_[1];
    }

public:
    /* operators */
    CortDemotest_voidReq() = default;
    CortDemotest_voidReq(const CortDemotest_voidReq&) = default;
    CortDemotest_voidReq& operator=(const CortDemotest_voidReq& rhs) = default;
    bool operator==(const CortDemotest_voidReq& rhs) const{
        if (header != rhs.header)
            return false;
        if (p != rhs.p)
            return false;
        return true;
    }

    bool operator!=(const CortDemotest_voidReq& rhs) const{
        return !(*this == rhs);
    }

    size_t operator()(const CortDemotest_voidReq& rhs) const{
        size_t hash_code = 0;
        hash_code = 0
            ^ header(header)
            ^ std::hash<int32_t>()(p);
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
    int32_t p;

    /* for optional */
    bitset<2> attrs_ = 0;
    bitset<2> status_;

public:
    static CortDemotest_voidReqTypeTree s_type_tree;
};

struct hash_CortDemotest_voidReq
{
    size_t operator()(const CortDemotest_voidReq& rhs) const{
        return rhs(rhs);
    }
};

class CortDemotest_nullReqTypeTree{
public:
    CortDemotest_nullReqTypeTree(soce::proto::TypeTree& type_tree);
};

class CortDemotest_nullReq : public soce::fads::FadsMessage{
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

    void set_p(const int32_t& _p){
        p = _p;
        status_[1] = true;
    }
    void set_p(int32_t&& _p){
        p = std::move(_p);
        status_[1] = true;
    }
    const int32_t& get_p() const {
        return p;
    }
    int32_t& mutable_p(){
        status_[1] = true;
        return p;
    }
    bool has_p() const{
        return status_[1];
    }

public:
    /* operators */
    CortDemotest_nullReq() = default;
    CortDemotest_nullReq(const CortDemotest_nullReq&) = default;
    CortDemotest_nullReq& operator=(const CortDemotest_nullReq& rhs) = default;
    bool operator==(const CortDemotest_nullReq& rhs) const{
        if (header != rhs.header)
            return false;
        if (p != rhs.p)
            return false;
        return true;
    }

    bool operator!=(const CortDemotest_nullReq& rhs) const{
        return !(*this == rhs);
    }

    size_t operator()(const CortDemotest_nullReq& rhs) const{
        size_t hash_code = 0;
        hash_code = 0
            ^ header(header)
            ^ std::hash<int32_t>()(p);
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
    int32_t p;

    /* for optional */
    bitset<2> attrs_ = 0;
    bitset<2> status_;

public:
    static CortDemotest_nullReqTypeTree s_type_tree;
};

struct hash_CortDemotest_nullReq
{
    size_t operator()(const CortDemotest_nullReq& rhs) const{
        return rhs(rhs);
    }
};

class CortDemotest_condcastRespTypeTree{
public:
    CortDemotest_condcastRespTypeTree(soce::proto::TypeTree& type_tree);
};

class CortDemotest_condcastResp : public soce::fads::FadsMessage{
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

public:
    /* operators */
    CortDemotest_condcastResp() = default;
    CortDemotest_condcastResp(const CortDemotest_condcastResp&) = default;
    CortDemotest_condcastResp& operator=(const CortDemotest_condcastResp& rhs) = default;
    bool operator==(const CortDemotest_condcastResp& rhs) const{
        if (header != rhs.header)
            return false;
        return true;
    }

    bool operator!=(const CortDemotest_condcastResp& rhs) const{
        return !(*this == rhs);
    }

    size_t operator()(const CortDemotest_condcastResp& rhs) const{
        size_t hash_code = 0;
        hash_code = 0
            ^ header(header);
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

    /* for optional */
    bitset<1> attrs_ = 0;
    bitset<1> status_;

public:
    static CortDemotest_condcastRespTypeTree s_type_tree;
};

struct hash_CortDemotest_condcastResp
{
    size_t operator()(const CortDemotest_condcastResp& rhs) const{
        return rhs(rhs);
    }
};

class CortDemotest_condcastReqTypeTree{
public:
    CortDemotest_condcastReqTypeTree(soce::proto::TypeTree& type_tree);
};

class CortDemotest_condcastReq : public soce::fads::FadsMessage{
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

    void set_p(const int32_t& _p){
        p = _p;
        status_[1] = true;
    }
    void set_p(int32_t&& _p){
        p = std::move(_p);
        status_[1] = true;
    }
    const int32_t& get_p() const {
        return p;
    }
    int32_t& mutable_p(){
        status_[1] = true;
        return p;
    }
    bool has_p() const{
        return status_[1];
    }

public:
    /* operators */
    CortDemotest_condcastReq() = default;
    CortDemotest_condcastReq(const CortDemotest_condcastReq&) = default;
    CortDemotest_condcastReq& operator=(const CortDemotest_condcastReq& rhs) = default;
    bool operator==(const CortDemotest_condcastReq& rhs) const{
        if (header != rhs.header)
            return false;
        if (p != rhs.p)
            return false;
        return true;
    }

    bool operator!=(const CortDemotest_condcastReq& rhs) const{
        return !(*this == rhs);
    }

    size_t operator()(const CortDemotest_condcastReq& rhs) const{
        size_t hash_code = 0;
        hash_code = 0
            ^ header(header)
            ^ std::hash<int32_t>()(p);
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
    int32_t p;

    /* for optional */
    bitset<2> attrs_ = 0;
    bitset<2> status_;

public:
    static CortDemotest_condcastReqTypeTree s_type_tree;
};

struct hash_CortDemotest_condcastReq
{
    size_t operator()(const CortDemotest_condcastReq& rhs) const{
        return rhs(rhs);
    }
};

class CortDemotest_uncondcastRespTypeTree{
public:
    CortDemotest_uncondcastRespTypeTree(soce::proto::TypeTree& type_tree);
};

class CortDemotest_uncondcastResp : public soce::fads::FadsMessage{
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

public:
    /* operators */
    CortDemotest_uncondcastResp() = default;
    CortDemotest_uncondcastResp(const CortDemotest_uncondcastResp&) = default;
    CortDemotest_uncondcastResp& operator=(const CortDemotest_uncondcastResp& rhs) = default;
    bool operator==(const CortDemotest_uncondcastResp& rhs) const{
        if (header != rhs.header)
            return false;
        return true;
    }

    bool operator!=(const CortDemotest_uncondcastResp& rhs) const{
        return !(*this == rhs);
    }

    size_t operator()(const CortDemotest_uncondcastResp& rhs) const{
        size_t hash_code = 0;
        hash_code = 0
            ^ header(header);
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

    /* for optional */
    bitset<1> attrs_ = 0;
    bitset<1> status_;

public:
    static CortDemotest_uncondcastRespTypeTree s_type_tree;
};

struct hash_CortDemotest_uncondcastResp
{
    size_t operator()(const CortDemotest_uncondcastResp& rhs) const{
        return rhs(rhs);
    }
};

class CortDemotest_uncondcastReqTypeTree{
public:
    CortDemotest_uncondcastReqTypeTree(soce::proto::TypeTree& type_tree);
};

class CortDemotest_uncondcastReq : public soce::fads::FadsMessage{
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

    void set_p(const int32_t& _p){
        p = _p;
        status_[1] = true;
    }
    void set_p(int32_t&& _p){
        p = std::move(_p);
        status_[1] = true;
    }
    const int32_t& get_p() const {
        return p;
    }
    int32_t& mutable_p(){
        status_[1] = true;
        return p;
    }
    bool has_p() const{
        return status_[1];
    }

public:
    /* operators */
    CortDemotest_uncondcastReq() = default;
    CortDemotest_uncondcastReq(const CortDemotest_uncondcastReq&) = default;
    CortDemotest_uncondcastReq& operator=(const CortDemotest_uncondcastReq& rhs) = default;
    bool operator==(const CortDemotest_uncondcastReq& rhs) const{
        if (header != rhs.header)
            return false;
        if (p != rhs.p)
            return false;
        return true;
    }

    bool operator!=(const CortDemotest_uncondcastReq& rhs) const{
        return !(*this == rhs);
    }

    size_t operator()(const CortDemotest_uncondcastReq& rhs) const{
        size_t hash_code = 0;
        hash_code = 0
            ^ header(header)
            ^ std::hash<int32_t>()(p);
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
    int32_t p;

    /* for optional */
    bitset<2> attrs_ = 0;
    bitset<2> status_;

public:
    static CortDemotest_uncondcastReqTypeTree s_type_tree;
};

struct hash_CortDemotest_uncondcastReq
{
    size_t operator()(const CortDemotest_uncondcastReq& rhs) const{
        return rhs(rhs);
    }
};

}}//end namespace

#endif

