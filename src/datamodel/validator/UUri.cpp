// SPDX-FileCopyrightText: 2024 Contributors to the Eclipse Foundation
//
// See the NOTICE file(s) distributed with this work for additional
// information regarding copyright ownership.
//
// This program and the accompanying materials are made available under the
// terms of the Apache License Version 2.0 which is available at
// https://www.apache.org/licenses/LICENSE-2.0
//
// SPDX-License-Identifier: Apache-2.0

#include "up-cpp/datamodel/validator/UUri.h"

namespace uprotocol::datamodel::validator::uri {

using namespace uprotocol;

std::string_view message(Reason reason) {
    switch (reason) {
	case Reason::EMPTY:
	    return "The URI authority_name is empty or whitespace.";
	case Reason::RESERVED_VERSION:
	    return "The version is 0 (reserved).";
	case Reason::RESERVED_RESOURCE:
	    return "The resource is 0 for non-RPC-response URIs.";
	case Reason::DISALLOWED_WILDCARD:
	    return "The URI contains a wildcard in one or more fields that is not allowed for this form.";
        case Reason::BAD_RESOURCE_ID:
	    return "The resource ID is not in the allowed range for this URI form.";
	default:
	    return "Unknown reason.";
    }
}

bool uses_wildcards(const v1::UUri& uuri) {
    if (uuri.authority_name().find("*") != std::string::npos) {
        return true;
    }
    if ((uuri.ue_id() & 0xFFFF) == 0xFFFF) { // service ID
        return true;
    }
    if ((uuri.ue_id() & 0xFFFF0000) == 0) {  // service instance ID
        return true;
    }
    if (uuri.ue_version_major() == 0xFF) {
        return true;
    }
    if (uuri.resource_id() == 0xFFFF) {
        return true;
    }
    return false;
}

ValidationResult isValid(const v1::UUri& uuri) {
    auto [valid, reason] = isValidRpcMethod(uuri);
    if (!valid) {
        auto [valid, reason] = isValidRpcResponse(uuri);
        if (!valid) {
            auto [valid, reason] = isValidPublishTopic(uuri);
            if (!valid) {
                return isValidNotification(uuri);
            }
        }
    }

    return { true, std::nullopt };
}

ValidationResult isValidRpcMethod(const v1::UUri& uuri) {
    if (uuri.authority_name().empty()) {
        return { false, Reason::EMPTY };
    }

    // disallow wildcards
    if (uses_wildcards(uuri)) {
        return { false, Reason::DISALLOWED_WILDCARD };
    }

    // check resource ID [0x0001, 0x7FFF]
    uint16_t resource_id = uuri.resource_id() & 0xFFFF;
    if (resource_id == 0 || resource_id > 0x7FFF) {
        return { false, Reason::BAD_RESOURCE_ID };
    }

    return { true, std::nullopt };
}

ValidationResult isValidRpcResponse(const v1::UUri& uuri) {
    if (uuri.authority_name().empty()) {
        return { false, Reason::EMPTY };
    }

    // disallow wildcards
    if (uses_wildcards(uuri)) {
        return { false, Reason::DISALLOWED_WILDCARD };
    }

    if (uuri.resource_id() != 0) {
        return { false, Reason::BAD_RESOURCE_ID };
    }
    return { true, std::nullopt };
}

ValidationResult isValidDefaultSource(const v1::UUri& uuri) {
    return isValidRpcResponse(uuri);
}

ValidationResult isValidPublishTopic(const v1::UUri& uuri) {
    if (uuri.authority_name().empty()) {
        return { false, Reason::EMPTY };
    }

    // disallow wildcards
    if (uses_wildcards(uuri)) {
        return { false, Reason::DISALLOWED_WILDCARD };
    }

    if ((uuri.resource_id() & 0xFFFF) < 0x8000) {
        return { false, Reason::BAD_RESOURCE_ID };
    }

    return { true, std::nullopt };
}

ValidationResult isValidNotification(const v1::UUri& uuri) {
    if (uuri.authority_name().empty()) {
        return { false, Reason::EMPTY };
    }

    // disallow wildcards
    if (uses_wildcards(uuri)) {
        return { false, Reason::DISALLOWED_WILDCARD };
    }

    if ((uuri.resource_id() & 0xFFFF) < 0x8000) {
        return { false, Reason::BAD_RESOURCE_ID };
    }

    return { true, std::nullopt };
}

ValidationResult isValidSubscription(const v1::UUri& uuri) {
    if (uuri.authority_name().empty()) {
        return { false, Reason::EMPTY };
    }

    if ((uuri.resource_id() & 0xFFFF) < 0x8000) {
        return { false, Reason::BAD_RESOURCE_ID };
    }

    return isValidPublishTopic(uuri);
}

ValidationResult isEmpty(const v1::UUri& uuri) {
    if (uuri.authority_name().find_first_not_of(" ") != std::string::npos) {
        return { false, Reason::EMPTY };
    }

    if (uuri.ue_id() != 0) {
        return { false, Reason::RESERVED_RESOURCE };
    }

    if (uuri.ue_version_major() != 0) {
        return { false, Reason::RESERVED_VERSION };
    }

    if (uuri.resource_id() != 0) {
        return { false, Reason::BAD_RESOURCE_ID };
    }

    return { true, std::nullopt };
}

bool isLocal(const v1::UUri& uuri) {
    return (uuri.authority_name().empty());
}

} // namespace
