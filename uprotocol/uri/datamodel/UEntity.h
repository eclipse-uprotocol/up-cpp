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
#ifndef _UENTITY_H_
#define _UENTITY_H_

#include <algorithm>
#include <cctype>
#include <optional>
#include <string>
#include "UriFormat.h"

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
     * @param version The software version. If not supplied, the latest version of the service will be used.
     * @param id A numeric identifier for the software entity which is a one-to-one correspondence with the software name.
     * @return Returns a complete UEntity with all the information so that it can be used in long form UUri serialisation and micro form UUri serialisation.
     */
    static UEntity resolvedFormat(const std::string& name,
                                  const std::optional<uint8_t> version,
                                  const std::optional<uint16_t> id) {
        bool resolved = !isBlank(name) && id.has_value();
        return UEntity{name, version, id, resolved};
    }

    /**
     * Static factory method for creating a uE using the software entity name, that can be used in long form UUri serialisation.
     * @param name The software entity name, such as petapp or body.access.
     * @return Returns an UEntity with the name where the version is the latest version of the service and can only be serialized
     *      to long UUri format.
     */
    static UEntity longFormat(const std::string& name) {
        return UEntity{name, std::nullopt, std::nullopt, false};
    }

    /**
     * Static factory method for creating a uE using the software entity name, that can be used long form UUri serialisation.
     * @param name The software entity name, such as petapp or body.access.
     * @param version The software entity version.
     * @return Returns an UEntity with the name and the version of the service and can only be serialized
     *      to long UUri format.
     */
    static UEntity longFormat(const std::string& name,
                              const std::optional<uint8_t> version) {
        return UEntity{name, version, std::nullopt, false};
    }

    /**
     * Static factory method for creating a uE using the software entity identification number, that can be used to serialize micro UUris.
     * @param id A numeric identifier for the software entity which is a one-to-one correspondence with the software name.
     * @return Returns an UEntity with the name where the version is the latest version of the service and can only be serialized
     *      to long micro UUri format.
     */
    static UEntity microFormat(const std::optional<uint16_t> id) {
        return UEntity{"", std::nullopt, id, false};
    }

    /**
     * Static factory method for creating a uE using the software entity identification number, that can be used to serialize micro UUris.
     * @param id A numeric identifier for the software entity which is a one-to-one correspondence with the software name.
     * @param version The software entity version.
     * @return Returns an UEntity with the name and the version of the service and can only be serialized
     *      to micro UUri format.
     */
    static UEntity microFormat(const std::optional<uint16_t> id,
                               const std::optional<uint8_t> version) {
      return UEntity{"", version, id, false};
    }

    /**
     * Static factory method for creating an empty software entity, to avoid working with null<br>
     * @return Returns an empty software entity that has a blank name, no unique id and no version information.
     */
    static UEntity empty() {
        static const auto EMPTY = UEntity("", std::nullopt, std::nullopt, false);
        return EMPTY;
    }

    /**
     * Indicates that this software entity is an empty container and has no valuable information in building uProtocol sinks or sources.
     * @return Returns true if this software entity is an empty container and has no valuable information in building uProtocol sinks or sources.
     */
    [[nodiscard]] bool isEmpty() const override {
        return name_.empty() && !getVersion().has_value() && !getId().has_value();
    }

    /**
     * Return true if the UEntity contains both the name and IDs meaning it contains all the information to be serialized
     *      into a long UUri or a micro form UUri.
     * @return  Returns true of this resource contains resolved information meaning it contains all the information to be serialized
     *      into a long UUri or a micro form UUri.
     */
    [[nodiscard]] bool isResolved() const override { return markedResolved_; }

    /**
     * Determine if this software entity can be serialised into a long UUri form.
     * @return Returns true if this software entity can be serialised into a long UUri form, meaning it has at least a name.
     */
    [[nodiscard]] bool isLongForm() const override { return !name_.empty(); }

    /**
     * Returns true if the Uri part contains the id's which will allow the Uri part to be serialized into micro form.
     * @return Returns true if the Uri part can be serialized into micro form, meaning is has at least a unique numeric identifier.
     */
    [[nodiscard]] bool isMicroForm() const override { return id_.has_value(); }

    /**
     * @return Returns the name of the software such as petpp or body.access.
     */
    [[nodiscard]] std::string getName() const { return name_; }

    /**
     * @return Returns the software version if it exists.
     * If the version does not exist, the latest version of the service will be used.
     */
    [[nodiscard]] std::optional<uint8_t> getVersion() const { return version_; }

    /**
     * @return Returns the software id if it exists. The software id represents the numeric identifier of the uE.
     */
    [[nodiscard]] std::optional<uint16_t> getId() const { return id_; }

    /**
     * Compare this UEntity with another UEntity.
     * @return Returns true if the UEntitys are equal.
     */
    [[nodiscard]] bool operator==(const UEntity& o) const {
        if (this == &o) {
            return true;
        }
        return (markedResolved_ == o.markedResolved_) &&
              (name_ == o.name_) &&
              (version_ == o.version_) &&
              (id_ == o.id_);
    }

    /**
     * Convert this UEntity to a string representation.
     * @return Returns a string representation of this UEntity.
     */
    [[nodiscard]] std::string toString() const {
        std::string versionString = (version_ == std::nullopt) ? "null" : std::to_string(version_.value());
        std::string idString = (id_ == std::nullopt) ? "null" : std::to_string(id_.value());
        std::string resolvedString = markedResolved_ ? "true" : "false";
        return std::string("UEntity{") + "name='" + name_ + "', " +
                           "version=" + versionString + ", " +
                           "id=" + idString + ", "
                           "markedResolved=" + resolvedString + "}";
    }

private:
    /**
     * Build a Software Entity that represents a communicating piece of software.
     * @param name The name of the software such as petapp or body.access.
     * @param version The software version. If not supplied, the latest version of the service will be used.
     * @param id A numeric identifier for the software entity which is a one-to-one correspondence with the software name.
     * @param markedResolved Indicates that this uResource was populated with intent of having all data.
     */
    UEntity(const std::string& name,
            const std::optional<uint8_t> version,
            const std::optional<uint16_t> id,
            const bool markedResolved)
            : version_(version), id_(id), markedResolved_(markedResolved) {
        name_ = isBlank(name) ? "" : name;
    }

    /**
     * Utility method to verify if the string is blank.
     * @param str   The string to be checked
     * @return bool Returns true if the string is blank.
     */
    [[nodiscard]] static bool isBlank(std::string_view str) {
        return std::all_of(str.begin(), str.end(), [](char ch) { return std::isspace(ch); });
    }

    /**
     * The name of the Software Entity.
    */
    std::string name_;
    /**
     * The Major version of the Software Entity.
    */
    const std::optional<uint8_t> version_;
    /**
     * The id of the Software Entity.
    */
    const std::optional<uint16_t> id_;
    /**
     * Indicates if this UEntity is resolved.
    */
    const bool markedResolved_ = false;

}; // class UEntity

} // namespace uprotocol::uri

#endif // _UENTITY_H_
