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
                       const size_t maxNumOfThreads)
                : queue_(maxQueueSize, 
                        std::chrono::milliseconds(timeout_)),
                terminate_(false),
                maxNumOfThreads_(maxNumOfThreads),
                numOfThreads_(0) {};
            
            ~ThreadPool() {

                terminate_ = true;

                /* wait for the threads to terminate*/
                for (size_t i = 0; i < threads_.size(); ++i) {
                    threads_[i].get();
                }
            }

            static void worker(CyclicQueue<std::function<void()>> &queue, 
                               bool &terminate) {

                std::function<void()> funcPtr;
                while (true == queue.waitPop(funcPtr) && (!terminate)) {
                    funcPtr();
                } 
            }

            // Submit a function to be executed asynchronously by the pool
            template<typename F, typename...Args>
            auto submit(F&& f, Args&&... args) -> std::future<decltype(f(args...))> {

                if (true == terminate_) {
                    spdlog::error("Thread pool is marked for termination");
                    return std::future<typename std::result_of<F(Args...)>::type>();
                }

                std::lock_guard<std::mutex> lock(mutex_);

                // Create a function with bounded parameters ready to execute
                std::function<decltype(f(args...))()> func = std::bind(std::forward<F>(f), 
                                                                       std::forward<Args>(args)...);

                // Encapsulate it into a shared ptr in order to be able to copy construct / assign 
                auto task_ptr = std::make_shared<std::packaged_task<decltype(f(args...))()>>(func);

                // Wrap packaged task into void function
                std::function<void()> wrapper_func = [task_ptr]() {
                    (*task_ptr)(); 
                };

                if (true == queue_.isFull()) {
                    spdlog::error("queue is full");
                    return std::future<typename std::result_of<F(Args...)>::type>();
                }

                if (false == queue_.push(wrapper_func)) {
                    spdlog::error("failed to push to queue");
                    return std::future<typename std::result_of<F(Args...)>::type>();
                }

                /* cleanup finished threads */
                for (size_t i = 0; i < threads_.size();) {
                    std::future_status status = threads_[i].wait_for(0ms);
                    if (std::future_status::ready == status) {
                        // Remove the thread from the vector
                        threads_.erase(threads_.begin() + i);
                        // Decrease the count of threads
                        numOfThreads_.fetch_sub(1);
                    } else {
                        ++i;
                    }
                }

                if (numOfThreads_.load() < maxNumOfThreads_) {
                    threads_.push_back(std::async(std::launch::async, worker, std::ref(queue_), std::ref(terminate_)));
                    numOfThreads_.fetch_add(1);
                }

                // Return future from promise
                return task_ptr->get_future();
            }

    private:

        CyclicQueue<std::function<void()>> queue_;
       
        bool terminate_;
        
        size_t maxNumOfThreads_;

        std::atomic<std::size_t> numOfThreads_;
    
        std::vector<std::future<void>> threads_;
        
        std::mutex mutex_;

        static constexpr auto timeout_ = 100;
    };
}

#endif //THREADPOOL_H