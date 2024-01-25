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
#ifndef UENTITY_H_
#define UENTITY_H_

#include <algorithm>
#include <cctype>
#include <optional>
#include <string>
#include "../tools/Utils.h"
#include "../uprotocol-core-api/src/main/proto/uri.pb.h"

namespace uprotocol::uri {

/**
 * Data representation of an <b> Software Entity - uE</b><br>
 * Software entities are distinguished by using a unique name or a unique id along with the specific version of the software.
 * An Software Entity is a piece of software deployed somewhere on a uDevice.<br>
 * The Software Entity is used in the source and sink parts of communicating software.<br>
 * A uE that publishes events is a <b>Service</b> role.<br>
 * A uE that consumes events is an <b>Application</b> role.
 */
class UEntity : public UriFormat {
public:
    /**
     * Constructor for building a UEntity with empty contents.
     */
    UEntity() { UEntity::empty(); }

    /**
     * Create a complete UEntity with all the information so that it can be used in long form UUri serialisation and micro form UUri serialisation.
     * In the case of missing elements such as name or id, the UEntity will not be marked as resolvable and will not be usable in serialisation formats.
     * @param name The name of the software such as petapp or body.access.
     * @param major_version The software version. If not supplied, the latest version of the service will be used.
     * @param minor_version The software version. If not supplied, the latest version of the service will be used.
     * @param id A numeric identifier for the software entity which is a one-to-one correspondence with the software name.
     * @return Returns a complete UEntity with all the information so that it can be used in long form UUri serialisation and micro form UUri serialisation.
     */
    [[nodiscard]] static auto resolvedFormat(const std::string& name,
                               const std::optional<uint8_t> major_version,
                               const std::optional<uint8_t> minor_version,
                               const std::optional<uint16_t> id) -> UEntity {
        return UEntity(name, major_version, minor_version, id);
    }
    
    /**
     * get the protobuf entity
     * @return uprotocol::v1::UEntity
     */
    [[nodiscard]] auto getProtobufEntity() const noexcept -> uprotocol::v1::UEntity {
        return entity_;
    }

    /**
     * Static factory method for creating a uE using the software entity name, that can be used in long form UUri serialisation.
     * @param name The software entity name, such as petapp or body.access.
     * @return Returns an UEntity with the name where the version is the latest version of the service and can only be serialized
     *      to long UUri format.
     */
    [[nodiscard]] static auto longFormat(const std::string& name) -> UEntity {
        return UEntity(name, std::nullopt, std::nullopt, std::nullopt);
    }

    /**
     * Static factory method for creating a uE using the software entity name, that can be used long form UUri serialisation.
     * @param name The software entity name, such as petapp or body.access.
     * @param version The software entity version.
     * @return Returns an UEntity with the name and the version of the service and can only be serialized
     *      to long UUri format.
     */
    [[nodiscard]] static auto longFormat(const std::string& name,
                              const std::optional<uint8_t> version) -> UEntity {
        return UEntity(name, version, std::nullopt,std::nullopt);
    }

    /**
     * Static factory method for creating a uE using the software entity identification number, that can be used to serialize micro UUris.
     * @param id A numeric identifier for the software entity which is a one-to-one correspondence with the software name.
     * @return Returns an UEntity with the name where the version is the latest version of the service and can only be serialized
     *      to long micro UUri format.
     */
    [[nodiscard]] static auto microFormat(const std::optional<uint16_t> id) -> UEntity {
        return UEntity("", std::nullopt, std::nullopt,id);
    }

    /**
     * Static factory method for creating a uE using the software entity identification number, that can be used to serialize micro UUris.
     * @param id A numeric identifier for the software entity which is a one-to-one correspondence with the software name.
     * @param version The software entity version.
     * @return Returns an UEntity with the name and the version of the service and can only be serialized
     *      to micro UUri format.
     */
    [[nodiscard]] static auto microFormat(const std::optional<uint16_t> id,
                               const std::optional<uint8_t> version) -> UEntity {
      return UEntity("", version, std::nullopt, id);
    }

