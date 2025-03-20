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

#ifndef UP_CPP_TRANSPORT_UTRANSPORT_H
#define UP_CPP_TRANSPORT_UTRANSPORT_H

#include <up-cpp/utils/CallbackConnection.h>
#include <up-cpp/utils/Expected.h>
#include <uprotocol/v1/umessage.pb.h>
#include <uprotocol/v1/uri.pb.h>
#include <uprotocol/v1/ustatus.pb.h>

#include <optional>

namespace uprotocol::transport {

/// @brief Abstract base class for all transport implementations.
///
/// All implementations derived from this class must meet the following
/// requirements:
///
/// * [MUST] An implementation MUST support multiple simultaneous instantiatons
///          from within the same application.
///
/// * [MAY]  An implimentation MAY require that each new instance within an
///          application have a unique configuration from existing instances.
///
/// * [MUST] An implementation MUST allow for multiple users of the same
///          instance.
///
/// * [MUST] An implementation MUST be thread-safe.
///
/// * [MUST] Throw an exception if the transport fails to initialize or the
///          configuration is invalid.
class UTransport {
	/// @brief Connection interface used for self-terminating listener
	/// registrations
	using CallbackConnection =
	    utils::callbacks::Connection<void, const v1::UMessage&>;

public:
	/// @brief Constructor
	///
	/// @param Default Authority and Entity ID representing the uEntity that
	///        owns this transport instance.
	///
	/// @throws InvalidUUri if the provided UUri is not valid as an entity URI
	///         Validation is done with isValidDefaultEntityUri().
	///
	/// @see uprotocol::datamodel::validator::uri::isValidEntityUri()
	/// @see uprotocol::datamodel::validator::uri::InvalidUUri
	explicit UTransport(v1::UUri);

	/// @brief Send a message.
	///
	/// @param message UMessage to be sent.
	///
	/// @throws InvalidUMessage if the message doesn't pass the isValid() check.
	///
	/// @see uprotocol::datamodel::validator::message::isValid()
	/// @see uprotocol::datamodel::validator::message::InvalidUMessage
	///
	/// @returns * OKSTATUS if the payload has been successfully
	///            sent (ACK'ed)
	///          * FAILSTATUS with the appropriate failure.
	[[nodiscard]] v1::UStatus send(const v1::UMessage& message);

	/// @brief Callback function (void(const UMessage&))
	using ListenCallback = typename CallbackConnection::Callback;

	/// @brief Handle representing the callback connection.
	///
	/// These handles will automatically disconnect the callback when
	/// destroyed or when .reset() is called. They can be cast to bool
	/// to check if they are connected.
	using ListenHandle = typename CallbackConnection::Handle;

	/// @brief Return value where a ListenHandle is provided on success and a
	///        v1::UStatus is provided on error.
	using HandleOrStatus = utils::Expected<ListenHandle, v1::UStatus>;

	/// @brief Register listener to be called when UMessage is received
	///        for the given resource ID.
	///
	/// Automatically populates the sink_filter URI by calling getEntityUri()
	/// then replacing that URI's resource ID with sink_resource_filter.
	///
	/// @see registerListener(ListenCallback&&, const v1::UUri& source_filter,
	///                       std::optional<v1::UUri>&& sink_filter)
	///      for more details on the filter parameters.
	///
	/// @param listener Callback to be called when a message is received
	///                 matching the provided filters. The received UMessage
	///                 will be passed to the callback.
	/// @param source_filter URI to use as a source filter for incoming
	///                      messages.
	/// @param sink_resource_filter Resource ID that will be combined with
	///                             getEntityUri() to produce a sink filter.
	///
	/// @throws datamodel::validator::InvalidUUri when any URI does not pass
	///         validation.
	///
	/// @returns * A connected ListenHandle if the listener was registered
	///            successfully.
	///          * A v1::UStatus containing a FAILSTATUS code otherwise.
	[[nodiscard]] HandleOrStatus registerListener(
	    ListenCallback&& listener, const v1::UUri& source_filter,
	    uint16_t sink_resource_filter);

	/// @brief Register listener to be called when UMessage is received
	///        matching the URI filters provided.
	///
	/// @param listener Callback to be called when a message is received
	///                 matching the provided filters. The received UMessage
	///                 will be passed to the callback.
	/// @param source_filter URI that will be compared to the source URI
	///                      attribute on incoming messages. This could contain
	///                      a URI for a specific entity/resource, or contain
	///                      wildcard values.
	/// @param sink_filter (optional)
	///                    * When set, this URI will be compared to the sink
	///                      URI attribute on incoming messages. This would
	///                      typically be based on the URI retrieved from
	///                      getEntityUri(), but could have other values or
	///                      wildcards present.
	///                    * When unset, it will be assumed that the
	///                      source_filter will be used like a topic for
	///                      pub/sub-like messaging. This is effectively a
	///                      wildcard, but the precise handling of this state
	///                      is transport implementation specific.
	///
	/// @see uprotocol::datamodel::validator::uri::isValid()
	/// @see uprotocol::datamodel::validator::uri::isValidSubscription()
	/// @see uprotocol::datamodel::validator::uri::InvalidUUri
	///
	/// @throws datamodel::validator::InvalidUUri when any URI does not pass
	///         validation with isValid().
	/// @throws datamodel::validator::InvalidUUri the sink_filter is unset and
	///         source_filter does not pass isValidSubscription().
	///
	/// @returns * A connected ListenHandle if the listener was registered
	///            successfully.
	///          * A v1::UStatus containing a FAILSTATUS code otherwise.
	[[nodiscard]] HandleOrStatus registerListener(
	    ListenCallback&& listener, const v1::UUri& source_filter,
	    std::optional<v1::UUri>&& sink_filter = {});

