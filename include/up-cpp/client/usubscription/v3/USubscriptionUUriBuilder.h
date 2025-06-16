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

#ifndef UP_CPP_CLIENT_USUBSCRIPTION_V3_USUBSCRIPTIONUURIBUILDER_H
#define UP_CPP_CLIENT_USUBSCRIPTION_V3_USUBSCRIPTIONUURIBUILDER_H

#include <uprotocol/core/usubscription/v3/usubscription.pb.h>
#include <uprotocol/uoptions.pb.h>

namespace uprotocol::core::usubscription::v3 {
/// @struct uSubscriptionUUriBuilder
/// @brief Structure to build uSubscription request URIs.
///
/// This structure is used to build URIs for uSubscription service. It uses the
/// service options from uSubscription proto to set the authority name, ue_id,
/// ue_version_major, and the notification topic resource ID in the URI.
struct USubscriptionUUriBuilder {
private:
	/// URI for the uSubscription service
	v1::UUri base_uri_;
	/// Resource ID of the notification topic
	uint32_t sink_resource_id_;

public:
	/// @brief Constructor for uSubscriptionUUriBuilder.
	USubscriptionUUriBuilder();

	USubscriptionUUriBuilder& setAuthorityName(
	    const std::string& authority_name);

	USubscriptionUUriBuilder& setUEntityId(u_int32_t ue_id);

	USubscriptionUUriBuilder& setInstanceId(u_int16_t instance_id);

	USubscriptionUUriBuilder& setServiceId(u_int16_t service_id);

	USubscriptionUUriBuilder& setResourceId(u_int32_t resource_id);
	/// @brief Get the URI with a specific resource ID.
	///
	/// @param resource_id The resource ID to set in the URI.
	///
	/// @return The URI with the specified resource ID.
	v1::UUri getServiceUriWithResourceId(uint32_t resource_id) const;

	/// @brief Get the notification URI.
	///
	/// @return The notification URI.
	v1::UUri getNotificationUri();
};

}  // namespace uprotocol::core::usubscription::v3

#endif  // UP_CPP_CLIENT_USUBSCRIPTION_V3_USUBSCRIPTIONUURIBUILDER_H
