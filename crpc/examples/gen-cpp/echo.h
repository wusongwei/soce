#ifndef _echo_1266766164_H_
#define _echo_1266766164_H_

#include <string>
#include <bitset>
#include <stdint.h>
#include "gen-cpp/crpc-header.h"
#include "proto/soce-proto.h"
#include "proto/type-tree.h"

using std::string;
using std::bitset;

namespace crpc{ namespace sample{ 

class EchoechoRespTypeTree{
public:
    EchoechoRespTypeTree(soce::proto::TypeTree& type_tree);
};

class EchoechoResp{
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
    EchoechoResp() = default;
    EchoechoResp(const EchoechoResp&) = default;
    EchoechoResp& operator=(const EchoechoResp& rhs) = default;
    bool operator==(const EchoechoResp& rhs) const{
        if (header != rhs.header)
            return false;
        if (result != rhs.result)
            return false;
        return true;
    }

    bool operator!=(const EchoechoResp& rhs) const{
        return !(*this == rhs);
    }

    size_t operator()(const EchoechoResp& rhs) const{
        size_t hash_code = 0;
        hash_code = 0
            ^ header(header)
            ^ std::hash<int32_t>()(result);
        return hash_code;
    }

    size_t read(soce::proto::ProtoIf* proto, bool required, bool with_type);
    size_t write(soce::proto::ProtoIf* proto, bool required, bool has_set, bool with_type = true) const;
    size_t deserialize(soce::proto::ProtoIf* proto);
    size_t serialize(soce::proto::ProtoIf* proto) const;
    static soce::proto::TypeTree* get_type_tree();

private:
    /* class members */
    soce::crpc::CrpcRespHeader header;
    int32_t result;

    /* for optional */
    bitset<2> attrs_ = 0;
    bitset<2> status_;

public:
    static EchoechoRespTypeTree s_type_tree;
};

struct hash_EchoechoResp
{
    size_t operator()(const EchoechoResp& rhs) const{
        return rhs(rhs);
    }
};

class EchoechoReqTypeTree{
public:
    EchoechoReqTypeTree(soce::proto::TypeTree& type_tree);
};

class EchoechoReq{
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
    EchoechoReq() = default;
    EchoechoReq(const EchoechoReq&) = default;
    EchoechoReq& operator=(const EchoechoReq& rhs) = default;
    bool operator==(const EchoechoReq& rhs) const{
        if (header != rhs.header)
            return false;
        if (p != rhs.p)
            return false;
        return true;
    }

    bool operator!=(const EchoechoReq& rhs) const{
        return !(*this == rhs);
    }

    size_t operator()(const EchoechoReq& rhs) const{
        size_t hash_code = 0;
        hash_code = 0
            ^ header(header)
            ^ std::hash<int32_t>()(p);
        return hash_code;
    }

    size_t read(soce::proto::ProtoIf* proto, bool required, bool with_type);
    size_t write(soce::proto::ProtoIf* proto, bool required, bool has_set, bool with_type = true) const;
    size_t deserialize(soce::proto::ProtoIf* proto);
    size_t serialize(soce::proto::ProtoIf* proto) const;
    static soce::proto::TypeTree* get_type_tree();

private:
    /* class members */
    soce::crpc::CrpcReqHeader header;
    int32_t p;

    /* for optional */
    bitset<2> attrs_ = 0;
    bitset<2> status_;

public:
    static EchoechoReqTypeTree s_type_tree;
};

struct hash_EchoechoReq
{
    size_t operator()(const EchoechoReq& rhs) const{
        return rhs(rhs);
    }
};

class Echodo_void_testRespTypeTree{
public:
    Echodo_void_testRespTypeTree(soce::proto::TypeTree& type_tree);
};

class Echodo_void_testResp{
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
    Echodo_void_testResp() = default;
    Echodo_void_testResp(const Echodo_void_testResp&) = default;
    Echodo_void_testResp& operator=(const Echodo_void_testResp& rhs) = default;
    bool operator==(const Echodo_void_testResp& rhs) const{
        if (header != rhs.header)
            return false;
        return true;
    }

    bool operator!=(const Echodo_void_testResp& rhs) const{
        return !(*this == rhs);
    }

