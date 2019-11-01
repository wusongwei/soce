/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements. See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership. The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#include <errno.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "crpc/client/base-sync-client.h"
#include "utils/time-helper.h"

namespace soce {
namespace crpc {

    BaseSyncClient::BaseSyncClient()
    {
        data_ = new char[cap_];
    }

    BaseSyncClient::~BaseSyncClient()
    {
        if (fd_ > 0){
            close(fd_);
        }

        if (data_){
            delete [] data_;
        }
    }

    int BaseSyncClient::init(const std::string& host, int port)
    {
        host_ = host;
        port_ = port;
        return (port_ > 65535) ? -1 : 0;
    }

    RpcStatus BaseSyncClient::do_req(const std::string& service,
                                     const std::string& method,
                                     soce::crpc::RpcType type,
                                     int64_t req_id,
                                     soce::crpc::CrpcReqHeader& req_header,
                                     soce::fads::FadsMessage* req_msg,
                                     soce::crpc::CrpcRespHeader& resp_header,
                                     soce::fads::FadsMessage* resp_msg)
    {
        soce::proto::BinaryProto bp;
        RpcStatus status;

        init_req_header(req_header, service, method, type, req_id);
        int rc = do_before_itcptor(req_header, service, method, resp_msg, status);
        if (rc != 0){
            return status;
        }

        if (!target_server_.empty()) {
            req_header.set_target_addr(target_server_);
        }

        bp.reset();
        if (0 == req_msg->serialize(&bp)) {
            return kRpcStatusProtoError;
        }

        status = send_req(bp.data(), bp.size());
        if (status != kRpcStatusOK) {
            return status;
        }

        uint32_t timeout = get_timeout(method);
        status = recv_resp(timeout);
        if (status != kRpcStatusOK) {
            return status;
        }

        status = deserialize_resp(resp_msg, data_, size_);
        if (status != kRpcStatusOK) {
            return status;
        }

        return do_after_itcptor(resp_header, service, method);
    }

    RpcStatus BaseSyncClient::do_req(const std::string& service,
                                     const std::string& method,
                                     soce::crpc::RpcType type,
                                     int64_t req_id,
                                     soce::crpc::CrpcReqHeader& req_header,
                                     soce::fads::FadsMessage* req_msg)
    {
        soce::proto::BinaryProto bp;

        init_req_header(req_header, service, method, type, req_id);
        if (!target_server_.empty()) {
            req_header.set_target_addr(target_server_);
        }

        bp.reset();
        if (req_msg->serialize(&bp)) {
            return kRpcStatusProtoError;
        }

        return send_req(bp.data(), bp.size());
    }

    RpcStatus BaseSyncClient::send_req(const char* data, size_t len)
    {
        reset();

        RpcStatus status = do_connect();
        if (status != kRpcStatusOK){
            return status;
        }

        char buf[4];
        for (size_t i = 0; i<4; ++i){
            buf[i] = (len >> (3-i)*8) & 0xff;
        }
        status = send_data(buf, 4);
        if (status != kRpcStatusOK){
            return status;
        }

        return send_data(data, len);
    }

    RpcStatus BaseSyncClient::send_data(const char* data, size_t len)
    {
        size_t pos = 0;
        ssize_t s = 0;
        // 发送给服务器端，由于设置了非阻塞，部分错误是可以恢复的
        while (pos < len){
            s = write(fd_, data + pos, len - pos);
            if (s < 0 && (errno == EAGAIN || errno == EINTR)){
                continue;
            }
            pos += s;
            break;
        }

        return s <= 0 ? kRpcStatusIoError : kRpcStatusOK;
    }

    void BaseSyncClient::reset()
    {
        size_ = 0;
        remain_len_ = 0;
        state_ = kStateRecvHeader;
    }

    RpcStatus BaseSyncClient::recv_resp(uint32_t timeout)
    {
        RpcStatus status = kRpcStatusOK;
        size_t last_time = 0;
        struct timeval* ptv = NULL;
        struct timeval tv;

        while (1){
            fd_set readfds;
            FD_ZERO(&readfds);
            FD_SET(fd_, &readfds);

            if (timeout != 0){
                if (last_time == 0) {
                    last_time = soce::utils::TimeHelper::get_time_ms();
                }else{
                    size_t cur_time = soce::utils::TimeHelper::get_time_ms();
                    if (cur_time - last_time >= timeout){
                        return kRpcStatusTimeout;
                    }

                    size_t time_left = timeout - cur_time + last_time;
                    tv.tv_sec = time_left / 1000;
                    tv.tv_usec = (time_left % 1000) * 1000;
                    ptv = &tv;
                }
            }

            int n = select(fd_ + 1, &readfds, NULL, NULL, ptv);
            if (n >= 1) {
                status = do_recv_resp();
                if (status != kRpcStatusOK) {
                    break;
                }

                if (is_completed()){
                    status = kRpcStatusOK;
                    break;
                }else{
                    continue;
                }
            }else if(n == 0){
                status = kRpcStatusTimeout;
                break;
            }else{
                status = kRpcStatusIoError;
                break;
            }
        }

        return status;
    }

    RpcStatus BaseSyncClient::do_connect()
    {
        if (fd_ > 0) {
            return kRpcStatusOK;
        }

        struct sockaddr_in serverAddr;
        fd_ = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
        if (-1 == fd_){
            return kRpcStatusIoError;
        }

        memset(&serverAddr,0,sizeof(serverAddr));
        serverAddr.sin_family=AF_INET;
        serverAddr.sin_addr.s_addr=inet_addr(host_.c_str());
        serverAddr.sin_port=htons(port_);

        if(-1 == connect(fd_, (struct sockaddr*)&serverAddr, sizeof(serverAddr))) {
            return kRpcStatusIoError;
        }

        int fl = fcntl(fd_, F_GETFL);
        fcntl(fd_, F_SETFL,fl | O_NONBLOCK);

        return kRpcStatusOK;
    }

    RpcStatus BaseSyncClient::do_recv_resp()
    {
        if (state_ == kStateRecvHeader) {
            if (do_recv(4 - size_)) {
                return kRpcStatusIoError;
            }

            if (4 == size_) {
                state_ = kStateRecvBody;
            }
            remain_len_ = get_msg_len();
            if (4 + remain_len_ > cap_) {
                cap_ = 4 + remain_len_;
                char* data = new char[cap_];
                memcpy(data, data_, size_);
                delete [] data_;
                data_ = data;
            }
        }
        else {
            if (do_recv(remain_len_ + 4 - size_)) {
                return kRpcStatusIoError;
            }
        }
        return kRpcStatusOK;
    }

    int BaseSyncClient::do_recv(size_t total)
    {
        ssize_t s = read(fd_, data_ + size_, total);
        if (s > 0){
            size_ += s;
            return 0;
        }

        return (errno == EAGAIN || errno == EINTR) ? 0 : -1;
    }

    size_t BaseSyncClient::get_msg_len()
    {
        uint32_t size = 0;
        for (size_t i = 0; i < 4; ++i){
            size = (size << 8) | (*(data_ + i) & 0xFF);
        }

        return size;
    }

    bool BaseSyncClient::is_completed()
    {
        return (state_ == kStateRecvBody && size_ >= 4 + remain_len_) ? true : false;
    }
} // namespace crpc
} // namespace soce
