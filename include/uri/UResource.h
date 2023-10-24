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
#ifndef _URESOURCE_H_
#define _URESOURCE_H_

#include <algorithm>
#include <cctype>
#include <optional>
#include <string>
#include "UriFormat.h"

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
    UResource() { UResource::empty(); }

    /**
     * Build a UResource that has all elements resolved and can be serialized in a long UUri or a micro UUri.
     * @param name The name of the resource as a noun such as door or window, or in the case a method that manipulates the resource, a verb.
     * @param instance An instance of a resource such as front_left.
     * @param message The Message type matches the protobuf service IDL message name that defines structured data types.
     *                A message is a data structure type used to define data that is passed in  events and rpc methods.
     * @param id The numeric representation of this uResource.
     * @return Returns a UResource that has all the information that is needed to serialize into a long UUri or a micro UUri.
     */
    static UResource resolvedFormat(const std::string& name,
                                    const std::string& instance,
                                    const std::string& message,
                                    const std::optional<uint16_t> id) {
        bool resolved = !isBlank(name) && id.has_value();
        return UResource{name, instance, message, id, resolved};
    }

    /**
     * Build a UResource that can be serialized into a long UUri. Mostly used for publishing messages.
     * @param name The name of the resource as a noun such as door or window, or in the case a method that manipulates the resource, a verb.
     * @return Returns a UResource that can be serialized into a long UUri.
     */
    static UResource longFormat(const std::string& name) {
        return UResource{name, "", "", std::nullopt, false};
    }

    /**
     * Build a UResource that can be serialized into a long UUri. Mostly used for publishing messages.
     * @param name The name of the resource as a noun such as door or window, or in the case a method that manipulates the resource, a verb.
     * @param instance An instance of a resource such as front_left.
     * @param message The Message type matches the protobuf service IDL message name that defines structured data types.
     *                A message is a data structure type used to define data that is passed in  events and rpc methods.
     * @return Returns a UResource that can be serialised into a long UUri.
     */
    static UResource longFormat(const std::string& name,
                                const std::string& instance,
                                const std::string& message) {
        return UResource{name, instance, message, std::nullopt, false};
    }

    /**
     * Build a UResource that can be serialised into a micro UUri. Mostly used for publishing messages.
     * @param id The numeric representation of this uResource.
     * @return Returns a UResource that can be serialised into a micro UUri.
     */
    static UResource microFormat(const std::optional<uint16_t> id) {
        return UResource{"", "", "", id, false};
    }

    /**
     * Build a UResource for rpc request, using only the long format.
     * @param methodName The RPC method name.
     * @return Returns a UResource used for an RPC request that could be serialised in long format.
     */
    static UResource forRpcRequest(const std::string& methodName) {
        return UResource{"rpc", methodName, "", std::nullopt, false};
    }

    /**
     * Build a UResource for rpc request, using only the micro format.
     * @param methodId The numeric representation method name for the RPC.
     * @return Returns a UResource used for an RPC request that could be serialised in micro format.
     */
    static UResource forRpcRequest(const std::optional<uint16_t> methodId) {
        return UResource{"rpc", "", "", methodId, false};
    }

    /**
     * Build a UResource for rpc request, using both the long and micro format information.
     * @param methodName The RPC method name.
     * @param methodId The numeric representation method name for the RPC.
     * @return Returns a UResource used for an RPC request that could be serialised in long and micro format.
     */
    static UResource forRpcRequest(const std::string& methodName,
                                   const std::optional<uint16_t> methodId) {
        bool resolved = !isBlank(methodName) && methodId.has_value();
        return UResource{"rpc", methodName, "", methodId, resolved};
    }

    /**
     * Static factory method for creating a response resource that is returned from RPC calls<br>
     * @return Returns a response  resource used for response RPC calls.
     */
    static UResource forRpcResponse() {
        static const auto RESPONSE = UResource("rpc", "response", "", 0, true);
        return RESPONSE;
    }

    /**
     * @return Returns true if this resource specifies an RPC method call or RPC response.
     */
    [[nodiscard]] bool isRPCMethod() const {
        return ((name_ == std::string("rpc") && !getInstance().empty()) ||
                (name_ == std::string("rpc") && getId().has_value()));
    }

    /**
     * Static factory method for creating an empty  resource, to avoid working with null<br>
     * @return Returns an empty  resource that has a blank name and no message instance information.
     */
    static UResource empty() {
        static const auto EMPTY = UResource("", "", "", std::nullopt, false);
        return EMPTY;
    }

    /**
     * Indicates that this resource is an empty container and has no valuable information in building uProtocol URI.
     * @return Returns true if this resource is an empty container and has no valuable information in building uProtocol URI.
     */
    [[nodiscard]] bool isEmpty() const override {
        return (name_.empty() || "rpc" == name_) &&
                instance_.empty() && message_.empty() && !id_.has_value();
    }

    /**
     * @return Returns the name of the resource as a noun such as door or window, or in the case a method that manipulates the resource, a verb.
     */
    [[nodiscard]] std::string getName() const { return name_; }

    /**
     * @return Returns the resource id if it exists.
     */
    [[nodiscard]] std::optional<uint16_t> getId() const { return id_; }

    /**
     * An instance of a resource such as front_left
     * or in the case of RPC a method name that manipulates the resource such as UpdateDoor.
     * @return Returns the resource instance of the resource if it exists.
     * If the instance does not exist, it is assumed that all the instances of the resource are wanted.
     */
    [[nodiscard]] std::string getInstance() const { return instance_; }

    /**
     * The Message type matches the protobuf service IDL that defines structured data types.
     * A message is a data structure type used to define data that is passed in  events and rpc methods.
     * @return Returns the Message type matches the protobuf service IDL that defines structured data types.
     */
    [[nodiscard]] std::string getMessage() const { return message_; }

    /**
     * Return true if this resource contains both ID and names.
     * Method type of UResource requires name, instance, and ID where a topic
     * type of UResource also requires message to not be null
     * @return  Returns true of this resource contains resolved information
     */
    [[nodiscard]] bool isResolved() const override { return markedResolved_; }

    /**
     * Returns true if the uResource contains names so that it can be serialized to long format.
     * @return Returns true if the uResource contains names so that it can be serialized to long format.
     */
    [[nodiscard]] bool isLongForm() const override {
        if (name_ == std::string("rpc")) {
            return !getInstance().empty();
        }
        return !name_.empty();
    }

    /**
     * Returns true if the uResource contains the id's which will allow the Uri part to be serialized into micro form.
     * @return Returns true if the uResource can be serialized into micro form.
     */
    [[nodiscard]] bool isMicroForm() const override { return id_.has_value(); }

    /**
     * Compare this UResource to another UResource.
     * @return Returns true if the UResource are equal.
     */
    bool operator==(const UResource& o) const {
        if (this == &o) {
            return true;
        }
        return (markedResolved_ == o.markedResolved_) &&
               (name_ == o.name_) &&
               (instance_ == o.instance_) &&
               (message_ == o.message_) &&
               (id_ == o.id_);
    }

    /**
     * Convert this UResource to a string.
     * @return Returns a string representation of this UResource.
     */
    [[nodiscard]] std::string toString() const {
        return std::string("UResource{") +  "name='" + name_ + "', " +
                           "instance='" + (instance_.empty() ? "null" : instance_) + "', " +
                           "message='" + (message_.empty() ? "null" : message_) + "', " +
                           "id=" + (id_.has_value() ? std::to_string(id_.value()) : "null") + ", " +
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
     * @param markedResolved Indicates that this uResource was populated with intent of having all data.
     */
    UResource(const std::string& name,
              const std::string& instance,
              const std::string& message,
              const std::optional<uint16_t> id,
              const bool markedResolved)
              : id_(id), markedResolved_(markedResolved) {
        name_ = isBlank(name) ? "" : name;
        instance_ = isBlank(instance) ? "" : instance;
        message_ = isBlank(message) ? "" : message;
    }

    /**
     * Utility method to verify if the string is blank.
     * @param str   The string to be checked
     * @return bool Returns true if the string is blank.
     */
    [[nodiscard]] static bool isBlank(std::string_view str) {
        return std::all_of(str.begin(), str.end(), isspace);
    }

    /**
     * The name of the resource such as "door".
     */
    std::string name_;
    /**
     * The instance of the resource such as "front_left".
     */
    std::string instance_;
    /**
     * The message type of the resource.
     */
    std::string message_;
    /**
     * The id of the resource.
     */
    std::optional<uint16_t> id_;
    /**
     * Indicates that this uResource was populated with intent of having all data.
     */
    bool markedResolved_;

}; // class UResource

} // namespace uprotocol::uri

#endif // _URESOURCE_H_