    size_t operator()(const Echodo_void_testResp& rhs) const{
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
    soce::crpc::CrpcRespHeader header;

    /* for optional */
    bitset<1> attrs_ = 0;
    bitset<1> status_;

public:
    static Echodo_void_testRespTypeTree s_type_tree;
};

struct hash_Echodo_void_testResp
{
    size_t operator()(const Echodo_void_testResp& rhs) const{
        return rhs(rhs);
    }
};

class Echodo_void_testReqTypeTree{
public:
    Echodo_void_testReqTypeTree(soce::proto::TypeTree& type_tree);
};

class Echodo_void_testReq{
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
    Echodo_void_testReq() = default;
    Echodo_void_testReq(const Echodo_void_testReq&) = default;
    Echodo_void_testReq& operator=(const Echodo_void_testReq& rhs) = default;
    bool operator==(const Echodo_void_testReq& rhs) const{
        if (header != rhs.header)
            return false;
        if (p != rhs.p)
            return false;
        return true;
    }

    bool operator!=(const Echodo_void_testReq& rhs) const{
        return !(*this == rhs);
    }

    size_t operator()(const Echodo_void_testReq& rhs) const{
        size_t hash_code = 0;
        hash_code = 0
            ^ header(header)
            ^ std::hash<int32_t>()(p);
        return hash_code;
    }

    size_t read(soce::proto::ProtoIf* proto, bool required, bool with_type);
    size_t write(soce::proto::ProtoIf* proto, bool required, bool has_set, bool with_type = true) const;
    size_t deserialize(soce::proto::ProtoIf* proto);
    size_t serialize(soce::proto::ProtoIf* proto) const;
    static soce::proto::TypeTree* get_type_tree();

private:
    /* class members */
    soce::crpc::CrpcReqHeader header;
    int32_t p;

    /* for optional */
    bitset<2> attrs_ = 0;
    bitset<2> status_;

public:
    static Echodo_void_testReqTypeTree s_type_tree;
};

struct hash_Echodo_void_testReq
{
    size_t operator()(const Echodo_void_testReq& rhs) const{
        return rhs(rhs);
    }
};

class Echodo_null_testReqTypeTree{
public:
    Echodo_null_testReqTypeTree(soce::proto::TypeTree& type_tree);
};

class Echodo_null_testReq{
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
    Echodo_null_testReq() = default;
    Echodo_null_testReq(const Echodo_null_testReq&) = default;
    Echodo_null_testReq& operator=(const Echodo_null_testReq& rhs) = default;
    bool operator==(const Echodo_null_testReq& rhs) const{
        if (header != rhs.header)
            return false;
        if (p != rhs.p)
            return false;
        return true;
    }

    bool operator!=(const Echodo_null_testReq& rhs) const{
        return !(*this == rhs);
    }

    size_t operator()(const Echodo_null_testReq& rhs) const{
        size_t hash_code = 0;
        hash_code = 0
            ^ header(header)
            ^ std::hash<int32_t>()(p);
        return hash_code;
    }

    size_t read(soce::proto::ProtoIf* proto, bool required, bool with_type);
    size_t write(soce::proto::ProtoIf* proto, bool required, bool has_set, bool with_type = true) const;
    size_t deserialize(soce::proto::ProtoIf* proto);
    size_t serialize(soce::proto::ProtoIf* proto) const;
    static soce::proto::TypeTree* get_type_tree();

private:
    /* class members */
    soce::crpc::CrpcReqHeader header;
    int32_t p;

    /* for optional */
    bitset<2> attrs_ = 0;
    bitset<2> status_;

public:
    static Echodo_null_testReqTypeTree s_type_tree;
};

struct hash_Echodo_null_testReq
{
    size_t operator()(const Echodo_null_testReq& rhs) const{
        return rhs(rhs);
    }
};

class Echodo_condcast_testRespTypeTree{
public:
    Echodo_condcast_testRespTypeTree(soce::proto::TypeTree& type_tree);
};

class Echodo_condcast_testResp{
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
    Echodo_condcast_testResp() = default;
    Echodo_condcast_testResp(const Echodo_condcast_testResp&) = default;
    Echodo_condcast_testResp& operator=(const Echodo_condcast_testResp& rhs) = default;
    bool operator==(const Echodo_condcast_testResp& rhs) const{
        if (header != rhs.header)
            return false;
        return true;
    }

