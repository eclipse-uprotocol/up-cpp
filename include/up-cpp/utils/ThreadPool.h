// Copyright (c) 2024 Contributors to the Eclipse Foundation                    
//                                                                              
// See the NOTICE file(s) distributed with this work for additional                
// information regarding copyright ownership.                                   
//                                                                              
// Licensed under the Apache License, Version 2.0 (the "License");                 
// you may not use this file except in compliance with the License.                
// You may obtain a copy of the License at                                      
//                                                                              
//     http://www.apache.org/licenses/LICENSE-2.0                               
//                                                                              
// Unless required by applicable law or agreed to in writing, software             
// distributed under the License is distributed on an "AS IS" BASIS,               
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.        
// See the License for the specific language governing permissions and            
// limitations under the License.                                               
//                                                                              
// SPDX-FileType: SOURCE                                                        
// SPDX-FileCopyrightText: 2024 Contributors to the Eclipse Foundation          
// SPDX-License-Identifier: Apache-2.0

#ifndef UP_CPP_UTILS_THREADPOOL_H
#define UP_CPP_UTILS_THREADPOOL_H

#include <functional>
#include <future>
#include <thread>
#include <vector>
#include <atomic>
#include <mutex>
#include <up-cpp/utils/CyclicQueue.h>
#include <chrono>

namespace uprotocol::utils {

    class ThreadPool
    {
        public:

            ThreadPool(const ThreadPool &) = delete;
            ThreadPool(ThreadPool &&) = delete;

            ThreadPool & operator=(const ThreadPool &) = delete;
            ThreadPool & operator=(ThreadPool &&) = delete;

            ThreadPool(const size_t max_queue_size,
                    const size_t max_num_of_threads,
					std::chrono::milliseconds task_timeout);

            ~ThreadPool();

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
            const std::chrono::milliseconds timeout_;
    };
} // namespace uprotocol::utils

#endif // UP_CPP_UTILS_THREADPOOL_H
