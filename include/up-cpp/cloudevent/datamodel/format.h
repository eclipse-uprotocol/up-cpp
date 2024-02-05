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

#ifndef CPP_CLOUDEVENT_FORMAT_H
#define CPP_CLOUDEVENT_FORMAT_H

#include <iostream>

namespace cloudevents::format {

/**
 * enum to define the types supported by the cloud event
 */
enum class DATA_TYPE_E {
  BINARY,    // byte array
  JSON,      // json may be clear text and may have different types like all
             // lower case
  PROTOBUF,  // proto.any we will not use it
};

/**
 * general purpose structure that hold ready to send data type
 * binary data will pass base 64 encoding and decoding to allow string
 */
struct formatted_event {
  DATA_TYPE_E type;  //
  std::string
      serialized_data;  // pay attention that BINARY type will need base 64
};

}  // namespace cloudevents::format
#endif  // CPP_CLOUDEVENT_FORMAT_H
