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
#ifndef UP_CPP_DATAMODEL_SERIALIZER_URI_H
#define UP_CPP_DATAMODEL_SERIALIZER_URI_H

#include <up-core-api/uri.pb.h>

namespace uprotocol::datamodel::serializer::uri {

struct AsString {
    [[nodiscard]] std::string serialize(const v1::UUri&);
    [[nodiscard]] v1::UUri deserialize(const std::string&);

    constexpr static v1::PayloadFomrat format{v1::PayloadFormat::UPAYLOAD_FORMAT_TEXT};
};

} // namespace uprotocol::datamodel::serializer::uri

#endif // UP_CPP_DATAMODEL_SERIALIZER_URI_H
