// SPDX-FileCopyrightText: 2024 Contributors to the Eclipse Foundation
//
// See the NOTICE file(s) distributed with this work for additional
// information regarding copyright ownership.
//
// This program and the accompanying materials are made available under the
// terms of the Apache License Version 2.0 which is available at
// https://www.apache.org/licenses/LICENSE-2.0
//
// SPDX-License-Identifier: Apache-2.0

#ifndef UP_CPP_UTILS_THREADPOOL_H
#define UP_CPP_UTILS_THREADPOOL_H

#include <up-cpp/utils/CyclicQueue.h>

#include <atomic>
#include <chrono>
#include <functional>
#include <future>
#include <mutex>
#include <thread>
#include <vector>

namespace uprotocol::utils {

class ThreadPool {
public:
	ThreadPool(const ThreadPool&) = delete;
	ThreadPool(ThreadPool&&) = delete;

	ThreadPool& operator=(const ThreadPool&) = delete;
	ThreadPool& operator=(ThreadPool&&) = delete;

	ThreadPool(size_t max_queue_size, size_t max_num_of_threads,
	           std::chrono::milliseconds task_timeout);

	~ThreadPool();

	// Submit a function to be executed asynchronously by the pool
	template <typename F, typename... Args>
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
}  // namespace uprotocol::utils

#endif  // UP_CPP_UTILS_THREADPOOL_H
