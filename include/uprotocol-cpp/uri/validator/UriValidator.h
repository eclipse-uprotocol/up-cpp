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
 */
#ifndef _URI_VALIDATOR_H_
#define _URI_VALIDATOR_H_

#include "LongUriSerializer.h"
#include "UStatus.h"
#include <string.h>

using namespace uprotocol::utransport;
namespace uprotocol::uri {
/* UriValidator class to validate uri*/
    class UriValidator {
        public:

         /**
          * isValidUri a {@link uri} to ensure that it has at least a name for the
          * uEntity.
          *
          * @param uri {@link uri} to validate.
          * @return Returns bool containing a true or a false
          */

          static bool valid_uri(const std::string& uri);

         /**
          * Validate a {@link UUri} to ensure that it has at least a name for the
          * uEntity.
          *
          * @param uri {@link UUri} to validate.
          * @return Returns Status containing a success or a failure with the error
          * message.
          */
          static UStatus validate(const UUri& uri);

         /**
          * Validate a {@link UUri} that is meant to be used as an RPC method URI. Used
          * in Request sink values and Response source values.
          *
          * @param uri {@link UUri} to validate.
          * @return Returns UStatus containing a success or a failure with the error
          * message.
          */
          static UStatus validateRpcMethod(const UUri& uri);

          /**
           * Validate a {@link UUri} that is meant to be used as an RPC method URI. Used in Request sink values and Response source values.
           *
           * @param uri {@link UUri} to validate.
           * @return Returns Status containing a success or a failure with the error message.
           */
          static UStatus validateRpcResponse(const UUri& uri);

          private:
          UriValidator() = default;

          /**
           * Returns true if URI is of type RPC.
           *
           * @param uri {@link UUri} to check if it is of type RPC method
           * @return Returns true if URI is of type RPC.
           */
          static bool isRpcMethod(const UUri& uri);

          /**
           * Returns true if URI is of type RPC response.
           *
           * @param uri {@link UUri} to check response
           * @return Returns true if URI is of type RPC response.
           */
          static bool isRpcResponse(const UUri& uri);

    };  // class UriValidator

}  // namespace uprotocol::uri

#endif  // _URI_VALIDATOR_H_