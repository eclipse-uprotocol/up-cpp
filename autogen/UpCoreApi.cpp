#include "UpCoreApi.h"
#include <iostream>

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


template <typename T>
static void trace_outer(const char * func, int lineno, const void* _this, const std::shared_ptr<T>& pImpl)
{
    using namespace std;
    cout << func << " line=" << lineno << " this=" << _this << " pImpl=" << pImpl.get() << '/' << pImpl.use_count() << endl;
}
#define TRACE_OUTER(_this, pImpl) trace_outer(__PRETTY_FUNCTION__, __LINE__, _this, pImpl);


static void trace_impl(const char * func, int lineno, const void* _this, const void* ptr)
{
    using namespace std;
    cout << func << " lineno=" << lineno << " this=" << _this << " ptr=" << ptr << endl;
}
#define TRACE_IMPL(_this, ptr) trace_impl(__PRETTY_FUNCTION__, __LINE__, _this, ptr);


#include "uattributes.pb.h"
#include "umessage.pb.h"
#include "upayload.pb.h"
#include "uri.pb.h"
#include "ustatus.pb.h"
#include "uuid.pb.h"
                         
struct UUID::Impl {
    uprotocol::v1::UUID* ptr;

    inline Impl()
    {
        ptr = new uprotocol::v1::UUID();
        TRACE_IMPL(this, ptr)
    }

    inline ~Impl()
    {
        TRACE_IMPL(this, ptr)
        delete ptr;
        TRACE_IMPL(this, ptr)
    }
};

UUID::UUID() : pImpl(new Impl)
{
    TRACE_OUTER(this, pImpl)
}

UUID::~UUID()
{
    TRACE_OUTER(this, pImpl)
}

std::string UUID::serialize() const
{
    std::string output;
    auto result = pImpl->ptr->SerializeToString(&output);
    return output;
}

void UUID::deserialize(const std::string& data)
{
    pImpl->ptr->ParseFromString(data);
}

std::string UUID::to_string() const
{
    return pImpl->ptr->DebugString();
}

UUID& UUID::msb(const std::uint64_t& arg)
{
    TRACE_OUTER(this, pImpl)
    pImpl->ptr->set_msb(arg);
    return *this;
}

std::uint64_t UUID::msb() const
{
    TRACE_OUTER(this, pImpl)
    return pImpl->ptr->msb();
}


UUID& UUID::lsb(const std::uint64_t& arg)
{
    TRACE_OUTER(this, pImpl)
    pImpl->ptr->set_lsb(arg);
    return *this;
}

std::uint64_t UUID::lsb() const
{
    TRACE_OUTER(this, pImpl)
    return pImpl->ptr->lsb();
}


struct UStatus::Impl {
    uprotocol::v1::UStatus* ptr;

    inline Impl()
    {
        ptr = new uprotocol::v1::UStatus();
        TRACE_IMPL(this, ptr)
    }

    inline ~Impl()
    {
        TRACE_IMPL(this, ptr)
        delete ptr;
        TRACE_IMPL(this, ptr)
    }
};

UStatus::UStatus() : pImpl(new Impl)
{
    TRACE_OUTER(this, pImpl)
}

UStatus::~UStatus()
{
    TRACE_OUTER(this, pImpl)
}

std::string UStatus::serialize() const
{
    std::string output;
    auto result = pImpl->ptr->SerializeToString(&output);
    return output;
}

void UStatus::deserialize(const std::string& data)
{
    pImpl->ptr->ParseFromString(data);
}

std::string UStatus::to_string() const
{
    return pImpl->ptr->DebugString();
}

UStatus& UStatus::code(const UCode& arg)
{
    TRACE_OUTER(this, pImpl)
    pImpl->ptr->set_code(static_cast<uprotocol::v1::UCode>(arg));
    return *this;
}

UCode UStatus::code() const
{
    return static_cast<UCode>(pImpl->ptr->code());
}


UStatus& UStatus::message(const std::string& arg)
{
    TRACE_OUTER(this, pImpl)
    pImpl->ptr->set_message(arg);
    return *this;
}

std::optional<std::string> UStatus::message() const
{
    TRACE_OUTER(this, pImpl)
    if (!pImpl->ptr->has_message()) return std::optional<std::string>();
    return pImpl->ptr->message();
}


