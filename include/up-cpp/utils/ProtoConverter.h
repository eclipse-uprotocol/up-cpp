#ifndef UP_CPP_UTILS_PROTOCONVERTER_H
#define UP_CPP_UTILS_PROTOCONVERTER_H

#include <google/protobuf/timestamp.pb.h>
#include <uprotocol/core/usubscription/v3/usubscription.pb.h>
#include <uprotocol/v1/umessage.pb.h>
#include <uprotocol/v1/ustatus.pb.h>

#include <chrono>
#include <optional>

#include "up-cpp/datamodel/builder/Payload.h"
#include "up-cpp/utils/Expected.h"

namespace uprotocol::utils {
template <typename T>
using TOrStatus = utils::Expected<T, v1::UStatus>;
using UnexpectedStatus = utils::Unexpected<v1::UStatus>;
using PayloadOrStatus =
    utils::Expected<datamodel::builder::Payload, v1::UStatus>;
using core::usubscription::v3::FetchSubscribersRequest;
using core::usubscription::v3::FetchSubscriptionsRequest;
using core::usubscription::v3::NotificationsRequest;
using uprotocol::core::usubscription::v3::SubscribeAttributes;
using uprotocol::core::usubscription::v3::SubscriberInfo;
using uprotocol::core::usubscription::v3::SubscriptionRequest;
using uprotocol::core::usubscription::v3::UnsubscribeRequest;

struct ProtoConverter {
	/// @brief Converts std::chrono::time_point to google::protobuf::Timestamp
	///
	/// @param tp the time point to convert
	/// @return the converted google::protobuf::Timestamp
	static google::protobuf::Timestamp ConvertToProtoTimestamp(
	    const std::chrono::system_clock::time_point& tp);

	/// @brief Builds a SubscriberInfo from the given parameters
	///
	/// @param entity_uri the UUri of the entity subscribing
	/// @return the built SubscriberInfo
	static SubscriberInfo BuildSubscriberInfo(const v1::UUri& entity_uri);

	/// @brief Builds a SubscribeAttributes from the given parameters
	///
	/// @param when_expire the optional time point when the subscription expires
	/// @param subscription_details the details of the subscription
	/// @param sample_period_ms the optional sample period in milliseconds
	/// @return the built SubscribeAttributes
	static SubscribeAttributes BuildSubscribeAttributes(
	    std::optional<std::chrono::system_clock::time_point> when_expire,
	    std::optional<google::protobuf::Any> subscription_details,
	    std::optional<std::chrono::milliseconds> sample_period_ms);

	/// @brief Builds a SubscriptionRequest from the given parameters
	///
	/// @param subscription_topic the UUri of the topic to subscribe to
	/// @param attributes the SubscribeAttributes for the subscription
	/// @return the built SubscriptionRequest
	static SubscriptionRequest BuildSubscriptionRequest(
	    const v1::UUri& subscription_topic,
	    std::optional<SubscribeAttributes> attributes = {});

	/// @brief Builds a UnsubscribeRequest from the given parameters
	///
	/// @param subscription_topic the UUri of the topic to unsubscribe from
	/// @return the built UnsubscribeRequest
	static UnsubscribeRequest BuildUnSubscribeRequest(
	    const v1::UUri& subscription_topic);

	/// @brief Builds a FetchSubscriptionsRequest from the given topic
	///
	/// @param topic the UUri of the topic to fetch subscriptions for
	/// @return the built FetchSubscriptionsRequest
	static FetchSubscriptionsRequest BuildFetchSubscriptionsRequest(
	    const v1::UUri& topic);

	/// @brief Builds a FetchSubscriptionsRequest from the given subscriber
	/// information
	///
	/// @param subscriber the SubscriberInfo containing details of the
	/// subscriber
	/// @return the built FetchSubscriptionsRequest
	static FetchSubscriptionsRequest BuildFetchSubscriptionsRequest(
	    const SubscriberInfo& subscriber);

	/// @brief Builds a FetchSubscribersRequest from the given topic
	///
	/// @param topic the UUri of the topic to fetch subscribers for
	/// @return the built FetchSubscribersRequest
	static FetchSubscribersRequest BuildFetchSubscribersRequest(
	    const v1::UUri& topic);

