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

#include "UriValidator.h"
#include "UUri.h"
#include "UStatus.h"
#include "google/protobuf/stubs/status.h"
#include "UResource.h"
#include "LongUriSerializer.h"
#include <string.h>

using namespace uprotocol::uri;
using namespace uprotocol::utransport;

/**
 * isValidUri a {@link uri} to ensure that it has at least a name for the
 * uEntity.
 *
 * @param uri {@link uri} to validate.
 * @return Returns bool containing a true or a false
 */
bool UriValidator::isValidUri(const std::string& uri) {
    auto uri_view = LongUriSerializer::deserialize(uri);
    return !uri_view.isEmpty();   
}

/**
 * Validate a {@link UUri} to ensure that it has at least a name for the uEntity.
 *
 * @param uri {@link UUri} to validate.
 * 
 * @return Returns Status containing a success or a failure with the error message.
 */
UStatus UriValidator::validate(const UUri& uri) {
  if (uri.isEmpty()) {
    return UStatus::INVALID_ARGUMENT;
  }
  if (!uri.getUAuthority().isEmpty() && !uri.getUAuthority().isRemote()) {
    return UStatus::INVALID_ARGUMENT;
  }
  if (!uri.getUEntity().isEmpty()  && uri.getUEntity().getName().empty()) {
    return UStatus::INVALID_ARGUMENT;
  }

  return UStatus::OK;
}

/**
 * Validate a {@link UUri} that is meant to be used as an RPC method URI. Used in Request sink values and Response source values.
 *
 * @param uri {@link UUri} to validate.
 * @return Returns Status containing a success or a failure with the error message.
 */
UStatus UriValidator::validateRpcMethod(const UUri& uri) {
    UStatus status = UriValidator::validate(uri);
    if (status != UStatus::OK) {
        return status;
    }
    if (!UriValidator::isRpcMethod(uri)) {
        return UStatus::INVALID_ARGUMENT;
    }
    
    return UStatus::OK;
}

/**
 * Validate a {@link UUri} that is meant to be used as an RPC method URI. Used in Request sink values and Response source values.
 *
 * @param uri {@link UUri} to validate.
 * @return Returns Status containing a success or a failure with the error message.
 */
UStatus UriValidator::validateRpcResponse(const UUri& uri) {
    UStatus status = UriValidator::validate(uri);
    if (status != UStatus::OK) {
        return status;
    }
    if (!UriValidator::isRpcResponse(uri)) {
        return UStatus::INVALID_ARGUMENT;
    }
    
    return UStatus::OK;
}

/**
 * Returns true if URI is of type RPC.
 *
 * @param uri {@link UUri} to check if it is of type RPC method
 * @return Returns true if URI is of type RPC.
 */

bool UriValidator::isRpcMethod(const UUri& uri) {
    return (!uri.isEmpty() && 
     !uri.getUResource().isEmpty() &&
     uri.getUResource().isRPCMethod() && 
     !uri.getUResource().getInstance().empty() ||
     (uri.getUResource().getId().has_value() && 
     uri.getUResource().getId() != 0 && 
     uri.getUResource().getId() < 1000));
}

/**
 * Returns true if URI is of type RPC response.
 *
 * @param uri {@link UUri} to check response
 * @return Returns true if URI is of type RPC response.
 */
bool UriValidator::isRpcResponse(const UUri& uri) {
    return (UriValidator::isRpcMethod(uri) &&
     uri.getUResource().getInstance().find("reponse") ||
     (uri.getUResource().getId().has_value()  && uri.getUResource().getId() != 0));
}
