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

#ifndef UP_CPP_CLIENT_RPCCLIENT_H
#define UP_CPP_CLIENT_RPCCLIENT_H

#include <uprotocol/v1/umessage.pb.h>
#include <uprotocol/v1/ustatus.pb.h>
#include <up-cpp/datamodel/builder/UMessage.h>
#include <up-cpp/transport/UTransport.h>

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
	explicit RpcClient(std::shared_ptr<transport::UTransport> transport,
			v1::UUri&& method, v1::UPriority, std::chrono::milliseconds ttl,
			std::optional<uint32_t> permission_level = {},
			std::optional<std::string> token = {});

	/// @brief Contains either a UStatus or a UMessage
	using StatusOrMessage = std::variant<v1::UStatus, v1::UMessage>;

	/// @brief Invokes an RPC method by sending a request message.
	///
	/// @param build_args Arguments to forward to UMessageBuilder::build().
	///                   Note that this can be omitted completely to call
	///                   build() with no parameters.
	///
	/// @see datamodel::builder::UMessageBuilder::build
	///
	/// @returns A promised future that can resolve to one of:
	///          * A UStatus with a DEADLINE_EXCEEDED code if no response was
	///            received before the request expired (based on request TTL).
	///          * A UMessage containing the response from the RPC target.
	template <typename... Args>
	[[nodiscard]] std::future<StatusOrMessage> invokeMethod(Args&&... build_args);

	/// @brief Invokes an RPC method by sending a request message.
	///
	/// @tparam Serializer An object capable of serializing ValueT.
	/// @tparam ValueT Automatically inferred unserialized payload type.
	///
	/// @param value The payload data to serialize and send.
	///
	/// @see datamodel::builder::UMessageBuilder::build
	///
	/// @returns A promised future that can resolve to one of:
	///          * A UStatus with a DEADLINE_EXCEEDED code if no response was
	///            received before the request expired (based on request TTL).
	///          * A UMessage containing the response from the RPC target.
	template <typename Serializer, typename ValueT>
	[[nodiscard]] std::future<StatusOrMessage> invokeMethod(const ValueT&);

	~RpcClient() = default;

private:
	std::shared_ptr<transport::UTransport> transport_;
	datamodel::builder::UMessageBuilder builder_;
};

}  // namespace uprotocol::communication

#endif  // UP_CPP_CLIENT_RPCCLIENT_H
