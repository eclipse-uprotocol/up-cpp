#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <optional>
#include <variant>
#include <vector>

struct UUID;
struct UStatus;
struct UUri;
struct UAuthority;
struct UEntity;
struct UResource;
struct UUriBatch;
struct UAttributes;
struct CallOptions;
struct UPayload;
struct UMessage;

struct UUID {
    struct Impl;
    std::shared_ptr<Impl> pImpl;

    UUID();
    std::string serialize() const;
    void deserialize(const std::string&);
    std::string to_string() const;

    UUID& msb(const std::uint64_t&);
    std::uint64_t msb() const;

    UUID& lsb(const std::uint64_t&);
    std::uint64_t lsb() const;
};

enum UCode {
    OK = 0,
    CANCELLED = 1,
    UNKNOWN = 2,
    INVALID_ARGUMENT = 3,
    DEADLINE_EXCEEDED = 4,
    NOT_FOUND = 5,
    ALREADY_EXISTS = 6,
    PERMISSION_DENIED = 7,
    UNAUTHENTICATED = 16,
    RESOURCE_EXHAUSTED = 8,
    FAILED_PRECONDITION = 9,
    ABORTED = 10,
    OUT_OF_RANGE = 11,
    UNIMPLEMENTED = 12,
    INTERNAL = 13,
    UNAVAILABLE = 14,
    DATA_LOSS = 15
};

struct UStatus {
    struct Impl;
    std::shared_ptr<Impl> pImpl;

    UStatus();
    std::string serialize() const;
    void deserialize(const std::string&);
    std::string to_string() const;

    UStatus& code(const UCode&);
    UCode code() const;

    UStatus& message(const std::string&);
    std::optional<std::string> message() const;
};

struct UUri {
    struct Impl;
    std::shared_ptr<Impl> pImpl;

    UUri();
    std::string serialize() const;
    void deserialize(const std::string&);
    std::string to_string() const;

    UUri& authority(const UAuthority&);
    UAuthority authority() const;

    UUri& entity(const UEntity&);
    UEntity entity() const;

    UUri& resource(const UResource&);
    UResource resource() const;
};

struct UAuthority {
    struct Impl;
    std::shared_ptr<Impl> pImpl;

    UAuthority();
    std::string serialize() const;
    void deserialize(const std::string&);
    std::string to_string() const;

    UAuthority& name(const std::string&);
    std::optional<std::string> name() const;
};

struct UEntity {
    struct Impl;
    std::shared_ptr<Impl> pImpl;

    UEntity();
    std::string serialize() const;
    void deserialize(const std::string&);
    std::string to_string() const;

    UEntity& name(const std::string&);
    std::string name() const;

    UEntity& id(const std::uint32_t&);
    std::optional<std::uint32_t> id() const;

    UEntity& version_major(const std::uint32_t&);
    std::optional<std::uint32_t> version_major() const;

    UEntity& version_minor(const std::uint32_t&);
    std::optional<std::uint32_t> version_minor() const;
};

struct UResource {
    struct Impl;
    std::shared_ptr<Impl> pImpl;

    UResource();
    std::string serialize() const;
    void deserialize(const std::string&);
    std::string to_string() const;

    UResource& name(const std::string&);
    std::string name() const;

    UResource& instance(const std::string&);
    std::optional<std::string> instance() const;

    UResource& message(const std::string&);
    std::optional<std::string> message() const;

    UResource& id(const std::uint32_t&);
    std::optional<std::uint32_t> id() const;
};

struct UUriBatch {
    struct Impl;
    std::shared_ptr<Impl> pImpl;

    UUriBatch();
    std::string serialize() const;
    void deserialize(const std::string&);
    std::string to_string() const;
};

enum UMessageType {
    UMESSAGE_TYPE_UNSPECIFIED = 0,
    UMESSAGE_TYPE_PUBLISH = 1,
    UMESSAGE_TYPE_REQUEST = 2,
    UMESSAGE_TYPE_RESPONSE = 3,
    UMESSAGE_TYPE_NOTIFICATION = 4
};

enum UPriority {
    UPRIORITY_UNSPECIFIED = 0,
    UPRIORITY_CS0 = 1,
    UPRIORITY_CS1 = 2,
    UPRIORITY_CS2 = 3,
    UPRIORITY_CS3 = 4,
    UPRIORITY_CS4 = 5,
    UPRIORITY_CS5 = 6,
    UPRIORITY_CS6 = 7
};

struct UAttributes {
    struct Impl;
    std::shared_ptr<Impl> pImpl;

    UAttributes();
    std::string serialize() const;
    void deserialize(const std::string&);
    std::string to_string() const;

    UAttributes& id(const UUID&);
    UUID id() const;

    UAttributes& type(const UMessageType&);
    UMessageType type() const;

    UAttributes& source(const UUri&);
    UUri source() const;

    UAttributes& sink(const UUri&);
    UUri sink() const;

    UAttributes& priority(const UPriority&);
    UPriority priority() const;

    UAttributes& ttl(const std::uint32_t&);
    std::optional<std::uint32_t> ttl() const;

    UAttributes& permission_level(const std::uint32_t&);
    std::optional<std::uint32_t> permission_level() const;

    UAttributes& commstatus(const UCode&);
    std::optional<UCode> commstatus() const;

    UAttributes& reqid(const UUID&);
    UUID reqid() const;

    UAttributes& token(const std::string&);
    std::optional<std::string> token() const;

    UAttributes& traceparent(const std::string&);
    std::optional<std::string> traceparent() const;
};

struct CallOptions {
    struct Impl;
    std::shared_ptr<Impl> pImpl;

    CallOptions();
    std::string serialize() const;
    void deserialize(const std::string&);
    std::string to_string() const;

    CallOptions& priority(const UPriority&);
    UPriority priority() const;

    CallOptions& ttl(const std::uint32_t&);
    std::uint32_t ttl() const;

    CallOptions& token(const std::string&);
    std::optional<std::string> token() const;
};

enum UPayloadFormat {
    UPAYLOAD_FORMAT_UNSPECIFIED = 0,
    UPAYLOAD_FORMAT_PROTOBUF_WRAPPED_IN_ANY = 1,
    UPAYLOAD_FORMAT_PROTOBUF = 2,
    UPAYLOAD_FORMAT_JSON = 3,
    UPAYLOAD_FORMAT_SOMEIP = 4,
    UPAYLOAD_FORMAT_SOMEIP_TLV = 5,
    UPAYLOAD_FORMAT_RAW = 6,
    UPAYLOAD_FORMAT_TEXT = 7
};

struct UPayload {
    struct Impl;
    std::shared_ptr<Impl> pImpl;

    UPayload();
    std::string serialize() const;
    void deserialize(const std::string&);
    std::string to_string() const;

    UPayload& data(const std::variant<std::uint64_t,std::basic_string<uint8_t>>&);
    std::variant<std::uint64_t,std::basic_string<uint8_t>> data() const;

    UPayload& length(const std::int32_t&);
    std::optional<std::int32_t> length() const;

    UPayload& format(const UPayloadFormat&);
    UPayloadFormat format() const;
};

struct UMessage {
    struct Impl;
    std::shared_ptr<Impl> pImpl;

    UMessage();
    std::string serialize() const;
    void deserialize(const std::string&);
    std::string to_string() const;

    UMessage& attributes(const UAttributes&);
    UAttributes attributes() const;

    UMessage& payload(const UPayload&);
    UPayload payload() const;
};

