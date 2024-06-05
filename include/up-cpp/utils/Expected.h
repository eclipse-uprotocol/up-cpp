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
template <typename E>
struct Unexpected {
	E storage;

	constexpr Unexpected(const Unexpected&) = default;
	constexpr Unexpected(Unexpected&&) = default;

	constexpr explicit Unexpected(const E& rhs) : storage(rhs) {}
	constexpr explicit Unexpected(E&& rhs) : storage(std::move(rhs)) {}

	template <class... Args>
	constexpr explicit Unexpected(std::in_place_t, Args&&... args)
	    : storage(args...) {}

	// template <class U, class... Args>
	// constexpr explicit Unexpected(std::in_place_t, std::initializer_list<U>
	// il, Args&&... args)

	constexpr const E& error() const& noexcept { return std::move(storage); }

	// this one is checked
	constexpr E& error() & noexcept { return storage; }

	constexpr const E&& error() const&& noexcept { return std::move(storage); }

	constexpr E&& error() && noexcept { return std::move(storage); }
};

/// @brief A stripped-down version of std::expected from C++23.
template <typename E, typename U>
struct Expected {
	std::variant<E, Unexpected<U>> storage;

public:
	constexpr Expected() { storage = E(); }
	constexpr Expected(E&& rhs) : storage(std::move(rhs)) {}
	constexpr Expected(Unexpected<U>&& rhs) : storage(std::move(rhs)) {}

	constexpr Expected(const Unexpected<U>& rhs) {
		std::get<Unexpected<U>>(storage).storage = std::move(rhs);
	}

	template <class X = E>
	constexpr explicit Expected(X&& rhs) {
		std::get<E>(storage) = std::forward<X>(rhs);
	}

	template <class X = U>
	constexpr explicit Expected(Unexpected<X>&& rhs) {
		std::get<E>(storage) = rhs.error();
	}

	constexpr Expected(const Expected& rhs) { storage = rhs.storage; }

	constexpr bool has_value() const noexcept {
		return std::holds_alternative<E>(storage);
	}

	constexpr explicit operator bool() const noexcept {
		return std::holds_alternative<E>(storage);
	}

	template <class X>
	constexpr E value_or(X&& v) const& noexcept {
		return has_value() ? std::get<E>(storage) : v;
	}

	constexpr const E& value() const& {
		if (!has_value())
			throw BadExpectedAccess(
			    "Attempt to access value() when unexpected.");
		return std::move(std::get<E>(storage));
	}

	constexpr E value() && {
		if (!has_value())
			throw BadExpectedAccess(
			    "Attempt to access value() when unexpected.");
		return std::move(std::get<E>(storage));
	}

	constexpr const U& error() const& {
		if (has_value())
			throw BadExpectedAccess(
			    "Attempt to access error() when not unexpected.");
		return std::move(std::get<Unexpected<U>>(storage).error());
	}

	constexpr U error() && {
		if (has_value())
			throw BadExpectedAccess(
			    "Attempt to access error() when not unexpected.");
		return std::move(std::get<Unexpected<U>>(storage)).error();
	}

	constexpr E& operator*() {
		if (!has_value())
			throw BadExpectedAccess(
			    "Attempt to non-const dereference expected value when "
			    "unexpected.");
		return std::get<E>(storage);
	}

	constexpr const E& operator*() const {
		if (!has_value())
			throw BadExpectedAccess(
			    "Attempt to const dereference expected value when unexpected.");
		return std::get<E>(storage);
	}

	constexpr E* operator->() {
		if (!has_value())
			throw BadExpectedAccess(
			    "Attempt to dereference expected pointer when unexpected.");
		return &**this;
	}

private:
	static_assert(!std::is_void_v<E>,
	              "We don't allow E==void (unlike std::expected)");

	static_assert(
	    std::is_destructible_v<E> && !std::is_array_v<E> &&
	        !std::is_reference_v<E>,
	    "Expected requires E to meet the C++ 'destructable' requirement");
};

/// @}

}  // namespace uprotocol::utils

#endif  // UP_CPP_UTILS_EXPECTED_H