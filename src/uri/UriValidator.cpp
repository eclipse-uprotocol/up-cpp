/*
 * Copyright (c) 2023 - 2024 General Motors GTO LLC
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
 * SPDX-FileCopyrightText: 2023 - 2024 General Motors GTO LLC
 * SPDX-License-Identifier: Apache-2.0
 */

#include <up-cpp/uri/validator/UriValidator.h>
#include "up-core-api/uri.pb.h"
#include "up-core-api/ustatus.pb.h"
#include <up-cpp/uri/serializer/LongUriSerializer.h>
#include <up-cpp/uri/serializer/MicroUriSerializer.h>
#include <string.h>

using namespace uprotocol::uri;
using namespace uprotocol::v1;

/**
 * @brief isValidUri a {@link uri} to ensure that it has at least a name for the uEntity.
 *
 * @param[in] uri {@link uri} to validate.
 * @return[out] Returns bool containing a true (valid) or a false (invalid)
 */
bool UriValidator::isValidUri(const std::string& uri) {
    UUri uri_view = LongUriSerializer::deserialize(uri);
    return !isEmpty(uri_view);   
}

/**
 * @brief Validate a {@link UUri} to ensure that it has at least a name for the uEntity.
 *
 * @param[in] uri {@link UUri} to validate.
 * 
 * @return[out] Returns Status containing a success or a failure with the error message.
 */
UStatus UriValidator::validate(const UUri& uri) {
    UStatus status;
    if ((nullptr == &uri) || isEmpty(uri)) {
        status.set_code(UCode::INVALID_ARGUMENT);
        status.set_message("Uri is empty.");
    } else if (uri.has_authority() && !isRemote(uri.authority())) {
        status.set_code(UCode::INVALID_ARGUMENT);
        status.set_message("Uri is remote missing uAuthority.");
    } else if (!isEmpty(uri.entity())  && isBlank( uri.entity().name())) {
        status.set_code(UCode::INVALID_ARGUMENT);
        status.set_message("Uri is missing uSoftware Entity name.");
    } else {
        status.set_code(UCode::OK);
        status.set_message("validate Success.");
    }
    return status;
}

/**
 * @brief Validate a {@link UUri} that is meant to be used as an RPC method URI. Used in Request sink values 
 * and Response source values.
 *
 * @param[in] uri {@link UUri} to validate.
 * @return[out] Returns Status containing a success or a failure with the error message.
 */
UStatus UriValidator::validateRpcMethod(const UUri& uri) {
    UStatus status = UriValidator::validate(uri);
    if (status.code() != UCode::OK) {
        return status;
    }
    if (!UriValidator::isRpcMethod(uri)) {
      status.set_code(UCode::INVALID_ARGUMENT);
      status.set_message("Invalid RPC method uri. Uri should be the method to be called, or method from response.");
    }
  return status;
}

/**
 * @brief Validate a {@link UUri} that is meant to be used as an RPC method URI. Used in Request sink values 
 * and Response source values.
 *
 * @param[in] uri {@link UUri} to validate.
 * @return[out] Returns Status containing a success or a failure with the error message.
 */
UStatus UriValidator::validateRpcResponse(const UUri& uri) {
    UStatus status = UriValidator::validate(uri);
    if (status.code() != UCode::OK) {
        return status;
    }
    if (!UriValidator::isRpcResponse(uri)) {
        status.set_code(UCode::INVALID_ARGUMENT);
        status.set_message("Invalid RPC response type.");
    }
    return status;
}

/**
 * @brief Returns true if URI is of type RPC.
 *
 * @param[in] uri {@link UUri} to check if it is of type RPC method
 * @return[out] Returns true if URI is of type RPC.
 */

bool UriValidator::isRpcMethod(const UUri& uri) {
    return (nullptr != &uri) 
    && (!isEmpty(uri.resource())) 
    && (uri.resource().has_instance()) 
    && (!uri.resource().instance().empty()) 
    || (uri.resource().has_id()) 
    && (uri.resource().id() != 0) 
    && (uri.resource().id() < 1000);
}

/**
 * @brief Returns true if URI contains both names and numeric representations of the names inside its belly.
 * Meaning that this UUri can be serialized to long or micro formats.
 *
 * @param[in] uri {@link UUri} to check if resolved.
 * @return[out] Returns true if URI contains both names and numeric representations of the names inside its belly.
 * Meaning that this UUri can buree serialized to long or micro formats.
 */
bool UriValidator::isResolved(const UUri& uri) {
  return (!isEmpty(uri)) 
         && (isLongForm(uri)) 
         && (isMicroForm(uri));
}

/**
 * @brief Returns true if URI is of type RPC response.
 *
 * @param[in] uri {@link UUri} to check response
 * @return[out] Returns true if URI is of type RPC response.
 */
bool UriValidator::isRpcResponse(const UUri& uri) {
    if (nullptr == &uri) {
        return false;
    }
    return (uri.resource().name().find("rpc")) 
    && (uri.resource().instance().find("response")) 
    || (uri.resource().has_id()) 
    && (uri.resource().id() != 0);
}