    bool operator!=(const Echodo_condcast_testResp& rhs) const{
        return !(*this == rhs);
    }

    size_t operator()(const Echodo_condcast_testResp& rhs) const{
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
    soce::crpc::CrpcRespHeader header;

    /* for optional */
    bitset<1> attrs_ = 0;
    bitset<1> status_;

public:
    static Echodo_condcast_testRespTypeTree s_type_tree;
};

struct hash_Echodo_condcast_testResp
{
    size_t operator()(const Echodo_condcast_testResp& rhs) const{
        return rhs(rhs);
    }
};

class Echodo_condcast_testReqTypeTree{
public:
    Echodo_condcast_testReqTypeTree(soce::proto::TypeTree& type_tree);
};

class Echodo_condcast_testReq{
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
    Echodo_condcast_testReq() = default;
    Echodo_condcast_testReq(const Echodo_condcast_testReq&) = default;
    Echodo_condcast_testReq& operator=(const Echodo_condcast_testReq& rhs) = default;
    bool operator==(const Echodo_condcast_testReq& rhs) const{
        if (header != rhs.header)
            return false;
        if (p != rhs.p)
            return false;
        return true;
    }

    bool operator!=(const Echodo_condcast_testReq& rhs) const{
        return !(*this == rhs);
    }

    size_t operator()(const Echodo_condcast_testReq& rhs) const{
        size_t hash_code = 0;
        hash_code = 0
            ^ header(header)
            ^ std::hash<int32_t>()(p);
        return hash_code;
    }

    size_t read(soce::proto::ProtoIf* proto, bool required, bool with_type);
    size_t write(soce::proto::ProtoIf* proto, bool required, bool has_set, bool with_type = true) const;
    size_t deserialize(soce::proto::ProtoIf* proto);
    size_t serialize(soce::proto::ProtoIf* proto) const;
    static soce::proto::TypeTree* get_type_tree();

private:
    /* class members */
    soce::crpc::CrpcReqHeader header;
    int32_t p;

    /* for optional */
    bitset<2> attrs_ = 0;
    bitset<2> status_;

public:
    static Echodo_condcast_testReqTypeTree s_type_tree;
};

struct hash_Echodo_condcast_testReq
{
    size_t operator()(const Echodo_condcast_testReq& rhs) const{
        return rhs(rhs);
    }
};

class Echodo_uncondcast_testRespTypeTree{
public:
    Echodo_uncondcast_testRespTypeTree(soce::proto::TypeTree& type_tree);
};

class Echodo_uncondcast_testResp{
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
    Echodo_uncondcast_testResp() = default;
    Echodo_uncondcast_testResp(const Echodo_uncondcast_testResp&) = default;
    Echodo_uncondcast_testResp& operator=(const Echodo_uncondcast_testResp& rhs) = default;
    bool operator==(const Echodo_uncondcast_testResp& rhs) const{
        if (header != rhs.header)
            return false;
        return true;
    }

    bool operator!=(const Echodo_uncondcast_testResp& rhs) const{
        return !(*this == rhs);
    }

    size_t operator()(const Echodo_uncondcast_testResp& rhs) const{
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
    soce::crpc::CrpcRespHeader header;

    /* for optional */
    bitset<1> attrs_ = 0;
    bitset<1> status_;

public:
    static Echodo_uncondcast_testRespTypeTree s_type_tree;
};

struct hash_Echodo_uncondcast_testResp
{
    size_t operator()(const Echodo_uncondcast_testResp& rhs) const{
        return rhs(rhs);
    }
};

class Echodo_uncondcast_testReqTypeTree{
public:
    Echodo_uncondcast_testReqTypeTree(soce::proto::TypeTree& type_tree);
};

class Echodo_uncondcast_testReq{
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
    Echodo_uncondcast_testReq() = default;
    Echodo_uncondcast_testReq(const Echodo_uncondcast_testReq&) = default;
    Echodo_uncondcast_testReq& operator=(const Echodo_uncondcast_testReq& rhs) = default;
    bool operator==(const Echodo_uncondcast_testReq& rhs) const{
        if (header != rhs.header)
            return false;
        if (p != rhs.p)
            return false;
        return true;
    }

