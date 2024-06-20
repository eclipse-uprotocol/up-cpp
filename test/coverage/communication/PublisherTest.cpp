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

#include <gtest/gtest.h>
#include <up-cpp/communication/Publisher.h>

#include "UTransportMock.h"
#include "up-cpp/communication/Publisher.h"
#include "up-cpp/datamodel/builder/UMessage.h"
#include <up-cpp/datamodel/builder/Payload.h>
#include <up-cpp/datamodel/serializer/UUri.h>
#include <uprotocol/v1/uri.pb.h>

using namespace uprotocol::communication;
using namespace uprotocol::datamodel::builder;
using namespace uprotocol::v1;



namespace uprotocol::test {
using namespace uprotocol::datamodel::builder;

class TestPublisher : public testing::Test {
protected:
	// Run once per TEST_F.
	// Used to set up clean environments per test.
	void SetUp() override {
		topic_.set_authority_name("10.0.0.1");
        topic_.set_ue_id(0x00011101);
        topic_.set_ue_version_major(0xF8);
        topic_.set_resource_id(0x8101);
        transportMock_ = std::make_shared<UTransportMock>(topic_);

        format_ = UPayloadFormat::UPAYLOAD_FORMAT_TEXT;
        priority_ = UPriority::UPRIORITY_CS1;
        ttl_ = std::chrono::milliseconds(1000);
	}

	void TearDown() override {}

	// Run once per execution of the test application.
	// Used for setup of all tests. Has access to this instance.
	TestPublisher() = default;
	~TestPublisher() = default;

	// Run once per execution of the test application.
	// Used only for global setup outside of tests.
	static void SetUpTestSuite() {}
	static void TearDownTestSuite() {}

    std::shared_ptr<UTransportMock> transportMock_;
    UUri topic_;
    UPayloadFormat format_;
    std::optional<UPriority> priority_;
    std::optional<std::chrono::milliseconds> ttl_;

};

TEST_F(TestPublisher, PublisherSuccess) {
	std::string testPayloadStr = "test_payload";

	// EXPECT_CALL(*transportMock, send(_)).Times(1).WillOnce(Return(UStatus::USTATUS_OK));
	Payload testPayload(testPayloadStr, format_);
    Publisher publisher(transportMock_, topic_, format_, priority_, ttl_);

    // EXPECT_CALL(*transportMock, send(_)).Times(1).WillOnce(Return(UStatus::USTATUS_OK));
	auto status = publisher.publish(std::move(testPayload));

    EXPECT_EQ(status.code(), v1::UCode::OK);
}





}  // namespace
