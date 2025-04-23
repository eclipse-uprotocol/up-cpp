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

#ifndef UP_CPP_DATAMODEL_BUILDER_UUID_H
#define UP_CPP_DATAMODEL_BUILDER_UUID_H

#include <uprotocol/v1/uuid.pb.h>

#include <chrono>
#include <cstdint>
#include <functional>
#include <memory>

namespace uprotocol::datamodel::builder {

/// @brief Builder class designed to build UUID v8 objects for uProtocol.
///
/// @see https://github.com/eclipse-uprotocol/up-spec/blob/main/basics/uuid.adoc
struct UuidBuilder {
	/// @brief Get a UuidBuilder in the default, production mode.
	///
	/// @remarks This should be used in most cases.
	///
	/// @returns A UuidBuilder in the default, production mode.
	static UuidBuilder getBuilder();

	/// @brief Get a UuidBuilder in the test mode.
	///
	/// The testing mode of UuidBuilder allows for the time and random sources
	/// to be replaced such that deterministic tests can be written in
	/// situations where the normal behavior of UuidBuilder would interfere.
	///
	/// @remarks The provided UuidBuilder starts with an identical state to
	///          one returned by getBuilder() with one difference: it will
	///          allow customization of its behavior through the `withX()`
	///          interfaces.
	///
	/// @returns A UuidBuilder in the test mode.
	static UuidBuilder getTestBuilder();

	/// @brief Sets the time source for a UuidBuilder in test mode.
	///
	/// @post All built UUIDs will use the provided function to get time
	///       values instead of calling `std::chrono::system_clock::now()`
	///
	/// @note This can only be used with a UuidBuilder created with the
	///       getTestBuilder() interface.
	///
	/// @param A callable that returns a time_point. Will be called whenever
	///        build() is called to populate the time field in the built UUID.
	///
	/// @throws std::domain_error If called on a non-test UuidBuilder.
	/// @returns A reference to thus UuidBuilder.
	UuidBuilder& withTimeSource(
	    std::function<std::chrono::system_clock::time_point()>&&);

	/// @brief Sets the random value source for a UuidBuilder in test mode.
	///
	/// @post All built UUIDs will use the provided function to get random
	///       values instead of using a true random source.
	///
	/// @note This can only be used with a UuidBuilder created with the
	///       getTestBuilder() interface.
	///
	/// @param A callable that returns a uint64_t. Will be called whenever
	///        build() is called in place of the default random value source.
	///
	/// @throws std::domain_error If called on a non-test UuidBuilder.
	/// @returns A reference to thus UuidBuilder.
	UuidBuilder& withRandomSource(std::function<uint64_t()>&&);

	/// @brief Creates a uProtocol UUID based on the builder's current state.
	///
	/// @remarks As part of the UUID v7/v8 spec, there is a shared state for
	///          all UUID builders within a process. Test builders can override
	///          this with the withIndependentState() interface.
	v1::UUID build();

private:
	explicit UuidBuilder(bool testing);

	const bool testing_{false};
	std::function<std::chrono::system_clock::time_point()> time_source_;
	std::function<uint64_t()> random_source_;
};

}  // namespace uprotocol::datamodel::builder

#endif  // UP_CPP_DATAMODEL_BUILDER_UUID_H
