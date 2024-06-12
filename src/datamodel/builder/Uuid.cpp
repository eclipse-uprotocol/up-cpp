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

namespace uprotocol::datamodel::builder {

struct UuidBuilder::UuidSharedState {
	std::mt19937_64 random_engine{std::random_device{}()};
	uint16_t counter{0};
	std::chrono::time_point<std::chrono::system_clock,
	                        std::chrono::milliseconds>
	    last_unix_ts_ms{};
	uint64_t rand_b{0};
	bool rand_b_initialized{false};
};

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

UuidBuilder& UuidBuilder::withIndependentState() {
	if (!testing_) {
		throw std::domain_error(
		    "Cannot set independent state on non-test UuidBuilder");
	}
	shared_state_ = std::make_shared<UuidSharedState>();
	return *this;
}

v1::UUID UuidBuilder::build() {
	v1::UUID uuid;
	auto now = time_source_ ? time_source_() : std::chrono::system_clock::now();
	auto unix_ts_ms =
	    std::chrono::time_point_cast<std::chrono::milliseconds>(now);

	if (unix_ts_ms != shared_state_->last_unix_ts_ms) {
		// Reset the counter if the timestamp tick has changed
		shared_state_->counter = 0;
		shared_state_->last_unix_ts_ms = unix_ts_ms;
	}

	uint64_t msb = static_cast<uint64_t>(unix_ts_ms.time_since_epoch().count())
	               << 16;
	msb |= static_cast<uint64_t>(8) << 12;  // Set the version to 8

	if (shared_state_->counter == 4095) {
		// Counter has reached maximum value, freeze it
		msb |= shared_state_->counter;
	} else {
		msb |= shared_state_->counter++;
	}

	uint64_t lsb;
	if (!shared_state_->rand_b_initialized) {
		shared_state_->rand_b = random_source_
		                            ? random_source_()
		                            : std::uniform_int_distribution<uint64_t>{}(
		                                  shared_state_->random_engine) &
		                                  0x3FFFFFFFFFFFFFFF;
		shared_state_->rand_b_initialized = true;
	}
	lsb = shared_state_->rand_b;

	lsb |= 0x8000000000000000;  // Set the variant to 10b

	uuid.set_msb(msb);
	uuid.set_lsb(lsb);
	return uuid;
}

UuidBuilder::UuidBuilder(bool testing)
    : testing_(testing),
      time_source_(nullptr),
      random_source_(nullptr),
      shared_state_(std::make_shared<UuidSharedState>()) {}

}  // namespace uprotocol::datamodel::builder