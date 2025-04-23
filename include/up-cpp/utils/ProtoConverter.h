#ifndef UP_CPP_UTILS_PROTOCONVERTER_H
#define UP_CPP_UTILS_PROTOCONVERTER_H

#include <google/protobuf/timestamp.pb.h>
#include <uprotocol/core/usubscription/v3/usubscription.pb.h>

#include <chrono>
#include <optional>

namespace uprotocol::utils {
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
};
};      // namespace uprotocol::utils
#endif  // UP_CPP_UTILS_PROTOCONVERTER_H
