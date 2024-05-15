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

#ifndef UP_CPP_DATAMODEL_BUILDER_UPAYLOAD_H
#define UP_CPP_DATAMODEL_BUILDER_UPAYLOAD_H

#include <uprotocol/v1/upayload.pb.h>

#include <cstdint>
#include <string>
#include <vector>

namespace uprotocol::datamodel::builder {

/// @brief Interface for composing UPayload objects
///
/// Allows for implicit conversions at interfaces that require a payload.
struct UPayload {
	/// @brief Constructs a UPayload builder with the payload populated by
	///        a serialized protobuf.
	///
	/// @tparam ProtobufT Automatically inferred protobuf message type.
	///                   Must be derived from google::protobuf::message.
	/// @param A protobuf message that will be serialized using its
	///        SerializeToString() method before embedding into the
	///        v1::UPayload.
	///
	/// @remarks The UPayloadFormat will automatically be set to
	///          UPAYLOAD_FORMAT_PROTOBUF
	template <typename ProtobufT>
	explicit UPayload(const ProtobufT&);

	/// @brief Creates a UPayload builder with the payload populated by the
	///        result of Serializer::serialize(data).
	///
	/// This interface would be invoked something like this:
	///
	///     builder::UPayload(ToPayload(), foo);
	///
	/// @note Serializer types should only contain static methods. As such,
	///       there would be nothing to construct and the temporary s argument
	///       would compile out.
	///
	/// @tparam Serializer An object capable of serializing ValueT. Must
	///                    provide a `static UPayload serialize(ValueT)`
	///                    method. This will be inferred from the parameters.
	/// @tparam ValueT Automatically inferred unserialized payload value
	///                type, serializable using the Serializer.
	///
	/// @param s An instance of Serializer. Note that only the static
	///          `Serializer::serialize(data)` will be called - the s instance
	///          will compile out.
	/// @param data Data to be packed into the UPayload.
	template <typename Serializer, typename ValueT>
	UPayload(Serializer s, const ValueT& data);

	/// @brief Creates a UPayload builder with a provided pre-serialized data.
	///
	/// @param value_bytes A byte array containing the serialized payload.
	/// @param format The data format of the payload in value_bytes.
	UPayload(const std::vector<uint8_t>& value_bytes,
	         v1::UPayloadFormat format);

	/// @brief Creates a UPayload builder with a provided pre-serialized data.
	///
	/// @param value A string containing the serialized payload.
	/// @param format The data format of the payload in value_bytes.
	///
	/// @note This would typically be used for UPAYLOAD_FORMAT_TEXT or
	///       UPAYLOAD_FORMAT_JSON, but can be used for other payload formats.
	UPayload(const std::string& value, v1::UPayloadFormat format);

	/// @brief Creates a UPayload builder with a provided pre-serialized data.
	///
	/// The contents of value will be moved into the v1::UPayload object.
	///
	/// @param value A string containing the serialized payload.
	/// @param format The data format of the payload in value_bytes.
	///
	/// @note This would typically be used for UPAYLOAD_FORMAT_TEXT or
	///       UPAYLOAD_FORMAT_JSON, but can be used for other payload formats.
	UPayload(std::string&& value, v1::UPayloadFormat format);

	/// @brief Creates a UPayload builder with a pre-made v1::UPayload.
	///
	/// The provided v1::UPayload will be moved.
	///
	/// @param A pre-packaged v1::UPayload.
	explicit UPayload(v1::UPayload&&);

	/// @brief Creates a UPayload builder with a pre-made v1::UPayload.
	///
	/// @param A pre-packaged v1::UPayload.
	explicit UPayload(const v1::UPayload&);

	/// @brief Get the internal v1::UPayload from this builder.
	constexpr const v1::UPayload& build() const;

	/// @brief Get an xvalue of the internal v1::UPayload that can be moved
	///        to another v1::UPayload.
	///
	/// @post This UPayload builder will no longer be valid. Calling `build()`
	///       or `movePayload()` after this is undefined behavior.
	constexpr v1::UPayload&& movePayload() &&;

private:
	v1::UPayload payload_;
};

}  // namespace uprotocol::datamodel::builder

#endif  // UP_CPP_DATAMODEL_BUILDER_UPAYLOAD_H
