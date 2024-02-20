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

#ifndef _ULISTENER_H_
#define _ULISTENER_H_

#include <uprotocol-cpp/uri/datamodel/UUri.h>
#include <src/main/proto/ustatus.pb.h>
#include "up-core-api/uattributes.pb.h"
#include "UPayload.h"

namespace uprotocol::utransport {

	class UListener {
		public:

			/**
			* Method called to handle/process events.
			* @param topic Topic the underlying source of the message.
			* @param payload Payload of the message.
			* @param attributes Transportation attributes
			* @return Returns an Ack every time a message is received and processed.
			*/
			virtual UStatus onReceive(const UUri &uri, 
									const UPayload &payload, 
									const UAttributes &attributes) const = 0; 

			virtual ~UListener() {} 
	};
}


#endif /* _ULISTENER_H_*/