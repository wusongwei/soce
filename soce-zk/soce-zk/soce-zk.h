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

#ifndef _SOCE_ZKCPP_a8348Uzexy_H_
#define _SOCE_ZKCPP_a8348Uzexy_H_

#include <string>
#include <vector>
#include <utility>
#include <memory>
#include "types.h"

namespace soce{
namespace zookeeper{

    class SoceZk
    {
    public:
        using Errno = enum{
            OK = 0,
            kZkError = -1,
            kCrtEvtfd = -10,
            kInvalidHandler = -11,
            kTimeout = -12,
            kNoNode = -20,
            kCrtDir = -30,
            kNodeExist = -31,
        };

    public:
        /*
         * Connect to Zookeeper server synchronously.
         *
         * @param addr The address list, like ip:port,ip:port.
         * @param timeout Timeout.
         *
         * @return OK Success
         *         kCrtEvtfd Failed to create eventfd for event notification.
         *         kInvalidHandler Failed to create handler.
         *         kTimeout Timeout.
         */
        int init(const std::string& addr, uint32_t timeout);

        /*
         * Create a node synchronously.
         *
         * @param path The path of the node.
         * @param value The value of the node.
         * @param mode Create mode
         *             kCrtModePresistent Create a presistent node.
         *             kCrtModeEphemeral Create an ephemeral node.
         *             kCrtModeSequence Create a sequential presistent node.
         *             kCrtModeEphSeq Create a sequential ephemeral node.
         *
         * @return OK Success
         *         kCrtDir Failed to create the parent node.
         *         kInvalidHandler Handler is NULL.
         *         kNodeExist No such node.
         *         kZkError zoo_create() failed for other reason.
         */
        int create(const std::string& path,
                   const std::string& value,
                   CreateMode mode);
        /*
         * Delete a node synchronously.
         *
         * @param path The path of the node.
         *
         * @return OK Success
         *         kZkError zoo_delete() failed.
         */
        int del(const std::string& path);

        /*
         * Check the existence of a node synchronously.
         *
         * @param path The path of the node.
         * @param mode Watch mode
         *             kWatchModeNone Do not watch.
         *             kWatchModeOnce Watch once.
         *             kWatchModeRepeat Watch forever.
         * @param exist The result.
         *
         * @return OK Success
         *         kInvalidHandler Handler is NULL.
         *         kNoNode No such node.
         *         kZkError zoo_exist() failed for other reason.
         */
        int exist(const std::string& path, WatchMode mode, bool& exist);

        /*
         * Get the content of a node synchronously.
         *
         * @param path The path of the node.
         * @param mode Watch mode
         *             kWatchModeNone Do not watch.
         *             kWatchModeOnce Watch once.
         *             kWatchModeRepeat Watch forever.
         * @param value The result.
         *
         * @return OK Success
         *         kInvalidHandler Handler is NULL.
         *         kNoNode No such node.
         *         kZkError zoo_get() failed for other reason.
         */
        int get(const std::string& path, WatchMode mode, std::string& value);

        /*
         * Set the content of a node synchronously.
         *
         * @param path The path of the node.
         * @param value The content to set.
         *
         * @return OK Success
         *         kInvalidHandler Handler is NULL.
         *         kNoNode No such node.
         *         kZkError zoo_set() failed for other reason.
         */
        int set(const std::string& path, const std::string& value);

        /*
         * Get the children list (and content) of a node synchronously.
         * Only the children change will trigger the watch.
         *
         * @param path The path of the node.
         * @param mode Watch mode
         *             kWatchModeNone Do not watch.
         *             kWatchModeOnce Watch once.
         *             kWatchModeRepeat Watch forever.
         * @param value The result.
         *
         * @return OK Success
         *         kInvalidHandler Handler is NULL.
         *         kNoNode No such node.
         *         kZkError zoo_get_children() failed for other reason.
         */
        int get_children(const std::string& path, WatchMode mode, std::vector<std::string>& value);
        int get_children_content(const std::string& path,
                                 WatchMode mode,
                                 std::vector<std::pair<std::string, std::string>>& value);

        /*
         * Get the eventfd which is used to notify the main thread that some event happend.
         *
         * @return The eventfd.
         */
        int get_notify_evtfd();

        /*
         * Get the event detail.
         *
         * @param notify The event detail.
         */
        void get_notifies(std::vector<NotifyInfo>& notifies);

    public:
        class SoceZkImpl;

    private:
        std::shared_ptr<SoceZkImpl> impl_;
    };

}} // end namespace

#endif
