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

#ifndef UP_CPP_UTILS_EXPECTED_H
#define UP_CPP_UTILS_EXPECTED_H

#include <stdexcept>
#include <type_traits>
#include <variant>

namespace uprotocol::utils {

// A sentinel keeping watch for when we switch to C++23 and gain access to
// the real std::expected. Our limited copy should be replaced immediately.
static_assert(!__has_cpp_attribute(__cpp_lib_expected),
              "Replace uprotocol::utils::Expected with std::expected");

//////////////////////////////////////////////////////////////////////////////////////////////
// The following implementation is based on a the Expect the Unexpected youtube
// video by Andrei Alexandrescu from CppCon2018.
//
// This implementation is currently missing methods that use reference
// qualifiers. Reference qualifiers on methods was supposed to be introduced in
// C++11, but its doesn't seem to be working in actual g++ 11.4.1-3 in docker
// nor earlier versions.8 Could be the issue is specific to non-const versus
// const. The loss of this part of the implementation means swapping is not
// being done, and useless copies are being done. This should be reexamined if
// there are use cases where the expected value ends up being a large object.
//
//////////////////////////////////////////////////////////////////////////////////////////////

/// @name Temporary substitute for std::expected
/// @remarks See the reference for std::expected:
///          https://en.cppreference.com/w/cpp/utility/expected
///          No further documentation is provided in this file.
/// @{
struct BadExpectedAccess : public std::runtime_error {
	BadExpectedAccess() : std::runtime_error("generic BadExpectedAccess") {}
	BadExpectedAccess(const char* arg) : std::runtime_error(arg) {}
};

/// @brief Required tagging type for cases where expected and unexpected type
/// are identical.
template <typename T>
class Unexpected {
	T t;

public:
	Unexpected(const T& arg) { t = arg; }
	const T& value() const { return t; }
};

/// @brief A stripped-down version of std::expected from C++23.
template <typename T, typename E>
struct Expected {
	std::variant<T, E> storage;
public:
	constexpr Expected() { storage = T(); }
	constexpr Expected(const T& rhs) { storage = rhs; }
	constexpr Expected(const Unexpected<E>& rhs) {
		storage = rhs.value();
	}
	template <class U = T>
	constexpr explicit Expected(U&& rhs) {
		storage = std::forward<U>(rhs);
	}

	constexpr Expected(const Expected& rhs) {
		storage = rhs;
	}

	constexpr bool has_value() const noexcept { return std::holds_alternative<T>(storage); }

	constexpr explicit operator bool() const noexcept { return std::holds_alternative<T>(storage); }

	template <class U>
	constexpr T value_or(U&& v) const& noexcept {
		return has_value() ? std::get<T>(storage) : v;
	}

	constexpr T value() const {
		if (!has_value())
			throw BadExpectedAccess(
			    "Attempt to access value() when unexpected.");
		return std::get<T>(storage);
	}

	constexpr const E& error() const {
		if (has_value())
			throw BadExpectedAccess(
			    "Attempt to access error() when not unexpected.");
		return std::get<E>(storage);
	}

	constexpr T& operator*() {
		if (!has_value())
			throw BadExpectedAccess(
			    "Attempt to non-const dereference expected value when "
			    "unexpected.");
		return std::get<T>(storage);
	}

	constexpr const T& operator*() const {
		if (!has_value())
			throw BadExpectedAccess(
			    "Attempt to const dereference expected value when unexpected.");
		return std::get<T>(storage);
	}

	constexpr T* operator->() {
		if (!has_value())
			throw BadExpectedAccess(
			    "Attempt to dereference expected pointer when unexpected.");
		return &**this;
	}

private:
	static_assert(!std::is_void_v<T>,
	              "We don't allow T==void (unlike std::expected)");

	static_assert(
	    std::is_destructible_v<T> && !std::is_array_v<T> &&
	        !std::is_reference_v<T>,
	    "Expected requires T to meet the C++ 'destructable' requirement");
};
/// @}

}  // namespace uprotocol::utils

#endif  // UP_CPP_UTILS_EXPECTED_H