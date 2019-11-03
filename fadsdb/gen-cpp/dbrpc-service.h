#ifndef _dbrpc_service_23148924_H_
#define _dbrpc_service_23148924_H_

#include "crpc/base-service.h"
#include "crpc/client/base-sync-client.h"
#include "crpc/client/base-cort-client.h"

namespace soce{ namespace fadsdb{ 

class FadsDbServiceIf: public soce::crpc::BaseService
{
public:
    FadsDbServiceIf(){
        attrs_.set_name("FadsDbService");
        soce::crpc::attr::MethodFilters filter;
        attrs_.mutable_methods()["exec"] = filter;
        handlers_["exec"] = std::bind(&FadsDbServiceIf::exec_handler, this, std::placeholders::_1);
    }

    int set_exec_filter(const std::string& filter);
    std::string exec_handler(const std::string& data);
    virtual string exec(const int64_t& id, const string& req) = 0;

    virtual std::string get_name(){
        return "FadsDbService";
    }

};

class FadsDbServiceSyncClient : public soce::crpc::BaseSyncClient
{
public:
    soce::crpc::RpcStatus exec(string& _result, const int64_t& id, const string& req);
    void set_exec_timeout(uint32_t timeout)
    {
        method_timeout_["exec"] = timeout;
    }

};

class FadsDbServiceCortClient : public soce::crpc::BaseCortClient
{
public:
    soce::crpc::RpcStatus exec(string& _result, const int64_t& id, const string& req);
    void set_exec_timeout(uint32_t timeout)
    {
        method_timeout_["exec"] = timeout;
    }

};

}}//end namespace

#endif
