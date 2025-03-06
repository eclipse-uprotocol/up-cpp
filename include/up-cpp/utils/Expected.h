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
#include <utility>
#include <variant>

namespace uprotocol::utils {

// A sentinel keeping watch for when we switch to C++23 and gain access to
// the real std::expected. Our limited copy should be replaced immediately.
static_assert(!__has_cpp_attribute(__cpp_lib_expected),
              "Replace uprotocol::utils::Expected with std::expected");

/// @name Temporary substitute for std::expected
/// @remarks See the reference for std::expected:
///          https://en.cppreference.com/w/cpp/utility/expected
///          No further documentation is provided in this file.
/// @{
struct BadExpectedAccess : public std::runtime_error {
	template <typename... Args>
	explicit BadExpectedAccess(Args&&... args)
	    : std::runtime_error(std::forward<Args>(args)...) {}
};

/// @brief Required tagging type for cases where expected and unexpected type
/// are identical.
template <typename E>
class Unexpected {
public:
	constexpr Unexpected(const Unexpected&) = default;
	constexpr Unexpected(Unexpected&&) noexcept= default;

	constexpr explicit Unexpected(const E& rhs) : storage_(rhs) {}
	constexpr explicit Unexpected(E&& rhs) : storage_(std::move(rhs)) {}

	constexpr const E& error() const& noexcept { return storage_; }
	constexpr E&& error() && noexcept { return std::move(storage_); }

private:
	E storage_;
};

/// @brief A stripped-down version of std::expected from C++23.
template <typename T, typename E>
class Expected {
public:
	constexpr explicit Expected(T arg) : storage_(std::forward<T>(arg)) {}
	//It E and T are the same type, this can cause problems. Previously, this was in use by implicid conversion
	// constexpr explicit Expected(E arg) : storage_(std::forward<Unexpected<E>>(Unexpected<E>(arg))) {}
	constexpr explicit Expected(Unexpected<E> arg) : storage_(std::forward<Unexpected<E>>(arg)) {}

	constexpr Expected(const Expected&) = default;
	constexpr Expected(Expected&&) noexcept = default;

	[[nodiscard]] constexpr bool has_value() const noexcept {
		return std::holds_alternative<T>(storage_);
	}

	constexpr explicit operator bool() const noexcept {
		return std::holds_alternative<T>(storage_);
	}

	template <class X>
	constexpr T value_or(X&& v) const& noexcept {
		return has_value() ? std::get<T>(storage_)
		                   : static_cast<T>(std::forward<X>(v));
	}

	constexpr const T& value() const& {
		if (!has_value()) {
			throw BadExpectedAccess(
				"Attempt to access value() when unexpected.");
		}
		return std::get<T>(storage_);
	}

	constexpr T value() && {
		if (!has_value()) {
			throw BadExpectedAccess(
				"Attempt to access value() when unexpected.");
		}
		return std::move(std::get<T>(storage_));
	}

	constexpr const E& error() const& {
		if (has_value()) {
			throw BadExpectedAccess(
				"Attempt to access error() when not unexpected.");
		}
		return std::get<Unexpected<E>>(storage_).error();
	}

	constexpr E error() && {
		if (has_value()) {
			throw BadExpectedAccess(
				"Attempt to access error() when not unexpected.");
		}
		return std::move(std::get<Unexpected<E>>(storage_)).error();
	}

	constexpr const T& operator*() const {
		if (!has_value()) {
			throw BadExpectedAccess(
				"Attempt to dereference expected value when unexpected.");
		}
		return std::get<T>(storage_);
	}

	constexpr const T* operator->() const {
		if (!has_value()) {
			throw BadExpectedAccess(
				"Attempt to dereference expected pointer when unexpected.");
		}
		return &std::get<T>(storage_);
	}

private:
	std::variant<T, Unexpected<E>> storage_;

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