struct UUriBatch::Impl {
    uprotocol::v1::UUriBatch* ptr;

    inline Impl()
    {
        ptr = new uprotocol::v1::UUriBatch();
        TRACE_IMPL(this, ptr)
    }

    inline ~Impl()
    {
        TRACE_IMPL(this, ptr)
        delete ptr;
        TRACE_IMPL(this, ptr)
    }
};

UUriBatch::UUriBatch() : pImpl(new Impl)
{
    TRACE_OUTER(this, pImpl)
}

UUriBatch::~UUriBatch()
{
    TRACE_OUTER(this, pImpl)
}

std::string UUriBatch::serialize() const
{
    std::string output;
    auto result = pImpl->ptr->SerializeToString(&output);
    return output;
}

void UUriBatch::deserialize(const std::string& data)
{
    pImpl->ptr->ParseFromString(data);
}

std::string UUriBatch::to_string() const
{
    return pImpl->ptr->DebugString();
}


struct UResource::Impl {
    uprotocol::v1::UResource* ptr;

    inline Impl()
    {
        ptr = new uprotocol::v1::UResource();
        TRACE_IMPL(this, ptr)
    }

    inline ~Impl()
    {
        TRACE_IMPL(this, ptr)
        delete ptr;
        TRACE_IMPL(this, ptr)
    }
};

UResource::UResource() : pImpl(new Impl)
{
    TRACE_OUTER(this, pImpl)
}

UResource::~UResource()
{
    TRACE_OUTER(this, pImpl)
}

std::string UResource::serialize() const
{
    std::string output;
    auto result = pImpl->ptr->SerializeToString(&output);
    return output;
}

void UResource::deserialize(const std::string& data)
{
    pImpl->ptr->ParseFromString(data);
}

std::string UResource::to_string() const
{
    return pImpl->ptr->DebugString();
}

UResource& UResource::name(const std::string& arg)
{
    TRACE_OUTER(this, pImpl)
    pImpl->ptr->set_name(arg);
    return *this;
}

std::string UResource::name() const
{
    TRACE_OUTER(this, pImpl)
    return pImpl->ptr->name();
}


UResource& UResource::instance(const std::string& arg)
{
    TRACE_OUTER(this, pImpl)
    pImpl->ptr->set_instance(arg);
    return *this;
}

std::optional<std::string> UResource::instance() const
{
    TRACE_OUTER(this, pImpl)
    if (!pImpl->ptr->has_instance()) return std::optional<std::string>();
    return pImpl->ptr->instance();
}


UResource& UResource::message(const std::string& arg)
{
    TRACE_OUTER(this, pImpl)
    pImpl->ptr->set_message(arg);
    return *this;
}

std::optional<std::string> UResource::message() const
{
    TRACE_OUTER(this, pImpl)
    if (!pImpl->ptr->has_message()) return std::optional<std::string>();
    return pImpl->ptr->message();
}


UResource& UResource::id(const std::uint32_t& arg)
{
    TRACE_OUTER(this, pImpl)
    pImpl->ptr->set_id(arg);
    return *this;
}

std::optional<std::uint32_t> UResource::id() const
{
    TRACE_OUTER(this, pImpl)
    if (!pImpl->ptr->has_id()) return std::optional<std::uint32_t>();
    return pImpl->ptr->id();
}


struct UEntity::Impl {
    uprotocol::v1::UEntity* ptr;

    inline Impl()
    {
        ptr = new uprotocol::v1::UEntity();
        TRACE_IMPL(this, ptr)
    }

    inline ~Impl()
    {
        TRACE_IMPL(this, ptr)
        delete ptr;
        TRACE_IMPL(this, ptr)
    }
};

UEntity::UEntity() : pImpl(new Impl)
{
    TRACE_OUTER(this, pImpl)
}

UEntity::~UEntity()
{
    TRACE_OUTER(this, pImpl)
}

std::string UEntity::serialize() const
{
    std::string output;
    auto result = pImpl->ptr->SerializeToString(&output);
    return output;
}

void UEntity::deserialize(const std::string& data)
{
    pImpl->ptr->ParseFromString(data);
}

std::string UEntity::to_string() const
{
    return pImpl->ptr->DebugString();
}

