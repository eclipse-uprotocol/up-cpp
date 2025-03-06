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

#ifndef UP_CPP_UTILS_CYCLICQUEUE_H
#define UP_CPP_UTILS_CYCLICQUEUE_H

#include <condition_variable>
#include <mutex>
#include <queue>

namespace uprotocol::utils {

/// @brief Queue that enforces a maximum size by evicting the oldest entry to
///        make room for new ones.
template <typename T>
class CyclicQueue final {
public:
	explicit CyclicQueue(size_t max_size);

	CyclicQueue(const CyclicQueue&) = delete;
	CyclicQueue& operator=(const CyclicQueue&) = delete;

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
	bool tryPopUntil(T& popped_value,
	                 std::chrono::system_clock::time_point when) noexcept;

	size_t size() const noexcept;

	void clear() noexcept;

private:
	size_t queueMaxSize_;
	mutable std::mutex mutex_;
	std::condition_variable conditionVariable_;
	std::queue<T> queue_;
};

}  // namespace uprotocol::utils

#endif  // UP_CPP_UTILS_CYCLICQUEUE_H
