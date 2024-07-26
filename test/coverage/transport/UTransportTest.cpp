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

namespace {

bool operator==(const uprotocol::v1::UUri& lhs,
                const uprotocol::v1::UUri& rhs) {
	using namespace google::protobuf::util;
	return MessageDifferencer::Equals(lhs, rhs);
}

bool operator==(const uprotocol::v1::UMessage& lhs,
                const uprotocol::v1::UMessage& rhs) {
	using namespace google::protobuf::util;
	return MessageDifferencer::Equals(lhs, rhs);
}

#if 0
bool operator==(const uprotocol::v1::UStatus& lhs,
                const uprotocol::v1::UCode& rhs) {
	return lhs.code() == rhs;
}
#endif

class TestUTransport : public testing::Test {
protected:
	// Run once per TEST_F.
	// Used to set up clean environments per test.
	void SetUp() override {}

	void TearDown() override {}

	// Run once per execution of the test application.
	// Used for setup of all tests. Has access to this instance.
	TestUTransport() = default;
	~TestUTransport() = default;

	// Run once per execution of the test application.
	// Used only for global setup outside of tests.
	static void SetUpTestSuite() {}
	static void TearDownTestSuite() {}

	static std::shared_ptr<uprotocol::test::UTransportMock> makeMockTransport(
	    const uprotocol::v1::UUri& uri) {
		return std::make_shared<uprotocol::test::UTransportMock>(uri);
	}

	static std::shared_ptr<uprotocol::transport::UTransport> makeTransport(
	    const uprotocol::v1::UUri& uri) {
		return makeMockTransport(uri);
	}

	static std::shared_ptr<uprotocol::transport::UTransport> makeTransport(
	    std::shared_ptr<uprotocol::test::UTransportMock> mock) {
		return mock;
	}

	static uprotocol::v1::UUri getValidUri() {
		uprotocol::v1::UUri uri;
		uri.set_authority_name("UTransportTest");
		uri.set_ue_id(0xdeadbeef);
		uri.set_ue_version_major(16);
		uri.set_resource_id(0);
		return uri;
	}

