#include "example-service.h"
#include "example.h"
#include "proto/binary-proto.h"

namespace crpc{ namespace example{ 

std::string CortDemoIf::test_uncondcast_handler(const std::string& data){
    CortDemotest_uncondcastReq req;
    CortDemotest_uncondcastResp resp;
    return handle_request(data, "CortDemo", "test_uncondcast", &req, req.mutable_header(), &resp, resp.mutable_header(), [&]{
        test_uncondcast(req.get_p());
    });
}

int CortDemoIf::set_echo_filter(const std::string& filter)
{
    return add_filter(CortDemoechoReq::get_type_tree(), "CortDemo", "echo", filter);
}

std::string CortDemoIf::echo_handler(const std::string& data){
    CortDemoechoReq req;
    CortDemoechoResp resp;
    return handle_request(data, "CortDemo", "echo", &req, req.mutable_header(), &resp, resp.mutable_header(), [&]{
        resp.mutable_result() = echo(req.get_p());
    });
}

int CortDemoIf::set_test_void_filter(const std::string& filter)
{
    return add_filter(CortDemotest_voidReq::get_type_tree(), "CortDemo", "test_void", filter);
}

std::string CortDemoIf::test_void_handler(const std::string& data){
    CortDemotest_voidReq req;
    CortDemotest_voidResp resp;
    return handle_request(data, "CortDemo", "test_void", &req, req.mutable_header(), &resp, resp.mutable_header(), [&]{
        test_void(req.get_p());
    });
}

int CortDemoIf::set_test_condcast_filter(const std::string& filter)
{
    return add_filter(CortDemotest_condcastReq::get_type_tree(), "CortDemo", "test_condcast", filter);
}

std::string CortDemoIf::test_condcast_handler(const std::string& data){
    CortDemotest_condcastReq req;
    CortDemotest_condcastResp resp;
    return handle_request(data, "CortDemo", "test_condcast", &req, req.mutable_header(), &resp, resp.mutable_header(), [&]{
        test_condcast(req.get_p());
    });
}

int CortDemoIf::set_test_null_filter(const std::string& filter)
{
    return add_filter(CortDemotest_nullReq::get_type_tree(), "CortDemo", "test_null", filter);
}

std::string CortDemoIf::test_null_handler(const std::string& data){
    CortDemotest_nullReq req;
    return handle_request(data, &req, req.mutable_header(), [&]{
        test_null(req.get_p());
    });
}

int SyncDemoIf::set_hello_filter(const std::string& filter)
{
    return add_filter(SyncDemohelloReq::get_type_tree(), "SyncDemo", "hello", filter);
}

std::string SyncDemoIf::hello_handler(const std::string& data){
    SyncDemohelloReq req;
    SyncDemohelloResp resp;
    return handle_request(data, "SyncDemo", "hello", &req, req.mutable_header(), &resp, resp.mutable_header(), [&]{
        resp.mutable_result() = hello(req.get_msg());
    });
}


soce::crpc::RpcStatus CortDemoSyncClient::test_uncondcast(const int32_t& p)
{
    int64_t _req_id = 0;
    soce::crpc::RpcType _type = soce::crpc::kRpcTypeReq;
    CortDemotest_uncondcastReq _req;
    CortDemotest_uncondcastResp _resp;
    _type = soce::crpc::kRpcTypeUncondCast;
    _req.set_p(p);
    soce::crpc::RpcStatus status = do_req("CortDemo", "test_uncondcast", _type, _req_id, _req.mutable_header(), &_req, _resp.mutable_header(), &_resp);
    if (status != soce::crpc::kRpcStatusOK){
        return status;
    }

    return soce::crpc::kRpcStatusOK;
}

soce::crpc::RpcStatus CortDemoSyncClient::echo(int32_t& _result, const int32_t& p)
{
    int64_t _req_id = 0;
    soce::crpc::RpcType _type = soce::crpc::kRpcTypeReq;
    CortDemoechoReq _req;
    CortDemoechoResp _resp;
    _req.set_p(p);
    soce::crpc::RpcStatus status = do_req("CortDemo", "echo", _type, _req_id, _req.mutable_header(), &_req, _resp.mutable_header(), &_resp);
    if (status != soce::crpc::kRpcStatusOK){
        return status;
    }

    _result = _resp.get_result();
    return soce::crpc::kRpcStatusOK;
}

soce::crpc::RpcStatus CortDemoSyncClient::test_void(const int32_t& p)
{
    int64_t _req_id = 0;
    soce::crpc::RpcType _type = soce::crpc::kRpcTypeReq;
    CortDemotest_voidReq _req;
    CortDemotest_voidResp _resp;
    _req.set_p(p);
    soce::crpc::RpcStatus status = do_req("CortDemo", "test_void", _type, _req_id, _req.mutable_header(), &_req, _resp.mutable_header(), &_resp);
    if (status != soce::crpc::kRpcStatusOK){
        return status;
    }

    return soce::crpc::kRpcStatusOK;
}

soce::crpc::RpcStatus CortDemoSyncClient::test_condcast(const int32_t& p)
{
    int64_t _req_id = 0;
    soce::crpc::RpcType _type = soce::crpc::kRpcTypeReq;
    CortDemotest_condcastReq _req;
    CortDemotest_condcastResp _resp;
    _type = soce::crpc::kRpcTypeCondCast;
    _req.set_p(p);
    soce::crpc::RpcStatus status = do_req("CortDemo", "test_condcast", _type, _req_id, _req.mutable_header(), &_req, _resp.mutable_header(), &_resp);
    if (status != soce::crpc::kRpcStatusOK){
        return status;
    }

    return soce::crpc::kRpcStatusOK;
}

void CortDemoSyncClient::test_null(const int32_t& p)
{
    int64_t _req_id = 0;
    soce::crpc::RpcType _type = soce::crpc::kRpcTypeReq;
    CortDemotest_nullReq _req;
    _type = soce::crpc::kRpcTypeNullReq;
    _req.set_p(p);
    do_req("CortDemo", "test_null", _type, _req_id, _req.mutable_header(), &_req);
}

soce::crpc::RpcStatus SyncDemoSyncClient::hello(string& _result, const string& msg)
{
    int64_t _req_id = 0;
    soce::crpc::RpcType _type = soce::crpc::kRpcTypeReq;
    SyncDemohelloReq _req;
    SyncDemohelloResp _resp;
    _req.set_msg(msg);
    soce::crpc::RpcStatus status = do_req("SyncDemo", "hello", _type, _req_id, _req.mutable_header(), &_req, _resp.mutable_header(), &_resp);
    if (status != soce::crpc::kRpcStatusOK){
        return status;
    }

    _result = _resp.get_result();
    return soce::crpc::kRpcStatusOK;
}


soce::crpc::RpcStatus CortDemoCortClient::test_uncondcast(const int32_t& p)
{
    int64_t _req_id = 0;
    soce::crpc::RpcType _type = soce::crpc::kRpcTypeReq;
    CortDemotest_uncondcastReq _req;
    CortDemotest_uncondcastResp _resp;
    _type = soce::crpc::kRpcTypeUncondCast;
    _req.set_p(p);
    soce::crpc::RpcStatus status = do_req("CortDemo", "test_uncondcast", _type, _req_id, _req.mutable_header(), &_req, _resp.mutable_header(), &_resp);
    if (status != soce::crpc::kRpcStatusOK){
        return status;
    }

    return soce::crpc::kRpcStatusOK;
}

soce::crpc::RpcStatus CortDemoCortClient::echo(int32_t& _result, const int32_t& p)
{
    int64_t _req_id = 0;
    soce::crpc::RpcType _type = soce::crpc::kRpcTypeReq;
    CortDemoechoReq _req;
    CortDemoechoResp _resp;
    _req.set_p(p);
    soce::crpc::RpcStatus status = do_req("CortDemo", "echo", _type, _req_id, _req.mutable_header(), &_req, _resp.mutable_header(), &_resp);
    if (status != soce::crpc::kRpcStatusOK){
        return status;
    }

    _result = _resp.get_result();
    return soce::crpc::kRpcStatusOK;
}

soce::crpc::RpcStatus CortDemoCortClient::test_void(const int32_t& p)
{
    int64_t _req_id = 0;
    soce::crpc::RpcType _type = soce::crpc::kRpcTypeReq;
    CortDemotest_voidReq _req;
    CortDemotest_voidResp _resp;
    _req.set_p(p);
    soce::crpc::RpcStatus status = do_req("CortDemo", "test_void", _type, _req_id, _req.mutable_header(), &_req, _resp.mutable_header(), &_resp);
    if (status != soce::crpc::kRpcStatusOK){
        return status;
    }

    return soce::crpc::kRpcStatusOK;
}

soce::crpc::RpcStatus CortDemoCortClient::test_condcast(const int32_t& p)
{
    int64_t _req_id = 0;
    soce::crpc::RpcType _type = soce::crpc::kRpcTypeReq;
    CortDemotest_condcastReq _req;
    CortDemotest_condcastResp _resp;
    _type = soce::crpc::kRpcTypeCondCast;
    _req.set_p(p);
    soce::crpc::RpcStatus status = do_req("CortDemo", "test_condcast", _type, _req_id, _req.mutable_header(), &_req, _resp.mutable_header(), &_resp);
    if (status != soce::crpc::kRpcStatusOK){
        return status;
    }

    return soce::crpc::kRpcStatusOK;
}

void CortDemoCortClient::test_null(const int32_t& p)
{
    int64_t _req_id = 0;
    soce::crpc::RpcType _type = soce::crpc::kRpcTypeReq;
    CortDemotest_nullReq _req;
    _type = soce::crpc::kRpcTypeNullReq;
    _req.set_p(p);
    do_req("CortDemo", "test_null", _type, _req_id, _req.mutable_header(), &_req);
}

soce::crpc::RpcStatus SyncDemoCortClient::hello(string& _result, const string& msg)
{
    int64_t _req_id = 0;
    soce::crpc::RpcType _type = soce::crpc::kRpcTypeReq;
    SyncDemohelloReq _req;
    SyncDemohelloResp _resp;
    _req.set_msg(msg);
    soce::crpc::RpcStatus status = do_req("SyncDemo", "hello", _type, _req_id, _req.mutable_header(), &_req, _resp.mutable_header(), &_resp);
    if (status != soce::crpc::kRpcStatusOK){
        return status;
    }

    _result = _resp.get_result();
    return soce::crpc::kRpcStatusOK;
}

}}//end namespace