    /**
     * Static factory method for creating an empty software entity, to avoid working with null<br>
     * @return Returns an empty software entity that has a blank name, no unique id and no version information.
     */
    static auto empty() -> UEntity {
        static const auto empty = UEntity("", std::nullopt, std::nullopt, std::nullopt);
        return empty;
    }

    /**
     * Indicates that this software entity is an empty container and has no valuable information in building uProtocol sinks or sources.
     * @return Returns true if this software entity is an empty container and has no valuable information in building uProtocol sinks or sources.
     */
    [[nodiscard]] auto isEmpty() const -> bool override {
        return entity_.name().empty() && !getVersion().has_value() && !getId().has_value();
    }
    
    /**
     * return the hash of the resource
     * @return std::size_t hash
     */
    [[nodiscard]] auto getHash() const -> std::size_t override {
        return hash_;
    }
    
    /**
     * Return true if the UEntity contains both the name and IDs meaning it contains all the information to be serialized
     *      into a long UUri or a micro form UUri.
     * @return  Returns true of this resource contains resolved information meaning it contains all the information to be serialized
     *      into a long UUri or a micro form UUri.
     */
    [[nodiscard]] auto isResolved() const noexcept -> bool override { return markedResolved_; }

    /**
     * Determine if this software entity can be serialised into a long UUri form.
     * @return Returns true if this software entity can be serialised into a long UUri form, meaning it has at least a name.
     */
    [[nodiscard]] auto isLongForm() const -> bool override {
        return !entity_.name().empty();
    }

    /**
     * Returns true if the Uri part contains the id's which will allow the Uri part to be serialized into micro form.
     * @return Returns true if the Uri part can be serialized into micro form, meaning is has at least a unique numeric identifier.
     */
    [[nodiscard]] auto isMicroForm() const -> bool override {
        return entity_.has_id();
    }

    /**
     * @return Returns the name of the software such as petpp or body.access.
     */
    [[nodiscard]] auto getName() const -> std::string {
        return entity_.name();
    }

    /**
     * @return Returns the software major version if it exists.
     * If the version does not exist, the latest version of the service will be used.
     */
    [[nodiscard]] auto getMajorVersion() const -> std::optional<uint8_t> {
        if (entity_.has_version_major()) {
            return entity_.version_major();
        }
        return std::nullopt;
    }
    
    /**
     * @return Returns the software major version for compatability with old getVersion.
     * If the version does not exist, the latest version of the service will be used.
     */
    [[nodiscard]] auto getVersion() const -> std::optional<uint8_t> {
        return getMajorVersion();
    }
    
    /**
     * @return Returns the software minor version if it exists.
     * If the version does not exist, the latest version of the service will be used.
     */
    [[nodiscard]] auto getMinorVersion() const -> std::optional<uint8_t> {
        if (entity_.has_version_minor()) {
            return entity_.version_minor();
        }
        return std::nullopt;
    }

    /**
     * @return Returns the software id if it exists. The software id represents the numeric identifier of the uE.
     */
    [[nodiscard]] auto getId() const -> std::optional<uint16_t> {
        if (entity_.has_id()) {
            return entity_.id();
        }
        return std::nullopt;
    }

    /**
     * Compare this UEntity with another UEntity.
     * @return Returns true if the UEntitys are equal.
     */
    [[nodiscard]] auto operator==(const UEntity& o) const -> bool {
        if (this == &o) {
            return true;
        }
        if (entity_.has_version_major() != o.entity_.has_version_major() ||
            entity_.has_version_minor() != o.entity_.has_version_minor() ||
            entity_.has_id() != o.entity_.has_id() ||
            entity_.name() != o.entity_.name() ||
            markedResolved_ != o.markedResolved_) {
            return false;
        }
        bool ret_val = true;
        if (entity_.has_version_major() && o.entity_.has_version_major()) {
            ret_val = entity_.version_major() == o.entity_.version_major();
        }
        
        if (entity_.has_version_minor() && o.entity_.has_version_minor()) {
            ret_val = ret_val && entity_.version_minor() == o.entity_.version_minor();
        }
        
        if (entity_.has_id() && o.entity_.has_id()) {
            ret_val = ret_val && entity_.id() == o.entity_.id();
        }
        
        return ret_val;        
    }