UEntity& UEntity::name(const std::string& arg)
{
    TRACE_OUTER(this, pImpl)
    pImpl->ptr->set_name(arg);
    return *this;
}

std::string UEntity::name() const
{
    TRACE_OUTER(this, pImpl)
    return pImpl->ptr->name();
}


UEntity& UEntity::id(const std::uint32_t& arg)
{
    TRACE_OUTER(this, pImpl)
    pImpl->ptr->set_id(arg);
    return *this;
}

std::optional<std::uint32_t> UEntity::id() const
{
    TRACE_OUTER(this, pImpl)
    if (!pImpl->ptr->has_id()) return std::optional<std::uint32_t>();
    return pImpl->ptr->id();
}


UEntity& UEntity::version_major(const std::uint32_t& arg)
{
    TRACE_OUTER(this, pImpl)
    pImpl->ptr->set_version_major(arg);
    return *this;
}

std::optional<std::uint32_t> UEntity::version_major() const
{
    TRACE_OUTER(this, pImpl)
    if (!pImpl->ptr->has_version_major()) return std::optional<std::uint32_t>();
    return pImpl->ptr->version_major();
}


UEntity& UEntity::version_minor(const std::uint32_t& arg)
{
    TRACE_OUTER(this, pImpl)
    pImpl->ptr->set_version_minor(arg);
    return *this;
}

std::optional<std::uint32_t> UEntity::version_minor() const
{
    TRACE_OUTER(this, pImpl)
    if (!pImpl->ptr->has_version_minor()) return std::optional<std::uint32_t>();
    return pImpl->ptr->version_minor();
}


struct UAuthority::Impl {
    uprotocol::v1::UAuthority* ptr;

    inline Impl()
    {
        ptr = new uprotocol::v1::UAuthority();
        TRACE_IMPL(this, ptr)
    }

    inline ~Impl()
    {
        TRACE_IMPL(this, ptr)
        delete ptr;
        TRACE_IMPL(this, ptr)
    }
};

UAuthority::UAuthority() : pImpl(new Impl)
{
    TRACE_OUTER(this, pImpl)
}

UAuthority::~UAuthority()
{
    TRACE_OUTER(this, pImpl)
}

std::string UAuthority::serialize() const
{
    std::string output;
    auto result = pImpl->ptr->SerializeToString(&output);
    return output;
}

void UAuthority::deserialize(const std::string& data)
{
    pImpl->ptr->ParseFromString(data);
}

std::string UAuthority::to_string() const
{
    return pImpl->ptr->DebugString();
}

UAuthority& UAuthority::name(const std::string& arg)
{
    TRACE_OUTER(this, pImpl)
    pImpl->ptr->set_name(arg);
    return *this;
}

std::optional<std::string> UAuthority::name() const
{
    TRACE_OUTER(this, pImpl)
    if (!pImpl->ptr->has_name()) return std::optional<std::string>();
    return pImpl->ptr->name();
}



struct UUri::Impl {
    uprotocol::v1::UUri* ptr;

    inline Impl()
    {
        ptr = new uprotocol::v1::UUri();
        TRACE_IMPL(this, ptr)
    }

    inline ~Impl()
    {
        TRACE_IMPL(this, ptr)
        delete ptr;
        TRACE_IMPL(this, ptr)
    }
};

UUri::UUri() : pImpl(new Impl)
{
    TRACE_OUTER(this, pImpl)
}

UUri::~UUri()
{
    TRACE_OUTER(this, pImpl)
}

std::string UUri::serialize() const
{
    std::string output;
    auto result = pImpl->ptr->SerializeToString(&output);
    return output;
}

void UUri::deserialize(const std::string& data)
{
    pImpl->ptr->ParseFromString(data);
}

std::string UUri::to_string() const
{
    return pImpl->ptr->DebugString();
}

UUri& UUri::authority(const UAuthority& arg)
{
    TRACE_OUTER(this, pImpl)
    pImpl->ptr->set_allocated_authority(arg.pImpl->ptr);
    return *this;
}

UAuthority UUri::authority() const
{
    TRACE_OUTER(this, pImpl)
    UAuthority ret;
    *(ret.pImpl->ptr) = pImpl->ptr->authority();
    return ret;
}


