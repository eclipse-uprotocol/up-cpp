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
#include <up-cpp/datamodel/builder/UPayload.h>
#include <up-cpp/transport/UTransport.h>
#include <up-cpp/utils/Expected.h>

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

	/// @brief as found in v1::UAttributes
	using Commstatus = uint32_t;

	/// @brief Contains either a UPayload (when successful) or a UStatus / Commstatus when an error occurred.
	using StatusOrPayload = utils::Expected<v1::UPayload, std::variant<v1::UStatus, Commstatus>>;

	/// @brief Invokes an RPC method by sending a request message.
	///
	/// @param A UPayload builder containing the payload to be sent with the
	///        request.
	///
	/// @returns A promised future that can resolve to one of:
	///          * A UStatus with a DEADLINE_EXCEEDED code if no response was
	///            received before the request expired (based on request TTL).
	///          * A UStatus with the value returned by UTransport::send().
	///          * A Commstatus as received in the response message (if not OK).
	///          * A UPayload containing the response from the RPC target.
	[[nodiscard]] std::future<StatusOrMessage> invokeMethod(datamodel::builder::UPayload&&);

	~RpcClient() = default;

private:
	std::shared_ptr<transport::UTransport> transport_;
	datamodel::builder::UMessageBuilder builder_;
};

}  // namespace uprotocol::communication

#endif  // UP_CPP_CLIENT_RPCCLIENT_H
