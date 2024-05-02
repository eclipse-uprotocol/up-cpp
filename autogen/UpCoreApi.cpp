#include "UpCoreApi.h"

template <typename T, typename FN, class... VAR_TYPES>
void generic_set_check(const std::variant<VAR_TYPES...>& v, FN&& fn)
{
    if constexpr (std::is_same<T, std::basic_string<uint8_t>>::value) {
        if (!std::holds_alternative<std::basic_string<uint8_t>>(v)) return;
        auto value = std::get<std::basic_string<uint8_t>>(v);
        const char* ptr = (const char*)value.data();
        std::string sv(ptr, ptr+value.size());
        fn(sv);
    }
    else {
        if (!std::holds_alternative<T>(v)) return;
        fn(std::get<T>(v));
    }
}

template <typename T, typename V>
T generic_get_convert(const V& value)
{
    if constexpr (std::is_same<V, std::string>::value && std::is_same<T, std::basic_string<uint8_t>>::value) {
        auto ptr = (const uint8_t*)value.data();
        return std::basic_string<uint8_t>(ptr, ptr + value.size());
    }
    else {
        return value;
    }
}

#include "uattributes.pb.h"
#include "umessage.pb.h"
#include "upayload.pb.h"
#include "uri.pb.h"
#include "ustatus.pb.h"
#include "uuid.pb.h"
                         
struct UUID::Impl {
    uprotocol::v1::UUID* ptr;
    inline Impl() { ptr = new uprotocol::v1::UUID(); }
    inline ~Impl() { delete ptr; }
};

UUID::UUID() : pImpl(new Impl) {}

std::string UUID::serialize() const
{
    std::string output;
    auto result = pImpl->ptr->SerializeToString(&output);
    return output;
}

void UUID::deserialize(const std::string& data) { pImpl->ptr->ParseFromString(data); }

std::string UUID::to_string() const { return pImpl->ptr->DebugString(); }

UUID& UUID::msb(const std::uint64_t& arg)
{
    pImpl->ptr->set_msb(arg);
    return *this;
}

std::uint64_t UUID::msb() const
{
    return pImpl->ptr->msb();
}


UUID& UUID::lsb(const std::uint64_t& arg)
{
    pImpl->ptr->set_lsb(arg);
    return *this;
}

std::uint64_t UUID::lsb() const
{
    return pImpl->ptr->lsb();
}


struct UStatus::Impl {
    uprotocol::v1::UStatus* ptr;
    inline Impl() { ptr = new uprotocol::v1::UStatus(); }
    inline ~Impl() { delete ptr; }
};

UStatus::UStatus() : pImpl(new Impl) {}

std::string UStatus::serialize() const
{
    std::string output;
    auto result = pImpl->ptr->SerializeToString(&output);
    return output;
}

void UStatus::deserialize(const std::string& data) { pImpl->ptr->ParseFromString(data); }

std::string UStatus::to_string() const { return pImpl->ptr->DebugString(); }

UStatus& UStatus::code(const UCode& arg)
{
    pImpl->ptr->set_code(static_cast<uprotocol::v1::UCode>(arg));
    return *this;
}

UCode UStatus::code() const
{
    return static_cast<UCode>(pImpl->ptr->code());
}


UStatus& UStatus::message(const std::string& arg)
{
    pImpl->ptr->set_message(arg);
    return *this;
}

std::optional<std::string> UStatus::message() const
{
    if (!pImpl->ptr->has_message()) return std::optional<std::string>();
    return pImpl->ptr->message();
}


struct UUriBatch::Impl {
    uprotocol::v1::UUriBatch* ptr;
    inline Impl() { ptr = new uprotocol::v1::UUriBatch(); }
    inline ~Impl() { delete ptr; }
};

UUriBatch::UUriBatch() : pImpl(new Impl) {}

std::string UUriBatch::serialize() const
{
    std::string output;
    auto result = pImpl->ptr->SerializeToString(&output);
    return output;
}

void UUriBatch::deserialize(const std::string& data) { pImpl->ptr->ParseFromString(data); }

std::string UUriBatch::to_string() const { return pImpl->ptr->DebugString(); }


struct UResource::Impl {
    uprotocol::v1::UResource* ptr;
    inline Impl() { ptr = new uprotocol::v1::UResource(); }
    inline ~Impl() { delete ptr; }
};

UResource::UResource() : pImpl(new Impl) {}

std::string UResource::serialize() const
{
    std::string output;
    auto result = pImpl->ptr->SerializeToString(&output);
    return output;
}

void UResource::deserialize(const std::string& data) { pImpl->ptr->ParseFromString(data); }

std::string UResource::to_string() const { return pImpl->ptr->DebugString(); }

