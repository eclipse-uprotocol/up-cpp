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

#ifndef UP_CPP_DATAMODEL_BUILDER_PAYLOAD_H
#define UP_CPP_DATAMODEL_BUILDER_PAYLOAD_H

#include <google/protobuf/any.pb.h>
#include <uprotocol/v1/uattributes.pb.h>

#include <cstdint>
#include <string>
#include <tuple>
#include <vector>

namespace uprotocol::datamodel::builder {

/// @brief Interface for preparing payloads for inclusion in a UMessage
///
/// Allows for implicit conversions at interfaces that require a payload.
struct Payload {
	/// @brief Protobuf uses std::string to represent the Bytes type from
	///        messages.
	using PbBytes = std::string;

	/// @brief A serialized payload as a pairing of bytes and format.
	using Serialized = std::tuple<PbBytes, v1::UPayloadFormat>;

	/// @brief The two types of data that can be stored in a Serialized payload.
	enum PayloadType { Data, Format };

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
	explicit Payload(const ProtobufT& message) {
		std::string serialized;
		message.SerializeToString(&serialized);
		payload_ =
		    std::make_tuple(std::move(serialized),
		                    v1::UPayloadFormat::UPAYLOAD_FORMAT_PROTOBUF);
	}

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
	Payload(Serializer s [[maybe_unused]], const ValueT& data) {
		auto serialized_data = Serializer::serialize(data);
		if (!UPayloadFormat_IsValid(
		        std::get<PayloadType::Format>(serialized_data))) {
			throw std::out_of_range("Invalid Serializer payload format");
		}
		payload_ = std::move(serialized_data);
	}

	/// @brief Creates a Payload builder with a provided pre-serialized data.
	///
	/// @param value_bytes A byte array containing the serialized payload.
	/// @param format The data format of the payload in value_bytes.
	///
	/// @throws std::out_of_range If format is not valid for v1::UPayloadFormat
	Payload(const std::vector<uint8_t>& value_bytes,
	        v1::UPayloadFormat format);

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
	explicit Payload(Serialized&&);

	/// @brief Creates a Payload builder with a provided protobuf::Any.
	///
	/// The contents of value will be moved into the Payload object.
	///
	/// @param An initialized google::protobuf::Any object..
	explicit Payload(const google::protobuf::Any&);

	/// @brief Move constructor.
	Payload(Payload&&) noexcept;

	/// @brief Copy constructor.
	Payload(const Payload&);

	Payload& operator=(Payload&&) noexcept;
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
	/// @throws PayloadMoved if called after buildMove() has already been
	/// called.
	[[nodiscard]] const Serialized& buildCopy() const;

	/// @brief Get an xvalue of the internal data that can be moved into a
	///        UMessage.
	///
	/// @post This Payload builder will no longer be valid. Calling
	/// `buildCopy()`
	///       or `movePayload()` after this will result in an exception.
	///
	/// @throws PayloadMoved if called after buildMove() has already been
	/// called.
	[[nodiscard]] Serialized buildMove() &&;

private:
	Serialized payload_;
	bool moved_{false};
};

}  // namespace uprotocol::datamodel::builder

#endif  // UP_CPP_DATAMODEL_BUILDER_PAYLOAD_H
