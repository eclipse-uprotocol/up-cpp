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
#ifndef URESOURCE_H_
#define URESOURCE_H_

#include <algorithm>
#include <cctype>
#include <optional>
#include <string>
#include "UriFormat.h"
#include "src/main/proto/uri.pb.h"

namespace uprotocol::uri {

/**
 * A service API - defined in the UEntity - has Resources and Methods. Both of these are represented by the UResource class.<br>
 * A uResource represents a resource from a Service such as "door" and an optional specific instance such as "front_left". In addition, it can optionally contain
 * the name of the resource Message type, such as "Door". The Message type matches the protobuf service IDL that defines structured data types. <br>
 * An UResource is something that can be manipulated/controlled/exposed by a service. Resources are unique when prepended with UAuthority that represents the device and
 * UEntity that represents the service.
 */
class UResource : public UriFormat {
public:
    /**
     * Constructor for building a UResource with empty contents.
     */
    //UResource() { UResource::empty(); }

    /**
     * Build a UResource that has all elements resolved and can be serialized in a long UUri or a micro UUri.
     * @param name The name of the resource as a noun such as door or window, or in the case a method that manipulates the resource, a verb.
     * @param instance An instance of a resource such as front_left.
     * @param message The Message type matches the protobuf service IDL message name that defines structured data types.
     *                A message is a data structure type used to define data that is passed in  events and rpc methods.
     * @param id The numeric representation of this uResource.
     * @return Returns a UResource that has all the information that is needed to serialize into a long UUri or a micro UUri.
     */
    static auto resolvedFormat(const std::string& name,
                                    const std::string& instance,
                                    const std::string& message,
                                    const std::optional<uint16_t> id) -> UResource {
        bool resolved = !isBlank(name) && id.has_value();
        return UResource(name, instance, message, id, resolved);
    }

    /**
     * Build a UResource that can be serialized into a long UUri. Mostly used for publishing messages.
     * @param name The name of the resource as a noun such as door or window, or in the case a method that manipulates the resource, a verb.
     * @return Returns a UResource that can be serialized into a long UUri.
     */
    static auto longFormat(const std::string& name) -> UResource {
        return UResource(name, "", "", std::nullopt, false);
    }

    /**
     * Build a UResource that can be serialized into a long UUri. Mostly used for publishing messages.
     * @param name The name of the resource as a noun such as door or window, or in the case a method that manipulates the resource, a verb.
     * @param instance An instance of a resource such as front_left.
     * @param message The Message type matches the protobuf service IDL message name that defines structured data types.
     *                A message is a data structure type used to define data that is passed in  events and rpc methods.
     * @return Returns a UResource that can be serialised into a long UUri.
     */
    static auto longFormat(const std::string& name,
                                const std::string& instance,
                                const std::string& message) -> UResource {
        return UResource(name, instance, message, std::nullopt, false);
    }

    /**
     * Build a UResource that can be serialised into a micro UUri. Mostly used for publishing messages.
     * @param id The numeric representation of this uResource.
     * @return Returns a UResource that can be serialised into a micro UUri.
     */
    static auto microFormat(const std::optional<uint16_t> id) -> UResource {
        return UResource("", "", "", id, false);
    }

    /**
     * Build a UResource for rpc request, using only the long format.
     * @param methodName The RPC method name.
     * @return Returns a UResource used for an RPC request that could be serialised in long format.
     */
    static auto forRpcRequest(const std::string& method_name) -> UResource {
        return UResource("rpc", method_name, "", std::nullopt, false);
    }

    /**
     * Build a UResource for rpc request, using only the micro format.
     * @param methodId The numeric representation method name for the RPC.
     * @return Returns a UResource used for an RPC request that could be serialised in micro format.
     */
    static auto forRpcRequest(const std::optional<uint16_t> method_id) -> UResource {
        return UResource("rpc", "", "", method_id, false);
    }

