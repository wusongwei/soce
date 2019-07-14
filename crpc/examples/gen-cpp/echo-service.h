#ifndef _echo_service_123408386_H_
#define _echo_service_123408386_H_

#include "crpc/base-service.h"

namespace crpc{ namespace sample{ 

class GatewayIf: public soce::crpc::BaseService
{
public:
    GatewayIf(){
        attrs_.set_name("Gateway");
        soce::crpc::attr::MethodFilters filter;
        attrs_.mutable_methods()["recv"] = filter;
        handlers_["recv"] = std::bind(&GatewayIf::recv_handler, this, std::placeholders::_1);
    }

    int set_recv_filter(const std::string& filter);
    std::string recv_handler(const std::string& data);
    virtual void recv(const string& msg) = 0;

    virtual std::string get_name(){
        return "Gateway";
    }

};

class EchoIf: public soce::crpc::BaseService
{
public:
    EchoIf(){
        attrs_.set_name("Echo");
        soce::crpc::attr::MethodFilters filter;
        attrs_.mutable_methods()["do_uncondcast_test"] = filter;
        handlers_["do_uncondcast_test"] = std::bind(&EchoIf::do_uncondcast_test_handler, this, std::placeholders::_1);
        attrs_.mutable_methods()["do_condcast_test"] = filter;
        handlers_["do_condcast_test"] = std::bind(&EchoIf::do_condcast_test_handler, this, std::placeholders::_1);
        attrs_.mutable_methods()["do_null_test"] = filter;
        handlers_["do_null_test"] = std::bind(&EchoIf::do_null_test_handler, this, std::placeholders::_1);
        attrs_.mutable_methods()["do_void_test"] = filter;
        handlers_["do_void_test"] = std::bind(&EchoIf::do_void_test_handler, this, std::placeholders::_1);
        attrs_.mutable_methods()["echo"] = filter;
        handlers_["echo"] = std::bind(&EchoIf::echo_handler, this, std::placeholders::_1);
    }

    std::string do_uncondcast_test_handler(const std::string& data);
    virtual void do_uncondcast_test(const int32_t& p) = 0;

    int set_do_condcast_test_filter(const std::string& filter);
    std::string do_condcast_test_handler(const std::string& data);
    virtual void do_condcast_test(const int32_t& p) = 0;

    int set_do_null_test_filter(const std::string& filter);
    std::string do_null_test_handler(const std::string& data);
    virtual void do_null_test(const int32_t& p) = 0;

    int set_do_void_test_filter(const std::string& filter);
    std::string do_void_test_handler(const std::string& data);
    virtual void do_void_test(const int32_t& p) = 0;

    int set_echo_filter(const std::string& filter);
    std::string echo_handler(const std::string& data);
    virtual int32_t echo(const int32_t& p) = 0;

    virtual std::string get_name(){
        return "Echo";
    }

};

class GatewayCortClient
{
public:
    int recv(const string& msg);
    void set_timeout(uint32_t timeout){
        timeout_ = timeout;
    }

    void set_recv_timeout(uint32_t timeout){
        method_timeout_["recv"] = timeout;
    }

    void set_target_addr(const std::string& addr){
        target_addr_ = addr;    }

private:
    uint32_t timeout_ = 0;
    std::unordered_map<std::string, uint32_t> method_timeout_;
    std::string target_addr_;
};

class EchoCortClient
{
public:
    void do_uncondcast_test(const int32_t& p);
    void do_condcast_test(const int32_t& p);
    void do_null_test(const int32_t& p);
    int do_void_test(const int32_t& p);
    int echo(int32_t& _return, const int32_t& p);
    void set_timeout(uint32_t timeout){
        timeout_ = timeout;
    }

    void set_do_uncondcast_test_timeout(uint32_t timeout){
        method_timeout_["do_uncondcast_test"] = timeout;
    }

    void set_do_condcast_test_timeout(uint32_t timeout){
        method_timeout_["do_condcast_test"] = timeout;
    }

    void set_do_null_test_timeout(uint32_t timeout){
        method_timeout_["do_null_test"] = timeout;
    }

    void set_do_void_test_timeout(uint32_t timeout){
        method_timeout_["do_void_test"] = timeout;
    }

    void set_echo_timeout(uint32_t timeout){
        method_timeout_["echo"] = timeout;
    }

    void set_target_addr(const std::string& addr){
        target_addr_ = addr;    }

private:
    uint32_t timeout_ = 0;
    std::unordered_map<std::string, uint32_t> method_timeout_;
    std::string target_addr_;
};

}}//end namespace

#endif
