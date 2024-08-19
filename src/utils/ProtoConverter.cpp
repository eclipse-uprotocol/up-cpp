#include "up-cpp/utils/ProtoConverter.h"

namespace uprotocol::utils {
google::protobuf::Timestamp ProtoConverter::ConvertToProtoTimestamp(
    const std::chrono::system_clock::time_point& tp) {
	google::protobuf::Timestamp timestamp;
	auto duration = tp.time_since_epoch();
	auto seconds =
	    std::chrono::duration_cast<std::chrono::seconds>(duration).count();
	auto nanoseconds =
	    std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count() %
	    1000000000ll;

	timestamp.set_seconds(seconds);
	timestamp.set_nanos(static_cast<int32_t>(nanoseconds));

	return timestamp;
}

// SubscriberInfo builder
SubscriberInfo ProtoConverter::BuildSubscriberInfo(
    const v1::UUri& entity_uri,
    std::optional<google::protobuf::Any> subscriber_details) {
	SubscriberInfo subscriber_info;

	// Create a new instance of UUri and copy the contents from entity_uri
	*subscriber_info.mutable_uri() = entity_uri;

	if (subscriber_details.has_value()) {
		subscriber_info.add_details()->CopyFrom(subscriber_details.value());
	}

	return subscriber_info;
}

// SubscribeAttributes builder
SubscribeAttributes ProtoConverter::BuildSubscribeAttributes(
    std::optional<std::chrono::system_clock::time_point> when_expire,
    std::optional<google::protobuf::Any> subscription_details,
    std::optional<std::chrono::milliseconds> sample_period_ms) {
	SubscribeAttributes attributes;

	if (when_expire.has_value()) {
		*attributes.mutable_expire() =
		    ConvertToProtoTimestamp(when_expire.value());
	}

	if (subscription_details.has_value()) {
		attributes.add_details()->CopyFrom(subscription_details.value());
	}

	if (sample_period_ms.has_value()) {
		attributes.set_sample_period_ms(
		    static_cast<uint32_t>(sample_period_ms.value().count()));
	}

	return attributes;
}

// SubscriptionRequest builder
SubscriptionRequest ProtoConverter::BuildSubscriptionRequest(
    const v1::UUri& subscription_topic, SubscriberInfo& subscriber_info,
    std::optional<SubscribeAttributes> attributes) {
	SubscriptionRequest subscription_request;
	*subscription_request.mutable_topic() = subscription_topic;
	*subscription_request.mutable_subscriber() = subscriber_info;

	// Use mutable attributes if provided
	if (attributes.has_value()) {
		*subscription_request.mutable_attributes() =
		    std::move(attributes.value());
	}

	return subscription_request;
}

UnsubscribeRequest ProtoConverter::BuildUnSubscribeRequest(
    const v1::UUri& subscription_topic, SubscriberInfo& subscriber_info) {
	UnsubscribeRequest unsubscribe_request;
	*unsubscribe_request.mutable_topic() = subscription_topic;
	*unsubscribe_request.mutable_subscriber() = subscriber_info;

	return unsubscribe_request;
}

}  // namespace uprotocol::utils
