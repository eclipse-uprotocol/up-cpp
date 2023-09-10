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

#include <memory>
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
                        this->_type = UMessageType::UNDEFINED;
                        this->_priority = UPriority::UNDEFINED;
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
                    * @return Returns a constructed UAttributes.
                    */
                    UAttributes(UUID id, UMessageType type, UPriority priority)
                    {
                        this->_id = id;
                        this->_type = type;
                        this->_priority = priority;
                        // optional 
                        this->_ttl = 0;
                        this->_token = "";
                        this->_hint = USerializationHint::UNKNOWN;
                        this->_sink = nullptr;
                        this->_plevel = 0;
                        this->_commstatus = 0;
                        this->_reqid = nullptr;
                    }
                                    
                   /**
                    * Static factory method for creating an empty ultifi cloud event attributes object, to avoid working with null.
                    * @return Returns an empty transport attributes that indicates that there are no added additional attributes to configure.
                    */
                    UAttributes* empty()
                    {
                        return new UAttributes();
                    }

                    bool isEmpty()
                    {
                        return ((this->_id == nullptr) && (this->_type == UMessageType::UNDEFINED) && (this->_priority == UPriority::UNDEFINED) && 
                            (this->_ttl == 0) && (this->_token == "") && (this->_hint == USerializationHint::UNKNOWN) && (this->_sink == nullptr) && 
                            (this->_plevel == 0) && (this->_commstatus == 0) && (this->_reqid == nullptr));
                    }

                    /**
                    * Unique identifier for the message.
                    * @return Returns the unique identifier for the message.
                    */
                    UUID id() const
                    {
                        return _id;
                    }

                    /**
                    * Message type.
                    * @return Returns the message type.
                    */
                    UMessageType type() const
                    {
                        return _type;
                    }

                    /**
                    * uProtocol Prioritization classifications. 
                    * @return Returns the configured uProtocol Prioritization classifications.
                    */
                    UPriority priority() const
                    {
                        return _priority;
                    }

                    /**
                    * hint regarding the bytes contained within the UPayload.
                    * @return Returns an Optional hint regarding the bytes contained within the UPayload.
                    */
                    std::optional<int32_t> ttl() const
                    {
                        if (_ttl)
                        {
                            return _ttl;
                        }

                        return std::nullopt;
                    }

                    /**
                    * Oauth2 access token to perform the access request defined in the request message.
                    * @return Returns an Optional token attribute.
                    */
                    std::optional<std::string> token() const
                    {
                        if (!_token.value().empty()) 
                        {
                            return _token;
                        }

                        return std::nullopt;
                    }

                    /**
                    * How long this event should live for after it was generated (in milliseconds).
                    * Events without this attribute (or value is 0) MUST NOT timeout.
                    * @return Returns an Optional time to live attribute.
                    */
                    std::optional<USerializationHint> serializationHint() const
                    {
                        if (_hint)
                        {
                            return _hint;
                        }
                        
                        return std::nullopt;
                    }

                    /**
                    * an explicit destination URI.
                    * @return Returns an Optional destination URI attribute.
                    */
                    std::optional<UUri> sink() const
                    {
                        if (_sink)
                        {
                            return *_sink;
                        }
                        
                        return std::nullopt;
                    }

                    /**
                    * The reqid is used to return a response for a specific request.
                    * @return Returns an Optional requestId attribute.
                    */
                    std::optional<UUID> reqid() const
                    {
                        if (!_reqid)
                        {
                            return _reqid;
                        }
                        
                        return std::nullopt;
                    }

                    /**
                    * The permission level of the message.
                    * @return Returns an Optional permission level attribute.
                    */
                    std::optional<int32_t> plevel() const
                    {
                        if (_plevel)
                        {
                            return _plevel;
                        }
                        
                        return std::nullopt;
                    }

                    /**
                    * The communication status of the message.
                    * @return Returns an Optional communication status attribute.
                    */
                    std::optional<int32_t> commstatus() const
                    {
                        if (_commstatus)
                        {
                            return _commstatus;
                        }
                        
                        return std::nullopt;
                    }

                    class UAttributesBuilder
                    {
                        public:
                            UAttributesBuilder(UUID id, UMessageType type, UPriority priority)
                            {
                                _attributes = std::make_shared<UAttributes>();

                                if (nullptr != _attributes)
                                {
                                    _attributes->_id = id;
                                    _attributes->_type = type;
                                    _attributes->_priority = priority;
                                }
                            }

                            UAttributesBuilder& withTtl(int ttl)
                            {
                                _attributes->_ttl = ttl;
                                return *this;
                            }

                            UAttributesBuilder& withToken(const std::string& token)
                            {
                                _attributes->_token = token;
                                return *this;
                            }

                            UAttributesBuilder& withHint(const USerializationHint& hint)
                            {
                                _attributes->_hint = hint;
                                return *this;
                            }

                            UAttributesBuilder& withSink(const UUri& sink)
                            {
                                std::optional<UUri> optionalUri = sink; 
                                _attributes->_sink = &optionalUri;
                                return *this;
                            }

                            UAttributesBuilder& withPermissionLevel(int plevel)
                            {
                                _attributes->_plevel = plevel;
                                return *this;
                            }

                            UAttributesBuilder& withCommStatus(int commstatus)
                            {
                                _attributes->_commstatus = commstatus;
                                return *this;
                            }

                            UAttributesBuilder& withReqId(const UUID& reqid)
                            {
                                _attributes->_reqid = reqid;
                                return *this;
                            }

                            UAttributes build()
                            {
                                return *_attributes;
                            }

                        private:
                            std::shared_ptr<UAttributes> _attributes;
                    };
                    
                private:

                    UUID _id;                                   // Unique identifier for the message
                    UMessageType _type;                         // Message type
                    UPriority _priority;                        // Message priority
                    // Optional Attributes 
                    std::optional<int32_t> _ttl;                // Time to live in milliseconds
                    std::optional<std::string> _token;          // Authorization token used for TAP
                    std::optional<USerializationHint> _hint;    // Hint regarding the bytes contained within the UPayload
                    std::optional<UUri> *_sink;                 // Explicit destination URI
                    std::optional<int32_t> _plevel;             // Permission Level
                    std::optional<int32_t> _commstatus;         // Communication Status
                    std::optional<UUID> _reqid;                 // Request ID
            };
        }
    } 
}

#endif /* _UATTRIBUTES_*/