    /**
     * Build a UResource for rpc request, using both the long and micro format information.
     * @param methodName The RPC method name.
     * @param methodId The numeric representation method name for the RPC.
     * @return Returns a UResource used for an RPC request that could be serialised in long and micro format.
     */
    static auto forRpcRequest(const std::string& method_name,
                                   const std::optional<uint16_t> method_id) -> UResource {
        bool resolved = !isBlank(method_name) && method_id.has_value();
        return UResource("rpc", method_name, "", method_id, resolved);
    }

    /**
     * Static factory method for creating a response resource that is returned from RPC calls<br>
     * @return Returns a response  resource used for response RPC calls.
     */
    static auto forRpcResponse() -> UResource {
        static const auto response = UResource("rpc", "response", "", 0, true);
        return response;
    }

    /**
     * @return Returns true if this resource specifies an RPC method call or RPC response.
     */
    [[nodiscard]] auto isRPCMethod() const -> bool {
        return ((resource_.name() == std::string("rpc") && !getInstance().empty()) ||
                (resource_.name() == std::string("rpc") && getId().has_value()));
    }

    /**
     * Static factory method for creating an empty  resource, to avoid working with null<br>
     * @return Returns an empty  resource that has a blank name and no message instance information.
     */
    static auto createEmpty() -> UResource {
        static const auto empty = UResource("", "", "", std::nullopt, false);
        return empty;
    }

    /**
     * Indicates that this resource is an empty container and has no valuable information in building uProtocol URI.
     * @return Returns true if this resource is an empty container and has no valuable information in building uProtocol URI.
     */
    [[nodiscard]] auto isEmpty() const -> bool override {
        auto name = resource_.name();
        return (name.empty() || "rpc" == name) &&
                resource_.instance().empty() && resource_.message().empty() && !resource_.has_id();
    }
    
    /**
     * return the hash of the resource
     * @return std::size_t hash
     */
    [[nodiscard]] std::size_t getHash() const override {
        return hash_;
    }
    
    /**
     * return the uResorce protobuf object of type uprotocol::v1::UResource
     * @return uprotocol::v1::UResource protobuf class
     */
    [[nodiscard]] auto getProtoUresource() const ->  uprotocol::v1::UResource {
         return resource_;
    }
    /**
     * @return Returns the name of the resource as a noun such as door or window, or in the case a method that manipulates the resource, a verb.
     */
    [[nodiscard]] auto getName() const -> std::string { return resource_.name(); }

    /**
     * @return Returns the resource id if it exists.
     */
    [[nodiscard]] auto getId() const -> std::optional<uint16_t> { 
        if (resource_.has_id()) {
            return resource_.id();
        }
        return std::nullopt;
    }

    /**
     * An instance of a resource such as front_left
     * or in the case of RPC a method name that manipulates the resource such as UpdateDoor.
     * @return Returns the resource instance of the resource if it exists.
     * If the instance does not exist, it is assumed that all the instances of the resource are wanted.
     */
    [[nodiscard]] auto getInstance() const -> std::string { return resource_.instance(); }

    /**
     * The Message type matches the protobuf service IDL that defines structured data types.
     * A message is a data structure type used to define data that is passed in  events and rpc methods.
     * @return Returns the Message type matches the protobuf service IDL that defines structured data types.
     */
    [[nodiscard]] auto getMessage() const -> std::string { return resource_.message(); }

    /**
     * Return true if this resource contains both ID and names.
     * Method type of UResource requires name, instance, and ID where a topic
     * type of UResource also requires message to not be null
     * @return  Returns true of this resource contains resolved information
     */
    [[nodiscard]] auto isResolved() const -> bool override { return markedResolved_; }

    /**
     * Returns true if the uResource contains names so that it can be serialized to long format.
     * @return Returns true if the uResource contains names so that it can be serialized to long format.
     */
    [[nodiscard]] auto isLongForm() const -> bool override {
        if (resource_.name() == std::string("rpc")) {
            return !getInstance().empty();
        }
        return !resource_.name().empty();
    }

