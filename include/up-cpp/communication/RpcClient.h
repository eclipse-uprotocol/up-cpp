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

#ifndef UP_CPP_COMMUNICATION_RPCCLIENT_H
#define UP_CPP_COMMUNICATION_RPCCLIENT_H

#include <spdlog/spdlog.h>
#include <up-cpp/datamodel/builder/Payload.h>
#include <up-cpp/datamodel/builder/UMessage.h>
#include <up-cpp/transport/UTransport.h>
#include <up-cpp/utils/Expected.h>
#include <up-cpp/utils/ProtoConverter.h>
#include <uprotocol/v1/umessage.pb.h>
#include <uprotocol/v1/uri.pb.h>
#include <uprotocol/v1/ustatus.pb.h>

#include <future>
#include <memory>
#include <string>
#include <variant>

namespace uprotocol::communication {
template <typename R>
using ResponseOrStatus = utils::Expected<R, v1::UStatus>;
using UnexpectedStatus = utils::Unexpected<v1::UStatus>;

/// @brief Interface for uEntities to invoke RPC methods.
///
/// Like all L2 client APIs, the RpcClient is a wrapper on top of the L1
/// UTransport API; in this instance, it is the request-initiating half of the
/// RPC model.
struct RpcClient {
	/// @brief Constructs a client connected to a given transport
	///
	/// @param method UUri of the method this client will invoke.
	/// @param priority Priority of request messages when invoking methods.
	/// @param ttl Time from the moment `invokeMethod()` is called that the
	///            request will still be considered valid. Must be >0.
	/// @param payload_format (Optional) If sending a payload, this sets the
	///                       format that will be expected when invokeMethod()
	///                       is called. Empty request payloads can only be
	///                       sent if this was not set.
	/// @param permission_level Permission level of this client
	/// @param token TAP token for accessing restricted services
	///
	/// For guidance on the permeission_level and token parameters, see:
	/// https://github.com/eclipse-uprotocol/up-spec/blob/main/basics/permissions.adoc
	explicit RpcClient(std::shared_ptr<transport::UTransport> transport,
	                   v1::UPriority priority, std::chrono::milliseconds ttl,
	                   std::optional<v1::UPayloadFormat> payload_format = {},
	                   std::optional<uint32_t> permission_level = {},
	                   std::optional<std::string> token = {});

	/// @brief Contains either a UMessage (when successful) or a UStatus
	///        when an error occurred. The UStatus could be set based
	///        on the commstatus attributes field in any returned messages.
	using MessageOrStatus = utils::Expected<v1::UMessage, v1::UStatus>;

	/// @brief Callback connections for callbacks passed to invokeMethod()
	using Connection = utils::callbacks::Connection<void, MessageOrStatus&&>;

	/// @brief Callback function signature used in the callback form of
	///        invokeMethod()
	using Callback = Connection::Callback;

	/// @brief Handle that must be held for a callback to remain registered
	///        for the duration of an RPC call.
	using InvokeHandle = Connection::Handle;

	/// @brief Extension to std::future with template type that also holds a
	/// callback handle
	template <typename T>
	class InvokeProtoFuture {
		InvokeHandle callback_handle_;
		std::future<utils::Expected<T, v1::UStatus>> future_;

	public:
		InvokeProtoFuture() = default;
		InvokeProtoFuture(InvokeProtoFuture&& other) noexcept = default;
		InvokeProtoFuture& operator=(InvokeProtoFuture&& other) noexcept =
		    default;

		InvokeProtoFuture(std::future<utils::Expected<T, v1::UStatus>>&& future,
		                  InvokeHandle&& handle) noexcept
		    : callback_handle_(std::move(handle)), future_(std::move(future)) {}

		/// @name Passthroughs for std::future
		/// @{
		auto get() { return future_.get(); }
		[[nodiscard]] auto valid() const noexcept { return future_.valid(); }
		void wait() const { future_.wait(); }
		template <typename... Args>
		auto wait_for(Args&&... args) const {
			return future_.wait_for(std::forward<Args>(args)...);
		}
		template <typename... Args>
		auto wait_until(Args&&... args) const {
			return future_.wait_until(std::forward<Args>(args)...);
		}
		/// @}
	};