UResource& UResource::name(const std::string& arg)
{
    pImpl->ptr->set_name(arg);
    return *this;
}

std::string UResource::name() const
{
    return pImpl->ptr->name();
}


UResource& UResource::instance(const std::string& arg)
{
    pImpl->ptr->set_instance(arg);
    return *this;
}

std::optional<std::string> UResource::instance() const
{
    if (!pImpl->ptr->has_instance()) return std::optional<std::string>();
    return pImpl->ptr->instance();
}


UResource& UResource::message(const std::string& arg)
{
    pImpl->ptr->set_message(arg);
    return *this;
}

std::optional<std::string> UResource::message() const
{
    if (!pImpl->ptr->has_message()) return std::optional<std::string>();
    return pImpl->ptr->message();
}


UResource& UResource::id(const std::uint32_t& arg)
{
    pImpl->ptr->set_id(arg);
    return *this;
}

std::optional<std::uint32_t> UResource::id() const
{
    if (!pImpl->ptr->has_id()) return std::optional<std::uint32_t>();
    return pImpl->ptr->id();
}


struct UEntity::Impl {
    uprotocol::v1::UEntity* ptr;
    inline Impl() { ptr = new uprotocol::v1::UEntity(); }
    inline ~Impl() { delete ptr; }
};

UEntity::UEntity() : pImpl(new Impl) {}

std::string UEntity::serialize() const
{
    std::string output;
    auto result = pImpl->ptr->SerializeToString(&output);
    return output;
}

void UEntity::deserialize(const std::string& data) { pImpl->ptr->ParseFromString(data); }

std::string UEntity::to_string() const { return pImpl->ptr->DebugString(); }

UEntity& UEntity::name(const std::string& arg)
{
    pImpl->ptr->set_name(arg);
    return *this;
}

std::string UEntity::name() const
{
    return pImpl->ptr->name();
}


UEntity& UEntity::id(const std::uint32_t& arg)
{
    pImpl->ptr->set_id(arg);
    return *this;
}

std::optional<std::uint32_t> UEntity::id() const
{
    if (!pImpl->ptr->has_id()) return std::optional<std::uint32_t>();
    return pImpl->ptr->id();
}


UEntity& UEntity::version_major(const std::uint32_t& arg)
{
    pImpl->ptr->set_version_major(arg);
    return *this;
}

std::optional<std::uint32_t> UEntity::version_major() const
{
    if (!pImpl->ptr->has_version_major()) return std::optional<std::uint32_t>();
    return pImpl->ptr->version_major();
}


UEntity& UEntity::version_minor(const std::uint32_t& arg)
{
    pImpl->ptr->set_version_minor(arg);
    return *this;
}

std::optional<std::uint32_t> UEntity::version_minor() const
{
    if (!pImpl->ptr->has_version_minor()) return std::optional<std::uint32_t>();
    return pImpl->ptr->version_minor();
}


struct UAuthority::Impl {
    uprotocol::v1::UAuthority* ptr;
    inline Impl() { ptr = new uprotocol::v1::UAuthority(); }
    inline ~Impl() { delete ptr; }
};

UAuthority::UAuthority() : pImpl(new Impl) {}

std::string UAuthority::serialize() const
{
    std::string output;
    auto result = pImpl->ptr->SerializeToString(&output);
    return output;
}

void UAuthority::deserialize(const std::string& data) { pImpl->ptr->ParseFromString(data); }

std::string UAuthority::to_string() const { return pImpl->ptr->DebugString(); }

UAuthority& UAuthority::name(const std::string& arg)
{
    pImpl->ptr->set_name(arg);
    return *this;
}

std::optional<std::string> UAuthority::name() const
{
    if (!pImpl->ptr->has_name()) return std::optional<std::string>();
    return pImpl->ptr->name();
}



struct UUri::Impl {
    uprotocol::v1::UUri* ptr;
    inline Impl() { ptr = new uprotocol::v1::UUri(); }
    inline ~Impl() { delete ptr; }
};

UUri::UUri() : pImpl(new Impl) {}

std::string UUri::serialize() const
{
    std::string output;
    auto result = pImpl->ptr->SerializeToString(&output);
    return output;
}

void UUri::deserialize(const std::string& data) { pImpl->ptr->ParseFromString(data); }

std::string UUri::to_string() const { return pImpl->ptr->DebugString(); }

UUri& UUri::authority(const UAuthority& arg)
{
    pImpl->ptr->set_allocated_authority(arg.pImpl->ptr);
    return *this;
}

UAuthority UUri::authority() const
{
    UAuthority ret;
    *(ret.pImpl->ptr) = pImpl->ptr->authority();
    return ret;
}


UUri& UUri::entity(const UEntity& arg)
{
    pImpl->ptr->set_allocated_entity(arg.pImpl->ptr);
    return *this;
}

