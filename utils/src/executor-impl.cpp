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

#include "executor-impl.h"
#include "transport/bypass-processor.h"

namespace soce {
namespace utils {

  ExecutorImpl::ExecutorImpl(size_t min_core,
                             size_t max_core,
                             size_t idle_sec,
                             size_t max_queue_size,
                             soce::transport::TransportIf* transport)
    : min_core_(min_core),
      max_core_(max_core),
      idle_sec_(idle_sec),
      max_queue_size_(max_queue_size),
      transport_(transport)
  {
    req_queue_.reset(new BlockingQueue<ExecutorRequest>(max_queue_size));
    transport_->watch(resp_queue_.get_consumer_fd(),
                      std::shared_ptr<soce::transport::BypassProcessor>(
                        new soce::transport::BypassProcessor(std::bind(&ExecutorImpl::handle_resp,
                                                                      this,
                                                                      std::placeholders::_1))
                        ));

    for (size_t i=0; i<min_core; ++i){
      add_worker();
    }
  }

  Executor::Status ExecutorImpl::submit(std::function<void()> func)
  {
    if (req_queue_->size() > min_core_){
      add_worker();
    }

    ExecutorRequest req;
    req.func = func;
    req.cort_id = SCortEngine.get_cur_cort_id();

    req_queue_->push(req);
    SCortEngine.yield();

    return Executor::Status::kOk;
  }

  Executor::Status ExecutorImpl::submit(std::function<void()> func, size_t tmout_ms)
  {
    if (req_queue_->size() > min_core_){
      add_worker();
    }

    ExecutorRequest req;
    req.func = func;
    req.cort_id = SCortEngine.get_cur_cort_id();

    uint64_t start_time = TimeHelper::get_time_ms();
    if (req_queue_->push(req, tmout_ms)){
      return Executor::Status::kTimeout;
    }

    uint64_t yield_timeout = TimeHelper::get_time_ms() - start_time;

    SCortEngine.yield(yield_timeout);
    return SCortEngine.is_timeout() ? Executor::Status::kTimeout : Executor::Status::kOk;
  }

  void ExecutorImpl::worker_entry(size_t worker_index, std::shared_ptr<BlockingQueue<ExecutorRequest>> req_queue)
  {
    uint64_t last_active = TimeHelper::get_time_ms();

    while (1) {
      ExecutorRequest req;
      uint64_t cur_time = TimeHelper::get_time_ms();

      if (req_queue->pop(req, 1000)){
        if (idle_sec_ != 0 && cur_time - last_active > idle_sec_ * 1000){
          detach(worker_index);
          break;
        }
      }else{
        last_active = cur_time;
        req.func();
        resp_queue_.produce(req.cort_id);
      }
    }
  }

  void ExecutorImpl::add_worker()
  {

    std::lock_guard<std::mutex> lck(mtx_);
    if (workers_.size() >= max_core_){
      return;
    }

    size_t worker_index = get_idle_worker_index();
    workers_[worker_index] = std::move(std::thread(&ExecutorImpl::worker_entry, this, worker_index, req_queue_));
  }

  size_t ExecutorImpl::get_idle_worker_index()
  {
    while (1){
      ++worker_index_;
      if (workers_.find(worker_index_) == workers_.end()){
        break;
      }
    }

    return worker_index_;
  }

  void ExecutorImpl::detach(size_t worker_index)
  {
    std::lock_guard<std::mutex> lck(mtx_);
    if (workers_.size() <= min_core_){
      return;
    }

    auto iter = workers_.find(worker_index);
    if (iter != workers_.end()){
      iter->second.detach();
      workers_.erase(iter);
    }
  }


  void ExecutorImpl::handle_resp(int fd)
  {
    (void) fd;

    soce::utils::DQVector<ExecutorResponse> resps;
    auto rc = resp_queue_.try_consume(resps);
    if (rc){
      return;
    }

    for (auto& i : resps) {
      SCortEngine.resume(i.cort_id);
    }
  }

} // namespace utils
} // namespace soce