	using InvokeFuture = InvokeProtoFuture<v1::UMessage>;

	/// @brief Invokes an RPC method by sending a request message.
	///
	/// @param The method that will be invoked
	/// @param A Payload builder containing the payload to be sent with the
	///        request.
	/// @param A callback that will be called with the result.
	///
	/// @post The provided callback will be called with one of:
	///       * A UStatus with a DEADLINE_EXCEEDED code if no response was
	///         received before the request expired (based on request TTL).
	///       * A UStatus with the value returned by UTransport::send().
	///       * A UStatus based on the commstatus received in the response
	///         message (if not OK).
	///       * A UMessage containing the response from the RPC target.
	[[nodiscard]] InvokeHandle invokeMethod(const v1::UUri&,
	                                        datamodel::builder::Payload&&,
	                                        Callback&&);

	/// @brief Invokes an RPC method by sending a request message.
	///
	/// @param The method that will be invoked
	/// @param A Payload builder containing the payload to be sent with the
	///        request.
	///
	/// @remarks This is a wrapper around the callback form of invokeMethod.
	///
	/// @returns A promised future that can resolve to one of:
	///          * A UStatus with a DEADLINE_EXCEEDED code if no response was
	///            received before the request expired (based on request TTL).
	///          * A UStatus with the value returned by UTransport::send().
	///          * A UStatus based on the commstatus received in the response
	///            message (if not OK).
	///          * A UMessage containing the response from the RPC target.
	[[nodiscard]] InvokeFuture invokeMethod(const v1::UUri&,
	                                        datamodel::builder::Payload&&);

	/// @brief Invokes an RPC method by sending a request message.
	///
	/// Request is sent with an empty payload. Can only be called if no payload
	/// format was provided at construction time.
	///
	/// @param The method that will be invoked
	/// @param A callback that will be called with the result.
	///
	/// @post The provided callback will be called with one of:
	///       * A UStatus with a DEADLINE_EXCEEDED code if no response was
	///         received before the request expired (based on request TTL).
	///       * A UStatus with the value returned by UTransport::send().
	///       * A UStatus based on the commstatus received in the response
	///         message (if not OK).
	///       * A UMessage containing the response from the RPC target.
	[[nodiscard]] InvokeHandle invokeMethod(const v1::UUri&, Callback&&);

	/// @brief Invokes an RPC method by sending a request message.
	///
	/// Request is sent with an empty payload. Can only be called if no payload
	/// format was provided at construction time.
	///
	/// @param The method that will be invoked
	/// @remarks This is a wrapper around the callback form of invokeMethod.
	///
	/// @returns A promised future that can resolve to one of:
	///          * A UStatus with a DEADLINE_EXCEEDED code if no response was
	///            received before the request expired (based on request TTL).
	///          * A UStatus with the value returned by UTransport::send().
	///          * A UStatus based on the commstatus received in the response
	///            message (if not OK).
	///          * A UMessage containing the response from the RPC target.
	[[nodiscard]] InvokeFuture invokeMethod(const v1::UUri&);

	/// @brief Invokes an RPC method by sending a request message directly
	/// from a protobuf object.
	///
	/// @param The method that will be invoked
	/// @param The protobuf object that will be sent as the payload
	/// @param A callback that will be called with the result.
	///
	/// @post The provided callback will be called with one of:
	///       * A UStatus with a DEADLINE_EXCEEDED code if no response was
	///         received before the request expired (based on request TTL).
	///       * A UStatus with the value returned by UTransport::send().
	///       * A UStatus based on the commstatus received in the response
	///         message (if not OK).
	///       * A UMessage containing the response from the RPC target.
	template <typename R>
	[[nodiscard]] InvokeHandle invokeMethodFromProto(const v1::UUri& method,
	                                                 const R& request_message,
	                                                 Callback&& callback) {
		auto payload_or_status =
		    uprotocol::utils::ProtoConverter::protoToPayload(request_message);

		if (!payload_or_status.has_value()) {
			return {};
		}

		datamodel::builder::Payload tmp_payload(payload_or_status.value());
		auto handle = invokeMethod(
		    builder_.withMethod(method).build(std::move(tmp_payload)),
		    std::move(callback));

		return handle;
	}

