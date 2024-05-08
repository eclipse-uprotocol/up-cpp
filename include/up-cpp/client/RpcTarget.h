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

#ifndef UP_CPP_CLIENT_RPCTARGET_H
#define UP_CPP_CLIENT_RPCTARGET_H

#include <up-core-api/umessage.pb.h>
#include <up-core-api/ustatus.pb.h>
#include <up-core-api/uri.pb.h>
#include <up-cpp/transport/UTransport.h>
#include <up-cpp/datamodel/builder/UMessage.h>
#include <up-cpp/datamodel/validator/UMessage.h>
#include <memory>
#include <utility>

namespace uprotocol::client {

/// @brief Interface for uEntities to act as a target of RPC requests.
///
/// Like all L2 client APIs, the RpcTarget is a wrapper on top of the L1
/// UTransport API; in this instance, it is the request-handling half of the
/// RPC model.
struct RpcTarget {
    /// @brief Callback function signature for implementing the RPC method.
    using RpcMethod = transport::UTransport::ListenCallback;

    /// @brief Constructs an initiator connected to a given transport.
	///
	/// @param transport Transport to offer the RPC method through.
	/// @param method_name URI representing the name clients will use to invoke
	///                    the RPC method.
	/// @param callback Method that will be called when requests are received.
    RpcTarget(std::shared_ptr<transport::UTransport> transport,
			const v1::UUri& method_name, RpcMethod&& callback);

	/// @brief Builder type for messages sent by this client
	using MessageBuilder = datamodel::builder::UMessageBuilder;

	/// @brief Gets a UMessageBuilder by forwarding parameters to
	///        UMessageBuilder::response().
	///
	/// @remarks Generally, passing the request message this response is being
	///          sent for is the safest option.
	///
	/// @see datamodel::builder::UMessageBuilder::response
	template<typename... Args>
	static MessageBuilder responseBuilder(Args&&... args) {
		return MessageBuilder::response(std::forward<Args>(args)...);
	}

	/// @brief Sends a response message.
	///
	/// Intended to pair with responseBuilder().
	///
	/// @throws InvalidUMessage if the message is not a response.
	v1::UStatus sendResponse(v1::UMessage&&);

	/// @brief Wrapper to build and send a response in a single step.
	///
	/// @note Does not allow for full customization of response attributes.
	///       For attribute customization, use responseBuilder() and
	///       sendResponse() instead.
	///
	/// @param request The request message to send a response to.
	/// @param build_args Arguments to forward to UMessageBuilder::build().
	///                   Note that this can be omitted completely to call
	///                   build() with no parameters.
	///
	/// @see datamodel::builder::UMessageBuilder::build
	template<typename... Args>
	v1::UStatus respondTo(const v1::UMessage& request, Args&&... build_args) {
		auto message = responseBuilder(request)
			.build(std::forward<Args>(build_args)...);
		return sendResponse(std::move(message));
	}

	/// @brief Wrapper to build and send a response in a single step.
	///
	/// @note Does not allow for full customization of response attributes.
	///       For attribute customization, use responseBuilder() and
	///       sendResponse() instead.
	///
	/// @tparam Serializer An object capable of serializing ValueT.
	/// @tparam ValueT Automatically inferred unserialized payload type.
	///
	/// @param request The request message to send a response to.
	/// @param value The payload data to serialize and send.
	///
	/// @see datamodel::builder::UMessageBuilder::build
	template<typename Serializer, typename ValueT>
	v1::UStatus respondTo(const v1::UMessage& request, const ValueT& value) {
		auto message = responseBuilder(request)
			.build<Serializer>(value);
		return sendResponse(std::move(message));
	}

	~RpcTarget() = default;

private:
	/// @brief Transport instance that will be used for communication
	std::shared_ptr<transport::UTransport> transport_;

	/// @brief Handle to the connected callback for the RPC method
	transport::UTransport::ListenHandle callback_handle_;
};

}  // namespace uprotocol::client

#endif  // UP_CPP_CLIENT_RPCTARGET_H
