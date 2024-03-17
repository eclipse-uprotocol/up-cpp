/*
 * Copyright (c) 2024 General Motors GTO LLC
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 * 
 * SPDX-FileType: SOURCE
 * SPDX-FileCopyrightText: 2024 General Motors GTO LLC
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _RPC_CLIENT_H_
#define _RPC_CLIENT_H_

#include <future>
#include <up-cpp/transport/UListener.h>
#include <up-cpp/transport/datamodel/UMessage.h>
#include <up-core-api/uattributes.pb.h>
#include <up-core-api/ustatus.pb.h>

namespace uprotocol::rpc {

    struct RpcResponse {
        uprotocol::v1::UStatus status;
        uprotocol::utransport::UMessage message;
    };

    /**
    * RpcClient is an interface used by code generators for uProtocol services defined in proto files such as
    * the core uProtocol services found in https://github.com/eclipse-uprotocol/uprotocol-core-api. the interface 
    * provides a clean contract for mapping a RPC request to a response. For more details please refer to
    * https://github.com/eclipse-uprotocol/uprotocol-spec/blob/main/up-l2/README.adoc[RpcClient Specifications]
    */
    class RpcClient 
    {
        public:
            /**
            * API for clients to invoke a method (send an RPC request) and receive the response (the returned 
            * {@link CompletionStage} {@link UPayload}. <br>
            * Client will set method to be the URI of the method they want to invoke, 
            * payload to the request message, and attributes with the various metadata for the 
            * method invocation.
            * @param methodUri The method URI to be invoked, ex (long form): /example.hello_world/1/rpc.SayHello.
            * @param requestPayload The request message to be sent to the server.
            * @param options RPC method invocation call options, see {@link CallOptions}
            * @return Returns the CompletionStage with the response message or exception with the failure
            * reason as {@link UStatus}.
            */
            virtual std::future<RpcResponse> invokeMethod(const uprotocol::v1::UUri &topic, 
                                                          const uprotocol::utransport::UPayload &payload, 
                                                          const uprotocol::v1::CallOptions &options) = 0;
            
            virtual uprotocol::v1::UStatus invokeMethod(const uprotocol::v1::UUri &topic,
                                                        const uprotocol::utransport::UPayload &payload,
                                                        const uprotocol::v1::CallOptions &options,
                                                        const uprotocol::utransport::UListener &callback) = 0;

            virtual ~RpcClient() {} 
    };
}
#endif /*_RPC_CLIENT_H_*/