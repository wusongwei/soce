#include "echo-service.h"
#include "echo.h"
#include "proto/binary-proto.h"

namespace crpc{ namespace sample{ 

int GatewayIf::set_recv_filter(const std::string& filter)
{
    return add_filter(GatewayrecvReq::get_type_tree(), "Gateway", "recv", filter);
}

std::string GatewayIf::recv_handler(const std::string& data){
    std::string rc;
    GatewayrecvReq req;
    soce::proto::BinaryProto bp;
    bp.init(const_cast<char*>(data.c_str() + 4), data.size() - 4);
    if (req.deserialize((soce::proto::ProtoIf*)&bp) == 0){
        return rc;
    }

    GatewayrecvResp resp;
    resp.mutable_header().set_type(soce::crpc::kRpcTypeResp);
    resp.mutable_header().set_req_id(req.get_header().get_req_id());
    resp.mutable_header().set_tid(req.get_header().get_tid());
    resp.mutable_header().set_status(soce::crpc::kRpcStatusOK);
    rc = SInterceptor.do_before_itcptor(req.mutable_header(), "Gateway", "recv", false);
    if (rc.empty()){
        raw_req_ = (void*)&req;
        recv(req.get_msg());
        raw_req_ = NULL;
        rc = SInterceptor.do_after_itcptor(resp.mutable_header(), "Gateway", "recv", false);
        if (rc.empty()){
            soce::proto::BinaryProto out;
            if (resp.serialize((soce::proto::ProtoIf*)&out)){
                rc.assign(out.data(), out.size());
            }
        }
    }
    return std::move(rc);
}

std::string EchoIf::do_uncondcast_test_handler(const std::string& data){
    std::string rc;
    Echodo_uncondcast_testReq req;
    soce::proto::BinaryProto bp;
    bp.init(const_cast<char*>(data.c_str() + 4), data.size() - 4);
    if (req.deserialize((soce::proto::ProtoIf*)&bp) == 0){
        return rc;
    }

    Echodo_uncondcast_testResp resp;
    resp.mutable_header().set_type(soce::crpc::kRpcTypeResp);
    resp.mutable_header().set_req_id(req.get_header().get_req_id());
    resp.mutable_header().set_tid(req.get_header().get_tid());
    resp.mutable_header().set_status(soce::crpc::kRpcStatusOK);
    rc = SInterceptor.do_before_itcptor(req.mutable_header(), "Echo", "do_uncondcast_test", false);
    if (rc.empty()){
        raw_req_ = (void*)&req;
        do_uncondcast_test(req.get_p());
        raw_req_ = NULL;
        rc = SInterceptor.do_after_itcptor(resp.mutable_header(), "Echo", "do_uncondcast_test", false);
        if (rc.empty()){
            soce::proto::BinaryProto out;
            if (resp.serialize((soce::proto::ProtoIf*)&out)){
                rc.assign(out.data(), out.size());
            }
        }
    }
    return std::move(rc);
}

int EchoIf::set_echo_filter(const std::string& filter)
{
    return add_filter(EchoechoReq::get_type_tree(), "Echo", "echo", filter);
}

std::string EchoIf::echo_handler(const std::string& data){
    std::string rc;
    EchoechoReq req;
    soce::proto::BinaryProto bp;
    bp.init(const_cast<char*>(data.c_str() + 4), data.size() - 4);
    if (req.deserialize((soce::proto::ProtoIf*)&bp) == 0){
        return rc;
    }

    EchoechoResp resp;
    resp.mutable_header().set_type(soce::crpc::kRpcTypeResp);
    resp.mutable_header().set_req_id(req.get_header().get_req_id());
    resp.mutable_header().set_tid(req.get_header().get_tid());
    resp.mutable_header().set_status(soce::crpc::kRpcStatusOK);
    rc = SInterceptor.do_before_itcptor(req.mutable_header(), "Echo", "echo", false);
    if (rc.empty()){
        raw_req_ = (void*)&req;
        resp.mutable_result() = echo(req.get_p());
        raw_req_ = NULL;
        rc = SInterceptor.do_after_itcptor(resp.mutable_header(), "Echo", "echo", false);
        if (rc.empty()){
            soce::proto::BinaryProto out;
            if (resp.serialize((soce::proto::ProtoIf*)&out)){
                rc.assign(out.data(), out.size());
            }
        }
    }
    return std::move(rc);
}

int EchoIf::set_do_null_test_filter(const std::string& filter)
{
    return add_filter(Echodo_null_testReq::get_type_tree(), "Echo", "do_null_test", filter);
}

std::string EchoIf::do_null_test_handler(const std::string& data){
    std::string rc;
    Echodo_null_testReq req;
    soce::proto::BinaryProto bp;
    bp.init(const_cast<char*>(data.c_str() + 4), data.size() - 4);
    if (req.deserialize((soce::proto::ProtoIf*)&bp) == 0){
        return rc;
    }

    rc = SInterceptor.do_before_itcptor(req.mutable_header(), "Echo", "do_null_test", false);
    if (rc.empty()){
        raw_req_ = (void*)&req;
        do_null_test(req.get_p());
        raw_req_ = NULL;
    }
    rc.clear();
    return std::move(rc);
}

int EchoIf::set_do_void_test_filter(const std::string& filter)
{
    return add_filter(Echodo_void_testReq::get_type_tree(), "Echo", "do_void_test", filter);
}

std::string EchoIf::do_void_test_handler(const std::string& data){
    std::string rc;
    Echodo_void_testReq req;
    soce::proto::BinaryProto bp;
    bp.init(const_cast<char*>(data.c_str() + 4), data.size() - 4);
    if (req.deserialize((soce::proto::ProtoIf*)&bp) == 0){
        return rc;
    }

    Echodo_void_testResp resp;
    resp.mutable_header().set_type(soce::crpc::kRpcTypeResp);
    resp.mutable_header().set_req_id(req.get_header().get_req_id());
    resp.mutable_header().set_tid(req.get_header().get_tid());
    resp.mutable_header().set_status(soce::crpc::kRpcStatusOK);
    rc = SInterceptor.do_before_itcptor(req.mutable_header(), "Echo", "do_void_test", false);
    if (rc.empty()){
        raw_req_ = (void*)&req;
        do_void_test(req.get_p());
        raw_req_ = NULL;
        rc = SInterceptor.do_after_itcptor(resp.mutable_header(), "Echo", "do_void_test", false);
        if (rc.empty()){
            soce::proto::BinaryProto out;
            if (resp.serialize((soce::proto::ProtoIf*)&out)){
                rc.assign(out.data(), out.size());
            }
        }
    }
    return std::move(rc);
}

int EchoIf::set_do_condcast_test_filter(const std::string& filter)
{
    return add_filter(Echodo_condcast_testReq::get_type_tree(), "Echo", "do_condcast_test", filter);
}

std::string EchoIf::do_condcast_test_handler(const std::string& data){
    std::string rc;
    Echodo_condcast_testReq req;
    soce::proto::BinaryProto bp;
    bp.init(const_cast<char*>(data.c_str() + 4), data.size() - 4);
    if (req.deserialize((soce::proto::ProtoIf*)&bp) == 0){
        return rc;
    }

    Echodo_condcast_testResp resp;
    resp.mutable_header().set_type(soce::crpc::kRpcTypeResp);
    resp.mutable_header().set_req_id(req.get_header().get_req_id());
    resp.mutable_header().set_tid(req.get_header().get_tid());
    resp.mutable_header().set_status(soce::crpc::kRpcStatusOK);
    rc = SInterceptor.do_before_itcptor(req.mutable_header(), "Echo", "do_condcast_test", false);
    if (rc.empty()){
        raw_req_ = (void*)&req;
        do_condcast_test(req.get_p());
        raw_req_ = NULL;
        rc = SInterceptor.do_after_itcptor(resp.mutable_header(), "Echo", "do_condcast_test", false);
        if (rc.empty()){
            soce::proto::BinaryProto out;
            if (resp.serialize((soce::proto::ProtoIf*)&out)){
                rc.assign(out.data(), out.size());
            }
        }
    }
    return std::move(rc);
}


int GatewayCortClient::recv(const string& msg)
{
    soce::crpc::WorkerThread* _dthread = soce::crpc::WorkerThread::s_self;
    int64_t _req_id = _dthread->get_reqid();
    int64_t _tid = _dthread->get_tid();
    soce::crpc::RpcType _type = soce::crpc::kRpcTypeReq;
    GatewayrecvReq _req;
    _req.mutable_header().set_type(_type);
    _req.mutable_header().set_service("Gateway");
    _req.mutable_header().set_method("recv");
    _req.mutable_header().set_req_id(_req_id);
    _req.mutable_header().set_tid(_tid);
    if (!target_addr_.empty()){
        _req.mutable_header().set_target_addr(target_addr_);
    }
    _req.set_msg(msg);
    std::string rc = SInterceptor.do_before_itcptor(_req.mutable_header(), "Gateway", "recv", true);
    soce::proto::BinaryProto _bp;
    std::string* resp_str = nullptr;
    (void)resp_str;
    if (!rc.empty()){
        resp_str = &rc;
    }
    else{
        if (_req.serialize((soce::proto::ProtoIf*)&_bp) == 0){
            return soce::crpc::kReqProtoError;
        }
        _dthread->append_req(_req.get_header(), std::move(std::string(_bp.data(), _bp.size())));

        uint32_t _req_timeout = timeout_;
        auto _iter = method_timeout_.find("recv");
        if (_iter != method_timeout_.end()){
            _req_timeout = _iter->second;
        }
        soce::cortengine::CortEngine::CortId _id = SCortEngine.get_cur_cort_id();
        _dthread->add_request(_req_id, _id);
        SCortEngine.yield(_req_timeout);
        if(SCortEngine.is_timeout()){
            _dthread->del_request(_req_id);
            return soce::crpc::kCortTimeout;
        }
        _dthread = soce::crpc::WorkerThread::s_self;
        resp_str = _dthread->get_last_resp();
    }
    if (!resp_str || resp_str->empty()){
        return soce::crpc::kServerNotAvailable;
    }
    if (*resp_str == "BroadcastOk"){
        return soce::crpc::kOk;
    }
    if (*resp_str == "BroadcastError"){
        return soce::crpc::kBroadcastError;
    }
    _bp.reset();
    _bp.init(const_cast<char*>(resp_str->c_str() + 4), resp_str->size() - 4);
    GatewayrecvResp _resp;
    if (_resp.deserialize((soce::proto::ProtoIf*)&_bp) == 0){
        return soce::crpc::kRespProtoError;
    }
    SInterceptor.do_after_itcptor(_resp.mutable_header(), "Gateway", "recv", true);
    if (_resp.get_header().get_status() != soce::crpc::kRpcStatusOK){
        return _resp.get_header().get_status();
    }
    return soce::crpc::kOk;
}

int EchoCortClient::do_uncondcast_test(const int32_t& p)
{
    soce::crpc::WorkerThread* _dthread = soce::crpc::WorkerThread::s_self;
    int64_t _req_id = _dthread->get_reqid();
    int64_t _tid = _dthread->get_tid();
    soce::crpc::RpcType _type = soce::crpc::kRpcTypeReq;
    Echodo_uncondcast_testReq _req;
    _type = soce::crpc::kRpcTypeUncondCast;
    _req.mutable_header().set_type(_type);
    _req.mutable_header().set_service("Echo");
    _req.mutable_header().set_method("do_uncondcast_test");
    _req.mutable_header().set_req_id(_req_id);
    _req.mutable_header().set_tid(_tid);
    if (!target_addr_.empty()){
        _req.mutable_header().set_target_addr(target_addr_);
    }
    _req.set_p(p);
    std::string rc = SInterceptor.do_before_itcptor(_req.mutable_header(), "Echo", "do_uncondcast_test", true);
    soce::proto::BinaryProto _bp;
    std::string* resp_str = nullptr;
    (void)resp_str;
    if (!rc.empty()){
        resp_str = &rc;
    }
    else{
        if (_req.serialize((soce::proto::ProtoIf*)&_bp) == 0){
            return soce::crpc::kReqProtoError;
        }
        _dthread->append_req(_req.get_header(), std::move(std::string(_bp.data(), _bp.size())));

        uint32_t _req_timeout = timeout_;
        auto _iter = method_timeout_.find("do_uncondcast_test");
        if (_iter != method_timeout_.end()){
            _req_timeout = _iter->second;
        }
        soce::cortengine::CortEngine::CortId _id = SCortEngine.get_cur_cort_id();
        _dthread->add_request(_req_id, _id);
        SCortEngine.yield(_req_timeout);
        if(SCortEngine.is_timeout()){
            _dthread->del_request(_req_id);
            return soce::crpc::kCortTimeout;
        }
        _dthread = soce::crpc::WorkerThread::s_self;
        resp_str = _dthread->get_last_resp();
    }
    if (!resp_str || resp_str->empty()){
        return soce::crpc::kServerNotAvailable;
    }
    if (*resp_str == "BroadcastOk"){
        return soce::crpc::kOk;
    }
    if (*resp_str == "BroadcastError"){
        return soce::crpc::kBroadcastError;
    }
    _bp.reset();
    _bp.init(const_cast<char*>(resp_str->c_str() + 4), resp_str->size() - 4);
    Echodo_uncondcast_testResp _resp;
    if (_resp.deserialize((soce::proto::ProtoIf*)&_bp) == 0){
        return soce::crpc::kRespProtoError;
    }
    SInterceptor.do_after_itcptor(_resp.mutable_header(), "Echo", "do_uncondcast_test", true);
    if (_resp.get_header().get_status() != soce::crpc::kRpcStatusOK){
        return _resp.get_header().get_status();
    }
    return soce::crpc::kOk;
}

int EchoCortClient::echo(int32_t& _return, const int32_t& p)
{
    soce::crpc::WorkerThread* _dthread = soce::crpc::WorkerThread::s_self;
    int64_t _req_id = _dthread->get_reqid();
    int64_t _tid = _dthread->get_tid();
    soce::crpc::RpcType _type = soce::crpc::kRpcTypeReq;
    EchoechoReq _req;
    _req.mutable_header().set_type(_type);
    _req.mutable_header().set_service("Echo");
    _req.mutable_header().set_method("echo");
    _req.mutable_header().set_req_id(_req_id);
    _req.mutable_header().set_tid(_tid);
    if (!target_addr_.empty()){
        _req.mutable_header().set_target_addr(target_addr_);
    }
    _req.set_p(p);
    std::string rc = SInterceptor.do_before_itcptor(_req.mutable_header(), "Echo", "echo", true);
    soce::proto::BinaryProto _bp;
    std::string* resp_str = nullptr;
    (void)resp_str;
    if (!rc.empty()){
        resp_str = &rc;
    }
    else{
        if (_req.serialize((soce::proto::ProtoIf*)&_bp) == 0){
            return soce::crpc::kReqProtoError;
        }
        _dthread->append_req(_req.get_header(), std::move(std::string(_bp.data(), _bp.size())));

        uint32_t _req_timeout = timeout_;
        auto _iter = method_timeout_.find("echo");
        if (_iter != method_timeout_.end()){
            _req_timeout = _iter->second;
        }
        soce::cortengine::CortEngine::CortId _id = SCortEngine.get_cur_cort_id();
        _dthread->add_request(_req_id, _id);
        SCortEngine.yield(_req_timeout);
        if(SCortEngine.is_timeout()){
            _dthread->del_request(_req_id);
            return soce::crpc::kCortTimeout;
        }
        _dthread = soce::crpc::WorkerThread::s_self;
        resp_str = _dthread->get_last_resp();
    }
    if (!resp_str || resp_str->empty()){
        return soce::crpc::kServerNotAvailable;
    }
    if (*resp_str == "BroadcastOk"){
        return soce::crpc::kOk;
    }
    if (*resp_str == "BroadcastError"){
        return soce::crpc::kBroadcastError;
    }
    _bp.reset();
    _bp.init(const_cast<char*>(resp_str->c_str() + 4), resp_str->size() - 4);
    EchoechoResp _resp;
    if (_resp.deserialize((soce::proto::ProtoIf*)&_bp) == 0){
        return soce::crpc::kRespProtoError;
    }
    SInterceptor.do_after_itcptor(_resp.mutable_header(), "Echo", "echo", true);
    if (_resp.get_header().get_status() != soce::crpc::kRpcStatusOK){
        return _resp.get_header().get_status();
    }
    _return = _resp.get_result();
    return soce::crpc::kOk;
}

void EchoCortClient::do_null_test(const int32_t& p)
{
    soce::crpc::WorkerThread* _dthread = soce::crpc::WorkerThread::s_self;
    int64_t _req_id = _dthread->get_reqid();
    int64_t _tid = _dthread->get_tid();
    soce::crpc::RpcType _type = soce::crpc::kRpcTypeReq;
    Echodo_null_testReq _req;
    _req.mutable_header().set_type(_type);
    _req.mutable_header().set_service("Echo");
    _req.mutable_header().set_method("do_null_test");
    _req.mutable_header().set_req_id(_req_id);
    _req.mutable_header().set_tid(_tid);
    if (!target_addr_.empty()){
        _req.mutable_header().set_target_addr(target_addr_);
    }
    _req.set_p(p);
    std::string rc = SInterceptor.do_before_itcptor(_req.mutable_header(), "Echo", "do_null_test", true);
    soce::proto::BinaryProto _bp;
    std::string* resp_str = nullptr;
    (void)resp_str;
    if (!rc.empty()){
        resp_str = &rc;
    }
    else{
        if (_req.serialize((soce::proto::ProtoIf*)&_bp) == 0){
            return;
        }
        _dthread->append_req(_req.get_header(), std::move(std::string(_bp.data(), _bp.size())));

    }
}

int EchoCortClient::do_void_test(const int32_t& p)
{
    soce::crpc::WorkerThread* _dthread = soce::crpc::WorkerThread::s_self;
    int64_t _req_id = _dthread->get_reqid();
    int64_t _tid = _dthread->get_tid();
    soce::crpc::RpcType _type = soce::crpc::kRpcTypeReq;
    Echodo_void_testReq _req;
    _req.mutable_header().set_type(_type);
    _req.mutable_header().set_service("Echo");
    _req.mutable_header().set_method("do_void_test");
    _req.mutable_header().set_req_id(_req_id);
    _req.mutable_header().set_tid(_tid);
    if (!target_addr_.empty()){
        _req.mutable_header().set_target_addr(target_addr_);
    }
    _req.set_p(p);
    std::string rc = SInterceptor.do_before_itcptor(_req.mutable_header(), "Echo", "do_void_test", true);
    soce::proto::BinaryProto _bp;
    std::string* resp_str = nullptr;
    (void)resp_str;
    if (!rc.empty()){
        resp_str = &rc;
    }
    else{
        if (_req.serialize((soce::proto::ProtoIf*)&_bp) == 0){
            return soce::crpc::kReqProtoError;
        }
        _dthread->append_req(_req.get_header(), std::move(std::string(_bp.data(), _bp.size())));

        uint32_t _req_timeout = timeout_;
        auto _iter = method_timeout_.find("do_void_test");
        if (_iter != method_timeout_.end()){
            _req_timeout = _iter->second;
        }
        soce::cortengine::CortEngine::CortId _id = SCortEngine.get_cur_cort_id();
        _dthread->add_request(_req_id, _id);
        SCortEngine.yield(_req_timeout);
        if(SCortEngine.is_timeout()){
            _dthread->del_request(_req_id);
            return soce::crpc::kCortTimeout;
        }
        _dthread = soce::crpc::WorkerThread::s_self;
        resp_str = _dthread->get_last_resp();
    }
    if (!resp_str || resp_str->empty()){
        return soce::crpc::kServerNotAvailable;
    }
    if (*resp_str == "BroadcastOk"){
        return soce::crpc::kOk;
    }
    if (*resp_str == "BroadcastError"){
        return soce::crpc::kBroadcastError;
    }
    _bp.reset();
    _bp.init(const_cast<char*>(resp_str->c_str() + 4), resp_str->size() - 4);
    Echodo_void_testResp _resp;
    if (_resp.deserialize((soce::proto::ProtoIf*)&_bp) == 0){
        return soce::crpc::kRespProtoError;
    }
    SInterceptor.do_after_itcptor(_resp.mutable_header(), "Echo", "do_void_test", true);
    if (_resp.get_header().get_status() != soce::crpc::kRpcStatusOK){
        return _resp.get_header().get_status();
    }
    return soce::crpc::kOk;
}

int EchoCortClient::do_condcast_test(const int32_t& p)
{
    soce::crpc::WorkerThread* _dthread = soce::crpc::WorkerThread::s_self;
    int64_t _req_id = _dthread->get_reqid();
    int64_t _tid = _dthread->get_tid();
    soce::crpc::RpcType _type = soce::crpc::kRpcTypeReq;
    Echodo_condcast_testReq _req;
    _type = soce::crpc::kRpcTypeCondCast;
    _req.mutable_header().set_type(_type);
    _req.mutable_header().set_service("Echo");
    _req.mutable_header().set_method("do_condcast_test");
    _req.mutable_header().set_req_id(_req_id);
    _req.mutable_header().set_tid(_tid);
    if (!target_addr_.empty()){
        _req.mutable_header().set_target_addr(target_addr_);
    }
    _req.set_p(p);
    std::string rc = SInterceptor.do_before_itcptor(_req.mutable_header(), "Echo", "do_condcast_test", true);
    soce::proto::BinaryProto _bp;
    std::string* resp_str = nullptr;
    (void)resp_str;
    if (!rc.empty()){
        resp_str = &rc;
    }
    else{
        if (_req.serialize((soce::proto::ProtoIf*)&_bp) == 0){
            return soce::crpc::kReqProtoError;
        }
        _dthread->append_req(_req.get_header(), std::move(std::string(_bp.data(), _bp.size())));

        uint32_t _req_timeout = timeout_;
        auto _iter = method_timeout_.find("do_condcast_test");
        if (_iter != method_timeout_.end()){
            _req_timeout = _iter->second;
        }
        soce::cortengine::CortEngine::CortId _id = SCortEngine.get_cur_cort_id();
        _dthread->add_request(_req_id, _id);
        SCortEngine.yield(_req_timeout);
        if(SCortEngine.is_timeout()){
            _dthread->del_request(_req_id);
            return soce::crpc::kCortTimeout;
        }
        _dthread = soce::crpc::WorkerThread::s_self;
        resp_str = _dthread->get_last_resp();
    }
    if (!resp_str || resp_str->empty()){
        return soce::crpc::kServerNotAvailable;
    }
    if (*resp_str == "BroadcastOk"){
        return soce::crpc::kOk;
    }
    if (*resp_str == "BroadcastError"){
        return soce::crpc::kBroadcastError;
    }
    _bp.reset();
    _bp.init(const_cast<char*>(resp_str->c_str() + 4), resp_str->size() - 4);
    Echodo_condcast_testResp _resp;
    if (_resp.deserialize((soce::proto::ProtoIf*)&_bp) == 0){
        return soce::crpc::kRespProtoError;
    }
    SInterceptor.do_after_itcptor(_resp.mutable_header(), "Echo", "do_condcast_test", true);
    if (_resp.get_header().get_status() != soce::crpc::kRpcStatusOK){
        return _resp.get_header().get_status();
    }
    return soce::crpc::kOk;
}

}}//end namespace

