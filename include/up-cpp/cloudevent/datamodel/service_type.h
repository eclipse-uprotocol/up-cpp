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

#ifndef CPP_CLOUDEVENT_SERVICE_TYPE_H
#define CPP_CLOUDEVENT_SERVICE_TYPE_H

#include <iostream>
#include <string>

#include "spdlog/spdlog.h"

namespace cloudevents::format {
class ServiceType {
 public:
  enum class MessageType_E {
    PUBLISH,
    FILE,
    REQUEST,
    RESPONSE,
    NOT_DEFINED,
  };

  inline static const std::string PUBLISH_MSG_TYPE_V1 = "pub.v1";
  inline static const std::string FILE_MSG_TYPE_V1 = "file.v1";
  inline static const std::string REQUEST_MSG_TYPE_V1 = "req.v1";
  inline static const std::string RESPONSE_MSG_TYPE_V1 = "res.v1";

  [[nodiscard]] static std::string ToString(MessageType_E type) {
    switch (type) {
      case MessageType_E::PUBLISH:
        return PUBLISH_MSG_TYPE_V1;
      case MessageType_E::FILE:
        return FILE_MSG_TYPE_V1;
      case MessageType_E::REQUEST:
        return REQUEST_MSG_TYPE_V1;
      case MessageType_E::RESPONSE:
        return RESPONSE_MSG_TYPE_V1;
      default:
        spdlog::info("type of message enum not defined");
        return "";
    }
  }

  [[nodiscard]] static MessageType_E getEnumType(
      const std::string_view& msg_type) {
    if (msg_type == PUBLISH_MSG_TYPE_V1) {
      return MessageType_E::PUBLISH;
    } else if (msg_type == FILE_MSG_TYPE_V1) {
      return MessageType_E::FILE;
    } else if (msg_type == REQUEST_MSG_TYPE_V1) {
      return MessageType_E::REQUEST;
    } else if (msg_type == RESPONSE_MSG_TYPE_V1) {
      return MessageType_E::RESPONSE;
    }
    spdlog::info("message type not defined\n");
    return MessageType_E::NOT_DEFINED;
  }
};
}  // namespace cloudevents::format

#endif  // CPP_CLOUDEVENT_SERVICE_TYPE_H
