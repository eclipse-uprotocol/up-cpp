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

#ifndef __CYCLIC_QUEUE_HPP__
#define __CYCLIC_QUEUE_HPP__

#include <queue>
#include <mutex>
#include <condition_variable>

namespace uprotocol::utils {

	template<typename T>
	class CyclicQueue final
	{
	public:
		explicit CyclicQueue(
			const size_t maxSize,
			const std::chrono::milliseconds milliseconds) :
				queueMaxSize_{maxSize},
				milliseconds_{milliseconds} {}

		CyclicQueue(const CyclicQueue&) = delete;
		CyclicQueue &operator=(const CyclicQueue&) = delete;

		virtual ~CyclicQueue() = default;

		bool push(T& data) noexcept	{
			std::unique_lock<std::mutex> uniqueLock(mutex_);
			if (queueMaxSize_ == queue_.size()) {
				queue_.pop();
			}

			queue_.push(std::move(data));
			uniqueLock.unlock();

			conditionVariable_.notify_one();
			
			return true;
		}

		bool isFull(void) const noexcept {
			std::unique_lock<std::mutex> uniqueLock(mutex_);

			return queueMaxSize_ == queue_.size();
		}

		bool isEmpty(void) const noexcept {
			std::unique_lock<std::mutex> uniqueLock(mutex_);

			return queue_.empty();
		}

		bool waitPop(T& popped_value) noexcept {
			std::unique_lock<std::mutex> uniqueLock(mutex_);
			if (queue_.empty()) {
				conditionVariable_.wait_for(
					uniqueLock,
					milliseconds_);

				if (queue_.empty())	{
					return false;
				}
			}

			popped_value = std::move(queue_.front());
			queue_.pop();

			return true;
		}

		size_t size(void) const noexcept {
			std::unique_lock<std::mutex> uniqueLock(mutex_);
			return queue_.size();
		}

		void clear(void) noexcept {
			std::unique_lock<std::mutex> uniqueLock(mutex_);
			while (!queue_.empty()) {
				queue_.pop();
			}
		}

	private:
		static constexpr std::chrono::milliseconds DefaultPopQueueTimeout { 5U };

		size_t queueMaxSize_;
		mutable std::mutex mutex_;
		std::condition_variable conditionVariable_;
		std::chrono::milliseconds milliseconds_ { DefaultPopQueueTimeout };
		std::queue<T> queue_;
	};
}
#endif // __CYCLIC_QUEUE_HPP__