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

#include <google/protobuf/util/message_differencer.h>
#include <gtest/gtest.h>
#include <up-cpp/datamodel/builder/UMessage.h>
#include <up-cpp/datamodel/validator/UMessage.h>
#include <up-cpp/datamodel/validator/UUri.h>
#include <up-cpp/transport/UTransport.h>

#include "UTransportMock.h"

constexpr uint32_t WILDCARD = 0xFFFF;
constexpr uint32_t RESOURCE_ID_F00D = 0xF00D;

namespace uprotocol {

bool operator==(const v1::UUri& lhs, const v1::UUri& rhs) {
	return google::protobuf::util::MessageDifferencer::Equals(lhs, rhs);
}

bool operator==(const v1::UMessage& lhs, const v1::UMessage& rhs) {
	return google::protobuf::util::MessageDifferencer::Equals(lhs, rhs);
}

class TestUTransport : public testing::Test {
protected:
	// Run once per TEST_F.
	// Used to set up clean environments per test.
	void SetUp() override {}

	void TearDown() override {}

	// Run once per execution of the test application.
	// Used for setup of all tests. Has access to this instance.
	TestUTransport() = default;

	// Run once per execution of the test application.
	// Used only for global setup outside of tests.
	static void SetUpTestSuite() {}
	static void TearDownTestSuite() {}

	static std::shared_ptr<test::UTransportMock> makeMockTransport(
	    const v1::UUri& uri) {
		return std::make_shared<test::UTransportMock>(uri);
	}

	static std::shared_ptr<transport::UTransport> makeTransport(
	    const v1::UUri& uri) {
		return makeMockTransport(uri);
	}

	static std::shared_ptr<transport::UTransport> makeTransport(
	    std::shared_ptr<test::UTransportMock> mock) {
		return mock;
	}

	static v1::UUri getValidUri() {
		constexpr uint32_t RANDOM_UE_ID = 0xdeadbeef;
		constexpr uint32_t RANDOM_VERSION_MAJOR = 16;
		v1::UUri uri;
		uri.set_authority_name("UTransportTest");
		uri.set_ue_id(RANDOM_UE_ID);
		uri.set_ue_version_major(RANDOM_VERSION_MAJOR);
		uri.set_resource_id(0);
		return uri;
	}