	/// @brief Invokes an RPC method by sending a request message directly
	/// from a protobuf object and converts the returned payload
	/// to protobuf.
	///
	/// @param The method that will be invoked
	/// @param The protobuf object that will be sent as the payload
	/// @param A callback that will be called with the result.
	///
	/// @returns A promised future that can resolve to one of:
	///          * A UStatus with a DEADLINE_EXCEEDED code if no response was
	///            received before the request expired (based on request TTL).
	///          * A UStatus with the value returned by UTransport::send().
	///          * A UStatus based on the commstatus received in the response
	///            message (if not OK).
	///		   	* A protobuf object constructed from the response from the RPC
	/// target.
	template <typename T, typename R>
	[[nodiscard]] InvokeProtoFuture<T> invokeMethodToProto(
	    const v1::UUri& method, const R& request_message) {
		auto result_promise =
		    std::make_shared<std::promise<ResponseOrStatus<T>>>();
		auto future = result_promise->get_future();
		auto handle = invokeMethodFromProto(
		    method, request_message,
		    [result_promise](const MessageOrStatus& message_or_status) {
			    if (!message_or_status.has_value()) {
				    result_promise->set_value(ResponseOrStatus<T>(
				        UnexpectedStatus(message_or_status.error())));
				    return;
			    }
			    auto response_or_status =
			        utils::ProtoConverter::extractFromProtobuf<T>(
			            message_or_status.value());

			    if (!response_or_status.has_value()) {
				    spdlog::error(
				        "invokeProtoMethod: Error when extracting response "
				        "from "
				        "protobuf.");
				    result_promise->set_value(response_or_status);
				    return;
			    }

			    result_promise->set_value(
			        ResponseOrStatus<T>(response_or_status.value()));
		    });

		return {std::move(future), std::move(handle)};
	}

	/// @brief Invokes an RPC method by sending a request message.
	///
	/// @param The method that will be invoked
	/// @param The protobuf object that will be sent as a payload.
	///
	/// @returns A promised future that can resolve to one of:
	///          * A UStatus with a DEADLINE_EXCEEDED code if no response was
	///            received before the request expired (based on request TTL).
	///          * A UStatus with the value returned by UTransport::send().
	///          * A UStatus based on the commstatus received in the response
	///            message (if not OK).
	///          * A UMessage containing the response from the RPC target.
	template <typename R>
	[[nodiscard]] InvokeFuture invokeMethodFromProto(const v1::UUri& method,
	                                                 const R& request_message) {
		auto result_promise =
		    std::make_shared<std::promise<ResponseOrStatus<v1::UMessage>>>();
		auto future = result_promise->get_future();

		auto handle = invokeMethodFromProto(
		    method, request_message,
		    [result_promise](const MessageOrStatus& message_or_status) {
			    if (!message_or_status.has_value()) {
				    result_promise->set_value(ResponseOrStatus<v1::UMessage>(
				        UnexpectedStatus(message_or_status.error())));
				    return;
			    }

			    result_promise->set_value(
			        ResponseOrStatus<v1::UMessage>(message_or_status.value()));
		    });

		return {std::move(future), std::move(handle)};
	}

	/// @brief Default move constructor (defined in RpcClient.cpp)
	RpcClient(RpcClient&&) noexcept;

	/// @brief Default destructor (defined in RpcClient.cpp)
	~RpcClient();

private:
	/// @brief Internal implementation of invokeMethod that handles all the
	///        shared logic for the public invokeMethod() methods.
	InvokeHandle invokeMethod(v1::UMessage&&, Callback&&);

	/// @brief Handle to a shared worker that monitors for and cancels expired
	///        requests.
	struct ExpireService;

	std::shared_ptr<transport::UTransport> transport_;
	std::chrono::milliseconds ttl_;
	datamodel::builder::UMessageBuilder builder_;
	std::unique_ptr<ExpireService> expire_service_;
};

}  // namespace uprotocol::communication

#endif  // UP_CPP_COMMUNICATION_RPCCLIENT_H