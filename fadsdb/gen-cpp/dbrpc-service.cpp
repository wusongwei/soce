#include "dbrpc-service.h"
#include "dbrpc.h"
#include "proto/binary-proto.h"

namespace soce{ namespace fadsdb{ 

int FadsDbServiceIf::set_exec_filter(const std::string& filter)
{
    return add_filter(FadsDbServiceexecReq::get_type_tree(), "FadsDbService", "exec", filter);
}

std::string FadsDbServiceIf::exec_handler(const std::string& data){
    FadsDbServiceexecReq req;
    FadsDbServiceexecResp resp;
    return handle_request(data, "FadsDbService", "exec", &req, req.mutable_header(), &resp, resp.mutable_header(), [&]{
        resp.mutable_result() = exec(req.get_id(), req.get_req());
    });
}


soce::crpc::RpcStatus FadsDbServiceSyncClient::exec(string& _result, const int64_t& id, const string& req)
{
    int64_t _req_id = 0;
    soce::crpc::RpcType _type = soce::crpc::kRpcTypeReq;
    FadsDbServiceexecReq _req;
    FadsDbServiceexecResp _resp;
    _req.set_id(id);
    _req.set_req(req);
    soce::crpc::RpcStatus status = do_req("FadsDbService", "exec", _type, _req_id, _req.mutable_header(), &_req, _resp.mutable_header(), &_resp);
    if (status != soce::crpc::kRpcStatusOK){
        return status;
    }

    _result = _resp.get_result();
    return soce::crpc::kRpcStatusOK;
}


soce::crpc::RpcStatus FadsDbServiceCortClient::exec(string& _result, const int64_t& id, const string& req)
{
    int64_t _req_id = 0;
    soce::crpc::RpcType _type = soce::crpc::kRpcTypeReq;
    FadsDbServiceexecReq _req;
    FadsDbServiceexecResp _resp;
    _req.set_id(id);
    _req.set_req(req);
    soce::crpc::RpcStatus status = do_req("FadsDbService", "exec", _type, _req_id, _req.mutable_header(), &_req, _resp.mutable_header(), &_resp);
    if (status != soce::crpc::kRpcStatusOK){
        return status;
    }

    _result = _resp.get_result();
    return soce::crpc::kRpcStatusOK;
}

}}//end namespace

