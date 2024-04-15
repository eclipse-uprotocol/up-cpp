/*
 * Copyright (c) 2024 General Motors GTO LLC
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 * 
 * SPDX-FileType: SOURCE
 * SPDX-FileCopyrightText: 2024 General Motors GTO LLC
 * SPDX-License-Identifier: Apache-2.0
 */

#include <up-cpp/transport/builder/UAttributesBuilder.h>
#include <up-cpp/uuid/factory/UuidFactory.h>
#include <gtest/gtest.h>
#include "up-cpp/uri/builder/BuildUUri.h"
#include "up-cpp/uri/builder/BuildEntity.h"
#include "up-cpp/uri/builder/BuildUResource.h"
#include <up-cpp/uuid/serializer/UuidSerializer.h>

#include <up-core-api/uri.pb.h>

using namespace uprotocol::uuid;
using namespace uprotocol::v1;
using namespace uprotocol::utransport;
using namespace uprotocol::uri;

// Test the UAttributes class
TEST(UAttributesTest, Class) 
{
    // Create an empty UAttributes object
    UAttributes attributes;

    auto id = UuidFactory::create();

    // Create a UAttributes object with some values
    UMessageType type = UMessageType::UMESSAGE_TYPE_PUBLISH;
    UPriority priority = UPriority::UPRIORITY_CS0;
    UUri uri;
    UAttributesBuilder builder(uri,id, type, priority);
    
    UAttributes nonEmptyAttributes = builder.build();


    // Test getters for the attributes
    EXPECT_EQ(nonEmptyAttributes.type(), type);
    EXPECT_EQ(nonEmptyAttributes.priority(), priority);
}

// Test building a UAttributes object used for request and response
TEST(UAttributesTest, BuildingRequestResponse) 
{
    // Return address
    auto source = BuildUUri()
                    .setEntity(BuildUEntity().setName("hartley_app").setMajorVersion(1).build())
                    .setResource(BuildUResource().setRpcResponse().build())
                    .build();
    
    // Method we want to invoke
    UUri sink = BuildUUri()
                    .setEntity(BuildUEntity().setName("test_service").setMajorVersion(1).build())
                    .setResource(BuildUResource().setRpcRequest("test_function").build())
                    .build();

    // Create a UAttributes object with some values
    UAttributes request = UAttributesBuilder().request(source, sink, UPriority::UPRIORITY_CS4, 1000).build();

    // Test getters for the attributes
    EXPECT_EQ(request.sink().entity().name(), "test_service");
    EXPECT_EQ(request.sink().resource().name(), "rpc");
    EXPECT_EQ(request.sink().resource().instance(), "test_function");
    EXPECT_EQ(request.ttl(), 1000);
    EXPECT_EQ(request.source().entity().name(), "hartley_app");
    EXPECT_EQ(request.source().resource().name(), "rpc");
    EXPECT_EQ(request.source().resource().id(), 0);
    EXPECT_EQ(request.source().resource().instance(), "response");
    EXPECT_EQ(request.type(), UMessageType::UMESSAGE_TYPE_REQUEST);
    EXPECT_EQ(request.priority(), UPriority::UPRIORITY_CS4);

    // Create a UAttributes response using the information from a request
    // the source becomes the sink and vice versa
    UAttributes response = UAttributesBuilder().response(sink, source, UPriority::UPRIORITY_CS4, request.id()).build();

    EXPECT_EQ(UuidSerializer::serializeToString(response.reqid()), UuidSerializer::serializeToString(request.id()));
    EXPECT_EQ(request.sink().entity().name(), response.source().entity().name());
    EXPECT_EQ(request.source().entity().name(), response.sink().entity().name());
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
