#include "spdlog/spdlog.h"
#include <uprotocol/transport/datamodel/UAttributes.h>
#include <uprotocol/uuid/factory/UuidFactory.h>
#include <gtest/gtest.h>

using namespace uprotocol::utransport;
using namespace uprotocol::uuid;

// Test the UAttributes class
TEST(UAttributesTest, Class) 
{
    // Create an empty UAttributes object
    UAttributes attributes;

    auto id = UuidFactory::create();

    // Create a UAttributes object with some values
    UMessageType type = UMessageType::PUBLISH;
    UPriority priority = UPriority::STANDARD;
    UAttributesBuilder builder(id, type, priority);
    
    builder.withTtl(100);
    builder.withToken("sample_token");
    builder.withHint(USerializationHint::JSON);
    builder.withSink(UUri(uri_datamodel::uri_authority::local(), 
                        uri_datamodel::UEntity::fromName("body.access"),
                        uri_datamodel::uri_resource::fromName("door")));
    builder.withPermissionLevel(5);
    builder.withCommStatus(200);
    builder.withReqId(id);
    UAttributes nonEmptyAttributes = builder.build();


    // Test getters for the attributes
    EXPECT_EQ(nonEmptyAttributes.type(), type);
    EXPECT_EQ(nonEmptyAttributes.priority(), priority);
    EXPECT_EQ(nonEmptyAttributes.ttl().value_or(0), 100);
    EXPECT_EQ(nonEmptyAttributes.token().value_or(""), "sample_token");
    EXPECT_EQ(nonEmptyAttributes.serializationHint().value(), USerializationHint::JSON);
    EXPECT_EQ(nonEmptyAttributes.plevel().value_or(0), 5);
    EXPECT_EQ(nonEmptyAttributes.commstatus().value_or(0), 200);
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