    /**
     * Returns true if the uResource contains the id's which will allow the Uri part to be serialized into micro form.
     * @return Returns true if the uResource can be serialized into micro form.
     */
    [[nodiscard]] auto isMicroForm() const -> bool override { return resource_.has_id(); }

    /**
     * Compare this UResource to another UResource.
     * @return Returns true if the UResource are equal.
     */
    auto operator==(const UResource& o) const -> bool {
        if (this == &o) {
            return true;
        }
        return (markedResolved_ == o.markedResolved_) &&
               (resource_.name() == o.resource_.name()) &&
               (resource_.instance() == o.resource_.instance()) &&
               (resource_.message() == o.resource_.message()) &&
               (resource_.id() == o.resource_.id());
    }

    /**
     * Convert this UResource to a string.
     * @return Returns a string representation of this UResource.
     */
    [[nodiscard]] auto toString() const -> std::string {
        return std::string("UResource{") +  "name='" + resource_.name() + "', " +
                           "instance='" + (resource_.has_instance() ? resource_.instance() : "null") + "', " +
                           "message='" + (resource_.has_message() ? resource_.message() : "null") + "', " +
                           "id=" + (resource_.has_id() ? std::to_string(resource_.id()) : "null") + ", " +
                           "markedResolved=" + (markedResolved_ ? "true" : "false") + "}";
    }

private:
    /**
     * Create a uResource. The resource is something that is manipulated by a service such as a door.
     * @param name      The name of the resource as a noun such as door or window, or in the case a method that manipulates the resource, a verb.
     * @param instance  An instance of a resource such as front_left.
     * @param message   The Message type matches the protobuf service IDL message name that defines structured data types.
     *                  A message is a data structure type used to define data that is passed in  events and rpc methods.
     * @param id        The numeric representation of this uResource.
     * @param marked_resolved Indicates that this uResource was populated with intent of having all data.
     */
    UResource(const std::string& name,
              const std::string& instance,
              const std::string& message,
              const std::optional<uint16_t> id,
              const bool marked_resolved)
              : markedResolved_(marked_resolved) {
        resource_.set_name(isBlank(name) ? "" : name);
        if (!instance.empty()) {
            resource_.set_instance(isBlank(instance) ? "" : instance);
        }
        if (!message.empty()) {
            resource_.set_message(isBlank(message) ? "" : message);
        }
        if (id.has_value()) {
            resource_.set_id(id.value());
        }
        
        hash_ = UResourceHash{}(resource_);
    }

    /**
     * Utility method to verify if the string is blank.
     * @param str   The string to be checked
     * @return bool Returns true if the string is blank.
     */
    [[nodiscard]] static auto isBlank(std::string_view str) -> bool {
        return std::all_of(str.begin(), str.end(), [](char ch) { return std::isspace(ch); });
    }
    

    struct UResourceHash {
        auto operator()(const uprotocol::v1::UResource &uri_resource) const noexcept -> std::size_t {
            std::size_t local_hash = std::hash < std::string > {}(uri_resource.name());
            local_hash = uri_resource.has_instance()
                    ? (std::hash < std::string > {}(uri_resource.instance()) << 1) ^ local_hash 
                    : local_hash;
            local_hash = uri_resource.has_message()
                    ? (std::hash < std::string > {}(uri_resource.message()) << 2) ^ local_hash
                    : local_hash;
            local_hash = uri_resource.has_id()
                    ? (std::hash <uint16_t> {}(static_cast<uint16_t>(uri_resource.id())) << 3) ^ local_hash
                    : local_hash;
            return local_hash;
        }
    };
    
   
    bool markedResolved_;
    uprotocol::v1::UResource resource_;
    std::size_t  hash_;
    std::string  uri_resource_string_long_;
    std::string  uri_resource_string_micro_;
    
}; // class UResource

} // namespace uprotocol::uri

#endif // URESOURCE_H_
