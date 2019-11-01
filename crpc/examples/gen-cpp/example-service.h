#ifndef _example_service_1907375978_H_
#define _example_service_1907375978_H_

#include "crpc/base-service.h"
#include "crpc/client/base-sync-client.h"
#include "crpc/client/base-cort-client.h"

namespace crpc{ namespace example{ 

class CortDemoIf: public soce::crpc::BaseService
{
public:
    CortDemoIf(){
        attrs_.set_name("CortDemo");
        soce::crpc::attr::MethodFilters filter;
        attrs_.mutable_methods()["test_uncondcast"] = filter;
        handlers_["test_uncondcast"] = std::bind(&CortDemoIf::test_uncondcast_handler, this, std::placeholders::_1);
        attrs_.mutable_methods()["echo"] = filter;
        handlers_["echo"] = std::bind(&CortDemoIf::echo_handler, this, std::placeholders::_1);
        attrs_.mutable_methods()["test_void"] = filter;
        handlers_["test_void"] = std::bind(&CortDemoIf::test_void_handler, this, std::placeholders::_1);
        attrs_.mutable_methods()["test_condcast"] = filter;
        handlers_["test_condcast"] = std::bind(&CortDemoIf::test_condcast_handler, this, std::placeholders::_1);
        attrs_.mutable_methods()["test_null"] = filter;
        handlers_["test_null"] = std::bind(&CortDemoIf::test_null_handler, this, std::placeholders::_1);
    }

    std::string test_uncondcast_handler(const std::string& data);
    virtual void test_uncondcast(const int32_t& p) = 0;

    int set_echo_filter(const std::string& filter);
    std::string echo_handler(const std::string& data);
    virtual int32_t echo(const int32_t& p) = 0;

    int set_test_void_filter(const std::string& filter);
    std::string test_void_handler(const std::string& data);
    virtual void test_void(const int32_t& p) = 0;

    int set_test_condcast_filter(const std::string& filter);
    std::string test_condcast_handler(const std::string& data);
    virtual void test_condcast(const int32_t& p) = 0;

    int set_test_null_filter(const std::string& filter);
    std::string test_null_handler(const std::string& data);
    virtual void test_null(const int32_t& p) = 0;

    virtual std::string get_name(){
        return "CortDemo";
    }

};

class SyncDemoIf: public soce::crpc::BaseService
{
public:
    SyncDemoIf(){
        attrs_.set_name("SyncDemo");
        soce::crpc::attr::MethodFilters filter;
        attrs_.mutable_methods()["hello"] = filter;
        handlers_["hello"] = std::bind(&SyncDemoIf::hello_handler, this, std::placeholders::_1);
    }

    int set_hello_filter(const std::string& filter);
    std::string hello_handler(const std::string& data);
    virtual string hello(const string& msg) = 0;

    virtual std::string get_name(){
        return "SyncDemo";
    }

};

class CortDemoSyncClient : public soce::crpc::BaseSyncClient
{
public:
    soce::crpc::RpcStatus test_uncondcast(const int32_t& p);
    soce::crpc::RpcStatus echo(int32_t& _result, const int32_t& p);
    soce::crpc::RpcStatus test_void(const int32_t& p);
    soce::crpc::RpcStatus test_condcast(const int32_t& p);
    void test_null(const int32_t& p);
    void set_test_uncondcast_timeout(uint32_t timeout)
    {
        method_timeout_["test_uncondcast"] = timeout;
    }

    void set_echo_timeout(uint32_t timeout)
    {
        method_timeout_["echo"] = timeout;
    }

    void set_test_void_timeout(uint32_t timeout)
    {
        method_timeout_["test_void"] = timeout;
    }

    void set_test_condcast_timeout(uint32_t timeout)
    {
        method_timeout_["test_condcast"] = timeout;
    }

    void set_test_null_timeout(uint32_t timeout)
    {
        method_timeout_["test_null"] = timeout;
    }

};

class SyncDemoSyncClient : public soce::crpc::BaseSyncClient
{
public:
    soce::crpc::RpcStatus hello(string& _result, const string& msg);
    void set_hello_timeout(uint32_t timeout)
    {
        method_timeout_["hello"] = timeout;
    }

};

class CortDemoCortClient : public soce::crpc::BaseCortClient
{
public:
    soce::crpc::RpcStatus test_uncondcast(const int32_t& p);
    soce::crpc::RpcStatus echo(int32_t& _result, const int32_t& p);
    soce::crpc::RpcStatus test_void(const int32_t& p);
    soce::crpc::RpcStatus test_condcast(const int32_t& p);
    void test_null(const int32_t& p);
    void set_test_uncondcast_timeout(uint32_t timeout)
    {
        method_timeout_["test_uncondcast"] = timeout;
    }

    void set_echo_timeout(uint32_t timeout)
    {
        method_timeout_["echo"] = timeout;
    }

    void set_test_void_timeout(uint32_t timeout)
    {
        method_timeout_["test_void"] = timeout;
    }

    void set_test_condcast_timeout(uint32_t timeout)
    {
        method_timeout_["test_condcast"] = timeout;
    }

    void set_test_null_timeout(uint32_t timeout)
    {
        method_timeout_["test_null"] = timeout;
    }

};

class SyncDemoCortClient : public soce::crpc::BaseCortClient
{
public:
    soce::crpc::RpcStatus hello(string& _result, const string& msg);
    void set_hello_timeout(uint32_t timeout)
    {
        method_timeout_["hello"] = timeout;
    }

};

}}//end namespace

#endif