UUri& UUri::entity(const UEntity& arg)
{
    TRACE_OUTER(this, pImpl)
    pImpl->ptr->set_allocated_entity(arg.pImpl->ptr);
    return *this;
}

UEntity UUri::entity() const
{
    TRACE_OUTER(this, pImpl)
    UEntity ret;
    *(ret.pImpl->ptr) = pImpl->ptr->entity();
    return ret;
}


UUri& UUri::resource(const UResource& arg)
{
    TRACE_OUTER(this, pImpl)
    pImpl->ptr->set_allocated_resource(arg.pImpl->ptr);
    return *this;
}

UResource UUri::resource() const
{
    TRACE_OUTER(this, pImpl)
    UResource ret;
    *(ret.pImpl->ptr) = pImpl->ptr->resource();
    return ret;
}


struct CallOptions::Impl {
    uprotocol::v1::CallOptions* ptr;

    inline Impl()
    {
        ptr = new uprotocol::v1::CallOptions();
        TRACE_IMPL(this, ptr)
    }

    inline ~Impl()
    {
        TRACE_IMPL(this, ptr)
        delete ptr;
        TRACE_IMPL(this, ptr)
    }
};

CallOptions::CallOptions() : pImpl(new Impl)
{
    TRACE_OUTER(this, pImpl)
}

CallOptions::~CallOptions()
{
    TRACE_OUTER(this, pImpl)
}

std::string CallOptions::serialize() const
{
    std::string output;
    auto result = pImpl->ptr->SerializeToString(&output);
    return output;
}

void CallOptions::deserialize(const std::string& data)
{
    pImpl->ptr->ParseFromString(data);
}

std::string CallOptions::to_string() const
{
    return pImpl->ptr->DebugString();
}

CallOptions& CallOptions::priority(const UPriority& arg)
{
    TRACE_OUTER(this, pImpl)
    pImpl->ptr->set_priority(static_cast<uprotocol::v1::UPriority>(arg));
    return *this;
}

UPriority CallOptions::priority() const
{
    return static_cast<UPriority>(pImpl->ptr->priority());
}


CallOptions& CallOptions::ttl(const std::uint32_t& arg)
{
    TRACE_OUTER(this, pImpl)
    pImpl->ptr->set_ttl(arg);
    return *this;
}

std::uint32_t CallOptions::ttl() const
{
    TRACE_OUTER(this, pImpl)
    return pImpl->ptr->ttl();
}


CallOptions& CallOptions::token(const std::string& arg)
{
    TRACE_OUTER(this, pImpl)
    pImpl->ptr->set_token(arg);
    return *this;
}

std::optional<std::string> CallOptions::token() const
{
    TRACE_OUTER(this, pImpl)
    if (!pImpl->ptr->has_token()) return std::optional<std::string>();
    return pImpl->ptr->token();
}


struct UAttributes::Impl {
    uprotocol::v1::UAttributes* ptr;

    inline Impl()
    {
        ptr = new uprotocol::v1::UAttributes();
        TRACE_IMPL(this, ptr)
    }

    inline ~Impl()
    {
        TRACE_IMPL(this, ptr)
        delete ptr;
        TRACE_IMPL(this, ptr)
    }
};

UAttributes::UAttributes() : pImpl(new Impl)
{
    TRACE_OUTER(this, pImpl)
}

UAttributes::~UAttributes()
{
    TRACE_OUTER(this, pImpl)
}

std::string UAttributes::serialize() const
{
    std::string output;
    auto result = pImpl->ptr->SerializeToString(&output);
    return output;
}

void UAttributes::deserialize(const std::string& data)
{
    pImpl->ptr->ParseFromString(data);
}

std::string UAttributes::to_string() const
{
    return pImpl->ptr->DebugString();
}

UAttributes& UAttributes::id(const UUID& arg)
{
    TRACE_OUTER(this, pImpl)
    pImpl->ptr->set_allocated_id(arg.pImpl->ptr);
    return *this;
}

UUID UAttributes::id() const
{
    TRACE_OUTER(this, pImpl)
    UUID ret;
    *(ret.pImpl->ptr) = pImpl->ptr->id();
    return ret;
}


