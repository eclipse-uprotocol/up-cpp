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

#include "up-cpp/datamodel/builder/Uuid.h"

#include <random>
#include <stdexcept>

#include "up-cpp/datamodel/constants/UuidConstants.h"

namespace uprotocol::datamodel::builder {

/*
struct uuidbuilder::uuidsharedstate {
    std::mt19937_64 random_engine{std::random_device{}()};
};
*/

UuidBuilder UuidBuilder::getBuilder() { return UuidBuilder(false); }

UuidBuilder UuidBuilder::getTestBuilder() { return UuidBuilder(true); }

UuidBuilder& UuidBuilder::withTimeSource(
    std::function<std::chrono::system_clock::time_point()>&& time_source) {
	if (!testing_) {
		throw std::domain_error(
		    "Cannot set time source on non-test UuidBuilder");
	}
	time_source_ = std::move(time_source);
	return *this;
}

UuidBuilder& UuidBuilder::withRandomSource(
    std::function<uint64_t()>&& random_source) {
	if (!testing_) {
		throw std::domain_error(
		    "Cannot set random source on non-test UuidBuilder");
	}
	random_source_ = std::move(random_source);
	return *this;
}

v1::UUID UuidBuilder::build() {
	v1::UUID uuid;
	auto now = time_source_ ? time_source_() : std::chrono::system_clock::now();
	auto unix_ts_ms =
	    std::chrono::time_point_cast<std::chrono::milliseconds>(now);
	std::mt19937_64 gen{std::random_device{}()};
	std::uniform_int_distribution<uint16_t> dist_a(0, UUID_RANDOM_A_MASK);
	std::uniform_int_distribution<uint64_t> dist_b(0, UUID_RANDOM_B_MASK);

	uint64_t msb = static_cast<uint64_t>(unix_ts_ms.time_since_epoch().count())
	               << UUID_TIMESTAMP_SHIFT;
	msb |= static_cast<uint64_t>(UUID_VERSION_7) << UUID_VERSION_SHIFT;
	msb |=
	    (random_source_ ? random_source_() : dist_a(gen)) & UUID_RANDOM_A_MASK;

	uint64_t lsb =
	    (random_source_ ? random_source_() : dist_b(gen)) & UUID_RANDOM_B_MASK;
	// set the Variant to 10b
	lsb |= static_cast<uint64_t>(UUID_VARIANT_RFC4122) << UUID_VARIANT_SHIFT;

	uuid.set_msb(msb);
	uuid.set_lsb(lsb);
	return uuid;
}

UuidBuilder::UuidBuilder(bool testing)
    : testing_(testing), time_source_(nullptr), random_source_(nullptr) {}

}  // namespace uprotocol::datamodel::builder