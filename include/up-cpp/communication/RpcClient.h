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

#include <up-cpp/datamodel/builder/Payload.h>
#include <up-cpp/datamodel/builder/UMessage.h>
#include <up-cpp/transport/UTransport.h>
#include <up-cpp/utils/Expected.h>
#include <uprotocol/v1/umessage.pb.h>
#include <uprotocol/v1/ustatus.pb.h>

#include <future>
#include <memory>
#include <string>
#include <variant>

namespace uprotocol::communication {

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
	                   v1::UUri&& method, v1::UPriority priority,
	                   std::chrono::milliseconds ttl,
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

	/// @brief Extension to std::future that also holds a callback handle
	class InvokeFuture {
		InvokeHandle callback_handle_;
		std::future<MessageOrStatus> future_;

	public:
		InvokeFuture();
		InvokeFuture(InvokeFuture&&) noexcept;
		InvokeFuture(std::future<MessageOrStatus>&&, InvokeHandle&&) noexcept;

		InvokeFuture& operator=(InvokeFuture&&) noexcept;

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

	/// @brief Invokes an RPC method by sending a request message.
	///
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
	[[nodiscard]] InvokeHandle invokeMethod(datamodel::builder::Payload&&,
	                                        Callback&&);

	/// @brief Invokes an RPC method by sending a request message.
	///
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
	[[nodiscard]] InvokeFuture invokeMethod(datamodel::builder::Payload&&);

	/// @brief Invokes an RPC method by sending a request message.
	///
	/// Request is sent with an empty payload. Can only be called if no payload
	/// format was provided at construction time.
	///
	/// @param A callback that will be called with the result.
	///
	/// @post The provided callback will be called with one of:
	///       * A UStatus with a DEADLINE_EXCEEDED code if no response was
	///         received before the request expired (based on request TTL).
	///       * A UStatus with the value returned by UTransport::send().
	///       * A UStatus based on the commstatus received in the response
	///         message (if not OK).
	///       * A UMessage containing the response from the RPC target.
	[[nodiscard]] InvokeHandle invokeMethod(Callback&&);

	/// @brief Invokes an RPC method by sending a request message.
	///
	/// Request is sent with an empty payload. Can only be called if no payload
	/// format was provided at construction time.
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
	[[nodiscard]] InvokeFuture invokeMethod();

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
