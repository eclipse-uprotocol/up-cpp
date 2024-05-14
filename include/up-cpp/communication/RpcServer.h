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

#ifndef UP_CPP_CLIENT_RPCSERVER_H
#define UP_CPP_CLIENT_RPCSERVER_H

#include <uprotocol/v1/umessage.pb.h>
#include <uprotocol/v1/uri.pb.h>
#include <uprotocol/v1/ustatus.pb.h>
#include <up-cpp/datamodel/builder/UMessage.h>
#include <up-cpp/datamodel/validator/UMessage.h>
#include <up-cpp/transport/UTransport.h>

#include <chrono>
#include <memory>
#include <utility>

namespace uprotocol::communication {

/// @brief Interface for uEntities to receive and respond to RPC requests.
///
/// Like all L2 client APIs, the RpcServer is a wrapper on top of the L1
/// UTransport API; in this instance, it is the request-handling half of the
/// RPC model.
struct RpcServer {
	/// @brief Callback function signature for implementing the RPC method.
	using RpcCallback = transport::UTransport::ListenCallback;

	/// @brief Constructs an initiator connected to a given transport.
	///
	/// @param transport Transport to offer the RPC method through.
	/// @param method_name URI representing the name clients will use to invoke
	///                    the RPC method.
	/// @param callback Method that will be called when requests are received.
	/// @param ttl (Optional) Time response will be valid from the moment
	///            respond() is called. Note that the original request's TTL
	///            may also still apply.
	RpcServer(std::shared_ptr<transport::UTransport> transport,
	          const v1::UUri& method_name, RpcCallback&& callback
		  std::optional<std::chrono::milliseconds> ttl = {});

	/// @brief Wrapper to build and send a response in a single step.
	///
	/// @param request The request message to send a response to.
	/// @param build_args Arguments to forward to UMessageBuilder::build().
	///                   Note that this can be omitted completely to call
	///                   build() with no parameters.
	///
	/// @see datamodel::builder::UMessageBuilder::build
	template <typename... Args>
	[[nodiscard]] v1::UStatus respond(const v1::UMessage& request, Args&&... build_args) const {
		using Builder = datamodel::builder::UMessageBuilder;
		auto builder = Builder::response(request);
		if (ttl_) {
			builder.withTtl(*ttl_);
		}
		return transport_->send(builder.build(std::forward<Args>(build_args)...));
	}

	/// @brief Wrapper to build and send a response in a single step.
	///
	/// @tparam Serializer An object capable of serializing ValueT.
	/// @tparam ValueT Automatically inferred unserialized payload type.
	///
	/// @param request The request message to send a response to.
	/// @param value The payload data to serialize and send.
	///
	/// @see datamodel::builder::UMessageBuilder::build
	template <typename Serializer, typename ValueT>
	[[nodiscard]] v1::UStatus respond(const v1::UMessage& request, const ValueT& value) const {
		using Builder = datamodel::builder::UMessageBuilder;
		auto builder = Builder::response(request);
		if (ttl_) {
			builder.withTtl(*ttl_);
		}
		return transport_->send(builder.build<Serializer>(std::forward<Args>(build_args)...));
	}

	~RpcServer() = default;

private:
	/// @brief Transport instance that will be used for communication
	std::shared_ptr<transport::UTransport> transport_;

	/// @brief Handle to the connected callback for the RPC method
	transport::UTransport::ListenHandle callback_handle_;

	/// @brief TTL to use for responses, if set at construction time
	std::optional<std::chrono::milliseconds> ttl_;
};

}  // namespace uprotocol::communication

#endif  // UP_CPP_CLIENT_RPCSERVER_H
