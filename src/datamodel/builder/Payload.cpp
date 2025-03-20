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

#include "up-cpp/datamodel/builder/Payload.h"
namespace uprotocol::datamodel::builder {

// Byte vector constructor
Payload::Payload(const std::vector<uint8_t>& value_bytes,
                 const v1::UPayloadFormat format) {
	if (!UPayloadFormat_IsValid(format)) {
		throw std::out_of_range("Invalid Byte vector payload format");
	}
	std::string serialized(value_bytes.begin(), value_bytes.end());
	payload_ = std::make_tuple(std::move(serialized), format);
}

// String constructor
Payload::Payload(const std::string& value, const v1::UPayloadFormat format) {
	if (!UPayloadFormat_IsValid(format)) {
		throw std::out_of_range("Invalid String payload format");
	}
	payload_ = std::make_tuple(value, format);
}

// Move string constructor
Payload::Payload(std::string&& value, const v1::UPayloadFormat format) {
	if (!UPayloadFormat_IsValid(format)) {
		throw std::out_of_range("Invalid RValue String payload format");
	}
	payload_ = std::make_tuple(std::move(value), format);
}

// Move Serialized constructor
Payload::Payload(Serialized&& serialized) {
	if (!UPayloadFormat_IsValid(std::get<PayloadType::Format>(serialized))) {
		throw std::out_of_range("Invalid RValue Serialized payload format");
	}
	payload_ = std::move(serialized);
}

// google::protobuf::Any constructor
Payload::Payload(const google::protobuf::Any& any) {
	payload_ = std::make_tuple(
	    any.SerializeAsString(),
	    uprotocol::v1::UPayloadFormat::UPAYLOAD_FORMAT_PROTOBUF_WRAPPED_IN_ANY);
}

// Move constructor
Payload::Payload(Payload&& other) noexcept
    : payload_(std::move(other.payload_)), moved_(other.moved_) {}

// Copy constructor
Payload::Payload(const Payload& other) = default;

// Move assignment operator
Payload& Payload::operator=(Payload&& other) noexcept {
	payload_ = std::move(other.payload_);
	moved_ = other.moved_;
	return *this;
}

// Copy assignment operator
Payload& Payload::operator=(const Payload& other) = default;

Payload::PayloadMoved::PayloadMoved(PayloadMoved&& other) noexcept
    : std::runtime_error(std::move(other)) {}

// PayloadMoved move assignment operator
Payload::PayloadMoved& Payload::PayloadMoved::operator=(
    PayloadMoved&& other) noexcept {
	std::runtime_error::operator=(std::move(other));
	return *this;
}

// PayloadMoved copy constructor
Payload::PayloadMoved::PayloadMoved(const PayloadMoved& other) = default;

// PayloadMoved copy assignment operator
Payload::PayloadMoved& Payload::PayloadMoved::operator=(
    const PayloadMoved& other) = default;

// buildCopy method
[[nodiscard]] const Payload::Serialized& Payload::buildCopy() const {
	if (moved_) {
		throw PayloadMoved("Payload has been already moved");
	}
	return payload_;
}

// buildMove method
[[nodiscard]] Payload::Serialized Payload::buildMove() && {
	// Check if payload_ is in the "moved" state
	if (moved_) {
		throw PayloadMoved("Payload has been already moved");
	}
	// Set payload_ to the "moved" state
	moved_ = true;
	return std::move(payload_);
}
}  // namespace uprotocol::datamodel::builder
