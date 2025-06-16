// SPDX-FileCopyrightText: 2025 Contributors to the Eclipse Foundation
//
// See the NOTICE file(s) distributed with this work for additional
// information regarding copyright ownership.
//
// This program and the accompanying materials are made available under the
// terms of the Apache License Version 2.0 which is available at
// https://www.apache.org/licenses/LICENSE-2.0
//
// SPDX-License-Identifier: Apache-2.0

#include "up-cpp/client/usubscription/v3/USubscriptionUUriBuilder.h"

namespace uprotocol::core::usubscription::v3 {

constexpr uint32_t SERVICE_ID_BITMASK = 0x0000FFFF;
constexpr uint32_t INSTANCE_ID_BITMASK = 0xFFFF0000;

USubscriptionUUriBuilder::USubscriptionUUriBuilder() {
	// Get the service descriptor
	const google::protobuf::ServiceDescriptor* service =
	    uSubscription::descriptor();
	const auto& service_options = service->options();

	// Get the service options
	const auto& service_version_major =
	    service_options.GetExtension(uprotocol::service_version_major);
	const auto& service_id =
	    service_options.GetExtension(uprotocol::service_id);
	const auto& notification_topic =
	    service_options.GetExtension(uprotocol::notification_topic, 0);

	// Set the values in the URI
	base_uri_.set_ue_id(service_id);
	base_uri_.set_ue_version_major(service_version_major);
	sink_resource_id_ = notification_topic.id();
}

USubscriptionUUriBuilder& USubscriptionUUriBuilder::setAuthorityName(
    const std::string& authority_name) {
	base_uri_.set_authority_name(authority_name);
	return *this;
}

USubscriptionUUriBuilder& USubscriptionUUriBuilder::setUEntityId(
    u_int32_t ue_id) {
	base_uri_.set_ue_id(ue_id);
	return *this;
}

constexpr uint16_t BITS_UINT_16 = 16;
USubscriptionUUriBuilder& USubscriptionUUriBuilder::setInstanceId(
    u_int16_t instance_id) {
	auto updated_ue_id = (SERVICE_ID_BITMASK & base_uri_.ue_id()) +
	                     (static_cast<uint32_t>(instance_id) << BITS_UINT_16);
	base_uri_.set_ue_id(updated_ue_id);
	return *this;
}

USubscriptionUUriBuilder& USubscriptionUUriBuilder::setServiceId(
    u_int16_t service_id) {
	auto updated_ue_id = (INSTANCE_ID_BITMASK & base_uri_.ue_id()) + service_id;
	base_uri_.set_ue_id(updated_ue_id);
	return *this;
}

USubscriptionUUriBuilder& USubscriptionUUriBuilder::setResourceId(
    u_int32_t resource_id) {
	base_uri_.set_resource_id(resource_id);
	return *this;
}

v1::UUri USubscriptionUUriBuilder::getServiceUriWithResourceId(
    uint32_t resource_id) const {
	v1::UUri uri = base_uri_;  // Copy the base URI
	uri.set_resource_id(resource_id);
	return uri;
}

v1::UUri USubscriptionUUriBuilder::getNotificationUri() {
	v1::UUri uri = base_uri_;  // Copy the base URI
	uri.set_resource_id(sink_resource_id_);
	return uri;
}

}  // namespace uprotocol::core::usubscription::v3