    /**
     * Convert this UEntity to a string representation.
     * @return Returns a string representation of this UEntity.
     */
    [[nodiscard]] auto toString() const -> std::string {
        std::string version = (entity_.has_version_major() ? std::to_string(entity_.version_major()) : "null") + 
                (entity_.has_version_minor() ? "." + std::to_string(entity_.version_minor()) : "");
        std::string id_string = entity_.has_id() ? std::to_string(entity_.id()) : "null";
        std::string resolved_string = markedResolved_ ? "true" : "false";
        auto name = entity_.name().empty() ? "null" : entity_.name();
        return std::string("UEntity{") + "name='" + name + "', " +
                           "version=" + version + ", " +
                           "id=" + id_string + ", "
                           "markedResolved=" + resolved_string + "}";
    }

private:
    /**
     * Build a Software Entity that represents a communicating piece of software.
     * @param name The name of the software such as petapp or body.access.
     * @param major_version The software version. If not supplied, the latest version of the service will be used.
     * @param minor_version The software version. If not supplied, the latest version of the service will be used.
     * @param id A numeric identifier for the software entity which is a one-to-one correspondence with the software name.
     */
    UEntity(const std::string& name,
            const std::optional<uint8_t> major_version,
            const std::optional<uint8_t> minor_version,
            const std::optional<uint16_t> id) {
        markedResolved_ = !isBlank(name) && id.has_value();
    
        entity_.set_name(isBlank(name) ? "" : name);
        if (major_version.has_value()) {
            entity_.set_version_major(major_version.value());
        }
        //don't put minor without major
        if (minor_version.has_value() && major_version.has_value()) {
            entity_.set_version_minor(minor_version.value());
        }
        if (id.has_value()) {
            entity_.set_id(id.value());
        }
        hash_ = UEntityHash{}(entity_);
    }

    /**
     * Utility method to verify if the string is blank.
     * @param str   The string to be checked
     * @return bool Returns true if the string is blank.
     */
    [[nodiscard]] static auto isBlank(std::string_view str) -> bool {
        return std::all_of(str.begin(), str.end(), [](char ch) { return std::isspace(ch); });
    }
    
    struct UEntityHash {
        auto operator()(const uprotocol::v1::UEntity &u_entity) const noexcept -> std::size_t {
            std::size_t h1 = std::hash < std::string > {}(u_entity.name());
            std::size_t h2 =
                    u_entity.has_version_major()
                    ? std::hash < std::uint8_t > {}(static_cast<uint8_t >(u_entity.version_major()))
                    : 0;
            std::size_t h3 =
                    u_entity.has_version_minor()
                    ? std::hash < std::uint8_t > {}(static_cast<uint8_t >(u_entity.version_minor()))
                    : 0;
            std::size_t h4 =
                    u_entity.has_id()
                    ? std::hash <uint16_t> {}(static_cast<uint16_t>(u_entity.id()))
                    : 0;
            return h1 ^ (h2 << 1) ^ (h3 << 2) ^
                   (h4 << 3);  // or use boost::hash_combine
        }
    };
    
    /**
     * The underlying protobuf entity.
     */
    uprotocol::v1::UEntity entity_;

    /**
     * Indicates that this uResource was populated with intent of having all data.
     */
    bool markedResolved_ = false;
    
    /**
     * The hash of the underlying protobuf entity.
     */
    std::size_t hash_ = 0;
    
    
    
}; // class UEntity

} // namespace uprotocol::uri

#endif // UENTITY_H_
