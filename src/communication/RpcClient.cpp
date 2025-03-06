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

#include <google/protobuf/util/message_differencer.h>

#include <chrono>
#include <queue>
#include <utility>

namespace {
namespace detail {

using uprotocol::v1::UStatus;
using ListenHandle = uprotocol::transport::UTransport::ListenHandle;

struct PendingRequest {
	friend struct ScrubablePendingQueue;
	friend struct ExpireWorker;

	PendingRequest(const std::chrono::steady_clock::time_point& when_expire,
		ListenHandle response_listener,
		std::function<void(UStatus)> expire,
		const size_t& instance_id)
		: when_expire_(when_expire),
		response_listener_(std::move(response_listener)),
		expire_(std::move(expire)),
		instance_id_(instance_id) {}

	auto operator>(const PendingRequest& other) const;
private:
	std::chrono::steady_clock::time_point when_expire_;
	ListenHandle response_listener_;
	std::function<void(UStatus)> expire_;
	size_t instance_id_{};
};


struct ScrubablePendingQueue
    : public std::priority_queue<PendingRequest, std::vector<PendingRequest>,
                                 std::greater<>> {
	~ScrubablePendingQueue();
	auto scrub(size_t instance_id);
	PendingRequest& top();
};

struct ExpireWorker {
	ExpireWorker();
	~ExpireWorker();
	void enqueue(PendingRequest&& pending);
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
	ExpireService() : instance_id_(next_instance_id_++) {}

	~ExpireService() { worker_.scrub(instance_id_); }