UEntity UUri::entity() const
{
    UEntity ret;
    *(ret.pImpl->ptr) = pImpl->ptr->entity();
    return ret;
}


UUri& UUri::resource(const UResource& arg)
{
    pImpl->ptr->set_allocated_resource(arg.pImpl->ptr);
    return *this;
}

UResource UUri::resource() const
{
    UResource ret;
    *(ret.pImpl->ptr) = pImpl->ptr->resource();
    return ret;
}


struct CallOptions::Impl {
    uprotocol::v1::CallOptions* ptr;
    inline Impl() { ptr = new uprotocol::v1::CallOptions(); }
    inline ~Impl() { delete ptr; }
};

CallOptions::CallOptions() : pImpl(new Impl) {}

std::string CallOptions::serialize() const
{
    std::string output;
    auto result = pImpl->ptr->SerializeToString(&output);
    return output;
}

void CallOptions::deserialize(const std::string& data) { pImpl->ptr->ParseFromString(data); }

std::string CallOptions::to_string() const { return pImpl->ptr->DebugString(); }

CallOptions& CallOptions::priority(const UPriority& arg)
{
    pImpl->ptr->set_priority(static_cast<uprotocol::v1::UPriority>(arg));
    return *this;
}

UPriority CallOptions::priority() const
{
    return static_cast<UPriority>(pImpl->ptr->priority());
}


CallOptions& CallOptions::ttl(const std::uint32_t& arg)
{
    pImpl->ptr->set_ttl(arg);
    return *this;
}

std::uint32_t CallOptions::ttl() const
{
    return pImpl->ptr->ttl();
}


CallOptions& CallOptions::token(const std::string& arg)
{
    pImpl->ptr->set_token(arg);
    return *this;
}

std::optional<std::string> CallOptions::token() const
{
    if (!pImpl->ptr->has_token()) return std::optional<std::string>();
    return pImpl->ptr->token();
}


struct UAttributes::Impl {
    uprotocol::v1::UAttributes* ptr;
    inline Impl() { ptr = new uprotocol::v1::UAttributes(); }
    inline ~Impl() { delete ptr; }
};

UAttributes::UAttributes() : pImpl(new Impl) {}

std::string UAttributes::serialize() const
{
    std::string output;
    auto result = pImpl->ptr->SerializeToString(&output);
    return output;
}

void UAttributes::deserialize(const std::string& data) { pImpl->ptr->ParseFromString(data); }

std::string UAttributes::to_string() const { return pImpl->ptr->DebugString(); }

UAttributes& UAttributes::id(const UUID& arg)
{
    pImpl->ptr->set_allocated_id(arg.pImpl->ptr);
    return *this;
}

UUID UAttributes::id() const
{
    UUID ret;
    *(ret.pImpl->ptr) = pImpl->ptr->id();
    return ret;
}


UAttributes& UAttributes::type(const UMessageType& arg)
{
    pImpl->ptr->set_type(static_cast<uprotocol::v1::UMessageType>(arg));
    return *this;
}

UMessageType UAttributes::type() const
{
    return static_cast<UMessageType>(pImpl->ptr->type());
}


UAttributes& UAttributes::source(const UUri& arg)
{
    pImpl->ptr->set_allocated_source(arg.pImpl->ptr);
    return *this;
}

UUri UAttributes::source() const
{
    UUri ret;
    *(ret.pImpl->ptr) = pImpl->ptr->source();
    return ret;
}


UAttributes& UAttributes::sink(const UUri& arg)
{
    pImpl->ptr->set_allocated_sink(arg.pImpl->ptr);
    return *this;
}

UUri UAttributes::sink() const
{
    UUri ret;
    *(ret.pImpl->ptr) = pImpl->ptr->sink();
    return ret;
}


UAttributes& UAttributes::priority(const UPriority& arg)
{
    pImpl->ptr->set_priority(static_cast<uprotocol::v1::UPriority>(arg));
    return *this;
}

UPriority UAttributes::priority() const
{
    return static_cast<UPriority>(pImpl->ptr->priority());
}


UAttributes& UAttributes::ttl(const std::uint32_t& arg)
{
    pImpl->ptr->set_ttl(arg);
    return *this;
}

std::optional<std::uint32_t> UAttributes::ttl() const
{
    if (!pImpl->ptr->has_ttl()) return std::optional<std::uint32_t>();
    return pImpl->ptr->ttl();
}


UAttributes& UAttributes::permission_level(const std::uint32_t& arg)
{
    pImpl->ptr->set_permission_level(arg);
    return *this;
}

