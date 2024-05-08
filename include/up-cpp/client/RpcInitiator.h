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

#ifndef UP_CPP_CLIENT_RPCINITIATOR_H
#define UP_CPP_CLIENT_RPCINITIATOR_H

#include <up-cpp/transport/UTransport.h>
#include <up-cpp/datamodel/builder/UMessage.h>
#include <up-core-api/umessage.pb.h>
#include <up-core-api/ustatus.pb.h>
#include <future>
#include <memory>
#include <variant>
#include <future>

namespace uprotocol::client {

/// @brief Interface for uEntities to initiate RPC requests
///
/// Like all L2 client APIs, the RpcInitiator is a wrapper on top of the L1
/// UTransport API; in this instance, it is the request-initiating half of the
/// RPC model.
class RpcInitiator {
public:
    /// @brief Constructs an initiator connected to a given transport
    explicit RpcInitiator(std::shared_ptr<transport::UTransport> transport);

    /// @brief Gets a new UMessageBuilder configured for generating RPC request
    ///        messages targeting a specific RPC method.
    ///
    /// @param target_method The method that will be invoked when the message
    ///                      is built and sent.
    /// @param ttl Time in milliseconds that an RPC request will remain valid
    ///            starting from when UMessageBuilder::build() is called.
    ///
    /// @remarks This message builder can be held and reused for recurring
    ///          requests to a particular target.
    /// @remarks Each call to this method will produce a new builder instance.
    ///
    /// @returns A request UMessageBuilder
    [[nodiscard]] datamodel::builder::UMessageBuilder requestBuilder(
            v1::UUri&& target_method,
            std::chrono::milliseconds ttl) const;

    /// @brief Contains either a UStatus or a UMessage
    using StatusOrMessage = std::variant<v1::UStatus, v1::UMessage>;

    /// @brief Invokes an RPC method by sending a request message.
    ///
    /// @param A request-type message that will be used to invoke invoke the
    ///        RPC method.
    ///
    /// @returns A promised future that can resolve to one of:
    ///          * A UStatus with a DEADLINE_EXCEEDED code if no response was
    ///            received before the request expired (based on request TTL).
    ///          * A UMessage containing the response from the RPC target.
    [[nodiscard]] std::future<StatusOrMessage> invokeMethod(v1::UMessage&&);

    ~RpcInitiator() = default;

private:
    std::shared_ptr<transport::UTransport> transport_;
};

}  // namespace uprotocol::client

#endif  // UP_CPP_CLIENT_RPCINITIATOR_H
