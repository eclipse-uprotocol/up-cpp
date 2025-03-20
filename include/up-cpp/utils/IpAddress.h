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

#ifndef UP_CPP_UTILS_IPADDRESS_H
#define UP_CPP_UTILS_IPADDRESS_H

#include <cstdint>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

namespace uprotocol::utils {

/// @brief Utility for converting IP addresses between representations and for
///        normalizing string representations.
///
/// IP addresses can be represented as either human-readable strings or binary
/// numbers, and may be either IPv4 or IPv6. Additionally, each address family
/// has multiple allowable string representations. For example these IPv6
/// addresses are all the same:
///
///   * `2001::C0FF:EE01`
///   * `2001:0000:0000:0000:0000:0000:c0ff:ee01`
///   * `2001:0000:0000:0000::0000:192.255.238.1`
///
/// This class provides three utilities:
///
///   1. Given a string, it will detect if it contains an IP address and which
///      family (IPv4 or IPv6) it contains.
///   2. Given a string IP address, it will convert the address to its binary
///      representation.
///   3. Given a binary representation of an IP address, it will convert it
///      to a string.
///   4. In all cases, it will produce a consistent, normalized string
///      representation of an IP address.
///
/// Point #4 is particularly important in any scenario where a direct string
/// match is used on an address, such as in the Zenoh URI <-> Topic mapping
/// (https://github.com/eclipse-uprotocol/up-spec/blob/main/up-l1/zenoh.adoc).
struct IpAddress {
	/// @brief Describes the type / family of an IP address
	enum class Type {
		/// @brief For IPv4 family addresses
		IpV4,
		/// @brief For IPv6 family addresses
		IpV6,
		/// @brief Used when constructed from a string that is not an IP
		Invalid
	};

	/// @brief Constructs an IP address from a string representation of
	///        an address.
	explicit IpAddress(std::string_view ip_string);

	/// @brief Constructs an IP address from a binary representation of
	///        an address.
	IpAddress(std::vector<uint8_t> const& ip_bytes, Type type);

	/// @brief Get the type of this IP address.
	[[nodiscard]] Type getType() const;

	/// @brief Gets the nomalized string representation of this IP address.
	[[nodiscard]] const std::string& getString() const;

	/// @brief Gets the binary representation of this IP address.
	[[nodiscard]] const std::vector<uint8_t>& getBytes() const;

	/// @brief Gets the binary representation of this IP address, wrapped in a
	///        string-like container to better interface with protobuf.
	///
	/// Protobuf uses std::string as a generic byte container, so this can be
	/// useful for embedding compact, binary representations of IP addresses
	/// into a protobuf message.
	[[nodiscard]] std::string getBytesString() const;

	/// @brief Number of bytes in IPv4 address.
	static constexpr uint8_t IP_V4_ADDRESS_BYTES = 4;

	/// @brief Number of bytes in IPv6 address.
	static constexpr uint8_t IP_V6_ADDRESS_BYTES = 16;

private:
	/// @brief Updates the state of this instance from the value of the
	///        ipString_ field.
	void fromString();

	/// @brief Updates the state of this instance from the value of the
	///        ipBytes_ field.
	void fromBytes();

	/// @brief Type of the IP addess contained in this instance.
	Type type_{Type::Invalid};

	/// @brief IP address in byte format.
	std::vector<uint8_t> ipBytes_{};

	/// @brief IP address in string format.
	std::string ipString_{};

	/// @name String container compatibility checks
	/// @{
	using Bytes = typename std::decay_t<decltype(ipBytes_)>::value_type;
	using StrBytes = typename std::string_view::value_type;
	using StrBytesPtr = typename std::string_view::const_pointer;

	static_assert(
	    sizeof(StrBytes) == sizeof(Bytes),
	    "Mismatch in size between string_view value type and ipBytes_ "
	    "value type invalidates reinterpret_cast used in constructor.");
	/// @}
};

}  // namespace uprotocol::utils

#endif  // UP_CPP_UTILS_IPADDRESS_H
