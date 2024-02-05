/*
 * Copyright (c) 2023 General Motors GTO LLC
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
 * SPDX-FileCopyrightText: 2023 General Motors GTO LLC
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _UTRANSPORT_
#define _UTRANSPORT_

#include <stdint.h>
#include <cstddef>
#include <up-cpp/transport/datamodel/UListener.h>
#include <up-cpp/transport/datamodel/UPayload.h>
#include <up-cpp/transport/datamodel/UAttributes.h>
#include <up-core-api/uri.pb.h>
#include <up-core-api/ustatus.pb.h>

using namespace uprotocol::utransport;
using namespace uprotocol::v1;

namespace uprotocol::utransport {

     class UTransport {

          public:
               
               /**
               * Transmit UPayload to the topic using the attributes defined in UTransportAttributes.
               * @param topic Resolved UUri topic to send the payload to.
               * @param payload Actual payload.
               * @param attributes Additional transport attributes.
               * @return Returns OKSTATUS if the payload has been successfully sent (ACK'ed), otherwise it
               * returns FAILSTATUS with the appropriate failure.
               */
               virtual UStatus send(const UUri &uri, 
                                    const UPayload &payload,
                                    const UAttributes &attributes) = 0;

               /**
               * Register listener to be called when UPayload is received for the specific topic.
               * @param topic Resolved UUri for where the message arrived via the underlying transport technology.
               * @param listener The method to execute to process the date for the topic.
               * @return Returns OKSTATUS if the listener is unregistered correctly, otherwise it returns FAILSTATUS
               * with the appropriate failure.
               */      
               virtual UStatus registerListener(const UUri &uri,
                                                const UListener &listner) = 0;

               /**
               * Unregister a listener for a given topic. Messages arriving on this topic will no longer be processed
               * by this listener.
               * @param topic Resolved UUri for where the listener was registered to receive messages from.
               * @param listener The method to execute to process the date for the topic.
               * @return Returns OKSTATUS if the listener is unregistered correctly, otherwise it returns FAILSTATUS
               * with the appropriate failure.
               * 
               */
               virtual UStatus unregisterListener(const UUri &uri, 
                                                  const UListener &listner) = 0;

               virtual UStatus receive(const UUri &uri, 
                                       const UPayload &payload, 
                                       const UAttributes &attributes) = 0;
               
               virtual ~UTransport() {} 
     };
}

#endif /*_UTRANSPORT_*/
