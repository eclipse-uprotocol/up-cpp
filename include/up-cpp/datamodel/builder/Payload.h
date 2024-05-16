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

#ifndef UP_CPP_DATAMODEL_BUILDER_PAYLOAD_H
#define UP_CPP_DATAMODEL_BUILDER_PAYLOAD_H

#include <uprotocol/v1/uattributes.pb.h>

#include <cstdint>
#include <string>
#include <tuple>
#include <vector>

namespace uprotocol::datamodel::builder {

/// @brief Interface for preparing payloads for inclusing in a UMessage
///
/// Allows for implicit conversions at interfaces that require a payload.
struct Payload {
	/// @brief Protobuf uses std::string to represent the Bytes type from
	///        messages.
	using PbBytes = std::string;

	/// @brief A serialized payload as a pairing of bytes and format.
	using Serialized = std::tuple<PbBytes, v1::UPayloadFormat>;

	/// @brief Constructs a Payload builder with the payload populated by
	///        a serialized protobuf.
	///
	/// @tparam ProtobufT Automatically inferred protobuf message type.
	///                   Must be derived from google::protobuf::message.
	/// @param A protobuf message that will be serialized using its
	///        SerializeToString() method.
	///
	/// @remarks The UPayloadFormat will automatically be set to
	///          UPAYLOAD_FORMAT_PROTOBUF
	template <typename ProtobufT>
	explicit Payload(const ProtobufT&);

	/// @brief Creates a Payload builder with the payload populated by the
	///        result of Serializer::serialize(data).
	///
	/// This interface would be invoked something like this:
	///
	///     builder::Payload(ToPayload(), foo);
	///
	/// @note Serializer types should only contain static methods. As such,
	///       there would be nothing to construct and the temporary s argument
	///       would compile out.
	///
	/// @tparam Serializer An object capable of serializing ValueT. Must
	///                    provide a `static Serialized serialize(ValueT)`
	///                    method. This will be inferred from the parameters.
	/// @tparam ValueT Automatically inferred unserialized payload value
	///                type, serializable using the Serializer.
	///
	/// @throws std::out_of_range If the serialized payload format is not valid
	///                           for v1::UPayloadFormat
	///
	/// @param s An instance of Serializer. Note that only the static
	///          `Serializer::serialize(data)` will be called - the s instance
	///          will compile out.
	/// @param data Data to be serialized and stored.
	template <typename Serializer, typename ValueT>
	Payload(Serializer s, const ValueT& data);

	/// @brief Creates a Payload builder with a provided pre-serialized data.
	///
	/// @param value_bytes A byte array containing the serialized payload.
	/// @param format The data format of the payload in value_bytes.
	///
	/// @throws std::out_of_range If format is not valid for v1::UPayloadFormat
	Payload(const std::vector<uint8_t>& value_bytes, v1::UPayloadFormat format);

	/// @brief Creates a Payload builder with a provided pre-serialized data.
	///
	/// @param value A string containing the serialized payload.
	/// @param format The data format of the payload in value_bytes.
	///
	/// @throws std::out_of_range If format is not valid for v1::UPayloadFormat
	///
	/// @note This would typically be used for UPAYLOAD_FORMAT_TEXT or
	///       UPAYLOAD_FORMAT_JSON, but can be used for other payload formats.
	Payload(const std::string& value, v1::UPayloadFormat format);

	/// @brief Creates a Payload builder with a provided pre-serialized data.
	///
	/// The contents of value will be moved into the Payload object.
	///
	/// @param value A string containing the serialized payload.
	/// @param format The data format of the payload in value_bytes.
	///
	/// @throws std::out_of_range If format is not valid for v1::UPayloadFormat
	///
	/// @note This would typically be used for UPAYLOAD_FORMAT_TEXT or
	///       UPAYLOAD_FORMAT_JSON, but can be used for other payload formats.
	Payload(std::string&& value, v1::UPayloadFormat format);

	/// @brief Creates a Payload builder with a provided pre-serialized data.
	///
	/// The contents of value will be moved into the Payload object.
	///
	/// @param A pairing of pre-serialized data and a format.
	///
	/// @throws std::out_of_range If the serialized payload format is not valid
	///                           for v1::UPayloadFormat
	Payload(Serialized&&);

	/// @brief Move constructor.
	Payload(Payload&&);

	/// @brief Copy constructor.
	Payload(const Payload&);

	Payload& operator=(Payload&&);
	Payload& operator=(const Payload&);

	/// @brief This exception indicates build() or move() has been called after
	///        move() has already been called.
	struct PayloadMoved : public std::runtime_error {
		// Inherit constructors
		using std::runtime_error::runtime_error;

		PayloadMoved(PayloadMoved&&) noexcept;
		PayloadMoved& operator=(PayloadMoved&&) noexcept;

		PayloadMoved(const PayloadMoved&);
		PayloadMoved& operator=(const PayloadMoved&);
	};

	/// @brief Get a reference to the internal data from this builder.
	///
	/// @throws PayloadMoved if called after move() has already been called.
	constexpr const Serialized& build() const;

	/// @brief Get an xvalue of the internal data that can be moved into a
	///        UMessage.
	///
	/// @post This Payload builder will no longer be valid. Calling `build()`
	///       or `movePayload()` after this will result in an exception.
	///
	/// @throws PayloadMoved if called after move() has already been called.
	constexpr Serialized move() &&;

private:
	Serialized payload_;
};

}  // namespace uprotocol::datamodel::builder

#endif  // UP_CPP_DATAMODEL_BUILDER_PAYLOAD_H
