// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: uattributes.proto

#ifndef GOOGLE_PROTOBUF_INCLUDED_uattributes_2eproto
#define GOOGLE_PROTOBUF_INCLUDED_uattributes_2eproto

#include <limits>
#include <string>

#include <google/protobuf/port_def.inc>
#if PROTOBUF_VERSION < 3021000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers. Please update
#error your headers.
#endif
#if 3021012 < PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers. Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/port_undef.inc>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/arena.h>
#include <google/protobuf/arenastring.h>
#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/metadata_lite.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>  // IWYU pragma: export
#include <google/protobuf/extension_set.h>  // IWYU pragma: export
#include <google/protobuf/generated_enum_reflection.h>
#include <google/protobuf/unknown_field_set.h>
#include "uri.pb.h"
#include "uuid.pb.h"
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>
#define PROTOBUF_INTERNAL_EXPORT_uattributes_2eproto
PROTOBUF_NAMESPACE_OPEN
namespace internal {
class AnyMetadata;
}  // namespace internal
PROTOBUF_NAMESPACE_CLOSE

// Internal implementation detail -- do not use these members.
struct TableStruct_uattributes_2eproto {
  static const uint32_t offsets[];
};
extern const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable descriptor_table_uattributes_2eproto;
namespace uprotocol {
namespace v1 {
class UAttributes;
struct UAttributesDefaultTypeInternal;
extern UAttributesDefaultTypeInternal _UAttributes_default_instance_;
}  // namespace v1
}  // namespace uprotocol
PROTOBUF_NAMESPACE_OPEN
template<> ::uprotocol::v1::UAttributes* Arena::CreateMaybeMessage<::uprotocol::v1::UAttributes>(Arena*);
PROTOBUF_NAMESPACE_CLOSE
namespace uprotocol {
namespace v1 {

enum UMessageType : int {
  UMESSAGE_TYPE_UNSPECIFIED = 0,
  UMESSAGE_TYPE_PUBLISH = 1,
  UMESSAGE_TYPE_REQUEST = 2,
  UMESSAGE_TYPE_RESPONSE = 3,
  UMessageType_INT_MIN_SENTINEL_DO_NOT_USE_ = std::numeric_limits<int32_t>::min(),
  UMessageType_INT_MAX_SENTINEL_DO_NOT_USE_ = std::numeric_limits<int32_t>::max()
};
bool UMessageType_IsValid(int value);
constexpr UMessageType UMessageType_MIN = UMESSAGE_TYPE_UNSPECIFIED;
constexpr UMessageType UMessageType_MAX = UMESSAGE_TYPE_RESPONSE;
constexpr int UMessageType_ARRAYSIZE = UMessageType_MAX + 1;

const ::PROTOBUF_NAMESPACE_ID::EnumDescriptor* UMessageType_descriptor();
template<typename T>
inline const std::string& UMessageType_Name(T enum_t_value) {
  static_assert(::std::is_same<T, UMessageType>::value ||
    ::std::is_integral<T>::value,
    "Incorrect type passed to function UMessageType_Name.");
  return ::PROTOBUF_NAMESPACE_ID::internal::NameOfEnum(
    UMessageType_descriptor(), enum_t_value);
}
inline bool UMessageType_Parse(
    ::PROTOBUF_NAMESPACE_ID::ConstStringParam name, UMessageType* value) {
  return ::PROTOBUF_NAMESPACE_ID::internal::ParseNamedEnum<UMessageType>(
    UMessageType_descriptor(), name, value);
}
enum UPriority : int {
  UPRIORITY_UNSPECIFIED = 0,
  UPRIORITY_CS0 = 1,
  UPRIORITY_CS1 = 2,
  UPRIORITY_CS2 = 3,
  UPRIORITY_CS3 = 4,
  UPRIORITY_CS4 = 5,
  UPRIORITY_CS5 = 6,
  UPRIORITY_CS6 = 7,
  UPriority_INT_MIN_SENTINEL_DO_NOT_USE_ = std::numeric_limits<int32_t>::min(),
  UPriority_INT_MAX_SENTINEL_DO_NOT_USE_ = std::numeric_limits<int32_t>::max()
};
bool UPriority_IsValid(int value);
constexpr UPriority UPriority_MIN = UPRIORITY_UNSPECIFIED;
constexpr UPriority UPriority_MAX = UPRIORITY_CS6;
constexpr int UPriority_ARRAYSIZE = UPriority_MAX + 1;

const ::PROTOBUF_NAMESPACE_ID::EnumDescriptor* UPriority_descriptor();
template<typename T>
inline const std::string& UPriority_Name(T enum_t_value) {
  static_assert(::std::is_same<T, UPriority>::value ||
    ::std::is_integral<T>::value,
    "Incorrect type passed to function UPriority_Name.");
  return ::PROTOBUF_NAMESPACE_ID::internal::NameOfEnum(
    UPriority_descriptor(), enum_t_value);
}
inline bool UPriority_Parse(
    ::PROTOBUF_NAMESPACE_ID::ConstStringParam name, UPriority* value) {
  return ::PROTOBUF_NAMESPACE_ID::internal::ParseNamedEnum<UPriority>(
    UPriority_descriptor(), name, value);
}
// ===================================================================

class UAttributes final :
    public ::PROTOBUF_NAMESPACE_ID::Message /* @@protoc_insertion_point(class_definition:uprotocol.v1.UAttributes) */ {
 public:
  inline UAttributes() : UAttributes(nullptr) {}
  ~UAttributes() override;
  explicit PROTOBUF_CONSTEXPR UAttributes(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized);

  UAttributes(const UAttributes& from);
  UAttributes(UAttributes&& from) noexcept
    : UAttributes() {
    *this = ::std::move(from);
  }

  inline UAttributes& operator=(const UAttributes& from) {
    CopyFrom(from);
    return *this;
  }
  inline UAttributes& operator=(UAttributes&& from) noexcept {
    if (this == &from) return *this;
    if (GetOwningArena() == from.GetOwningArena()
  #ifdef PROTOBUF_FORCE_COPY_IN_MOVE
        && GetOwningArena() != nullptr
  #endif  // !PROTOBUF_FORCE_COPY_IN_MOVE
    ) {
      InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }

  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* descriptor() {
    return GetDescriptor();
  }
  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* GetDescriptor() {
    return default_instance().GetMetadata().descriptor;
  }
  static const ::PROTOBUF_NAMESPACE_ID::Reflection* GetReflection() {
    return default_instance().GetMetadata().reflection;
  }
  static const UAttributes& default_instance() {
    return *internal_default_instance();
  }
  static inline const UAttributes* internal_default_instance() {
    return reinterpret_cast<const UAttributes*>(
               &_UAttributes_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    0;

  friend void swap(UAttributes& a, UAttributes& b) {
    a.Swap(&b);
  }
  inline void Swap(UAttributes* other) {
    if (other == this) return;
  #ifdef PROTOBUF_FORCE_COPY_IN_SWAP
    if (GetOwningArena() != nullptr &&
        GetOwningArena() == other->GetOwningArena()) {
   #else  // PROTOBUF_FORCE_COPY_IN_SWAP
    if (GetOwningArena() == other->GetOwningArena()) {
  #endif  // !PROTOBUF_FORCE_COPY_IN_SWAP
      InternalSwap(other);
    } else {
      ::PROTOBUF_NAMESPACE_ID::internal::GenericSwap(this, other);
    }
  }
  void UnsafeArenaSwap(UAttributes* other) {
    if (other == this) return;
    GOOGLE_DCHECK(GetOwningArena() == other->GetOwningArena());
    InternalSwap(other);
  }

  // implements Message ----------------------------------------------

  UAttributes* New(::PROTOBUF_NAMESPACE_ID::Arena* arena = nullptr) const final {
    return CreateMaybeMessage<UAttributes>(arena);
  }
  using ::PROTOBUF_NAMESPACE_ID::Message::CopyFrom;
  void CopyFrom(const UAttributes& from);
  using ::PROTOBUF_NAMESPACE_ID::Message::MergeFrom;
  void MergeFrom( const UAttributes& from) {
    UAttributes::MergeImpl(*this, from);
  }
  private:
  static void MergeImpl(::PROTOBUF_NAMESPACE_ID::Message& to_msg, const ::PROTOBUF_NAMESPACE_ID::Message& from_msg);
  public:
  PROTOBUF_ATTRIBUTE_REINITIALIZES void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  const char* _InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) final;
  uint8_t* _InternalSerialize(
      uint8_t* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const final;
  int GetCachedSize() const final { return _impl_._cached_size_.Get(); }

  private:
  void SharedCtor(::PROTOBUF_NAMESPACE_ID::Arena* arena, bool is_message_owned);
  void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(UAttributes* other);

  private:
  friend class ::PROTOBUF_NAMESPACE_ID::internal::AnyMetadata;
  static ::PROTOBUF_NAMESPACE_ID::StringPiece FullMessageName() {
    return "uprotocol.v1.UAttributes";
  }
  protected:
  explicit UAttributes(::PROTOBUF_NAMESPACE_ID::Arena* arena,
                       bool is_message_owned = false);
  public:

  static const ClassData _class_data_;
  const ::PROTOBUF_NAMESPACE_ID::Message::ClassData*GetClassData() const final;

  ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadata() const final;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  enum : int {
    kTokenFieldNumber = 9,
    kIdFieldNumber = 1,
    kSinkFieldNumber = 3,
    kReqidFieldNumber = 8,
    kTypeFieldNumber = 2,
    kPriorityFieldNumber = 4,
    kTtlFieldNumber = 5,
    kPermissionLevelFieldNumber = 6,
    kCommstatusFieldNumber = 7,
  };
  // optional string token = 9;
  bool has_token() const;
  private:
  bool _internal_has_token() const;
  public:
  void clear_token();
  const std::string& token() const;
  template <typename ArgT0 = const std::string&, typename... ArgT>
  void set_token(ArgT0&& arg0, ArgT... args);
  std::string* mutable_token();
  PROTOBUF_NODISCARD std::string* release_token();
  void set_allocated_token(std::string* token);
  private:
  const std::string& _internal_token() const;
  inline PROTOBUF_ALWAYS_INLINE void _internal_set_token(const std::string& value);
  std::string* _internal_mutable_token();
  public:

  // .uprotocol.v1.UUID id = 1;
  bool has_id() const;
  private:
  bool _internal_has_id() const;
  public:
  void clear_id();
  const ::uprotocol::v1::UUID& id() const;
  PROTOBUF_NODISCARD ::uprotocol::v1::UUID* release_id();
  ::uprotocol::v1::UUID* mutable_id();
  void set_allocated_id(::uprotocol::v1::UUID* id);
  private:
  const ::uprotocol::v1::UUID& _internal_id() const;
  ::uprotocol::v1::UUID* _internal_mutable_id();
  public:
  void unsafe_arena_set_allocated_id(
      ::uprotocol::v1::UUID* id);
  ::uprotocol::v1::UUID* unsafe_arena_release_id();

  // optional .uprotocol.v1.UUri sink = 3;
  bool has_sink() const;
  private:
  bool _internal_has_sink() const;
  public:
  void clear_sink();
  const ::uprotocol::v1::UUri& sink() const;
  PROTOBUF_NODISCARD ::uprotocol::v1::UUri* release_sink();
  ::uprotocol::v1::UUri* mutable_sink();
  void set_allocated_sink(::uprotocol::v1::UUri* sink);
  private:
  const ::uprotocol::v1::UUri& _internal_sink() const;
  ::uprotocol::v1::UUri* _internal_mutable_sink();
  public:
  void unsafe_arena_set_allocated_sink(
      ::uprotocol::v1::UUri* sink);
  ::uprotocol::v1::UUri* unsafe_arena_release_sink();

  // optional .uprotocol.v1.UUID reqid = 8;
  bool has_reqid() const;
  private:
  bool _internal_has_reqid() const;
  public:
  void clear_reqid();
  const ::uprotocol::v1::UUID& reqid() const;
  PROTOBUF_NODISCARD ::uprotocol::v1::UUID* release_reqid();
  ::uprotocol::v1::UUID* mutable_reqid();
  void set_allocated_reqid(::uprotocol::v1::UUID* reqid);
  private:
  const ::uprotocol::v1::UUID& _internal_reqid() const;
  ::uprotocol::v1::UUID* _internal_mutable_reqid();
  public:
  void unsafe_arena_set_allocated_reqid(
      ::uprotocol::v1::UUID* reqid);
  ::uprotocol::v1::UUID* unsafe_arena_release_reqid();

  // .uprotocol.v1.UMessageType type = 2;
  void clear_type();
  ::uprotocol::v1::UMessageType type() const;
  void set_type(::uprotocol::v1::UMessageType value);
  private:
  ::uprotocol::v1::UMessageType _internal_type() const;
  void _internal_set_type(::uprotocol::v1::UMessageType value);
  public:

  // .uprotocol.v1.UPriority priority = 4;
  void clear_priority();
  ::uprotocol::v1::UPriority priority() const;
  void set_priority(::uprotocol::v1::UPriority value);
  private:
  ::uprotocol::v1::UPriority _internal_priority() const;
  void _internal_set_priority(::uprotocol::v1::UPriority value);
  public:

  // optional int32 ttl = 5;
  bool has_ttl() const;
  private:
  bool _internal_has_ttl() const;
  public:
  void clear_ttl();
  int32_t ttl() const;
  void set_ttl(int32_t value);
  private:
  int32_t _internal_ttl() const;
  void _internal_set_ttl(int32_t value);
  public:

  // optional int32 permission_level = 6;
  bool has_permission_level() const;
  private:
  bool _internal_has_permission_level() const;
  public:
  void clear_permission_level();
  int32_t permission_level() const;
  void set_permission_level(int32_t value);
  private:
  int32_t _internal_permission_level() const;
  void _internal_set_permission_level(int32_t value);
  public:

  // optional int32 commstatus = 7;
  bool has_commstatus() const;
  private:
  bool _internal_has_commstatus() const;
  public:
  void clear_commstatus();
  int32_t commstatus() const;
  void set_commstatus(int32_t value);
  private:
  int32_t _internal_commstatus() const;
  void _internal_set_commstatus(int32_t value);
  public:

  // @@protoc_insertion_point(class_scope:uprotocol.v1.UAttributes)
 private:
  class _Internal;

  template <typename T> friend class ::PROTOBUF_NAMESPACE_ID::Arena::InternalHelper;
  typedef void InternalArenaConstructable_;
  typedef void DestructorSkippable_;
  struct Impl_ {
    ::PROTOBUF_NAMESPACE_ID::internal::HasBits<1> _has_bits_;
    mutable ::PROTOBUF_NAMESPACE_ID::internal::CachedSize _cached_size_;
    ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr token_;
    ::uprotocol::v1::UUID* id_;
    ::uprotocol::v1::UUri* sink_;
    ::uprotocol::v1::UUID* reqid_;
    int type_;
    int priority_;
    int32_t ttl_;
    int32_t permission_level_;
    int32_t commstatus_;
  };
  union { Impl_ _impl_; };
  friend struct ::TableStruct_uattributes_2eproto;
};
// ===================================================================


// ===================================================================

#ifdef __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
// UAttributes

// .uprotocol.v1.UUID id = 1;
inline bool UAttributes::_internal_has_id() const {
  return this != internal_default_instance() && _impl_.id_ != nullptr;
}
inline bool UAttributes::has_id() const {
  return _internal_has_id();
}
inline const ::uprotocol::v1::UUID& UAttributes::_internal_id() const {
  const ::uprotocol::v1::UUID* p = _impl_.id_;
  return p != nullptr ? *p : reinterpret_cast<const ::uprotocol::v1::UUID&>(
      ::uprotocol::v1::_UUID_default_instance_);
}
inline const ::uprotocol::v1::UUID& UAttributes::id() const {
  // @@protoc_insertion_point(field_get:uprotocol.v1.UAttributes.id)
  return _internal_id();
}
inline void UAttributes::unsafe_arena_set_allocated_id(
    ::uprotocol::v1::UUID* id) {
  if (GetArenaForAllocation() == nullptr) {
    delete reinterpret_cast<::PROTOBUF_NAMESPACE_ID::MessageLite*>(_impl_.id_);
  }
  _impl_.id_ = id;
  if (id) {
    
  } else {
    
  }
  // @@protoc_insertion_point(field_unsafe_arena_set_allocated:uprotocol.v1.UAttributes.id)
}
inline ::uprotocol::v1::UUID* UAttributes::release_id() {
  
  ::uprotocol::v1::UUID* temp = _impl_.id_;
  _impl_.id_ = nullptr;
#ifdef PROTOBUF_FORCE_COPY_IN_RELEASE
  auto* old =  reinterpret_cast<::PROTOBUF_NAMESPACE_ID::MessageLite*>(temp);
  temp = ::PROTOBUF_NAMESPACE_ID::internal::DuplicateIfNonNull(temp);
  if (GetArenaForAllocation() == nullptr) { delete old; }
#else  // PROTOBUF_FORCE_COPY_IN_RELEASE
  if (GetArenaForAllocation() != nullptr) {
    temp = ::PROTOBUF_NAMESPACE_ID::internal::DuplicateIfNonNull(temp);
  }
#endif  // !PROTOBUF_FORCE_COPY_IN_RELEASE
  return temp;
}
inline ::uprotocol::v1::UUID* UAttributes::unsafe_arena_release_id() {
  // @@protoc_insertion_point(field_release:uprotocol.v1.UAttributes.id)
  
  ::uprotocol::v1::UUID* temp = _impl_.id_;
  _impl_.id_ = nullptr;
  return temp;
}
inline ::uprotocol::v1::UUID* UAttributes::_internal_mutable_id() {
  
  if (_impl_.id_ == nullptr) {
    auto* p = CreateMaybeMessage<::uprotocol::v1::UUID>(GetArenaForAllocation());
    _impl_.id_ = p;
  }
  return _impl_.id_;
}
inline ::uprotocol::v1::UUID* UAttributes::mutable_id() {
  ::uprotocol::v1::UUID* _msg = _internal_mutable_id();
  // @@protoc_insertion_point(field_mutable:uprotocol.v1.UAttributes.id)
  return _msg;
}
inline void UAttributes::set_allocated_id(::uprotocol::v1::UUID* id) {
  ::PROTOBUF_NAMESPACE_ID::Arena* message_arena = GetArenaForAllocation();
  if (message_arena == nullptr) {
    delete reinterpret_cast< ::PROTOBUF_NAMESPACE_ID::MessageLite*>(_impl_.id_);
  }
  if (id) {
    ::PROTOBUF_NAMESPACE_ID::Arena* submessage_arena =
        ::PROTOBUF_NAMESPACE_ID::Arena::InternalGetOwningArena(
                reinterpret_cast<::PROTOBUF_NAMESPACE_ID::MessageLite*>(id));
    if (message_arena != submessage_arena) {
      id = ::PROTOBUF_NAMESPACE_ID::internal::GetOwnedMessage(
          message_arena, id, submessage_arena);
    }
    
  } else {
    
  }
  _impl_.id_ = id;
  // @@protoc_insertion_point(field_set_allocated:uprotocol.v1.UAttributes.id)
}

// .uprotocol.v1.UMessageType type = 2;
inline void UAttributes::clear_type() {
  _impl_.type_ = 0;
}
inline ::uprotocol::v1::UMessageType UAttributes::_internal_type() const {
  return static_cast< ::uprotocol::v1::UMessageType >(_impl_.type_);
}
inline ::uprotocol::v1::UMessageType UAttributes::type() const {
  // @@protoc_insertion_point(field_get:uprotocol.v1.UAttributes.type)
  return _internal_type();
}
inline void UAttributes::_internal_set_type(::uprotocol::v1::UMessageType value) {
  
  _impl_.type_ = value;
}
inline void UAttributes::set_type(::uprotocol::v1::UMessageType value) {
  _internal_set_type(value);
  // @@protoc_insertion_point(field_set:uprotocol.v1.UAttributes.type)
}

// optional .uprotocol.v1.UUri sink = 3;
inline bool UAttributes::_internal_has_sink() const {
  bool value = (_impl_._has_bits_[0] & 0x00000002u) != 0;
  PROTOBUF_ASSUME(!value || _impl_.sink_ != nullptr);
  return value;
}
inline bool UAttributes::has_sink() const {
  return _internal_has_sink();
}
inline const ::uprotocol::v1::UUri& UAttributes::_internal_sink() const {
  const ::uprotocol::v1::UUri* p = _impl_.sink_;
  return p != nullptr ? *p : reinterpret_cast<const ::uprotocol::v1::UUri&>(
      ::uprotocol::v1::_UUri_default_instance_);
}
inline const ::uprotocol::v1::UUri& UAttributes::sink() const {
  // @@protoc_insertion_point(field_get:uprotocol.v1.UAttributes.sink)
  return _internal_sink();
}
inline void UAttributes::unsafe_arena_set_allocated_sink(
    ::uprotocol::v1::UUri* sink) {
  if (GetArenaForAllocation() == nullptr) {
    delete reinterpret_cast<::PROTOBUF_NAMESPACE_ID::MessageLite*>(_impl_.sink_);
  }
  _impl_.sink_ = sink;
  if (sink) {
    _impl_._has_bits_[0] |= 0x00000002u;
  } else {
    _impl_._has_bits_[0] &= ~0x00000002u;
  }
  // @@protoc_insertion_point(field_unsafe_arena_set_allocated:uprotocol.v1.UAttributes.sink)
}
inline ::uprotocol::v1::UUri* UAttributes::release_sink() {
  _impl_._has_bits_[0] &= ~0x00000002u;
  ::uprotocol::v1::UUri* temp = _impl_.sink_;
  _impl_.sink_ = nullptr;
#ifdef PROTOBUF_FORCE_COPY_IN_RELEASE
  auto* old =  reinterpret_cast<::PROTOBUF_NAMESPACE_ID::MessageLite*>(temp);
  temp = ::PROTOBUF_NAMESPACE_ID::internal::DuplicateIfNonNull(temp);
  if (GetArenaForAllocation() == nullptr) { delete old; }
#else  // PROTOBUF_FORCE_COPY_IN_RELEASE
  if (GetArenaForAllocation() != nullptr) {
    temp = ::PROTOBUF_NAMESPACE_ID::internal::DuplicateIfNonNull(temp);
  }
#endif  // !PROTOBUF_FORCE_COPY_IN_RELEASE
  return temp;
}
inline ::uprotocol::v1::UUri* UAttributes::unsafe_arena_release_sink() {
  // @@protoc_insertion_point(field_release:uprotocol.v1.UAttributes.sink)
  _impl_._has_bits_[0] &= ~0x00000002u;
  ::uprotocol::v1::UUri* temp = _impl_.sink_;
  _impl_.sink_ = nullptr;
  return temp;
}
inline ::uprotocol::v1::UUri* UAttributes::_internal_mutable_sink() {
  _impl_._has_bits_[0] |= 0x00000002u;
  if (_impl_.sink_ == nullptr) {
    auto* p = CreateMaybeMessage<::uprotocol::v1::UUri>(GetArenaForAllocation());
    _impl_.sink_ = p;
  }
  return _impl_.sink_;
}
inline ::uprotocol::v1::UUri* UAttributes::mutable_sink() {
  ::uprotocol::v1::UUri* _msg = _internal_mutable_sink();
  // @@protoc_insertion_point(field_mutable:uprotocol.v1.UAttributes.sink)
  return _msg;
}
inline void UAttributes::set_allocated_sink(::uprotocol::v1::UUri* sink) {
  ::PROTOBUF_NAMESPACE_ID::Arena* message_arena = GetArenaForAllocation();
  if (message_arena == nullptr) {
    delete reinterpret_cast< ::PROTOBUF_NAMESPACE_ID::MessageLite*>(_impl_.sink_);
  }
  if (sink) {
    ::PROTOBUF_NAMESPACE_ID::Arena* submessage_arena =
        ::PROTOBUF_NAMESPACE_ID::Arena::InternalGetOwningArena(
                reinterpret_cast<::PROTOBUF_NAMESPACE_ID::MessageLite*>(sink));
    if (message_arena != submessage_arena) {
      sink = ::PROTOBUF_NAMESPACE_ID::internal::GetOwnedMessage(
          message_arena, sink, submessage_arena);
    }
    _impl_._has_bits_[0] |= 0x00000002u;
  } else {
    _impl_._has_bits_[0] &= ~0x00000002u;
  }
  _impl_.sink_ = sink;
  // @@protoc_insertion_point(field_set_allocated:uprotocol.v1.UAttributes.sink)
}

// .uprotocol.v1.UPriority priority = 4;
inline void UAttributes::clear_priority() {
  _impl_.priority_ = 0;
}
inline ::uprotocol::v1::UPriority UAttributes::_internal_priority() const {
  return static_cast< ::uprotocol::v1::UPriority >(_impl_.priority_);
}
inline ::uprotocol::v1::UPriority UAttributes::priority() const {
  // @@protoc_insertion_point(field_get:uprotocol.v1.UAttributes.priority)
  return _internal_priority();
}
inline void UAttributes::_internal_set_priority(::uprotocol::v1::UPriority value) {
  
  _impl_.priority_ = value;
}
inline void UAttributes::set_priority(::uprotocol::v1::UPriority value) {
  _internal_set_priority(value);
  // @@protoc_insertion_point(field_set:uprotocol.v1.UAttributes.priority)
}

// optional int32 ttl = 5;
inline bool UAttributes::_internal_has_ttl() const {
  bool value = (_impl_._has_bits_[0] & 0x00000008u) != 0;
  return value;
}
inline bool UAttributes::has_ttl() const {
  return _internal_has_ttl();
}
inline void UAttributes::clear_ttl() {
  _impl_.ttl_ = 0;
  _impl_._has_bits_[0] &= ~0x00000008u;
}
inline int32_t UAttributes::_internal_ttl() const {
  return _impl_.ttl_;
}
inline int32_t UAttributes::ttl() const {
  // @@protoc_insertion_point(field_get:uprotocol.v1.UAttributes.ttl)
  return _internal_ttl();
}
inline void UAttributes::_internal_set_ttl(int32_t value) {
  _impl_._has_bits_[0] |= 0x00000008u;
  _impl_.ttl_ = value;
}
inline void UAttributes::set_ttl(int32_t value) {
  _internal_set_ttl(value);
  // @@protoc_insertion_point(field_set:uprotocol.v1.UAttributes.ttl)
}

// optional int32 permission_level = 6;
inline bool UAttributes::_internal_has_permission_level() const {
  bool value = (_impl_._has_bits_[0] & 0x00000010u) != 0;
  return value;
}
inline bool UAttributes::has_permission_level() const {
  return _internal_has_permission_level();
}
inline void UAttributes::clear_permission_level() {
  _impl_.permission_level_ = 0;
  _impl_._has_bits_[0] &= ~0x00000010u;
}
inline int32_t UAttributes::_internal_permission_level() const {
  return _impl_.permission_level_;
}
inline int32_t UAttributes::permission_level() const {
  // @@protoc_insertion_point(field_get:uprotocol.v1.UAttributes.permission_level)
  return _internal_permission_level();
}
inline void UAttributes::_internal_set_permission_level(int32_t value) {
  _impl_._has_bits_[0] |= 0x00000010u;
  _impl_.permission_level_ = value;
}
inline void UAttributes::set_permission_level(int32_t value) {
  _internal_set_permission_level(value);
  // @@protoc_insertion_point(field_set:uprotocol.v1.UAttributes.permission_level)
}

// optional int32 commstatus = 7;
inline bool UAttributes::_internal_has_commstatus() const {
  bool value = (_impl_._has_bits_[0] & 0x00000020u) != 0;
  return value;
}
inline bool UAttributes::has_commstatus() const {
  return _internal_has_commstatus();
}
inline void UAttributes::clear_commstatus() {
  _impl_.commstatus_ = 0;
  _impl_._has_bits_[0] &= ~0x00000020u;
}
inline int32_t UAttributes::_internal_commstatus() const {
  return _impl_.commstatus_;
}
inline int32_t UAttributes::commstatus() const {
  // @@protoc_insertion_point(field_get:uprotocol.v1.UAttributes.commstatus)
  return _internal_commstatus();
}
inline void UAttributes::_internal_set_commstatus(int32_t value) {
  _impl_._has_bits_[0] |= 0x00000020u;
  _impl_.commstatus_ = value;
}
inline void UAttributes::set_commstatus(int32_t value) {
  _internal_set_commstatus(value);
  // @@protoc_insertion_point(field_set:uprotocol.v1.UAttributes.commstatus)
}

// optional .uprotocol.v1.UUID reqid = 8;
inline bool UAttributes::_internal_has_reqid() const {
  bool value = (_impl_._has_bits_[0] & 0x00000004u) != 0;
  PROTOBUF_ASSUME(!value || _impl_.reqid_ != nullptr);
  return value;
}
inline bool UAttributes::has_reqid() const {
  return _internal_has_reqid();
}
inline const ::uprotocol::v1::UUID& UAttributes::_internal_reqid() const {
  const ::uprotocol::v1::UUID* p = _impl_.reqid_;
  return p != nullptr ? *p : reinterpret_cast<const ::uprotocol::v1::UUID&>(
      ::uprotocol::v1::_UUID_default_instance_);
}
inline const ::uprotocol::v1::UUID& UAttributes::reqid() const {
  // @@protoc_insertion_point(field_get:uprotocol.v1.UAttributes.reqid)
  return _internal_reqid();
}
inline void UAttributes::unsafe_arena_set_allocated_reqid(
    ::uprotocol::v1::UUID* reqid) {
  if (GetArenaForAllocation() == nullptr) {
    delete reinterpret_cast<::PROTOBUF_NAMESPACE_ID::MessageLite*>(_impl_.reqid_);
  }
  _impl_.reqid_ = reqid;
  if (reqid) {
    _impl_._has_bits_[0] |= 0x00000004u;
  } else {
    _impl_._has_bits_[0] &= ~0x00000004u;
  }
  // @@protoc_insertion_point(field_unsafe_arena_set_allocated:uprotocol.v1.UAttributes.reqid)
}
inline ::uprotocol::v1::UUID* UAttributes::release_reqid() {
  _impl_._has_bits_[0] &= ~0x00000004u;
  ::uprotocol::v1::UUID* temp = _impl_.reqid_;
  _impl_.reqid_ = nullptr;
#ifdef PROTOBUF_FORCE_COPY_IN_RELEASE
  auto* old =  reinterpret_cast<::PROTOBUF_NAMESPACE_ID::MessageLite*>(temp);
  temp = ::PROTOBUF_NAMESPACE_ID::internal::DuplicateIfNonNull(temp);
  if (GetArenaForAllocation() == nullptr) { delete old; }
#else  // PROTOBUF_FORCE_COPY_IN_RELEASE
  if (GetArenaForAllocation() != nullptr) {
    temp = ::PROTOBUF_NAMESPACE_ID::internal::DuplicateIfNonNull(temp);
  }
#endif  // !PROTOBUF_FORCE_COPY_IN_RELEASE
  return temp;
}
inline ::uprotocol::v1::UUID* UAttributes::unsafe_arena_release_reqid() {
  // @@protoc_insertion_point(field_release:uprotocol.v1.UAttributes.reqid)
  _impl_._has_bits_[0] &= ~0x00000004u;
  ::uprotocol::v1::UUID* temp = _impl_.reqid_;
  _impl_.reqid_ = nullptr;
  return temp;
}
inline ::uprotocol::v1::UUID* UAttributes::_internal_mutable_reqid() {
  _impl_._has_bits_[0] |= 0x00000004u;
  if (_impl_.reqid_ == nullptr) {
    auto* p = CreateMaybeMessage<::uprotocol::v1::UUID>(GetArenaForAllocation());
    _impl_.reqid_ = p;
  }
  return _impl_.reqid_;
}
inline ::uprotocol::v1::UUID* UAttributes::mutable_reqid() {
  ::uprotocol::v1::UUID* _msg = _internal_mutable_reqid();
  // @@protoc_insertion_point(field_mutable:uprotocol.v1.UAttributes.reqid)
  return _msg;
}
inline void UAttributes::set_allocated_reqid(::uprotocol::v1::UUID* reqid) {
  ::PROTOBUF_NAMESPACE_ID::Arena* message_arena = GetArenaForAllocation();
  if (message_arena == nullptr) {
    delete reinterpret_cast< ::PROTOBUF_NAMESPACE_ID::MessageLite*>(_impl_.reqid_);
  }
  if (reqid) {
    ::PROTOBUF_NAMESPACE_ID::Arena* submessage_arena =
        ::PROTOBUF_NAMESPACE_ID::Arena::InternalGetOwningArena(
                reinterpret_cast<::PROTOBUF_NAMESPACE_ID::MessageLite*>(reqid));
    if (message_arena != submessage_arena) {
      reqid = ::PROTOBUF_NAMESPACE_ID::internal::GetOwnedMessage(
          message_arena, reqid, submessage_arena);
    }
    _impl_._has_bits_[0] |= 0x00000004u;
  } else {
    _impl_._has_bits_[0] &= ~0x00000004u;
  }
  _impl_.reqid_ = reqid;
  // @@protoc_insertion_point(field_set_allocated:uprotocol.v1.UAttributes.reqid)
}

// optional string token = 9;
inline bool UAttributes::_internal_has_token() const {
  bool value = (_impl_._has_bits_[0] & 0x00000001u) != 0;
  return value;
}
inline bool UAttributes::has_token() const {
  return _internal_has_token();
}
inline void UAttributes::clear_token() {
  _impl_.token_.ClearToEmpty();
  _impl_._has_bits_[0] &= ~0x00000001u;
}
inline const std::string& UAttributes::token() const {
  // @@protoc_insertion_point(field_get:uprotocol.v1.UAttributes.token)
  return _internal_token();
}
template <typename ArgT0, typename... ArgT>
inline PROTOBUF_ALWAYS_INLINE
void UAttributes::set_token(ArgT0&& arg0, ArgT... args) {
 _impl_._has_bits_[0] |= 0x00000001u;
 _impl_.token_.Set(static_cast<ArgT0 &&>(arg0), args..., GetArenaForAllocation());
  // @@protoc_insertion_point(field_set:uprotocol.v1.UAttributes.token)
}
inline std::string* UAttributes::mutable_token() {
  std::string* _s = _internal_mutable_token();
  // @@protoc_insertion_point(field_mutable:uprotocol.v1.UAttributes.token)
  return _s;
}
inline const std::string& UAttributes::_internal_token() const {
  return _impl_.token_.Get();
}
inline void UAttributes::_internal_set_token(const std::string& value) {
  _impl_._has_bits_[0] |= 0x00000001u;
  _impl_.token_.Set(value, GetArenaForAllocation());
}
inline std::string* UAttributes::_internal_mutable_token() {
  _impl_._has_bits_[0] |= 0x00000001u;
  return _impl_.token_.Mutable(GetArenaForAllocation());
}
inline std::string* UAttributes::release_token() {
  // @@protoc_insertion_point(field_release:uprotocol.v1.UAttributes.token)
  if (!_internal_has_token()) {
    return nullptr;
  }
  _impl_._has_bits_[0] &= ~0x00000001u;
  auto* p = _impl_.token_.Release();
#ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
  if (_impl_.token_.IsDefault()) {
    _impl_.token_.Set("", GetArenaForAllocation());
  }
#endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
  return p;
}
inline void UAttributes::set_allocated_token(std::string* token) {
  if (token != nullptr) {
    _impl_._has_bits_[0] |= 0x00000001u;
  } else {
    _impl_._has_bits_[0] &= ~0x00000001u;
  }
  _impl_.token_.SetAllocated(token, GetArenaForAllocation());
#ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
  if (_impl_.token_.IsDefault()) {
    _impl_.token_.Set("", GetArenaForAllocation());
  }
#endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
  // @@protoc_insertion_point(field_set_allocated:uprotocol.v1.UAttributes.token)
}

#ifdef __GNUC__
  #pragma GCC diagnostic pop
#endif  // __GNUC__

// @@protoc_insertion_point(namespace_scope)

}  // namespace v1
}  // namespace uprotocol

PROTOBUF_NAMESPACE_OPEN

template <> struct is_proto_enum< ::uprotocol::v1::UMessageType> : ::std::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::uprotocol::v1::UMessageType>() {
  return ::uprotocol::v1::UMessageType_descriptor();
}
template <> struct is_proto_enum< ::uprotocol::v1::UPriority> : ::std::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::uprotocol::v1::UPriority>() {
  return ::uprotocol::v1::UPriority_descriptor();
}

PROTOBUF_NAMESPACE_CLOSE

// @@protoc_insertion_point(global_scope)

#include <google/protobuf/port_undef.inc>
#endif  // GOOGLE_PROTOBUF_INCLUDED_GOOGLE_PROTOBUF_INCLUDED_uattributes_2eproto
