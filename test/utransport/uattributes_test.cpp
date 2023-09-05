#include <include/utransport/UAttributes.h>
#include <include/uuid/uuid_gen.h>

using namespace uprotocol::utransport::datamodel;

int main() 
{
    // Example usage of the UAttributes class and builder pattern
    UAttributes attributes = UAttributes::UAttributesBuilder()
        .withId({0})  // Replace {0} with the actual UUID value
        .withType(UMessageType::PUBLISH)
        .withPriority(UPriority::LOW)
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

    
    

    return 0;
}
