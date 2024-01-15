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
#ifndef UURI_H_
#define UURI_H_

#include <string>
#include <utility>
#include "UriFormat.h"
#include "UAuthority.h"
#include "UEntity.h"
#include "UResource.h"
#include "../uprotocol-core-api/src/main/proto/uri.pb.h"

namespace uprotocol::uri {

/**
 * Data representation of uProtocol <b>URI</b>.
 * This class will be used to represent the source and sink (destination) parts of the Packet, for example in a CloudEvent Packet. <br>
 * UUri is used as a method to uniquely identify devices, services, and resources on the  network.<br>
 * Where software is deployed, what the service is called along with a version and the resources in the service.
 * Defining a common URI for the system allows applications and/or services to publish and discover each other
 * as well as maintain a database/repository of microservices in the various vehicles.<br>
 * Example for long format serialization:
 *     //<device>.<domain>/<service>/<version>/<resource>#<message>
 */
class UUri : public UriFormat {
public:
    /**
     * Create a full  URI.
     * @param uAuthority The uAuthority represents the deployment location of a specific Software Entity .
     * @param uEntity The uEntity in the role of a service or in the role of an application is the software and version.
     * @param uResource The uResource is something that is manipulated by a service such as a Door.
     */
    static auto createUUri(const UAuthority& u_authority,
                           const UEntity& u_entity,
                           const UResource& u_resource) -> UUri {
        return UUri(u_authority, u_entity, u_resource);
    }
    
    
    static auto createLocalUUri(const UEntity& u_entity,
                                const std::string& u_resource) -> UUri {
        return UUri::createUUri(UAuthority::createEmpty().value(), u_entity, UResource::longFormat(u_resource));
    }
    
    static auto createLocalUUri(const UAuthority& u_authority,
                                const UEntity& u_entity,
                                UResource& u_resource) -> UUri {
        return UUri::createUUri(u_authority, u_entity, u_resource);
    }
    
    
    
    /**
     * Create an RPC Response UUri passing the Authority and Entity information.
     * @param uAuthority The uAuthority represents the deployment location of a specific Software Entity.
     * @param uEntity The SW entity information.
     * @return Returns a UUri of a constructed RPC Response.
     */
    static auto rpcResponse(UAuthority const& u_authority,
                            UEntity const& u_entity) -> UUri {
        return UUri(u_authority, u_entity, UResource::forRpcResponse());
    }

    /**
     * Static factory method for creating an empty  uri, to avoid working with null<br>
     * @return Returns an empty uri to avoid working with null.
     */
    static auto createEmpty() -> UUri {
        static const auto empty = UUri(UAuthority::createEmpty().value(), 
                                       UEntity::empty(), 
                                       UResource::createEmpty());
        return empty;
    }

    /**
     * Indicates that this  URI is an empty container and has no valuable information in building uProtocol sinks or sources.
     * @return Returns true if this  URI is an empty container and has no valuable information in building uProtocol sinks or sources.
     */
    [[nodiscard]] auto isEmpty() const -> bool override {
        return uAuthority_.isEmpty() && uEntity_.isEmpty() && uResource_.isEmpty();
    }
    
    /**
     * return the hash of the resource
     * @return std::size_t hash
     */
    [[nodiscard]] auto getHash() const -> std::size_t override {
        return hash_;
    }
    
    /**
     * Returns true if URI contains both names and numeric representations of the names inside its belly.
     * Meaning that this UUri can be serialized to long or micro formats.
     * @return Returns true if URI contains both names and numeric representations of the names inside its belly.
     *      Meaning that this UUri can buree serialized to long or micro formats.
     */
    [[nodiscard]] auto isResolved() const -> bool override {
        return uAuthority_.isResolved() && uEntity_.isResolved() && uResource_.isResolved();
    }

    /**
     * Determines if this UUri can be serialized into a long form UUri.
     * @return Returns true if this UUri can be serialized into a long form UUri.
     */
    [[nodiscard]] auto isLongForm() const -> bool override {
        return uAuthority_.isLongForm() &&
               (uEntity_.isLongForm() || uEntity_.isEmpty()) &&
               (uResource_.isLongForm() || uResource_.isEmpty());
    }

    /**
     * Determines if this UUri can be serialized into a micro form UUri.
     * @return Returns true if this UUri can be serialized into a micro form UUri.
     */
    [[nodiscard]] auto isMicroForm() const -> bool override {
        return uAuthority_.isMicroForm() && uEntity_.isMicroForm() && uResource_.isMicroForm();
    }

    /**
     * @return Returns the Authority represents the deployment location of a specific Software Entity.
     */
    [[nodiscard]] auto getUAuthority() const -> UAuthority { return uAuthority_; }

    /**
     * @return Returns the USE in the role of a service or in the role of an application.
     */
    [[nodiscard]] auto getUEntity() const -> UEntity { return uEntity_; }

    /**
     * @return Returns the  resource, something that is manipulated by a service such as a Door.
     */
    [[nodiscard]] auto getUResource() const -> UResource { return uResource_; }

    /**
     * Compares this UUri to the specified object.
     * @param o The object to compare this UUri against.
     * @return Returns true if and only if the specified object is a UUri whose components are equal to the components of this UUri.
     */
    auto operator==(const UUri& o) const -> bool {
        if (this == &o) {
            return true;
        }
        return (uAuthority_ == o.uAuthority_) &&
               (uEntity_ == o.uEntity_) &&
               (uResource_ == o.uResource_);
    }

    /**
     * Converts this UUri to a String.
     * @return Returns a string representation of this UUri.
     */
    [[nodiscard]] auto toString() const -> std::string {
        return std::string("UriPart{") +
                           "uAuthority=" + uAuthority_.toString() +
                           ", uEntity=" + uEntity_.toString() +
                           ", uResource=" + uResource_.toString() + '}';
    }

private:
    // private constructor
    UUri(const UAuthority& u_authority,
         const UEntity& u_entity,
         const UResource& u_resource)
            : uAuthority_(u_authority), uEntity_(u_entity), uResource_(u_resource) {
        if (!u_authority.isEmpty()) {
            uUri_.mutable_authority()->CopyFrom(u_authority.getProtobufAuthority());
        }
        if (!u_entity.isEmpty()) {
            uUri_.mutable_entity()->CopyFrom(u_entity.getProtobufEntity());
        }
        if (!u_resource.isEmpty()) {
            uUri_.mutable_resource()->CopyFrom(u_resource.getProtoUresource());
        }
        //todo hash
    }
    
    /**
     * The underlying protobuf uri.
     */
    uprotocol::v1::UUri uUri_;
    
    /**
     * UAuthority represents the deployment location of a specific Software Entity.
     */
    const UAuthority uAuthority_;
    /**
     * UEntity represents the role of a service or in the role of an application.
     */
    const UEntity uEntity_;
    /**
     * UResource represents something that is manipulated by a service such as a Door.
     */
    const UResource uResource_;

    std::size_t hash_ = 0;
    
}; // class UUri

}  // namespace uprotocol::uri

#endif  // UURI_H_
