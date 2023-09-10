#include "spdlog/spdlog.h"
#include <include/utransport/UAttributes.h>
#include <include/uuid/uuid_gen.h>

using namespace uprotocol::utransport::datamodel;

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
