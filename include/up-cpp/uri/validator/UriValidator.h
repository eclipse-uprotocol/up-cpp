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
#ifndef URI_VALIDATOR_H_
#define URI_VALIDATOR_H_

#include <up-cpp/uri/serializer/LongUriSerializer.h>
#include <up-cpp/uri/tools/Utils.h>
#include "up-core-api/uri.pb.h"
#include "up-core-api/ustatus.pb.h"
#include <string.h>

using namespace uprotocol::v1;
namespace uprotocol::uri {
/* UriValidator class to validate uri*/
    class UriValidator {
        public:

         /**
          * @brief a {@link uri} to ensure that it has at least a name for the uEntity.
          *
          * @param[in] uri {@link uri} to validate.
          * @return[out] Returns bool containing a true (valid) or a false (invalid)
          */

          static bool isValidUri(const std::string& uri);

         /**
          * @brief a {@link UUri} to ensure that it has at least a name for the uEntity.
          *
          * @param[in] uri {@link UUri} to validate.
          * @return[out] Returns Status containing a success or a failure with the error
          * message.
          */
          static UStatus validate(const UUri& uri);

         /**
          * @brief Validate a {@link UUri} that is meant to be used as an RPC method URI. Used in Request sink values 
          * and Response source values.
          *
          * @param[in] uri {@link UUri} to validate.
          * @return[out] Returns UStatus containing a success or a failure with the error
          * message.
          */
          static UStatus validateRpcMethod(const UUri& uri);

          /**
           * @brief Validate a {@link UUri} that is meant to be used as an RPC method URI. Used in Request sink values 
           * and Response source values.
           *
           * @param[in] uri {@link UUri} to validate.
           * @return[out] Returns Status containing a success or a failure with the error message.
           */
          static UStatus validateRpcResponse(const UUri& uri);

          /**
           * @brief Returns true if URI is of type RPC.
           *
           * @param[in] uri {@link UUri} to check if it is of type RPC method
           * @return[out] Returns true if URI is of type RPC.
           */
          static bool isRpcMethod(const UUri& uri);

          /**
           * @brief Returns true if URI contains both names and numeric representations of the names inside its belly. 
           * Meaning that this UUri can be serialized to long or micro formats.
           *
           * @param[in] uri {@link UUri} to check if resolved.
           * @return[out] Returns true if URI contains both names and numeric representations of the names inside its belly.
           * Meaning that this UUri can buree serialized to long or micro formats.
           */
          static bool isResolved(const UUri& uri);

          /**
           * @brief Returns true if URI is of type RPC response.
           *
           * @param[in] uri {@link UUri} to check response
           * @return[out] Returns true if URI is of type RPC response.
           */
          static bool isRpcResponse(const UUri& uri);

        private:
          UriValidator() = default;

    };  // class UriValidator

}  // namespace uprotocol::uri

#endif // _URI_VALIDATOR_H_