UAttributes& UAttributes::type(const UMessageType& arg)
{
    TRACE_OUTER(this, pImpl)
    pImpl->ptr->set_type(static_cast<uprotocol::v1::UMessageType>(arg));
    return *this;
}

UMessageType UAttributes::type() const
{
    return static_cast<UMessageType>(pImpl->ptr->type());
}


UAttributes& UAttributes::source(const UUri& arg)
{
    TRACE_OUTER(this, pImpl)
    pImpl->ptr->set_allocated_source(arg.pImpl->ptr);
    return *this;
}

UUri UAttributes::source() const
{
    TRACE_OUTER(this, pImpl)
    UUri ret;
    *(ret.pImpl->ptr) = pImpl->ptr->source();
    return ret;
}


UAttributes& UAttributes::sink(const UUri& arg)
{
    TRACE_OUTER(this, pImpl)
    pImpl->ptr->set_allocated_sink(arg.pImpl->ptr);
    return *this;
}

UUri UAttributes::sink() const
{
    TRACE_OUTER(this, pImpl)
    UUri ret;
    *(ret.pImpl->ptr) = pImpl->ptr->sink();
    return ret;
}


UAttributes& UAttributes::priority(const UPriority& arg)
{
    TRACE_OUTER(this, pImpl)
    pImpl->ptr->set_priority(static_cast<uprotocol::v1::UPriority>(arg));
    return *this;
}

UPriority UAttributes::priority() const
{
    return static_cast<UPriority>(pImpl->ptr->priority());
}


UAttributes& UAttributes::ttl(const std::uint32_t& arg)
{
    TRACE_OUTER(this, pImpl)
    pImpl->ptr->set_ttl(arg);
    return *this;
}

std::optional<std::uint32_t> UAttributes::ttl() const
{
    TRACE_OUTER(this, pImpl)
    if (!pImpl->ptr->has_ttl()) return std::optional<std::uint32_t>();
    return pImpl->ptr->ttl();
}


UAttributes& UAttributes::permission_level(const std::uint32_t& arg)
{
    TRACE_OUTER(this, pImpl)
    pImpl->ptr->set_permission_level(arg);
    return *this;
}

std::optional<std::uint32_t> UAttributes::permission_level() const
{
    TRACE_OUTER(this, pImpl)
    if (!pImpl->ptr->has_permission_level()) return std::optional<std::uint32_t>();
    return pImpl->ptr->permission_level();
}


UAttributes& UAttributes::commstatus(const UCode& arg)
{
    TRACE_OUTER(this, pImpl)
    pImpl->ptr->set_commstatus(static_cast<uprotocol::v1::UCode>(arg));
    return *this;
}

std::optional<UCode> UAttributes::commstatus() const
{
    TRACE_OUTER(this, pImpl)
    if (!pImpl->ptr->has_commstatus()) return std::optional<UCode>();
    return static_cast<UCode>(pImpl->ptr->commstatus());
}


UAttributes& UAttributes::reqid(const UUID& arg)
{
    TRACE_OUTER(this, pImpl)
    pImpl->ptr->set_allocated_reqid(arg.pImpl->ptr);
    return *this;
}

UUID UAttributes::reqid() const
{
    TRACE_OUTER(this, pImpl)
    UUID ret;
    *(ret.pImpl->ptr) = pImpl->ptr->reqid();
    return ret;
}


UAttributes& UAttributes::token(const std::string& arg)
{
    TRACE_OUTER(this, pImpl)
    pImpl->ptr->set_token(arg);
    return *this;
}

std::optional<std::string> UAttributes::token() const
{
    TRACE_OUTER(this, pImpl)
    if (!pImpl->ptr->has_token()) return std::optional<std::string>();
    return pImpl->ptr->token();
}


UAttributes& UAttributes::traceparent(const std::string& arg)
{
    TRACE_OUTER(this, pImpl)
    pImpl->ptr->set_traceparent(arg);
    return *this;
}

std::optional<std::string> UAttributes::traceparent() const
{
    TRACE_OUTER(this, pImpl)
    if (!pImpl->ptr->has_traceparent()) return std::optional<std::string>();
    return pImpl->ptr->traceparent();
}


struct UPayload::Impl {
    uprotocol::v1::UPayload* ptr;

