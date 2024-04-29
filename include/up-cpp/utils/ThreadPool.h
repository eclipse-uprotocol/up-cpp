/*
 * Copyright (c) 2023 General Motors GTO LLC
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 * SPDX-FileType: SOURCE
 * SPDX-FileCopyrightText: 2023 General Motors GTO LLC
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <functional>
#include <future>
#include <thread>
#include <vector>
#include <atomic>
#include <mutex>
#include <spdlog/spdlog.h>
#include <up-cpp/utils/CyclicQueue.h>

using namespace std;

namespace uprotocol::utils {

    class ThreadPool
    {
        public:

            ThreadPool(const ThreadPool &) = delete;
            ThreadPool(ThreadPool &&) = delete;

            ThreadPool & operator=(const ThreadPool &) = delete;
            ThreadPool & operator=(ThreadPool &&) = delete;

            ThreadPool(const size_t maxQueueSize,
                    const size_t maxNumOfThreads);

            ~ThreadPool();

            static void worker(
                    CyclicQueue<std::function<void()>> &queue,
                    bool &terminate);

            // Submit a function to be executed asynchronously by the pool
            template<typename F, typename...Args>
                auto submit(F&& f, Args&&... args) -> std::future<decltype(f(args...))>;

        private:

            CyclicQueue<std::function<void()>> queue_;
            bool terminate_;
            size_t maxNumOfThreads_;
            std::atomic<std::size_t> numOfThreads_;
            std::vector<std::future<void>> threads_;
            std::mutex mutex_;
            static constexpr auto timeoutMillisec_ = 100;
    };
}

#endif //THREADPOOL_H