    bool operator!=(const Echodo_uncondcast_testReq& rhs) const{
        return !(*this == rhs);
    }

    size_t operator()(const Echodo_uncondcast_testReq& rhs) const{
        size_t hash_code = 0;
        hash_code = 0
            ^ header(header)
            ^ std::hash<int32_t>()(p);
        return hash_code;
    }

    size_t read(soce::proto::ProtoIf* proto, bool required, bool with_type);
    size_t write(soce::proto::ProtoIf* proto, bool required, bool has_set, bool with_type = true) const;
    size_t deserialize(soce::proto::ProtoIf* proto);
    size_t serialize(soce::proto::ProtoIf* proto) const;
    static soce::proto::TypeTree* get_type_tree();

private:
    /* class members */
    soce::crpc::CrpcReqHeader header;
    int32_t p;

    /* for optional */
    bitset<2> attrs_ = 0;
    bitset<2> status_;

public:
    static Echodo_uncondcast_testReqTypeTree s_type_tree;
};

struct hash_Echodo_uncondcast_testReq
{
    size_t operator()(const Echodo_uncondcast_testReq& rhs) const{
        return rhs(rhs);
    }
};

class GatewayrecvRespTypeTree{
public:
    GatewayrecvRespTypeTree(soce::proto::TypeTree& type_tree);
};

class GatewayrecvResp{
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
    GatewayrecvResp() = default;
    GatewayrecvResp(const GatewayrecvResp&) = default;
    GatewayrecvResp& operator=(const GatewayrecvResp& rhs) = default;
    bool operator==(const GatewayrecvResp& rhs) const{
        if (header != rhs.header)
            return false;
        return true;
    }

    bool operator!=(const GatewayrecvResp& rhs) const{
        return !(*this == rhs);
    }

    size_t operator()(const GatewayrecvResp& rhs) const{
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
    soce::crpc::CrpcRespHeader header;

    /* for optional */
    bitset<1> attrs_ = 0;
    bitset<1> status_;

public:
    static GatewayrecvRespTypeTree s_type_tree;
};

struct hash_GatewayrecvResp
{
    size_t operator()(const GatewayrecvResp& rhs) const{
        return rhs(rhs);
    }
};

class GatewayrecvReqTypeTree{
public:
    GatewayrecvReqTypeTree(soce::proto::TypeTree& type_tree);
};

class GatewayrecvReq{
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
    GatewayrecvReq() = default;
    GatewayrecvReq(const GatewayrecvReq&) = default;
    GatewayrecvReq& operator=(const GatewayrecvReq& rhs) = default;
    bool operator==(const GatewayrecvReq& rhs) const{
        if (header != rhs.header)
            return false;
        if (msg != rhs.msg)
            return false;
        return true;
    }

    bool operator!=(const GatewayrecvReq& rhs) const{
        return !(*this == rhs);
    }

    size_t operator()(const GatewayrecvReq& rhs) const{
        size_t hash_code = 0;
        hash_code = 0
            ^ header(header)
            ^ std::hash<string>()(msg);
        return hash_code;
    }

    size_t read(soce::proto::ProtoIf* proto, bool required, bool with_type);
    size_t write(soce::proto::ProtoIf* proto, bool required, bool has_set, bool with_type = true) const;
    size_t deserialize(soce::proto::ProtoIf* proto);
    size_t serialize(soce::proto::ProtoIf* proto) const;
    static soce::proto::TypeTree* get_type_tree();

private:
    /* class members */
    soce::crpc::CrpcReqHeader header;
    string msg;

    /* for optional */
    bitset<2> attrs_ = 0;
    bitset<2> status_;

public:
    static GatewayrecvReqTypeTree s_type_tree;
};

struct hash_GatewayrecvReq
{
    size_t operator()(const GatewayrecvReq& rhs) const{
        return rhs(rhs);
    }
};

}}//end namespace

#endif