    inline Impl()
    {
        ptr = new uprotocol::v1::UPayload();
        TRACE_IMPL(this, ptr)
    }

    inline ~Impl()
    {
        TRACE_IMPL(this, ptr)
        delete ptr;
        TRACE_IMPL(this, ptr)
    }
};

UPayload::UPayload() : pImpl(new Impl)
{
    TRACE_OUTER(this, pImpl)
}

UPayload::~UPayload()
{
    TRACE_OUTER(this, pImpl)
}

std::string UPayload::serialize() const
{
    std::string output;
    auto result = pImpl->ptr->SerializeToString(&output);
    return output;
}

void UPayload::deserialize(const std::string& data)
{
    pImpl->ptr->ParseFromString(data);
}

std::string UPayload::to_string() const
{
    return pImpl->ptr->DebugString();
}

UPayload& UPayload::data(const std::variant<std::uint64_t, std::basic_string<uint8_t>>& arg)
{
    TRACE_OUTER(this, pImpl)
    generic_set_check<std::uint64_t>(arg, [&](auto larg){ pImpl->ptr->set_reference(larg); });
    generic_set_check<std::basic_string<uint8_t>>(arg, [&](auto larg){ pImpl->ptr->set_value(larg); });

    return *this;
}

std::variant<std::uint64_t, std::basic_string<uint8_t>> UPayload::data() const
{
    TRACE_OUTER(this, pImpl)
    if (pImpl->ptr->has_reference()) return generic_get_convert<std::uint64_t>(pImpl->ptr->reference());
    if (pImpl->ptr->has_value()) return generic_get_convert<std::basic_string<uint8_t>>(pImpl->ptr->value());
    return pImpl->ptr->reference();

}


UPayload& UPayload::length(const std::int32_t& arg)
{
    TRACE_OUTER(this, pImpl)
    pImpl->ptr->set_length(arg);
    return *this;
}

std::optional<std::int32_t> UPayload::length() const
{
    TRACE_OUTER(this, pImpl)
    if (!pImpl->ptr->has_length()) return std::optional<std::int32_t>();
    return pImpl->ptr->length();
}


UPayload& UPayload::format(const UPayloadFormat& arg)
{
    TRACE_OUTER(this, pImpl)
    pImpl->ptr->set_format(static_cast<uprotocol::v1::UPayloadFormat>(arg));
    return *this;
}

UPayloadFormat UPayload::format() const
{
    return static_cast<UPayloadFormat>(pImpl->ptr->format());
}


struct UMessage::Impl {
    uprotocol::v1::UMessage* ptr;

    inline Impl()
    {
        ptr = new uprotocol::v1::UMessage();
        TRACE_IMPL(this, ptr)
    }

    inline ~Impl()
    {
        TRACE_IMPL(this, ptr)
        delete ptr;
        TRACE_IMPL(this, ptr)
    }
};

UMessage::UMessage() : pImpl(new Impl)
{
    TRACE_OUTER(this, pImpl)
}

UMessage::~UMessage()
{
    TRACE_OUTER(this, pImpl)
}

std::string UMessage::serialize() const
{
    std::string output;
    auto result = pImpl->ptr->SerializeToString(&output);
    return output;
}

void UMessage::deserialize(const std::string& data)
{
    pImpl->ptr->ParseFromString(data);
}

std::string UMessage::to_string() const
{
    return pImpl->ptr->DebugString();
}

UMessage& UMessage::attributes(const UAttributes& arg)
{
    TRACE_OUTER(this, pImpl)
    pImpl->ptr->set_allocated_attributes(arg.pImpl->ptr);
    return *this;
}

UAttributes UMessage::attributes() const
{
    TRACE_OUTER(this, pImpl)
    UAttributes ret;
    *(ret.pImpl->ptr) = pImpl->ptr->attributes();
    return ret;
}


UMessage& UMessage::payload(const UPayload& arg)
{
    TRACE_OUTER(this, pImpl)
    pImpl->ptr->set_allocated_payload(arg.pImpl->ptr);
    return *this;
}

UPayload UMessage::payload() const
{
    TRACE_OUTER(this, pImpl)
    UPayload ret;
    *(ret.pImpl->ptr) = pImpl->ptr->payload();
    return ret;
}


