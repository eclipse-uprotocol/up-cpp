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

#include "up-cpp/communication/RpcClient.h"

#include <chrono>
#include <queue>

namespace {
namespace detail {

using namespace uprotocol;

struct PendingRequest {
	std::chrono::steady_clock::time_point when_expire;
	std::function<void(communication::RpcClient::Status)> expire;
	size_t instance_id;

	auto operator<(const PendingRequest& other) const;
};

struct ScrubablePendingQueue : public std::priority_queue<PendingRequest> {
	~ScrubablePendingQueue();
	auto scrub(size_t instance_id);
};

struct ExpireWorker {
	ExpireWorker();
	~ExpireWorker();
	void enqueue(PendingRequest&& request);
	void scrub(size_t instance_id);
	void doWork();

private:
	std::mutex pending_mtx_;
	ScrubablePendingQueue pending_;
	std::thread worker_;
	std::atomic<bool> stop_{false};
	std::condition_variable wake_worker_;
};

}  // namespace detail
}  // namespace

namespace uprotocol::communication {

////////////////////////////////////////////////////////////////////////////////
struct RpcClient::ExpireService {
	ExpireService() : instance_id_(next_instance_id++) {}

	~ExpireService() {}

	void enqueue(std::chrono::steady_clock::time_point when_expire,
	             std::function<void(RpcClient::Status)> expire) {}

private:
	static inline std::atomic<size_t> next_instance_id{0};
	static inline detail::ExpireWorker worker;
	size_t instance_id_;
};

////////////////////////////////////////////////////////////////////////////////
RpcClient::RpcClient(std::shared_ptr<transport::UTransport> transport,
                     v1::UUri&& method, v1::UPriority priority,
                     std::chrono::milliseconds ttl,
                     std::optional<v1::UPayloadFormat> payload_format,
                     std::optional<uint32_t> permission_level,
                     std::optional<std::string> token)
    : transport_(transport),
      builder_(datamodel::builder::UMessageBuilder::request(
          std::move(method), v1::UUri(transport_->getDefaultSource()), priority,
          ttl)),
      expire_service_(std::make_unique<ExpireService>()) {}

void RpcClient::invokeMethod(v1::UMessage&& request, Callback&& callback) {}

void RpcClient::invokeMethod(datamodel::builder::Payload&& payload,
                             Callback&& callback) {}

void RpcClient::invokeMethod(Callback&& callback) {}

std::future<RpcClient::MessageOrStatus> RpcClient::invokeMethod(
    datamodel::builder::Payload&& payload) {
	std::promise<MessageOrStatus> promise;
	auto future = promise.get_future();

	return future;
}

std::future<RpcClient::MessageOrStatus> RpcClient::invokeMethod() {
	std::promise<MessageOrStatus> promise;
	auto future = promise.get_future();

	return future;
}

RpcClient::RpcClient(RpcClient&&) = default;
RpcClient::~RpcClient() = default;

}  // namespace uprotocol::communication

///////////////////////////////////////////////////////////////////////////////
namespace {
namespace detail {

using namespace uprotocol;
using namespace std::chrono_literals;

auto PendingRequest::operator<(const PendingRequest& other) const {
	return when_expire < other.when_expire;
}

ScrubablePendingQueue::~ScrubablePendingQueue() {}

auto ScrubablePendingQueue::scrub(size_t instance_id) {}

ExpireWorker::ExpireWorker() {}

ExpireWorker::~ExpireWorker() {}

void ExpireWorker::enqueue(PendingRequest&& request) {}

void ExpireWorker::scrub(size_t instance_id) {}

void ExpireWorker::doWork() {}

}  // namespace detail
}  // namespace
