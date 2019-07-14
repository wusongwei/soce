#ifndef _test_16210394_H_
#define _test_16210394_H_

#include <string>
#include <bitset>
#include <stdint.h>
#include "proto/soce-proto.h"
#include "proto/type-tree.h"

using std::string;
using std::bitset;

namespace fads{ 

class WorkerTypeTree{
public:
    WorkerTypeTree(soce::proto::TypeTree& type_tree);
};

class Worker{
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

    void set_age(const int32_t& _age){
        age = _age;
        status_[1] = true;
    }
    void set_age(int32_t&& _age){
        age = std::move(_age);
        status_[1] = true;
    }
    const int32_t& get_age() const {
        return age;
    }
    int32_t& mutable_age(){
        status_[1] = true;
        return age;
    }
    bool has_age() const{
        return status_[1];
    }

public:
    /* operators */
    Worker() = default;
    Worker(const Worker&) = default;
    Worker& operator=(const Worker& rhs) = default;
    bool operator==(const Worker& rhs) const{
        if (name != rhs.name)
            return false;
        if (age != rhs.age)
            return false;
        return true;
    }

    bool operator!=(const Worker& rhs) const{
        return !(*this == rhs);
    }

    size_t operator()(const Worker& rhs) const{
        size_t hash_code = 0;
        hash_code = 0
            ^ std::hash<string>()(name)
            ^ std::hash<int32_t>()(age);
        return hash_code;
    }

    size_t read(soce::proto::ProtoIf* proto, bool required, bool with_type);
    size_t write(soce::proto::ProtoIf* proto, bool required, bool has_set, bool with_type = true) const;
    size_t deserialize(soce::proto::ProtoIf* proto);
    size_t serialize(soce::proto::ProtoIf* proto) const;
    static soce::proto::TypeTree* get_type_tree();

private:
    /* class members */
    string name;
    int32_t age;

    /* for optional */
    bitset<2> attrs_ = 0;
    bitset<2> status_;

    public:
    static WorkerTypeTree s_type_tree;
};

struct hash_Worker
{
    size_t operator()(const Worker& rhs) const{
        return rhs(rhs);
    }
};

class companyTypeTree{
public:
    companyTypeTree(soce::proto::TypeTree& type_tree);
};

class company{
public:
    void set_CEO(const string& _CEO){
        CEO = _CEO;
        status_[0] = true;
    }
    void set_CEO(string&& _CEO){
        CEO = std::move(_CEO);
        status_[0] = true;
    }
    const string& get_CEO() const {
        return CEO;
    }
    string& mutable_CEO(){
        status_[0] = true;
        return CEO;
    }
    bool has_CEO() const{
        return status_[0];
    }

    void set_worker(const Worker& _worker){
        worker = _worker;
        status_[1] = true;
    }
    void set_worker(Worker&& _worker){
        worker = std::move(_worker);
        status_[1] = true;
    }
    const Worker& get_worker() const {
        return worker;
    }
    Worker& mutable_worker(){
        status_[1] = true;
        return worker;
    }
    bool has_worker() const{
        return status_[1];
    }

public:
    /* operators */
    company() = default;
    company(const company&) = default;
    company& operator=(const company& rhs) = default;
    bool operator==(const company& rhs) const{
        if (CEO != rhs.CEO)
            return false;
        if (worker != rhs.worker)
            return false;
        return true;
    }

    bool operator!=(const company& rhs) const{
        return !(*this == rhs);
    }

    size_t operator()(const company& rhs) const{
        size_t hash_code = 0;
        hash_code = 0
            ^ std::hash<string>()(CEO)
            ^ worker(worker);
        return hash_code;
    }

    size_t read(soce::proto::ProtoIf* proto, bool required, bool with_type);
    size_t write(soce::proto::ProtoIf* proto, bool required, bool has_set, bool with_type = true) const;
    size_t deserialize(soce::proto::ProtoIf* proto);
    size_t serialize(soce::proto::ProtoIf* proto) const;
    static soce::proto::TypeTree* get_type_tree();

private:
    /* class members */
    string CEO;
    Worker worker;

    /* for optional */
    bitset<2> attrs_ = 0;
    bitset<2> status_;

    public:
    static companyTypeTree s_type_tree;
};

struct hash_company
{
    size_t operator()(const company& rhs) const{
        return rhs(rhs);
    }
};

}//end namespace

#endif

