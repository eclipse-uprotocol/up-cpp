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

namespace uprotocol 
{
    namespace utransport
    {
        namespace datamodel
        {
            public class UAttributes
            {
                public:
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
                    UAttributes(UUID id, UMessageType type, UPriority priority, Integer ttl, String token,
                            USerializationHint hint, UUri sink, Integer plevel, Integer commstatus, UUID reqid)
                    {
                        this.id = id;
                        this.type = type;
                        this.priority = priority;
                        this.ttl = ttl;
                        this.token = token;
                        this.hint = hint;
                        this.sink = sink;
                        this.plevel = plevel;
                        this.commstatus = commstatus;
                        this.reqid = reqid;
                    }

                    UAttributes(UAttributesBuilder builder) 
                    {
                        this(builder.id, builder.type, builder.priority, builder.ttl, builder.token, builder.hint, builder.sink,
                                builder.plevel, builder.commstatus, builder.reqid);
                    }

                     /**
                    * Static factory method for creating an empty ultifi cloud event attributes object, to avoid working with null.
                    * @return Returns an empty transport attributes that indicates that there are no added additional attributes to configure.
                    */
                    UAttributes empty()
                    {
                         return new UAttributes(null, null, null, null, null, null, null, null, null, null;
                    }

                    bool isEmpty()
                    {
                        return this.id == null && this.type == null && this.priority == null && this.ttl == null && this.token == null
                                && this.hint == null && this.sink == null && this.plevel == null && this.commstatus == null
                                && this.reqid == null;
                    }

                    /**
                    * Unique identifier for the message.
                    * @return Returns the unique identifier for the message.
                    */
                    UUID id()
                    {
                        return id;
                    }

                    /**
                    * Message type.
                    * @return Returns the message type.
                    */
                    UMessageType type()
                    {
                        return type;
                    }

                    /**
                    * uProtocol Prioritization classifications. 
                    * @return Returns the configured uProtocol Prioritization classifications.
                    */
                    UPriority priority()
                    {
                        return priority;
                    }

                    /**
                    * hint regarding the bytes contained within the UPayload.
                    * @return Returns an Optional hint regarding the bytes contained within the UPayload.
                    */
                    int32_t ttl() 
                    {
                        return ttl;
                    }

                    /**
                    * Oauth2 access token to perform the access request defined in the request message.
                    * @return Returns an Optional token attribute.
                    */
                    std::string token()
                    {
                        return token;
                    }

                    /**
                    * How long this event should live for after it was generated (in milliseconds).
                    * Events without this attribute (or value is 0) MUST NOT timeout.
                    * @return Returns an Optional time to live attribute.
                    */
                    USerializationHint serializationHint()
                    {
                        return hint;
                    }

                    /**
                    * an explicit destination URI.
                    * @return Returns an Optional destination URI attribute.
                    */
                    UUri sink()
                    {
                        return sink;
                    }

                    /**
                    * The reqid is used to return a response for a specific request.
                    * @return Returns an Optional requestId attribute.
                    */
                    UUID reqid()
                    {
                        return reqid;
                    }

                    /**
                    * The permission level of the message.
                    * @return Returns an Optional permission level attribute.
                    */
                    int32_t plevel()
                    {
                        return plevel;
                    }

                    /**
                    * The communication status of the message.
                    * @return Returns an Optional communication status attribute.
                    */
                    int32_t commstatus()
                    {
                        return commstatus;
                    }

                private:
                    UUID id;                  // Unique identifier for the message
                    UMessageType type;        // Message type
                    UPriority priority;       // Message priority

                    // Optional Attributes
                    int32_t ttl;              // Time to live in milliseconds
                    std::string token;        // Authorization token used for TAP
                    USerializationHint hint;  // Hint regarding the bytes contained within the UPayload
                    UUri sink;                // Explicit destination URI
                    int32_t plevel;           // Permission Level
                    int32_t commstatus;       // Communication Status
                    UUID reqid;               // Request ID
            }
                
            /**
            * Builder for the UAttributes object.
            */
            class UAttributesBuilder 
            {
                public:

                    UAttributesBuilder() {}

                    /**
                    * Add uProtocol Prioritization classifications.
                    * @param priority the uProtocol Prioritization classifications.
                    * @return Returns the UAttributesBuilder with the configured Priority.
                    */
                    UAttributesBuilder withPriority(UPriority priority) 
                    {
                        this.priority = priority;
                        return this;
                    }

                    /**
                    * Add the unique identifier for the message.
                    * @param id the unique identifier for the message.
                    * @return Returns the UAttributesBuilder with the configured id.
                    */
                    UAttributesBuilder withId(UUID id) 
                    {
                        this.id = id;
                        return this;
                    }

                    /**
                    * Add the message type.
                    * @param type the message type.
                    * @return Returns the UAttributesBuilder with the configured type.
                    */
                    UAttributesBuilder withType(UMessageType type)
                    {
                        this.type = type;
                        return this;
                    }

                    /**
                    * Add the time to live in milliseconds.
                    * @param ttl the time to live in milliseconds.
                    * @return Returns the UAttributesBuilder with the configured ttl.
                    */
                    UAttributesBuilder withTtl(int32_t ttl)
                    {
                        this.ttl = ttl;
                        return this;
                    }

                    /**
                    * Add the authorization token used for TAP.
                    * @param token the authorization token used for TAP.
                    * @return Returns the UAttributesBuilder with the configured token.
                    */
                    UAttributesBuilder withToken(std::string token)
                    {
                        this.token = token;
                        return this;
                    }

                    /**
                    * Add the hint regarding the bytes contained within the UPayload.
                    * @param hint the hint regarding the bytes contained within the UPayload.
                    * @return Returns the UAttributesBuilder with the configured hint.
                    */
                    UAttributesBuilder withHint(USerializationHint hint)
                    {
                        this.hint = hint;
                        return this;
                    }

                    /**
                    * Add the explicit destination URI.
                    * @param sink the explicit destination URI.
                    * @return Returns the UAttributesBuilder with the configured sink.
                    */
                    UAttributesBuilder withSink(UUri sink) 
                    {
                        this.sink = sink;
                        return this;
                    }

                    /**
                    * Add the permission level of the message.
                    * @param plevel the permission level of the message.
                    * @return Returns the UAttributesBuilder with the configured plevel.
                    */
                    UAttributesBuilder withPermissionLevel(int32_t plevel)
                    {
                        this.plevel = plevel;
                        return this;
                    }

                    /**
                    * Add the communication status of the message.
                    * @param commstatus the communication status of the message.
                    * @return Returns the UAttributesBuilder with the configured commstatus.
                    */
                    UAttributesBuilder withCommStatus(int32_t commstatus)
                    {
                        this.commstatus = commstatus;
                        return this;
                    }

                    /**
                    * Add the request ID.
                    * @param reqid the request ID.
                    * @return Returns the UAttributesBuilder with the configured reqid.
                    */
                    UAttributesBuilder withReqId(UUID reqid)
                    {
                        this.reqid = reqid;
                        return this;
                    }

                    /**
                    * Construct the UAttributes from the builder.
                    * @return Returns a constructed UAttributes.
                    */
                    UAttributes build()
                    {
                        // validation if needed
                        return new UAttributes(this);
                    }

                private:

                    UUID id;
                    UMessageType type;
                    UPriority priority;
                    int32_t ttl;
                    std::string token;
                    USerializationHint hint;
                    UUri sink;
                    int32_t plevel;
                    int32_t commstatus;
                    UUID reqid;
            }           
        }
    } 
}