	/// @note This interface has been replaced with alternatives that better
	///       align with the 1.6.0 uProtocol specifications. It is no longer
	///       recommended to use this method as it will be removed in a future
	///       release. It has been modified for uP 1.6.0 compliance.
	///
	/// @brief Register listener to be called when UMessage is received
	///        matching the provided filters.
	///
	/// @remarks This is a wrapper around the new, non-deprecated
	///          registerListener() method. Pay special attention to the
	///          parameters and their meanings.
	///
	/// @param sink_or_topic_filter
	///            Has two different meanings:
	///                * When source_filter is provided, this filter will be
	///                  matched against the sink field of received messages.
	///                * When source_filter is unset, this filter will be the
	///                  topic for pub/sub-like messaging, passed to the new
	///                  registerListener() as the source_filter.
	/// @param listener Callback to be called when a message is received
	///                 matching the provided filters. The received UMessage
	///                 will be passed to the callback.
	/// @param source_filter (Optional)
	///                      * When set, this filter will be matched against
	///                        the source field in received messages.
	///                      * When unset, the sink_or_topic_filter will be
	///                        treated as the topic for a pub/sub-like mode.
	///
	/// @throws datamodel::validator::InvalidUUri when any URI does not pass
	///         validation.
	///
	/// @returns * A connected ListenHandle if the listener was registered
	///            successfully.
	///          * A v1::UStatus containing a FAILSTATUS code otherwise.
	[[deprecated(
	    "See alternate overload of "
	    "registerListener()")]] [[nodiscard]] HandleOrStatus
	registerListener(const v1::UUri& sink_or_topic_filter,
	                 ListenCallback&& listener,
	                 std::optional<v1::UUri>&& source_filter = {});

	/// @brief Gets the default URI representing the entity that owns this
	///        transport instance.
	///
	/// @remarks This URI is consumed both by higher-layer APIs and in
	///          UTransport methods locally (where documented).
	///
	/// @returns Const reference to the entity URI.
	[[nodiscard]] const v1::UUri& getEntityUri() const;

	/// @see getEntityUri()
	[[deprecated("Replaced with getEntityUri()")]] [[nodiscard]] const v1::UUri&
	getDefaultSource() const;

	virtual ~UTransport() = default;

protected:
	/// @brief Send a message using the transport implementation.
	///
	/// @note Must be implemented by the transport library.
	///
	/// @param UMessage to be sent.
	///
	/// @returns * OKSTATUS if the payload has been successfully sent (ACK'ed)
	///          * FAILSTATUS with the appropriate failure.
	[[nodiscard]] virtual v1::UStatus sendImpl(const v1::UMessage&) = 0;

	/// @brief Represents the callable end of a callback connection.
	///
	/// This is a shared_ptr wrapping a callbacks::Connection. The
	/// Connection will automatically disconnect from the matching
	/// handle when destroyed. The Connection can be called directly
	/// to invoke the callback.
	using CallableConn = typename CallbackConnection::Callable;

	/// @brief Register a connected listener with the transport implementation.
	///
	/// @note The transport library is required to implement this.
	///
	/// @remarks If this doesn't return OKSTATUS, the outer registerListener()
	///          will reset the connection and return the UStatus as an error
	///          to the caller.
	///
	/// @param listener Connected CallerHandle representing the callback to be
	///                 called when a message is received.
	/// @param source_filter Filter to compare with the source attribute on
	///                      incoming messages. Could be a literal match or
	///                      match on wildcards.
	/// @param sink_filter (Optional) Filter to compare with the sink attribute
	///                    on incoming messages. If unset, the transport may
	///                    need to implement special behavior per UProtocol
	///                    spec. An unset sink_filter generally implies a
	///                    pub/sub-like messaging mode.
	///
	/// @returns * OKSTATUS if the listener was registered successfully.
	///          * FAILSTATUS with the appropriate failure otherwise.
	[[nodiscard]] virtual v1::UStatus registerListenerImpl(
	    CallableConn&& listener, const v1::UUri& source_filter,
	    std::optional<v1::UUri>&& sink_filter) = 0;

	/// @brief Clean up on listener disconnect.
	///
	/// The transport library can optionally implement this if it needs to
	/// clean up when a callbacks::Connection is dropped.
	///
	/// @note The default implementation does nothing.
	///
	/// @remarks CallerHandle instances are sortable and matchable on the
	///          connection they represent.
	///
	/// @param listener CallerHandle for the connection that has been broken.
	virtual void cleanupListener(const CallableConn& listener);

private:
	/// @brief URI for the entity owning this transport.
	///
	/// @remarks Many Layer 2 implementations will use this to form the source
	///          address for their calls to UTransport.
	const v1::UUri entity_uri_;
};

struct NullTransport : std::invalid_argument {
	template <typename... Args>
	explicit NullTransport(Args&&... args)
	    : std::invalid_argument(std::forward<Args>(args)...) {}

	template <typename... Args>
	NullTransport& operator=(Args&&... args) {
		std::invalid_argument::operator=(std::forward<Args>(args)...);
		return *this;
	}
};

}  // namespace uprotocol::transport

#endif  // UP_CPP_TRANSPORT_UTRANSPORT_H
