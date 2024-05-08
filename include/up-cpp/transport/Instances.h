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

#ifndef UP_CPP_TRANSPORT_INSTANCES_H
#define UP_CPP_TRANSPORT_INSTANCES_H

#include <up-core-api/uri.pb.h>
#include <up-cpp/transport/UTransport.h>

#include <filesystem>
#include <memory>
#include <string>
#include <tuple>

namespace uprotocol::transport {

/// @brief Interface for instantiating a UTransport implementation. Allows for
///        implementation-specific behavior (e.g. shared instances).
///
/// Explicit template instantiations for a given transport are provided in the
/// transport's compiled library. All transport libraries must meet these
/// requirements:
///
/// * [MUST] A transport implementation library (e.g. up-client-zenoh-cpp) MUST
///   provide an implementation of this function for its derived implementation
///   of UTransport.
/// * [MUST] Calls to getTransport with *different* parameter values MUST
///   result in different instances being returned.
/// * [MAY] Implementations MAY return the same instance when multiple calls
///   are made to getInstance with identical parameter values.
/// * [MUST] Returned instances MUST support sharing between multiple clients
///   (i.e. if the shared_ptr is copied to multiple client objects).
/// * [MUST] When all shared_ptrs to a transport instance are released, the
///   transport instance MUST be destroyed.
///
/// @tparam TransportImpl The transport implementation (derived from the base
///                       UTransport class) that will be instantiated and
///                       returned by this function.
///                       Note that `void` is a special case, described below.
///
/// @param default_source A UUri containing only an Authority and Entity that
///                       is used as the default source for some operations.
/// @param config A path to a transport implementation-specific configuration
///               file. For transports that do not need additional parameters
///               to operate, this path may be empty.
///
/// @throws std::filesystem::filesystem_error on config file access errors.
/// @returns A shared pointer to a TransportImpl, viewed through the abstract
///          base UTransport interface.
template <class TransportImpl = void>
[[nodiscard]] std::shared_ptr<UTransport> getTransport(
    const v1::UUri& default_source, const std::filesystem::path& config);

/// @brief Selects a transport implementation and corresponding config file
///        based on the contents of a provided configuration file.
///
/// The configuration file should be of the format:
///
///     TransportImplName
///     /path/to/transport/config/file
///
/// The readTransportConfig() function is provided for reading files in this
/// format and can be used when implementing getTransport<void>;
///
/// @note An implementation for this definition is not provided by any uP
///       libraries. uE implementations wishing to use this functionality MUST
///       link to a library providing a platform-appropriate implementation.
///       An empty example implementation is provided as part of this repo.
///
/// @see readTransportSelectFile Supporting utility function for loading a
///      transport-selecting configuration file and returning its parameters.
///
/// @param default_source A UUri containing only an Authority and Entity that
///                       is used as the default source for some operations.
/// @param config A path to a file containing the transport selection
///               parameters. The name from that file will be used to determine
///               which getTransport<TransportImpl> will be called, and the
///               path from that file will be passed as the transport's config.
///
/// @throws UnknownTransportImpl if the transport name loaded from the config
///         file is not supported by the implementation of this function.
/// @throws std::filesystem::filesystem_error on config file access errors.
/// @returns A shared pointer to a TransportImpl, viewed through the abstract
///          base UTransport interface.
template <>
[[nodiscard]] std::shared_ptr<UTransport> getTransport<void>(
    const v1::UUri& default_source, const std::filesystem::path& config);

/// @brief Reads a transport-selecting config file for getTransport<void>()
///
/// @param File to read configuration parameters from
///
/// @throws std::filesystem::filesystem_error on config file access errors.
/// @returns A string containing the name of the transport and the path to the
///          transport-specific configuration file, both as read from the
///          configuration file provided.
std::tuple<std::string, std::filesystem::path> readTransportConfig(
    const std::filesystem::path&);

/// @brief Exception thrown when using getTransport<void>() representing an
///        unknown transport name was read from the config file.
struct UnknownTransportImpl : public std::runtime_error {
	// Inherit constructors
	using runtime_error::runtime_error;

	// Move interfaces
	UnknownTransportImpl(UnknownTransportImpl&&) noexcept;
	UnknownTransportImpl& operator=(UnknownTransportImpl&&) noexcept;

	// Copy interfaces
	UnknownTransportImpl(const UnknownTransportImpl&);
	UnknownTransportImpl& operator=(const UnknownTransportImpl&);
};
}  // namespace uprotocol::transport

#endif  // UP_CPP_TRANSPORT_INSTANCES_H
