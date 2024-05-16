// Copyright (c) 2024 Contributors to the Eclipse Foundation
//
// See the NOTICE file(s) distributed with this work for additional
// information regarding copyright ownership.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// SPDX-FileType: SOURCE
// SPDX-FileCopyrightText: 2024 Contributors to the Eclipse Foundation
// SPDX-License-Identifier: Apache-2.0

#ifndef UP_CPP_UTILS_EXPECTED_H
#define UP_CPP_UTILS_EXPECTED_H

#include <exception>
#include <type_traits>

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
template <typename E>
struct BadExpectedAccess;

template <>
struct BadExpectedAccess<void> : public std::exception {
};

template <typename E>
struct BadExpectedAccess : public BadExpectedAccess<void> {
};

/// @brief A stripped-down version of std::expected from C++23.
template <typename T, typename E>
struct Expected {
	using value_type = T;
	using error_type = E;

	constexpr Expected();
	constexpr Expected(const Expected& other);
	constexpr Expected(Expected&& other) noexcept;

	constexpr explicit Expected(T&& v);
	constexpr explicit Expected(E&& e);

	Expected& operator=(const Expected& other);
	Expected& operator=(Expected&& other);

	Expected& operator=(const T& other);
	Expected& operator=(T&& other);

	Expected& operator=(const E& other);
	Expected& operator=(E&& other);
	
	constexpr explicit operator bool() const noexcept;
	constexpr bool has_value() const noexcept;

	constexpr T& value() &;
	constexpr const T& value() const&;
	constexpr T&& value() &&;
	constexpr const T&& value() const&&;

	constexpr const E& error() const& noexcept;
	constexpr E& error() & noexcept;
	constexpr const E&& error() const&& noexcept;
	constexpr E&& error() && noexcept;

	constexpr T value_or(T&& default_value) const&;
	constexpr T value_or(T&& default_value) &&;

	template<typename F>
	constexpr auto and_then(F&& f) &;
	template<typename F>
	constexpr auto and_then(F&& f) const&;
	template<typename F>
	constexpr auto and_then(F&& f) &&;
	template<typename F>
	constexpr auto and_then(F&& f) const&&;

	template<typename F>
	constexpr auto or_else(F&& f) &;
	template<typename F>
	constexpr auto or_else(F&& f) const&;
	template<typename F>
	constexpr auto or_else(F&& f) &&;
	template<typename F>
	constexpr auto or_else(F&& f) const&&;

private:
	static_assert(!std::is_void_v<T>,
			"We don't allow T==void (unlike std::expected)");

	static_assert(std::is_destructible_v<T> && !std::is_array_v<T> && !std::is_reference_v<T>,
			"Expected requires T to meet the C++ 'destructable' requirement");
};
/// @}

}  // namespace uprotocol::utils

#endif  // UP_CPP_UTILS_EXPECTED_H