	void enqueue(std::chrono::steady_clock::time_point when_expire,
	             transport::UTransport::ListenHandle&& response_listener,
	             std::function<void(v1::UStatus)> expire) const {
		auto pending = detail::PendingRequest(when_expire,
			std::move(response_listener),
			std::move(expire),
			instance_id_);

		worker_.enqueue(std::move(pending));
	}

private:
	static inline std::atomic<size_t> next_instance_id_{0};
	//constructor for ExpireWorker can throw an exception when trying to create new thread
	//this can be problematic when used in a static constructor
	static inline detail::ExpireWorker worker_; //NOLINT
	size_t instance_id_;
};

////////////////////////////////////////////////////////////////////////////////
RpcClient::RpcClient(std::shared_ptr<transport::UTransport> transport,
                     v1::UUri&& method, v1::UPriority priority,
                     std::chrono::milliseconds ttl,
                     std::optional<v1::UPayloadFormat> payload_format,
                     std::optional<uint32_t> permission_level,
                     std::optional<std::string> token)
    : transport_(std::move(transport)),
      ttl_(ttl),
      builder_(datamodel::builder::UMessageBuilder::request(
          std::move(method), v1::UUri(transport_->getEntityUri()), priority,
          ttl_)),
      expire_service_(std::make_unique<ExpireService>()) {
	if (payload_format) {
		builder_.withPayloadFormat(*payload_format);
	}

	if (permission_level) {
		builder_.withPermissionLevel(*permission_level);
	}

	if (token) {
		builder_.withToken(*token);
	}
}

RpcClient::InvokeHandle RpcClient::invokeMethod(v1::UMessage&& request,
                                                Callback&& callback) {
	auto when_expire = std::chrono::steady_clock::now() + ttl_;
	auto reqid = request.attributes().id();

	// There are multiple paths to calling the callback. It can be called for
	// errors communicating with the transport, errors returned from the
	// uProtocol network, when the request times out (by the ExpireWorker), or
	// when a response comes back from the RpcServer. Because several of these
	// sources are asynchronous, we need to ensure that the callback is called
	// once and only once. We achieve this using std::call_once as a wrapper
	// around any point where the callback is called. The std::once_flag is
	// shared between all of those separate points to ensure that only one
	// attempt to call the callback succeeds.
	auto callback_once = std::make_shared<std::once_flag>();

	auto connected_pair =
	    Connection::establish(std::move(callback));
	auto callback_handle = std::move(std::get<0>(connected_pair));
	auto callable = std::get<1>(connected_pair);

	///////////////////////////////////////////////////////////////////////////
	// Wraps the callback to handle receive filtering and commstatus checking.
	// This is likely less efficient than a single shared callback that maps
	// responses via request IDs, but we can always optimize once we
	// characterize performance.
	auto wrapper = [callable, reqid = std::move(reqid),
	                callback_once](const v1::UMessage& m) mutable {
		using MsgDiff = google::protobuf::util::MessageDifferencer;
		if (MsgDiff::Equals(m.attributes().reqid(), reqid)) {
			if (m.attributes().commstatus() == v1::UCode::OK) {
				std::call_once(*callback_once, [&callable, &m]() {
					MessageOrStatus message(m);
					callable(std::move(message));
				});
			} else {
				v1::UStatus status;
				status.set_code(m.attributes().commstatus());
				status.set_message("Received response with !OK commstatus");
				std::call_once(*callback_once, [&callable,
				                                status = std::move(status)]() {
					callable(utils::Expected<v1::UMessage,v1::UStatus>(utils::Unexpected<v1::UStatus>(status)));
				});
			}
		}
	};
	///////////////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////////////
	// Called when the request has expired or failed. Will be handed off to the
	// expiration monitoring service once the request has been sent.
	auto expire = [callable, callback_once](v1::UStatus&& reason) mutable {
		std::call_once(
		    *callback_once, [&callable, reason = std::move(reason)]() {
			    callable(utils::Expected<v1::UMessage,v1::UStatus>(utils::Unexpected<v1::UStatus>(reason)));
		    });
	};
	///////////////////////////////////////////////////////////////////////////

	auto maybe_handle = transport_->registerListener(
	    std::move(wrapper), request.attributes().sink(),
	    request.attributes().source());

	if (!maybe_handle) {
		expire(std::move(maybe_handle).error());
	} else {
		auto send_result = transport_->send(request);
		if (send_result.code() != v1::UCode::OK) {
			expire(std::move(send_result));
		} else {
			expire_service_->enqueue(when_expire,
			                         std::move(maybe_handle).value(),
			                         std::move(expire));
		}
	}

	return callback_handle;
}

RpcClient::InvokeHandle RpcClient::invokeMethod(
    datamodel::builder::Payload&& payload, Callback&& callback) {
	return invokeMethod(builder_.build(std::move(payload)),
	                    std::move(callback));
}

RpcClient::InvokeHandle RpcClient::invokeMethod(Callback&& callback) {
	return invokeMethod(builder_.build(), std::move(callback));
}

RpcClient::InvokeFuture RpcClient::invokeMethod(
    datamodel::builder::Payload&& payload) {
	// Note: functors need to be copy constructable. We work around this by
	// wrapping the promise in a shared_ptr. Unique access to it will be
	// assured by the implementation at the core of invokeMethod - it only
	// allows exactly one call to the callback via std::call_once.
	auto promise = std::make_shared<std::promise<MessageOrStatus>>();
	auto future = promise->get_future();
	auto handle = invokeMethod(
	    std::move(payload), [promise](const MessageOrStatus& maybe_message) mutable {
		    promise->set_value(maybe_message);
	    });

	return {std::move(future), std::move(handle)};
}

RpcClient::InvokeFuture RpcClient::invokeMethod() {
	// Note: functors need to be copy constructable. We work around this by
	// wrapping the promise in a shared_ptr. Unique access to it will be
	// assured by the implementation at the core of invokeMethod - it only
	// allows exactly one call to the callback via std::call_once.
	auto promise = std::make_shared<std::promise<MessageOrStatus>>();
	auto future = promise->get_future();
	auto handle =
	    invokeMethod([promise](const MessageOrStatus& maybe_message) mutable {
		    promise->set_value(maybe_message);
	    });

	return {std::move(future), std::move(handle)};
}

RpcClient::RpcClient(RpcClient&&) noexcept = default;
RpcClient::~RpcClient() = default;

RpcClient::InvokeFuture::InvokeFuture() = default;
RpcClient::InvokeFuture::InvokeFuture(InvokeFuture&& other) noexcept = default;
RpcClient::InvokeFuture& RpcClient::InvokeFuture::operator=(
    InvokeFuture&& other) noexcept = default;

RpcClient::InvokeFuture::InvokeFuture(std::future<MessageOrStatus>&& future,
                                      InvokeHandle&& handle) noexcept
    : callback_handle_(std::move(handle)), future_(std::move(future)) {}

}  // namespace uprotocol::communication

///////////////////////////////////////////////////////////////////////////////
namespace {
namespace detail {

using uprotocol::v1::UStatus;
using uprotocol::v1::UCode;
// using namespace std::chrono_literals;
using ListenHandle = uprotocol::transport::UTransport::ListenHandle;

auto PendingRequest::operator>(const PendingRequest& other) const {
	return when_expire_ > other.when_expire_;
}

ScrubablePendingQueue::~ScrubablePendingQueue() {
	const UStatus cancel_reason = []() {
		UStatus reason;
		reason.set_code(UCode::INTERNAL);
		reason.set_message(
		    "ERROR: ExpireWorker has shut down while requests are still "
		    "pending. This should never occur and likely indicates that an "
		    "RpcClient instance has been leaked somewhere.");
		return reason;
	}();

	for (auto& pending : c) {
		pending.expire_(cancel_reason);
	}
}

auto ScrubablePendingQueue::scrub(size_t instance_id) {
	// Collect all the expire lambdas so they can be called without the
	// lock held.
	std::vector<std::function<void(UStatus)>> all_expired;

	c.erase(
	    std::remove_if(c.begin(), c.end(),
	                   [instance_id, &all_expired](const PendingRequest& p) {
		                   if (instance_id == p.instance_id_) {
			                   all_expired.push_back(p.expire_);
			                   return true;
		                   }
		                   return false;
	                   }),
	    c.end());

	// TODO(missing_author) - is there a better way to shrink the internal container?
	// Maybe instead we should enforce a capacity limit
	constexpr size_t CAPACITY_SHRINK_THRESHOLD = 16;
	if ((c.capacity() > CAPACITY_SHRINK_THRESHOLD) &&
	    (c.size() < c.capacity() / 2)) {
		c.shrink_to_fit();
	}

	return all_expired;
}

// Exposing non-const version so the listen handle can be moved out
PendingRequest& ScrubablePendingQueue::top() { return c.front(); }

ExpireWorker::ExpireWorker() {
	worker_ = std::thread([this]() { doWork(); });
}

ExpireWorker::~ExpireWorker() {
	stop_ = true;
	{
		std::lock_guard const lock(pending_mtx_);
		wake_worker_.notify_one();
	}
	worker_.join();
}

void ExpireWorker::enqueue(PendingRequest&& pending) {
	std::lock_guard const lock(pending_mtx_);
	pending_.emplace(std::move(pending));
	wake_worker_.notify_one();
}

void ExpireWorker::scrub(size_t instance_id) {
	std::vector<std::function<void(UStatus)>> all_expired;
	{
		std::lock_guard const lock(pending_mtx_);
		all_expired = pending_.scrub(instance_id);
		wake_worker_.notify_one();
	}

	static const UStatus cancel_reason = []() {
		UStatus reason;
		reason.set_code(UCode::CANCELLED);
		reason.set_message("RpcClient for this request was discarded");
		return reason;
	}();

	for (auto& expire : all_expired) {
		expire(cancel_reason);
	}
}

void ExpireWorker::doWork() {
	while (!stop_) {
		const auto now = std::chrono::steady_clock::now();
		std::optional<decltype(PendingRequest::expire_)> maybe_expire;

		{
			ListenHandle expired_handle;
			std::lock_guard const lock(pending_mtx_);
			if (!pending_.empty()) {
				const auto when_expire = pending_.top().when_expire_;
				if (when_expire <= now) {
					maybe_expire = std::move(pending_.top().expire_);
					expired_handle =
					    std::move(pending_.top().response_listener_);
					pending_.pop();
				}
			}
		}

		if (maybe_expire) {
			auto& expire = *maybe_expire;

			static const UStatus expire_reason = []() {
				UStatus reason;
				reason.set_code(UCode::DEADLINE_EXCEEDED);
				reason.set_message("Request expired before response received");
				return reason;
			}();

			expire(expire_reason);

		} else {
			std::unique_lock lock(pending_mtx_);
			if (pending_.empty()) {
				wake_worker_.wait(
				    lock, [this]() { return stop_ || !pending_.empty(); });
			} else {
				// Reasons that we *expect* to wake:
				// * The time `wake_when` has arrived
				// * A new pending request has been placed at the top of the
				//   priority queue (either by insertion or deletion)
				// * The queue has been emptied (loop back to indefinite sleep)
				// * A stop has been requested
				auto wake_when = pending_.top().when_expire_;
				wake_worker_.wait_until(lock, wake_when, [this, &wake_when]() {
					auto when_next_wake = wake_when;
					if (!pending_.empty()) {
						when_next_wake = pending_.top().when_expire_;
					}
					return stop_ || when_next_wake != wake_when ||
					       pending_.empty() ||
					       (std::chrono::steady_clock::now() >= when_next_wake);
				});
			}
		}
	}
}

}  // namespace detail
}  // namespace
