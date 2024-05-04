// Copyright (c) 2024 General Motors GTO LLC
//
// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements.  See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership.  The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License.  You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.  See the License for the
// specific language governing permissions and limitations
// under the License.
//
// SPDX-FileType: SOURCE
// SPDX-FileCopyrightText: 2024 General Motors GTO LLC
// SPDX-License-Identifier: Apache-2.0
#ifndef UP_CPP_CLIENT_RPCTARGET_H
#define UP_CPP_CLIENT_RPCTARGET_H

#include <up-core-api/uattributes.pb.h>
#include <up-core-api/umessage.pb.h>
#include <up-core-api/ustatus.pb.h>
#include <up-cpp/transport/UTransport.h>
#include <functional>
#include <utility>

namespace uprotocol::client {

/// @brief Interface for uEntities to act as a target of RPC requests.
///
/// Like all L2 client APIs, the RpcTarget is a wrapper on top of the L1
/// UTransport API; in this instance, it is the request-handling half of the
/// RPC model.
struct RpcTarget {
    /// @brief Callback function signature for implementing the RPC method.
    using RpcMethod = UTransport::ListenCallback;

    /// @brief Constructs an initiator connected to a given transport.
	///
	/// @param transport Transport to offer the RPC method through.
	/// @param methodName URI representing the name clients will use to invoke
	///                   the RPC method.
	/// @param callback Method that will be called when requests are received.
    RpcTarget(std::shared_ptr<UTransport> transport,
			const v1::UUri& methodName, RpcMethod&& callback);

	/// @brief Gets a UMessageBuilder by forwarding parameters to
	///        UMessageBuilder::response().
	///
	/// @remarks Generally, passing the request message resulting in this
	///          response is the safest option.
	///
	/// @see datamodel::builder::UMessageBuilder::response
	template<typename... Args>
	static UMessageBuilder responseBuilder(Args&&... args) {
		return UMessageBuilder::response(std::forward<Args>(args)...);
	}

	/// @brief Sends a response message
	v1::UStatus sendResponse(v1::UMessage&&);

	/// @brief Wrapper to build and send a response in a single step.
	///
	/// @note Does not allow for full customization of response attributes.
	///       For attribute customization, use responseBuilder() and
	///       sendResponse() instead.
	///
	/// @param request The request message to send a response to.
	/// @param buildArgs Arguments to forward to UMessageBuilder::build().
	///                  Note that this can be omitted completely to call
	///                  build() with no parameters.
	///
	/// @see datamodel::builder::UMessageBuilder::build
	template<typename... Args>
	v1::UStatus respondTo(const v1::UMessage& request, Args&&... buildArgs) {
		auto message = responseBuilder(request)
			.buildWithPayload(std::forward<Args>(buildArgs)...);
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
	template<typename Serializer, template ValueT>
	v1::UStatus respondTo(const v1::UMessage& request, const ValueT& value) {
		auto message = responseBuilder(request)
			.buildWithPayload<Serializer>(value);
		return sendResponse(std::move(message));
	}

	~RpcTarget() = default;

protected:
	std::shared_ptr<UTransport> transport_;
};

}  // namespace uprotocol::rpc

#endif  // UP_CPP_CLIENT_RPCTARGET_H