	static uprotocol::v1::UUri getWildcardUri() {
		uprotocol::v1::UUri uri;
		uri.set_authority_name("*");
		uri.set_ue_id(0xFFFF);
		uri.set_ue_version_major(0xFF);
		uri.set_resource_id(0xFFFF);
		return uri;
	}
};

TEST_F(TestUTransport, CreateTransport) {
	EXPECT_NO_THROW(auto transport = makeTransport(getValidUri()));
}

using InvalidUUri = uprotocol::datamodel::validator::uri::InvalidUUri;

TEST_F(TestUTransport, CreateTransportInvalidUUri) {
	auto uri = getValidUri();
	uri.set_authority_name("*");
	EXPECT_THROW({ auto transport = makeTransport(uri); }, InvalidUUri);
}

using UMessageBuilder = uprotocol::datamodel::builder::UMessageBuilder;
using PayloadBuilder = uprotocol::datamodel::builder::Payload;

TEST_F(TestUTransport, SendOk) {
	auto transport_mock = makeMockTransport(getValidUri());
	auto transport = makeTransport(transport_mock);

	auto topic = getValidUri();
	topic.set_resource_id(0xABBA);
	PayloadBuilder payload(std::string("[\"Arrival\", \"Waterloo\"]"),
	                       uprotocol::v1::UPayloadFormat::UPAYLOAD_FORMAT_JSON);
	auto message =
	    UMessageBuilder::publish(std::move(topic)).build(std::move(payload));

	decltype(transport->send(message)) result;
	EXPECT_NO_THROW(result = transport->send(message));

	EXPECT_EQ(result.code(), uprotocol::v1::UCode::OK);
	EXPECT_EQ(transport_mock->send_count_, 1);
	EXPECT_TRUE(transport_mock->message_ == message);
}

using InvalidUMessge =
    uprotocol::datamodel::validator::message::InvalidUMessage;

TEST_F(TestUTransport, SendInvalidMessage) {
	auto transport_mock = makeMockTransport(getValidUri());
	auto transport = makeTransport(transport_mock);

	auto topic = getValidUri();
	topic.set_resource_id(0xF00D);
	auto message = UMessageBuilder::publish(std::move(topic)).build();
	message.mutable_attributes()->set_type(
	    uprotocol::v1::UMessageType::UMESSAGE_TYPE_REQUEST);

	decltype(transport->send(message)) result;
	EXPECT_THROW({ result = transport->send(message); }, InvalidUMessge);
	EXPECT_EQ(transport_mock->send_count_, 0);
}

TEST_F(TestUTransport, SendImplStatus) {
	auto transport_mock = makeMockTransport(getValidUri());
	auto transport = makeTransport(transport_mock);

	transport_mock->send_status_.set_code(
	    uprotocol::v1::UCode::PERMISSION_DENIED);

	auto topic = getValidUri();
	topic.set_resource_id(0xF00D);
	auto message = UMessageBuilder::publish(std::move(topic)).build();

	decltype(transport->send(message)) result;
	EXPECT_NO_THROW(result = transport->send(message));

	EXPECT_EQ(result.code(), uprotocol::v1::UCode::PERMISSION_DENIED);
}

TEST_F(TestUTransport, RegisterListenerOk) {
	auto transport_mock = makeMockTransport(getValidUri());
	auto transport = makeTransport(transport_mock);

	bool called = false;
	auto callback = [&called](const auto&) { called = true; };
	auto source_filter = getWildcardUri();

	uprotocol::transport::UTransport::ListenHandle handle;
	EXPECT_NO_THROW({
		auto maybe_handle =
		    transport->registerListener(callback, source_filter);

		EXPECT_TRUE(maybe_handle);
		if (maybe_handle) {
			handle = std::move(maybe_handle).value();
		}
	});

	EXPECT_TRUE(handle);

	EXPECT_FALSE(transport_mock->sink_filter_);
	EXPECT_TRUE(source_filter == transport_mock->source_filter_);
	EXPECT_TRUE(transport_mock->listener_);
	if (transport_mock->listener_) {
		auto callable = transport_mock->listener_.value();
		EXPECT_FALSE(called);
		auto topic = getValidUri();
		topic.set_resource_id(0xF00D);
		auto message = UMessageBuilder::publish(std::move(topic)).build();
		callable(message);
		EXPECT_TRUE(called);
	}
}

TEST_F(TestUTransport, RegisterListenerInvalidSource) {
	auto transport_mock = makeMockTransport(getValidUri());
	auto transport = makeTransport(transport_mock);

	bool called = false;
	auto callback = [&called](const auto&) { called = true; };
	auto source_filter = getWildcardUri();
	source_filter.set_resource_id(1);

	EXPECT_THROW(
	    {
		    auto maybe_handle =
		        transport->registerListener(callback, source_filter);
	    },
	    InvalidUUri);

	// Did not attempt to register a callback
	EXPECT_FALSE(transport_mock->sink_filter_);
	EXPECT_FALSE(transport_mock->listener_);
	EXPECT_FALSE(called);
}

TEST_F(TestUTransport, RegisterListenerImplStatus) {
	auto transport_mock = makeMockTransport(getValidUri());
	auto transport = makeTransport(transport_mock);

	transport_mock->registerListener_status_.set_code(
	    uprotocol::v1::UCode::INTERNAL);

	bool called = false;
	auto callback = [&called](const auto&) { called = true; };
	auto source_filter = getWildcardUri();

	uprotocol::v1::UStatus status;
	EXPECT_NO_THROW({
		auto maybe_handle =
		    transport->registerListener(callback, source_filter);

		EXPECT_FALSE(maybe_handle);
		if (!maybe_handle) {
			status = maybe_handle.error();
		}
	});

	// The listener that was sent to the impl is not connected
	EXPECT_TRUE(transport_mock->listener_);
	if (transport_mock->listener_) {
		auto callable = transport_mock->listener_.value();
		EXPECT_FALSE(callable);
		auto topic = getValidUri();
		topic.set_resource_id(0xF00D);
		auto message = UMessageBuilder::publish(std::move(topic)).build();
		callable(message);
		EXPECT_FALSE(called);
	}
}

TEST_F(TestUTransport, RegisterListenerWithSinkFilterOk) {
	auto transport_mock = makeMockTransport(getValidUri());
	auto transport = makeTransport(transport_mock);

	bool called = false;
	auto callback = [&called](const auto&) { called = true; };
	auto source_filter = getWildcardUri();
	auto sink_filter = getValidUri();
	sink_filter.set_resource_id(0xFFFF);

	uprotocol::transport::UTransport::ListenHandle handle;
	EXPECT_NO_THROW({
		auto maybe_handle =
		    transport->registerListener(callback, source_filter, sink_filter);

		EXPECT_TRUE(maybe_handle);
		if (maybe_handle) {
			handle = std::move(maybe_handle).value();
		}
	});

	EXPECT_TRUE(handle);

	EXPECT_TRUE(transport_mock->sink_filter_);
	if (transport_mock->sink_filter_) {
		EXPECT_TRUE(sink_filter == transport_mock->sink_filter_.value());
	}

	EXPECT_TRUE(source_filter == transport_mock->source_filter_);
	EXPECT_TRUE(transport_mock->listener_);
	if (transport_mock->listener_) {
		auto callable = transport_mock->listener_.value();
		EXPECT_FALSE(called);
		auto topic = getValidUri();
		topic.set_resource_id(0xF00D);
		auto message = UMessageBuilder::publish(std::move(topic)).build();
		callable(message);
		EXPECT_TRUE(called);
	}
}

TEST_F(TestUTransport, RegisterListenerWithSinkFilterInvalidSource) {
	auto transport_mock = makeMockTransport(getValidUri());
	auto transport = makeTransport(transport_mock);

	bool called = false;
	auto callback = [&called](const auto&) { called = true; };
	auto source_filter = getWildcardUri();
	auto sink_filter = getValidUri();
	sink_filter.set_resource_id(0xFFFF);

	// Make source invalid
	source_filter.set_ue_version_major(0xFFFF);

	EXPECT_THROW(
	    {
		    auto maybe_handle = transport->registerListener(
		        callback, source_filter, sink_filter);
	    },
	    InvalidUUri);

	// Did not attempt to register a callback
	EXPECT_FALSE(transport_mock->sink_filter_);
	EXPECT_FALSE(transport_mock->listener_);
	EXPECT_FALSE(called);
}

TEST_F(TestUTransport, RegisterListenerWithSinkFilterInvalidSink) {
	auto transport_mock = makeMockTransport(getValidUri());
	auto transport = makeTransport(transport_mock);

	bool called = false;
	auto callback = [&called](const auto&) { called = true; };
	auto source_filter = getWildcardUri();
	auto sink_filter = getValidUri();
	sink_filter.set_resource_id(0xFFFF);

	// Make sink invalid
	sink_filter.set_ue_version_major(0xFFFF);

	EXPECT_THROW(
	    {
		    auto maybe_handle = transport->registerListener(
		        callback, source_filter, sink_filter);
	    },
	    InvalidUUri);

	// Did not attempt to register a callback
	EXPECT_FALSE(transport_mock->sink_filter_);
	EXPECT_FALSE(transport_mock->listener_);
	EXPECT_FALSE(called);
}

TEST_F(TestUTransport, RegisterListenerWithSinkFilterImplStatus) {
	auto transport_mock = makeMockTransport(getValidUri());
	auto transport = makeTransport(transport_mock);

	transport_mock->registerListener_status_.set_code(
	    uprotocol::v1::UCode::NOT_FOUND);

	bool called = false;
	auto callback = [&called](const auto&) { called = true; };
	auto source_filter = getWildcardUri();
	auto sink_filter = getValidUri();
	sink_filter.set_resource_id(0xFFFF);

	uprotocol::v1::UStatus status;
	EXPECT_NO_THROW({
		auto maybe_handle =
		    transport->registerListener(callback, source_filter, sink_filter);

		EXPECT_FALSE(maybe_handle);
		if (!maybe_handle) {
			status = maybe_handle.error();
		}
	});

	// The listener that was sent to the impl is not connected
	EXPECT_TRUE(transport_mock->listener_);
	if (transport_mock->listener_) {
		auto callable = transport_mock->listener_.value();
		EXPECT_FALSE(callable);
		auto topic = getValidUri();
		topic.set_resource_id(0xF00D);
		auto message = UMessageBuilder::publish(std::move(topic)).build();
		callable(message);
		EXPECT_FALSE(called);
	}
}

TEST_F(TestUTransport, RegisterListenerWithSinkResourceOk) {
	auto transport_mock = makeMockTransport(getValidUri());
	auto transport = makeTransport(transport_mock);

	bool called = false;
	auto callback = [&called](const auto&) { called = true; };
	auto source_filter = getWildcardUri();

	uprotocol::transport::UTransport::ListenHandle handle;
	EXPECT_NO_THROW({
		auto maybe_handle =
		    transport->registerListener(callback, source_filter, 0xF00D);

		EXPECT_TRUE(maybe_handle);
		if (maybe_handle) {
			handle = std::move(maybe_handle).value();
		}
	});

	EXPECT_TRUE(handle);

	EXPECT_TRUE(transport_mock->sink_filter_);
	if (transport_mock->sink_filter_) {
		auto sink_filter = getValidUri();
		sink_filter.set_resource_id(0xF00D);
		EXPECT_TRUE(sink_filter == transport_mock->sink_filter_.value());
	}

	EXPECT_TRUE(source_filter == transport_mock->source_filter_);
	EXPECT_TRUE(transport_mock->listener_);
	if (transport_mock->listener_) {
		auto callable = transport_mock->listener_.value();
		EXPECT_FALSE(called);
		auto topic = getValidUri();
		topic.set_resource_id(0xF00D);
		auto message = UMessageBuilder::publish(std::move(topic)).build();
		callable(message);
		EXPECT_TRUE(called);
	}
}

TEST_F(TestUTransport, RegisterListenerWithSinkResourceInvalidSource) {
	auto transport_mock = makeMockTransport(getValidUri());
	auto transport = makeTransport(transport_mock);

	bool called = false;
	auto callback = [&called](const auto&) { called = true; };
	auto source_filter = getWildcardUri();

	// Make source invalid
	source_filter.set_ue_version_major(0xFFFF);

	EXPECT_THROW(
	    {
		    auto maybe_handle =
		        transport->registerListener(callback, source_filter, 0xABBA);
	    },
	    InvalidUUri);

	// Did not attempt to register a callback
	EXPECT_FALSE(transport_mock->sink_filter_);
	EXPECT_FALSE(transport_mock->listener_);
	EXPECT_FALSE(called);
}

// NOTE: it is not possible to produce an invalid sink filter with this method
// since it constrains the sink resource parameter to uint16_t

TEST_F(TestUTransport, RegisterListenerWithSinkResourceImplStatus) {
	auto transport_mock = makeMockTransport(getValidUri());
	auto transport = makeTransport(transport_mock);

	transport_mock->registerListener_status_.set_code(
	    uprotocol::v1::UCode::NOT_FOUND);

	bool called = false;
	auto callback = [&called](const auto&) { called = true; };
	auto source_filter = getWildcardUri();

	uprotocol::v1::UStatus status;
	EXPECT_NO_THROW({
		auto maybe_handle =
		    transport->registerListener(callback, source_filter, 0xFFFF);

		EXPECT_FALSE(maybe_handle);
		if (!maybe_handle) {
			status = maybe_handle.error();
		}
	});

	// The listener that was sent to the impl is not connected
	EXPECT_TRUE(transport_mock->listener_);
	if (transport_mock->listener_) {
		auto callable = transport_mock->listener_.value();
		// The callback isn't connected...
		EXPECT_FALSE(callable);
		// ...but we're still going to try to call it anyway
		auto topic = getValidUri();
		topic.set_resource_id(0xF00D);
		auto message = UMessageBuilder::publish(std::move(topic)).build();
		callable(message);
		EXPECT_FALSE(called);
	}
}

TEST_F(TestUTransport, DeprecatedRegisterListenerOk) {
	auto transport_mock = makeMockTransport(getValidUri());
	auto transport = makeTransport(transport_mock);

	bool called = false;
	auto callback = [&called](const auto&) { called = true; };
	auto topic_source_filter = getWildcardUri();

	uprotocol::transport::UTransport::ListenHandle handle;
	EXPECT_NO_THROW({
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

	EXPECT_FALSE(transport_mock->sink_filter_);
	EXPECT_TRUE(topic_source_filter == transport_mock->source_filter_);
	EXPECT_TRUE(transport_mock->listener_);
	if (transport_mock->listener_) {
		auto callable = transport_mock->listener_.value();
		EXPECT_FALSE(called);
		auto topic = getValidUri();
		topic.set_resource_id(0xF00D);
		auto message = UMessageBuilder::publish(std::move(topic)).build();
		callable(message);
		EXPECT_TRUE(called);
	}
}

TEST_F(TestUTransport, DeprecatedRegisterListenerWithSourceFilterOk) {
	auto transport_mock = makeMockTransport(getValidUri());
	auto transport = makeTransport(transport_mock);

	bool called = false;
	auto callback = [&called](const auto&) { called = true; };
	auto source_filter = getWildcardUri();
	auto sink_filter = getValidUri();
	sink_filter.set_resource_id(0xFFFF);

	uprotocol::transport::UTransport::ListenHandle handle;
	EXPECT_NO_THROW({
		auto maybe_handle =
		    transport->registerListener(sink_filter, callback, source_filter);

		EXPECT_TRUE(maybe_handle);
		if (maybe_handle) {
			handle = std::move(maybe_handle).value();
		}
	});

	EXPECT_TRUE(handle);

	EXPECT_TRUE(transport_mock->sink_filter_);
	if (transport_mock->sink_filter_) {
		EXPECT_TRUE(sink_filter == transport_mock->sink_filter_.value());
	}

	EXPECT_TRUE(source_filter == transport_mock->source_filter_);
	EXPECT_TRUE(transport_mock->listener_);
	if (transport_mock->listener_) {
		auto callable = transport_mock->listener_.value();
		EXPECT_FALSE(called);
		auto topic = getValidUri();
		topic.set_resource_id(0xF00D);
		auto message = UMessageBuilder::publish(std::move(topic)).build();
		callable(message);
		EXPECT_TRUE(called);
	}
}

}  // namespace
