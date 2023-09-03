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

#ifndef _UATTRIBUTES_
#define _UATTRIBUTES_

#include <include/uri/up_uri.h>
#include <include/uuid/uuid_gen.h>
#include "UMessageType.h"
#include "USerializationHint.h"
#include "UPriority.h"

using namespace uri_datamodel;

namespace uprotocol 
{
    namespace utransport
    {
        namespace datamodel
        {
            class UAttributes
            {               
                public:

                    class UAttributesBuilder; 

                    UAttributes()
                    {
                        this->_id = nullptr;
                        this->_type = nullptr;
                        this->_priority = UPriority::UNKNOWN;
                        this->_ttl = 0;
                        this->_token = "";
                        this->_hint = USerializationHint::UNKNOWN;
                        this->_sink = nullptr;
                        this->_plevel = 0;
                        this->_commstatus = 0;
                        this->_reqid = nullptr;
                    };
                    /**
                    * Construct the transport UAttributes object.
                    *
                    * @param id                Unique identifier for the message
                    * @param type              Message type
                    * @param priority          Message priority
                    * @param ttl               Time to live in milliseconds
                    * @param token             Authorization token used for TAP
                    * @param hint              Hint regarding the bytes contained within the UPayload
                    * @param sink              Explicit destination URI
                    * @param plevel            Permission Level
                    * @param commstatus        Communication Status
                    * @param reqid             Request ID
                    * @return Returns a constructed UAttributes.
                    */
                    UAttributes(UUID id, UMessageType &type, UPriority priority, int32_t ttl, std::string token,
                        USerializationHint hint, UUri &sink, int32_t plevel, int32_t commstatus, UUID reqid)
                    {
                        this->_id = id;
                        this->_type = &type;
                        this->_priority = priority;
                        this->_ttl = ttl;
                        this->_token = token;
                        this->_hint = hint;
                        this->_sink = &sink;
                        this->_plevel = plevel;
                        this->_commstatus = commstatus;
                        this->_reqid = reqid;
                    }
                    
                    UAttributes(UAttributesBuilder &builder)
                    {
                        (void)builder;
                    }
                  /**
                //     * Static factory method for creating an empty ultifi cloud event attributes object, to avoid working with null.
                //     * @return Returns an empty transport attributes that indicates that there are no added additional attributes to configure.
                //     */
                        // UAttributes empty()
                        // {
                        //     return new UAttributes(null, null, null, null, null, null, null, null, null, null;
                        // }

                    bool isEmpty()
                    {
                        return ((this->id == nullptr) && (this->type == nullptr) && (this->priority == UPriority::UNKNOWN) && (this->ttl == 0) && (this->token == "")
                                && (this->hint == USerializationHint::UNKNOWN) && (this->sink == nullptr) && (this->plevel == 0) && (this->commstatus == 0)
                                && (this->reqid == nullptr));
                    }

                    /**
                    * Unique identifier for the message.
                    * @return Returns the unique identifier for the message.
                    */
                    UUID id()
                    {
                        return _id;
                    }

                    /**
                    * Message type.
                    * @return Returns the message type.
                    */
                    UMessageType type()
                    {
                        return *_type;
                    }

                    /**
                    * uProtocol Prioritization classifications. 
                    * @return Returns the configured uProtocol Prioritization classifications.
                    */
                    UPriority priority()
                    {
                        return _priority;
                    }

                    /**
                    * hint regarding the bytes contained within the UPayload.
                    * @return Returns an Optional hint regarding the bytes contained within the UPayload.
                    */
                    int32_t ttl() 
                    {
                        return _ttl;
                    }

                    /**
                    * Oauth2 access token to perform the access request defined in the request message.
                    * @return Returns an Optional token attribute.
                    */
                    std::string token()
                    {
                        return _token;
                    }

                    /**
                    * How long this event should live for after it was generated (in milliseconds).
                    * Events without this attribute (or value is 0) MUST NOT timeout.
                    * @return Returns an Optional time to live attribute.
                    */
                    USerializationHint serializationHint()
                    {
                        return _hint;
                    }

                    /**
                    * an explicit destination URI.
                    * @return Returns an Optional destination URI attribute.
                    */
                    UUri sink()
                    {
                        return *_sink;
                    }

                    /**
                    * The reqid is used to return a response for a specific request.
                    * @return Returns an Optional requestId attribute.
                    */
                    UUID reqid()
                    {
                        return _reqid;
                    }

                    /**
                    * The permission level of the message.
                    * @return Returns an Optional permission level attribute.
                    */
                    int32_t plevel()
                    {
                        return _plevel;
                    }

                    /**
                    * The communication status of the message.
                    * @return Returns an Optional communication status attribute.
                    */
                    int32_t commstatus()
                    {
                        return _commstatus;
                    }

                    class UAttributesBuilder
                    {
                        public:
                            UAttributesBuilder();

                            UAttributesBuilder& withId(const UUID& id) {
                                attributes_._id = id;
                                return *this;
                            }

                            UAttributesBuilder& withType(UMessageType type) {
                                attributes_._type = &type;
                                return *this;
                            }

                            UAttributesBuilder& withPriority(UPriority priority) {
                                attributes_._priority = priority;
                                return *this;
                            }

                            UAttributesBuilder& withTtl(int ttl) {
                                attributes_._ttl = ttl;
                                return *this;
                            }

                            UAttributesBuilder& withToken(const std::string& token) {
                                attributes_._token = token;
                                return *this;
                            }

                            UAttributesBuilder& withHint(const USerializationHint& hint) {
                                attributes_._hint = hint;
                                return *this;
                            }

                            UAttributesBuilder& withSink(const UUri& sink) {
                                (void)sink;
                                // attributes_->_sink = &sink;
                                return *this;
                            }

                            UAttributesBuilder& withPermissionLevel(int plevel) {
                                attributes_._plevel = plevel;
                                return *this;
                            }

                            UAttributesBuilder& withCommStatus(int commstatus) {
                                attributes_._commstatus = commstatus;
                                return *this;
                            }

                            UAttributesBuilder& withReqId(const UUID& reqid) {
                                (void) reqid;
                                // attributes_->_reqid = &reqid;
                                return *this;
                            }

                            UAttributes build() {
                                return attributes_;
                            }

                        private:
                             UAttributes &attributes_;
                    };
                private:
                        UUID _id;                  // Unique identifier for the message
                        UMessageType *_type;        // Message type
                        UPriority _priority;       // Message priority

                //     // Optional Attributes
                        int32_t _ttl;              // Time to live in milliseconds
                        std::string _token;        // Authorization token used for TAP
                        USerializationHint _hint;  // Hint regarding the bytes contained within the UPayload
                        UUri *_sink;                // Explicit destination URI
                        int32_t _plevel;           // Permission Level
                        int32_t _commstatus;       // Communication Status
                        UUID _reqid;               // Request ID
            };
        }
    } 
}

#endif /* _UATTRIBUTES_*/