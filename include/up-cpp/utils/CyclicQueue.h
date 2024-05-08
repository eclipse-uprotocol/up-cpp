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

#ifndef UP_CPP_UTILS_CYCLICQUEUE_H
#define UP_CPP_UTILS_CYCLICQUEUE_H

#include <queue>
#include <mutex>
#include <condition_variable>

namespace uprotocol::utils {

/// @brief Queue that enforces a maximum size by evicting the oldest entry to
///        make room for new ones.
template<typename T>
class CyclicQueue final {
public:
	explicit CyclicQueue(const size_t max_size);

	CyclicQueue(const CyclicQueue&) = delete;
	CyclicQueue &operator=(const CyclicQueue&) = delete;

	virtual ~CyclicQueue() = default;

	void push(T&& data) noexcept;
	void push(const T& data) noexcept;

	bool isFull() const noexcept;
	bool isEmpty() const noexcept;

	// Blocking pop()
	bool pop(T& popped_value) noexcept;
	// Non-blocking pop()
	bool tryPop(T& popped_value) noexcept;
	// Time-limited blocking pop()s
	bool tryPopFor(T& popped_value, std::chrono::milliseconds limit) noexcept;
	bool tryPopUntil(T& popped_value, std::chrono::system_clock::time_point when) noexcept;

	size_t size() const noexcept;

	void clear() noexcept;

private:
	size_t queueMaxSize_;
	mutable std::mutex mutex_;
	std::condition_variable conditionVariable_;
	std::queue<T> queue_;
};

} // namespace uprotocol::utils

#endif // UP_CPP_UTILS_CYCLICQUEUE_H