	static v1::UUri getWildcardUri() {
		constexpr uint32_t WILDCARD_VERSION_MAJOR = 0xFF;
		v1::UUri uri;
		uri.set_authority_name("*");
		uri.set_ue_id(WILDCARD);
		uri.set_ue_version_major(WILDCARD_VERSION_MAJOR);
		uri.set_resource_id(WILDCARD);
		return uri;
	}

public:
	~TestUTransport() override = default;
};

TEST_F(TestUTransport, CreateTransport) {                            // NOLINT
	EXPECT_NO_THROW(auto transport = makeTransport(getValidUri()));  // NOLINT
}

using InvalidUUri = datamodel::validator::uri::InvalidUUri;

TEST_F(TestUTransport, CreateTransportInvalidUUri) {  // NOLINT
	auto uri = getValidUri();
	uri.set_authority_name("*");
	EXPECT_THROW({ auto transport = makeTransport(uri); },  // NOLINT
	             InvalidUUri);
}

using UMessageBuilder = datamodel::builder::UMessageBuilder;
using PayloadBuilder = datamodel::builder::Payload;

TEST_F(TestUTransport, SendOk) {  // NOLINT
	constexpr uint32_t RANDOM_RESOURCE_ID = 0xABBA;
	auto transport_mock = makeMockTransport(getValidUri());
	auto transport = makeTransport(transport_mock);

	auto topic = getValidUri();
	topic.set_resource_id(RANDOM_RESOURCE_ID);
	PayloadBuilder payload(std::string(R"(["Arrival", "Waterloo"])"),
	                       v1::UPayloadFormat::UPAYLOAD_FORMAT_JSON);
	auto message =
	    UMessageBuilder::publish(std::move(topic)).build(std::move(payload));

	decltype(transport->send(message)) result;
	EXPECT_NO_THROW(result = transport->send(message));  // NOLINT

	EXPECT_EQ(result.code(), v1::UCode::OK);
	EXPECT_EQ(transport_mock->getSendCount(), 1);
	EXPECT_TRUE(transport_mock->getMessage() == message);
}

using InvalidUMessge = datamodel::validator::message::InvalidUMessage;

TEST_F(TestUTransport, SendInvalidMessage) {  // NOLINT
	auto transport_mock = makeMockTransport(getValidUri());
	auto transport = makeTransport(transport_mock);

	auto topic = getValidUri();
	topic.set_resource_id(RESOURCE_ID_F00D);
	auto message = UMessageBuilder::publish(std::move(topic)).build();
	message.mutable_attributes()->set_type(
	    v1::UMessageType::UMESSAGE_TYPE_REQUEST);

	decltype(transport->send(message)) result;
	EXPECT_THROW({ result = transport->send(message); },  // NOLINT
	             InvalidUMessge);
	EXPECT_EQ(transport_mock->getSendCount(), 0);
}

TEST_F(TestUTransport, SendImplStatus) {  // NOLINT
	auto transport_mock = makeMockTransport(getValidUri());
	auto transport = makeTransport(transport_mock);

	transport_mock->getSendStatus().set_code(v1::UCode::PERMISSION_DENIED);

	auto topic = getValidUri();
	topic.set_resource_id(RESOURCE_ID_F00D);
	auto message = UMessageBuilder::publish(std::move(topic)).build();

	decltype(transport->send(message)) result;
	EXPECT_NO_THROW(result = transport->send(message));  // NOLINT

	EXPECT_EQ(result.code(), v1::UCode::PERMISSION_DENIED);
}

TEST_F(TestUTransport, RegisterListenerOk) {  // NOLINT
	auto transport_mock = makeMockTransport(getValidUri());
	auto transport = makeTransport(transport_mock);

	bool called = false;
	auto callback = [&called](const auto&) { called = true; };
	auto source_filter = getWildcardUri();

	transport::UTransport::ListenHandle handle;
	EXPECT_NO_THROW({  // NOLINT
		auto maybe_handle =
		    transport->registerListener(callback, source_filter);

		EXPECT_TRUE(maybe_handle);
		if (maybe_handle) {
			handle = std::move(maybe_handle).value();
		}
	});

	EXPECT_TRUE(handle);

	EXPECT_FALSE(transport_mock->getSinkFilter());
	EXPECT_TRUE(source_filter == transport_mock->getSourceFilter());
	EXPECT_TRUE(transport_mock->getListener());
	if (transport_mock->getListener()) {
		auto callable = transport_mock->getListener().value();
		EXPECT_FALSE(called);
		auto topic = getValidUri();
		topic.set_resource_id(RESOURCE_ID_F00D);
		auto message = UMessageBuilder::publish(std::move(topic)).build();
		callable(message);
		EXPECT_TRUE(called);
	}
}

TEST_F(TestUTransport, RegisterListenerInvalidSource) {  // NOLINT
	auto transport_mock = makeMockTransport(getValidUri());
	auto transport = makeTransport(transport_mock);

	bool called = false;
	auto callback = [&called](const auto&) { called = true; };
	auto source_filter = getWildcardUri();
	source_filter.set_resource_id(1);

	EXPECT_THROW(  // NOLINT
	    {
		    auto maybe_handle =
		        transport->registerListener(callback, source_filter);
	    },
	    InvalidUUri);

	// Did not attempt to register a callback
	EXPECT_FALSE(transport_mock->getSinkFilter());
	EXPECT_FALSE(transport_mock->getListener());
	EXPECT_FALSE(called);
}

TEST_F(TestUTransport, RegisterListenerImplStatus) {  // NOLINT
	auto transport_mock = makeMockTransport(getValidUri());
	auto transport = makeTransport(transport_mock);

	transport_mock->getRegisterListenerStatus().set_code(v1::UCode::INTERNAL);

	bool called = false;
	auto callback = [&called](const auto&) { called = true; };
	auto source_filter = getWildcardUri();

	v1::UStatus status;
	EXPECT_NO_THROW({  // NOLINT
		auto maybe_handle =
		    transport->registerListener(callback, source_filter);

		EXPECT_FALSE(maybe_handle);
		if (!maybe_handle) {
			status = maybe_handle.error();
		}
	});

	// The listener that was sent to the impl is not connected
	EXPECT_TRUE(transport_mock->getListener());
	if (transport_mock->getListener()) {
		auto callable = transport_mock->getListener().value();
		EXPECT_FALSE(callable);
		auto topic = getValidUri();
		topic.set_resource_id(RESOURCE_ID_F00D);
		auto message = UMessageBuilder::publish(std::move(topic)).build();
		callable(message);
		EXPECT_FALSE(called);
	}
}

TEST_F(TestUTransport, RegisterListenerWithSinkFilterOk) {  // NOLINT
	auto transport_mock = makeMockTransport(getValidUri());
	auto transport = makeTransport(transport_mock);

	bool called = false;
	auto callback = [&called](const auto&) { called = true; };
	auto source_filter = getWildcardUri();
	auto sink_filter = getValidUri();
	sink_filter.set_resource_id(WILDCARD);

	transport::UTransport::ListenHandle handle;
	EXPECT_NO_THROW({  // NOLINT
		auto maybe_handle =
		    transport->registerListener(callback, source_filter, sink_filter);

		EXPECT_TRUE(maybe_handle);
		if (maybe_handle) {
			handle = std::move(maybe_handle).value();
		}
	});

	EXPECT_TRUE(handle);

	EXPECT_TRUE(transport_mock->getSinkFilter());
	if (transport_mock->getSinkFilter()) {
		EXPECT_TRUE(sink_filter == transport_mock->getSinkFilter().value());
	}

	EXPECT_TRUE(source_filter == transport_mock->getSourceFilter());
	EXPECT_TRUE(transport_mock->getListener());
	if (transport_mock->getListener()) {
		auto callable = transport_mock->getListener().value();
		EXPECT_FALSE(called);
		auto topic = getValidUri();
		topic.set_resource_id(RESOURCE_ID_F00D);
		auto message = UMessageBuilder::publish(std::move(topic)).build();
		callable(message);
		EXPECT_TRUE(called);
	}
}

TEST_F(TestUTransport, RegisterListenerWithSinkFilterInvalidSource) {  // NOLINT
	auto transport_mock = makeMockTransport(getValidUri());
	auto transport = makeTransport(transport_mock);

	bool called = false;
	auto callback = [&called](const auto&) { called = true; };
	auto source_filter = getWildcardUri();
	auto sink_filter = getValidUri();
	sink_filter.set_resource_id(WILDCARD);

	// Make source invalid
	source_filter.set_ue_version_major(WILDCARD);

	EXPECT_THROW(  // NOLINT
	    {
		    auto maybe_handle = transport->registerListener(
		        callback, source_filter, sink_filter);
	    },
	    InvalidUUri);

	// Did not attempt to register a callback
	EXPECT_FALSE(transport_mock->getSinkFilter());
	EXPECT_FALSE(transport_mock->getListener());
	EXPECT_FALSE(called);
}

TEST_F(TestUTransport, RegisterListenerWithSinkFilterInvalidSink) {  // NOLINT
	auto transport_mock = makeMockTransport(getValidUri());
	auto transport = makeTransport(transport_mock);

	bool called = false;
	auto callback = [&called](const auto&) { called = true; };
	auto source_filter = getWildcardUri();
	auto sink_filter = getValidUri();
	sink_filter.set_resource_id(WILDCARD);

	// Make sink invalid
	sink_filter.set_ue_version_major(WILDCARD);

	EXPECT_THROW(  // NOLINT
	    {
		    auto maybe_handle = transport->registerListener(
		        callback, source_filter, sink_filter);
	    },
	    InvalidUUri);

	// Did not attempt to register a callback
	EXPECT_FALSE(transport_mock->getSinkFilter());
	EXPECT_FALSE(transport_mock->getListener());
	EXPECT_FALSE(called);
}

TEST_F(TestUTransport, RegisterListenerWithSinkFilterImplStatus) {  // NOLINT
	auto transport_mock = makeMockTransport(getValidUri());
	auto transport = makeTransport(transport_mock);

	transport_mock->getRegisterListenerStatus().set_code(v1::UCode::NOT_FOUND);

	bool called = false;
	auto callback = [&called](const auto&) { called = true; };
	auto source_filter = getWildcardUri();
	auto sink_filter = getValidUri();
	sink_filter.set_resource_id(WILDCARD);

	v1::UStatus status;
	EXPECT_NO_THROW({  // NOLINT
		auto maybe_handle =
		    transport->registerListener(callback, source_filter, sink_filter);

		EXPECT_FALSE(maybe_handle);
		if (!maybe_handle) {
			status = maybe_handle.error();
		}
	});

	// The listener that was sent to the impl is not connected
	EXPECT_TRUE(transport_mock->getListener());
	if (transport_mock->getListener()) {
		auto callable = transport_mock->getListener().value();
		EXPECT_FALSE(callable);
		auto topic = getValidUri();
		topic.set_resource_id(RESOURCE_ID_F00D);
		auto message = UMessageBuilder::publish(std::move(topic)).build();
		callable(message);
		EXPECT_FALSE(called);
	}
}

TEST_F(TestUTransport, RegisterListenerWithSinkResourceOk) {  // NOLINT
	auto transport_mock = makeMockTransport(getValidUri());
	auto transport = makeTransport(transport_mock);

	bool called = false;
	auto callback = [&called](const auto&) { called = true; };
	auto source_filter = getWildcardUri();

	transport::UTransport::ListenHandle handle;
	EXPECT_NO_THROW({  // NOLINT
		auto maybe_handle = transport->registerListener(callback, source_filter,
		                                                RESOURCE_ID_F00D);

		EXPECT_TRUE(maybe_handle);
		if (maybe_handle) {
			handle = std::move(maybe_handle).value();
		}
	});

	EXPECT_TRUE(handle);

	EXPECT_TRUE(transport_mock->getSinkFilter());
	if (transport_mock->getSinkFilter()) {
		auto sink_filter = getValidUri();
		sink_filter.set_resource_id(RESOURCE_ID_F00D);
		EXPECT_TRUE(sink_filter == transport_mock->getSinkFilter().value());
	}

	EXPECT_TRUE(source_filter == transport_mock->getSourceFilter());
	EXPECT_TRUE(transport_mock->getListener());
	if (transport_mock->getListener()) {
		auto callable = transport_mock->getListener().value();
		EXPECT_FALSE(called);
		auto topic = getValidUri();
		topic.set_resource_id(RESOURCE_ID_F00D);
		auto message = UMessageBuilder::publish(std::move(topic)).build();
		callable(message);
		EXPECT_TRUE(called);
	}
}

TEST_F(TestUTransport,  // NOLINT
       RegisterListenerWithSinkResourceInvalidSource) {
	auto transport_mock = makeMockTransport(getValidUri());
	auto transport = makeTransport(transport_mock);

	bool called = false;
	auto callback = [&called](const auto&) { called = true; };
	auto source_filter = getWildcardUri();

	// Make source invalid
	source_filter.set_ue_version_major(WILDCARD);

	EXPECT_THROW(  // NOLINT
	    {
		    auto maybe_handle =
		        transport->registerListener(callback, source_filter, 0xABBA);
	    },
	    InvalidUUri);

	// Did not attempt to register a callback
	EXPECT_FALSE(transport_mock->getSinkFilter());
	EXPECT_FALSE(transport_mock->getListener());
	EXPECT_FALSE(called);
}

// NOTE: it is not possible to produce an invalid sink filter with this method
// since it constrains the sink resource parameter to uint16_t

TEST_F(TestUTransport, RegisterListenerWithSinkResourceImplStatus) {  // NOLINT
	auto transport_mock = makeMockTransport(getValidUri());
	auto transport = makeTransport(transport_mock);

	transport_mock->getRegisterListenerStatus().set_code(v1::UCode::NOT_FOUND);

	bool called = false;
	auto callback = [&called](const auto&) { called = true; };
	auto source_filter = getWildcardUri();

	v1::UStatus status;
	EXPECT_NO_THROW({  // NOLINT
		auto maybe_handle =
		    transport->registerListener(callback, source_filter, WILDCARD);

		EXPECT_FALSE(maybe_handle);
		if (!maybe_handle) {
			status = maybe_handle.error();
		}
	});

	// The listener that was sent to the impl is not connected
	EXPECT_TRUE(transport_mock->getListener());
	if (transport_mock->getListener()) {
		auto callable = transport_mock->getListener().value();
		// The callback isn't connected...
		EXPECT_FALSE(callable);
		// ...but we're still going to try to call it anyway
		auto topic = getValidUri();
		topic.set_resource_id(RESOURCE_ID_F00D);
		auto message = UMessageBuilder::publish(std::move(topic)).build();
		callable(message);
		EXPECT_FALSE(called);
	}
}

TEST_F(TestUTransport, DeprecatedRegisterListenerOk) {  // NOLINT
	auto transport_mock = makeMockTransport(getValidUri());
	auto transport = makeTransport(transport_mock);

	bool called = false;
	auto callback = [&called](const auto&) { called = true; };
	auto topic_source_filter = getWildcardUri();

	transport::UTransport::ListenHandle handle;
	EXPECT_NO_THROW({  // NOLINT
		// When source_filter is omitted, sink_filter is treated as a publish
		// topic.
		auto maybe_handle =
		    transport->registerListener(topic_source_filter, callback, {});

		EXPECT_TRUE(maybe_handle);
		if (maybe_handle) {
			handle = std::move(maybe_handle).value();
		}
	});

	EXPECT_TRUE(handle);

	EXPECT_FALSE(transport_mock->getSinkFilter());
	EXPECT_TRUE(topic_source_filter == transport_mock->getSourceFilter());
	EXPECT_TRUE(transport_mock->getListener());
	if (transport_mock->getListener()) {
		auto callable = transport_mock->getListener().value();
		EXPECT_FALSE(called);
		auto topic = getValidUri();
		topic.set_resource_id(RESOURCE_ID_F00D);
		auto message = UMessageBuilder::publish(std::move(topic)).build();
		callable(message);
		EXPECT_TRUE(called);
	}
}

TEST_F(TestUTransport,  // NOLINT
       DeprecatedRegisterListenerWithSourceFilterOk) {
	auto transport_mock = makeMockTransport(getValidUri());
	auto transport = makeTransport(transport_mock);

	bool called = false;
	auto callback = [&called](const auto&) { called = true; };
	auto source_filter = getWildcardUri();
	auto sink_filter = getValidUri();
	sink_filter.set_resource_id(WILDCARD);

	transport::UTransport::ListenHandle handle;
	EXPECT_NO_THROW({  // NOLINT
		auto maybe_handle =
		    transport->registerListener(sink_filter, callback, source_filter);

		EXPECT_TRUE(maybe_handle);
		if (maybe_handle) {
			handle = std::move(maybe_handle).value();
		}
	});

	EXPECT_TRUE(handle);

	EXPECT_TRUE(transport_mock->getSinkFilter());
	if (transport_mock->getSinkFilter()) {
		EXPECT_TRUE(sink_filter == transport_mock->getSinkFilter().value());
	}

	EXPECT_TRUE(source_filter == transport_mock->getSourceFilter());
	EXPECT_TRUE(transport_mock->getListener());
	if (transport_mock->getListener()) {
		auto callable = transport_mock->getListener().value();
		EXPECT_FALSE(called);
		auto topic = getValidUri();
		topic.set_resource_id(RESOURCE_ID_F00D);
		auto message = UMessageBuilder::publish(std::move(topic)).build();
		callable(message);
		EXPECT_TRUE(called);
	}
}

}  // namespace uprotocol
