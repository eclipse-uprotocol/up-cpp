#include "spdlog/spdlog.h"
#include <include/utransport/UAttributes.h>
#include <include/uuid/uuid_gen.h>

using namespace uprotocol::utransport::datamodel;



#include <gtest/gtest.h>
#include "UAttributes.h"  // Include the header file where UAttributes and UAttributesBuilder are defined

// Test the UAttributes class
TEST(UAttributesTest, Class) 
{
    // Create an empty UAttributes object
    uprotocol::utransport::datamodel::UAttributes attributes;
    
    // Test the isEmpty() method for an empty object
    EXPECT_TRUE(attributes.isEmpty());

    st_uuid id;

    if (false == uuid_v6_generate(&id))
    {
        spdlog::error("uuid_v6_generate_str failed");
        return -1;
    }

    // Create a UAttributes object with some values
    uprotocol::utransport::datamodel::UMessageType type = uprotocol::utransport::datamodel::UMessageType::PUBLISH;
    uprotocol::utransport::datamodel::UPriority priority = uprotocol::utransport::datamodel::UPriority::STANDARD;
    uprotocol::utransport::datamodel::UAttributesBuilder builder(id, type, priority);
    builder.withTtl(100);
    builder.withToken("sample_token");
    builder.withHint(uprotocol::utransport::datamodel::USerializationHint::JSON);
    builder.withSink(UUri(uri_datamodel::uri_authority::local(), 
                        uri_datamodel::uri_entity::fromName("body.access"),
                        uri_datamodel::uri_resource::fromName("door")));
    builder.withPermissionLevel(5);
    builder.withCommStatus(200);
    builder.withReqId(id);
    uprotocol::utransport::datamodel::UAttributes nonEmptyAttributes = builder.build();

    // Test the isEmpty() method for a non-empty object
    EXPECT_FALSE(nonEmptyAttributes.isEmpty());

    // Test getters for the attributes
    EXPECT_EQ(nonEmptyAttributes.id(), id);
    EXPECT_EQ(nonEmptyAttributes.type(), type);
    EXPECT_EQ(nonEmptyAttributes.priority(), priority);
    EXPECT_EQ(nonEmptyAttributes.ttl().value_or(0), 100);
    EXPECT_EQ(nonEmptyAttributes.token().value_or(""), "sample_token");
    EXPECT_EQ(nonEmptyAttributes.serializationHint().value(), uprotocol::utransport::datamodel::USerializationHint::JSON);
    EXPECT_EQ(nonEmptyAttributes.sink().value().toString(), "http://example.com");
    EXPECT_EQ(nonEmptyAttributes.permissionLevel().value_or(0), 5);
    EXPECT_EQ(nonEmptyAttributes.commStatus().value_or(0), 200);
    EXPECT_EQ(nonEmptyAttributes.reqId().value(),id);
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}


int main() 
{
    st_uuid uuid;

    if (false == uuid_v6_generate(&uuid))
    {
        spdlog::error("uuid_v6_generate_str failed");
        return -1;
    }
    // Example usage of the UAttributes class and builder pattern
    UAttributes attributes = UAttributes::UAttributesBuilder(&uuid, UMessageType::PUBLISH, UPriority::LOW)
        .withTtl(1000)  // Replace 1000 with the actual TTL value
        .withToken("my_token")
        .withHint(USerializationHint::PROTOBUF)
        .withSink(UUri(uri_datamodel::uri_authority::local(), 
                        uri_datamodel::uri_entity::fromName("body.access"),
                        uri_datamodel::uri_resource::fromName("door")))
        .withPermissionLevel(1)  // Replace 1 with the actual permission level
        .withCommStatus(2)  // Replace 2 with the actual communication status
        .withReqId({0})  // Replace {1} with the actual UUID value
        .build();

    // Access attributes
    UUID id = attributes.id();
    UMessageType type = attributes.type();
    UPriority priority = attributes.priority();
    std::optional<int> ttl = attributes.ttl();
    std::optional<std::string> token = attributes.token();
    std::optional<USerializationHint> hint = attributes.serializationHint();
    std::optional<UUri> sink = attributes.sink();
    std::optional<int> plevel = attributes.plevel();
    std::optional<int> commstatus = attributes.commstatus();
    std::optional<UUID> reqid = attributes.reqid();

    st_uuid_v6_str uuidStr;

    if (false == uuid_v6_to_uuid_v6_str(id, &uuidStr))
    {
        spdlog::error("uuid_v6_to_uuid_v6_str failed");
        return -1;
    }

    spdlog::info(
        "id = ",
        uuidStr.ac_uuid);

    spdlog::info(
        "type = ",
        UMessageTypeToString(type).value());

    spdlog::info(
        "priority = ",
        UPriorityToString(priority).value());

    spdlog::info(
        "ttl = ",
        ttl.value());

    spdlog::info(
        "token = ",
        token.value());

    spdlog::info(
        "hint = ",
        USerializationHintToString(hint.value()).value());

    spdlog::info(
        "plevel = ",
        plevel.value());

    spdlog::info(
        "commstatus = ",
        commstatus.value());

    if (false == uuid_v6_to_uuid_v6_str(reqid.value(), &uuidStr))
    {
        spdlog::error("uuid_v6_to_uuid_v6_str failed");
        return -1;
    }

    spdlog::info(
        "reqId = ",
        uuidStr.ac_uuid);

    return 0;
}