	/// @brief Builds a NotificationsRequest from the given topic
	///
	/// @param topic the UUri of the topic to build a notification request for
	/// @return the built NotificationsRequest
	static NotificationsRequest BuildNotificationsRequest(
	    const v1::UUri& topic);

	/// @brief Deserializes a protobuf message from a given payload.
	///
	/// @tparam T The type to deserialize the message into.
	/// @param message The `v1::UMessage` containing the payload.
	/// @return `TOrStatus<T>` with the deserialized object or an error status.
	template <typename T>
	static TOrStatus<T> extractFromProtobuf(const v1::UMessage& message) {
		switch (message.attributes().payload_format()) {
			case v1::UPayloadFormat::UPAYLOAD_FORMAT_PROTOBUF: {
				T response;
				if (!response.ParseFromString(message.payload())) {
					v1::UStatus status;
					status.set_code(v1::UCode::INTERNAL);
					status.set_message(
					    "extractFromProtobuf: Error when parsing payload from "
					    "protobuf.");
					return TOrStatus<T>(UnexpectedStatus(status));
				}
				return TOrStatus<T>(response);
			}
			case v1::UPayloadFormat::UPAYLOAD_FORMAT_UNSPECIFIED:
			case v1::UPayloadFormat::UPAYLOAD_FORMAT_PROTOBUF_WRAPPED_IN_ANY: {
				google::protobuf::Any any;
				if (!any.ParseFromString(message.payload())) {
					v1::UStatus status;
					status.set_code(v1::UCode::INTERNAL);
					status.set_message(
					    "extractFromProtobuf: Error when parsing payload from "
					    "protobuf any.");
					return TOrStatus<T>(UnexpectedStatus(status));
				}
				T response;
				if (!any.UnpackTo(&response)) {
					v1::UStatus status;
					status.set_code(v1::UCode::INTERNAL);
					status.set_message(
					    "extractFromProtobuf: Error when unpacking any.");
					return TOrStatus<T>(UnexpectedStatus(status));
				}
				return TOrStatus<T>(response);
			}
			case v1::UPayloadFormat::UPAYLOAD_FORMAT_JSON:
			case v1::UPayloadFormat::UPAYLOAD_FORMAT_SOMEIP:
			case v1::UPayloadFormat::UPAYLOAD_FORMAT_SOMEIP_TLV:
			case v1::UPayloadFormat::UPAYLOAD_FORMAT_RAW:
			case v1::UPayloadFormat::UPAYLOAD_FORMAT_TEXT:
			case v1::UPayloadFormat::UPAYLOAD_FORMAT_SHM:
			case v1::UPayloadFormat::
			    UPayloadFormat_INT_MIN_SENTINEL_DO_NOT_USE_:
			case v1::UPayloadFormat::
			    UPayloadFormat_INT_MAX_SENTINEL_DO_NOT_USE_: {
				v1::UStatus status;
				status.set_code(v1::UCode::UNIMPLEMENTED);
				status.set_message("Unimplemented payload format.");
				return TOrStatus<T>(UnexpectedStatus(status));
			}
			default: {
				v1::UStatus status;
				status.set_code(v1::UCode::INVALID_ARGUMENT);
				status.set_message(
				    "Unknown/invalid/unsupported payload format.");
				return TOrStatus<T>(UnexpectedStatus(status));
			}
		}
	}

	/// @brief Serializes a protobuf object into a payload.
	///
	/// @tparam T The type of the protobuf object to serialize.
	/// @param proto The protobuf object to be converted into a payload.
	/// @return `PayloadOrStatus` containing the payload or an error status.
	template <typename T>
	static PayloadOrStatus protoToPayload(const T& proto) {
		google::protobuf::Any any;

		if (!any.PackFrom(proto)) {
			v1::UStatus status;
			status.set_code(v1::UCode::INTERNAL);
			status.set_message(
			    "protoToPayload: There was an error when serializing the "
			    "subscription request.");
			return PayloadOrStatus(UnexpectedStatus(status));
		}

		const datamodel::builder::Payload payload(any);

		return PayloadOrStatus(payload);
	}
};
};      // namespace uprotocol::utils
#endif  // UP_CPP_UTILS_PROTOCONVERTER_H