std::optional<std::uint32_t> UAttributes::permission_level() const
{
    if (!pImpl->ptr->has_permission_level()) return std::optional<std::uint32_t>();
    return pImpl->ptr->permission_level();
}


UAttributes& UAttributes::commstatus(const UCode& arg)
{
    pImpl->ptr->set_commstatus(static_cast<uprotocol::v1::UCode>(arg));
    return *this;
}

std::optional<UCode> UAttributes::commstatus() const
{
    if (!pImpl->ptr->has_commstatus()) return std::optional<UCode>();
    return static_cast<UCode>(pImpl->ptr->commstatus());
}


UAttributes& UAttributes::reqid(const UUID& arg)
{
    pImpl->ptr->set_allocated_reqid(arg.pImpl->ptr);
    return *this;
}

UUID UAttributes::reqid() const
{
    UUID ret;
    *(ret.pImpl->ptr) = pImpl->ptr->reqid();
    return ret;
}


UAttributes& UAttributes::token(const std::string& arg)
{
    pImpl->ptr->set_token(arg);
    return *this;
}

std::optional<std::string> UAttributes::token() const
{
    if (!pImpl->ptr->has_token()) return std::optional<std::string>();
    return pImpl->ptr->token();
}


UAttributes& UAttributes::traceparent(const std::string& arg)
{
    pImpl->ptr->set_traceparent(arg);
    return *this;
}

std::optional<std::string> UAttributes::traceparent() const
{
    if (!pImpl->ptr->has_traceparent()) return std::optional<std::string>();
    return pImpl->ptr->traceparent();
}


struct UPayload::Impl {
    uprotocol::v1::UPayload* ptr;
    inline Impl() { ptr = new uprotocol::v1::UPayload(); }
    inline ~Impl() { delete ptr; }
};

UPayload::UPayload() : pImpl(new Impl) {}

std::string UPayload::serialize() const
{
    std::string output;
    auto result = pImpl->ptr->SerializeToString(&output);
    return output;
}

void UPayload::deserialize(const std::string& data) { pImpl->ptr->ParseFromString(data); }

std::string UPayload::to_string() const { return pImpl->ptr->DebugString(); }

UPayload& UPayload::data(const std::variant<std::uint64_t, std::basic_string<uint8_t>>& arg)
{
    generic_set_check<std::uint64_t>(arg, [&](auto larg){ pImpl->ptr->set_reference(larg); });
    generic_set_check<std::basic_string<uint8_t>>(arg, [&](auto larg){ pImpl->ptr->set_value(larg); });

    return *this;
}

std::variant<std::uint64_t, std::basic_string<uint8_t>> UPayload::data() const
{
    if (pImpl->ptr->has_reference()) return generic_get_convert<std::uint64_t>(pImpl->ptr->reference());
    if (pImpl->ptr->has_value()) return generic_get_convert<std::basic_string<uint8_t>>(pImpl->ptr->value());
    return pImpl->ptr->reference();

}


UPayload& UPayload::length(const std::int32_t& arg)
{
    pImpl->ptr->set_length(arg);
    return *this;
}

std::optional<std::int32_t> UPayload::length() const
{
    if (!pImpl->ptr->has_length()) return std::optional<std::int32_t>();
    return pImpl->ptr->length();
}


UPayload& UPayload::format(const UPayloadFormat& arg)
{
    pImpl->ptr->set_format(static_cast<uprotocol::v1::UPayloadFormat>(arg));
    return *this;
}

UPayloadFormat UPayload::format() const
{
    return static_cast<UPayloadFormat>(pImpl->ptr->format());
}


struct UMessage::Impl {
    uprotocol::v1::UMessage* ptr;
    inline Impl() { ptr = new uprotocol::v1::UMessage(); }
    inline ~Impl() { delete ptr; }
};

UMessage::UMessage() : pImpl(new Impl) {}

std::string UMessage::serialize() const
{
    std::string output;
    auto result = pImpl->ptr->SerializeToString(&output);
    return output;
}

void UMessage::deserialize(const std::string& data) { pImpl->ptr->ParseFromString(data); }

std::string UMessage::to_string() const { return pImpl->ptr->DebugString(); }

UMessage& UMessage::attributes(const UAttributes& arg)
{
    pImpl->ptr->set_allocated_attributes(arg.pImpl->ptr);
    return *this;
}

UAttributes UMessage::attributes() const
{
    UAttributes ret;
    *(ret.pImpl->ptr) = pImpl->ptr->attributes();
    return ret;
}


UMessage& UMessage::payload(const UPayload& arg)
{
    pImpl->ptr->set_allocated_payload(arg.pImpl->ptr);
    return *this;
}

UPayload UMessage::payload() const
{
    UPayload ret;
    *(ret.pImpl->ptr) = pImpl->ptr->payload();
    return ret;
}


