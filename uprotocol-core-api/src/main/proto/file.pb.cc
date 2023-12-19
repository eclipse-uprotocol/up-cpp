// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: file.proto

#include "file.pb.h"

#include <algorithm>

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/wire_format_lite.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>

PROTOBUF_PRAGMA_INIT_SEG

namespace _pb = ::PROTOBUF_NAMESPACE_ID;
namespace _pbi = _pb::internal;

namespace uprotocol {
namespace v1 {
PROTOBUF_CONSTEXPR File::File(
    ::_pbi::ConstantInitialized): _impl_{
    /*decltype(_impl_._has_bits_)*/{}
  , /*decltype(_impl_._cached_size_)*/{}
  , /*decltype(_impl_.name_)*/{&::_pbi::fixed_address_empty_string, ::_pbi::ConstantInitialized{}}
  , /*decltype(_impl_.checksum_)*/nullptr
  , /*decltype(_impl_.size_)*/uint64_t{0u}} {}
struct FileDefaultTypeInternal {
  PROTOBUF_CONSTEXPR FileDefaultTypeInternal()
      : _instance(::_pbi::ConstantInitialized{}) {}
  ~FileDefaultTypeInternal() {}
  union {
    File _instance;
  };
};
PROTOBUF_ATTRIBUTE_NO_DESTROY PROTOBUF_CONSTINIT PROTOBUF_ATTRIBUTE_INIT_PRIORITY1 FileDefaultTypeInternal _File_default_instance_;
PROTOBUF_CONSTEXPR FileBatch::FileBatch(
    ::_pbi::ConstantInitialized): _impl_{
    /*decltype(_impl_.files_)*/{}
  , /*decltype(_impl_._cached_size_)*/{}} {}
struct FileBatchDefaultTypeInternal {
  PROTOBUF_CONSTEXPR FileBatchDefaultTypeInternal()
      : _instance(::_pbi::ConstantInitialized{}) {}
  ~FileBatchDefaultTypeInternal() {}
  union {
    FileBatch _instance;
  };
};
PROTOBUF_ATTRIBUTE_NO_DESTROY PROTOBUF_CONSTINIT PROTOBUF_ATTRIBUTE_INIT_PRIORITY1 FileBatchDefaultTypeInternal _FileBatch_default_instance_;
PROTOBUF_CONSTEXPR Checksum::Checksum(
    ::_pbi::ConstantInitialized): _impl_{
    /*decltype(_impl_.value_)*/{&::_pbi::fixed_address_empty_string, ::_pbi::ConstantInitialized{}}
  , /*decltype(_impl_.type_)*/0
  , /*decltype(_impl_._cached_size_)*/{}} {}
struct ChecksumDefaultTypeInternal {
  PROTOBUF_CONSTEXPR ChecksumDefaultTypeInternal()
      : _instance(::_pbi::ConstantInitialized{}) {}
  ~ChecksumDefaultTypeInternal() {}
  union {
    Checksum _instance;
  };
};
PROTOBUF_ATTRIBUTE_NO_DESTROY PROTOBUF_CONSTINIT PROTOBUF_ATTRIBUTE_INIT_PRIORITY1 ChecksumDefaultTypeInternal _Checksum_default_instance_;
}  // namespace v1
}  // namespace uprotocol
static ::_pb::Metadata file_level_metadata_file_2eproto[3];
static const ::_pb::EnumDescriptor* file_level_enum_descriptors_file_2eproto[1];
static constexpr ::_pb::ServiceDescriptor const** file_level_service_descriptors_file_2eproto = nullptr;

const uint32_t TableStruct_file_2eproto::offsets[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
  PROTOBUF_FIELD_OFFSET(::uprotocol::v1::File, _impl_._has_bits_),
  PROTOBUF_FIELD_OFFSET(::uprotocol::v1::File, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  ~0u,  // no _inlined_string_donated_
  PROTOBUF_FIELD_OFFSET(::uprotocol::v1::File, _impl_.name_),
  PROTOBUF_FIELD_OFFSET(::uprotocol::v1::File, _impl_.size_),
  PROTOBUF_FIELD_OFFSET(::uprotocol::v1::File, _impl_.checksum_),
  ~0u,
  0,
  ~0u,
  ~0u,  // no _has_bits_
  PROTOBUF_FIELD_OFFSET(::uprotocol::v1::FileBatch, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  ~0u,  // no _inlined_string_donated_
  PROTOBUF_FIELD_OFFSET(::uprotocol::v1::FileBatch, _impl_.files_),
  ~0u,  // no _has_bits_
  PROTOBUF_FIELD_OFFSET(::uprotocol::v1::Checksum, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  ~0u,  // no _inlined_string_donated_
  PROTOBUF_FIELD_OFFSET(::uprotocol::v1::Checksum, _impl_.type_),
  PROTOBUF_FIELD_OFFSET(::uprotocol::v1::Checksum, _impl_.value_),
};
static const ::_pbi::MigrationSchema schemas[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
  { 0, 9, -1, sizeof(::uprotocol::v1::File)},
  { 12, -1, -1, sizeof(::uprotocol::v1::FileBatch)},
  { 19, -1, -1, sizeof(::uprotocol::v1::Checksum)},
};

static const ::_pb::Message* const file_default_instances[] = {
  &::uprotocol::v1::_File_default_instance_._instance,
  &::uprotocol::v1::_FileBatch_default_instance_._instance,
  &::uprotocol::v1::_Checksum_default_instance_._instance,
};

const char descriptor_table_protodef_file_2eproto[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) =
  "\n\nfile.proto\022\014uprotocol.v1\"Z\n\004File\022\014\n\004na"
  "me\030\001 \001(\t\022\021\n\004size\030\002 \001(\004H\000\210\001\001\022(\n\010checksum\030"
  "\003 \001(\0132\026.uprotocol.v1.ChecksumB\007\n\005_size\"."
  "\n\tFileBatch\022!\n\005files\030\001 \003(\0132\022.uprotocol.v"
  "1.File\"C\n\010Checksum\022(\n\004type\030\001 \001(\0162\032.uprot"
  "ocol.v1.ChecksumType\022\r\n\005value\030\002 \001(\014*\\\n\014C"
  "hecksumType\022\035\n\031CHECKSUM_TYPE_UNSPECIFIED"
  "\020\000\022\025\n\021CHECKSUM_TYPE_MD5\020\001\022\026\n\022CHECKSUM_TY"
  "PE_SHA1\020\002B\034\n\030org.eclipse.uprotocol.v1P\001b"
  "\006proto3"
  ;
static ::_pbi::once_flag descriptor_table_file_2eproto_once;
const ::_pbi::DescriptorTable descriptor_table_file_2eproto = {
    false, false, 367, descriptor_table_protodef_file_2eproto,
    "file.proto",
    &descriptor_table_file_2eproto_once, nullptr, 0, 3,
    schemas, file_default_instances, TableStruct_file_2eproto::offsets,
    file_level_metadata_file_2eproto, file_level_enum_descriptors_file_2eproto,
    file_level_service_descriptors_file_2eproto,
};
PROTOBUF_ATTRIBUTE_WEAK const ::_pbi::DescriptorTable* descriptor_table_file_2eproto_getter() {
  return &descriptor_table_file_2eproto;
}

// Force running AddDescriptors() at dynamic initialization time.
PROTOBUF_ATTRIBUTE_INIT_PRIORITY2 static ::_pbi::AddDescriptorsRunner dynamic_init_dummy_file_2eproto(&descriptor_table_file_2eproto);
namespace uprotocol {
namespace v1 {
const ::PROTOBUF_NAMESPACE_ID::EnumDescriptor* ChecksumType_descriptor() {
  ::PROTOBUF_NAMESPACE_ID::internal::AssignDescriptors(&descriptor_table_file_2eproto);
  return file_level_enum_descriptors_file_2eproto[0];
}
bool ChecksumType_IsValid(int value) {
  switch (value) {
    case 0:
    case 1:
    case 2:
      return true;
    default:
      return false;
  }
}


// ===================================================================

class File::_Internal {
 public:
  using HasBits = decltype(std::declval<File>()._impl_._has_bits_);
  static void set_has_size(HasBits* has_bits) {
    (*has_bits)[0] |= 1u;
  }
  static const ::uprotocol::v1::Checksum& checksum(const File* msg);
};

const ::uprotocol::v1::Checksum&
File::_Internal::checksum(const File* msg) {
  return *msg->_impl_.checksum_;
}
File::File(::PROTOBUF_NAMESPACE_ID::Arena* arena,
                         bool is_message_owned)
  : ::PROTOBUF_NAMESPACE_ID::Message(arena, is_message_owned) {
  SharedCtor(arena, is_message_owned);
  // @@protoc_insertion_point(arena_constructor:uprotocol.v1.File)
}
File::File(const File& from)
  : ::PROTOBUF_NAMESPACE_ID::Message() {
  File* const _this = this; (void)_this;
  new (&_impl_) Impl_{
      decltype(_impl_._has_bits_){from._impl_._has_bits_}
    , /*decltype(_impl_._cached_size_)*/{}
    , decltype(_impl_.name_){}
    , decltype(_impl_.checksum_){nullptr}
    , decltype(_impl_.size_){}};

  _internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
  _impl_.name_.InitDefault();
  #ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
    _impl_.name_.Set("", GetArenaForAllocation());
  #endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
  if (!from._internal_name().empty()) {
    _this->_impl_.name_.Set(from._internal_name(), 
      _this->GetArenaForAllocation());
  }
  if (from._internal_has_checksum()) {
    _this->_impl_.checksum_ = new ::uprotocol::v1::Checksum(*from._impl_.checksum_);
  }
  _this->_impl_.size_ = from._impl_.size_;
  // @@protoc_insertion_point(copy_constructor:uprotocol.v1.File)
}

inline void File::SharedCtor(
    ::_pb::Arena* arena, bool is_message_owned) {
  (void)arena;
  (void)is_message_owned;
  new (&_impl_) Impl_{
      decltype(_impl_._has_bits_){}
    , /*decltype(_impl_._cached_size_)*/{}
    , decltype(_impl_.name_){}
    , decltype(_impl_.checksum_){nullptr}
    , decltype(_impl_.size_){uint64_t{0u}}
  };
  _impl_.name_.InitDefault();
  #ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
    _impl_.name_.Set("", GetArenaForAllocation());
  #endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
}

File::~File() {
  // @@protoc_insertion_point(destructor:uprotocol.v1.File)
  if (auto *arena = _internal_metadata_.DeleteReturnArena<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>()) {
  (void)arena;
    return;
  }
  SharedDtor();
}

inline void File::SharedDtor() {
  GOOGLE_DCHECK(GetArenaForAllocation() == nullptr);
  _impl_.name_.Destroy();
  if (this != internal_default_instance()) delete _impl_.checksum_;
}

void File::SetCachedSize(int size) const {
  _impl_._cached_size_.Set(size);
}

void File::Clear() {
// @@protoc_insertion_point(message_clear_start:uprotocol.v1.File)
  uint32_t cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  _impl_.name_.ClearToEmpty();
  if (GetArenaForAllocation() == nullptr && _impl_.checksum_ != nullptr) {
    delete _impl_.checksum_;
  }
  _impl_.checksum_ = nullptr;
  _impl_.size_ = uint64_t{0u};
  _impl_._has_bits_.Clear();
  _internal_metadata_.Clear<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>();
}

const char* File::_InternalParse(const char* ptr, ::_pbi::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  _Internal::HasBits has_bits{};
  while (!ctx->Done(&ptr)) {
    uint32_t tag;
    ptr = ::_pbi::ReadTag(ptr, &tag);
    switch (tag >> 3) {
      // string name = 1;
      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<uint8_t>(tag) == 10)) {
          auto str = _internal_mutable_name();
          ptr = ::_pbi::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
          CHK_(::_pbi::VerifyUTF8(str, "uprotocol.v1.File.name"));
        } else
          goto handle_unusual;
        continue;
      // optional uint64 size = 2;
      case 2:
        if (PROTOBUF_PREDICT_TRUE(static_cast<uint8_t>(tag) == 16)) {
          _Internal::set_has_size(&has_bits);
          _impl_.size_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else
          goto handle_unusual;
        continue;
      // .uprotocol.v1.Checksum checksum = 3;
      case 3:
        if (PROTOBUF_PREDICT_TRUE(static_cast<uint8_t>(tag) == 26)) {
          ptr = ctx->ParseMessage(_internal_mutable_checksum(), ptr);
          CHK_(ptr);
        } else
          goto handle_unusual;
        continue;
      default:
        goto handle_unusual;
    }  // switch
  handle_unusual:
    if ((tag == 0) || ((tag & 7) == 4)) {
      CHK_(ptr);
      ctx->SetLastTag(tag);
      goto message_done;
    }
    ptr = UnknownFieldParse(
        tag,
        _internal_metadata_.mutable_unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(),
        ptr, ctx);
    CHK_(ptr != nullptr);
  }  // while
message_done:
  _impl_._has_bits_.Or(has_bits);
  return ptr;
failure:
  ptr = nullptr;
  goto message_done;
#undef CHK_
}

uint8_t* File::_InternalSerialize(
    uint8_t* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {
  // @@protoc_insertion_point(serialize_to_array_start:uprotocol.v1.File)
  uint32_t cached_has_bits = 0;
  (void) cached_has_bits;

  // string name = 1;
  if (!this->_internal_name().empty()) {
    ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::VerifyUtf8String(
      this->_internal_name().data(), static_cast<int>(this->_internal_name().length()),
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::SERIALIZE,
      "uprotocol.v1.File.name");
    target = stream->WriteStringMaybeAliased(
        1, this->_internal_name(), target);
  }

  // optional uint64 size = 2;
  if (_internal_has_size()) {
    target = stream->EnsureSpace(target);
    target = ::_pbi::WireFormatLite::WriteUInt64ToArray(2, this->_internal_size(), target);
  }

  // .uprotocol.v1.Checksum checksum = 3;
  if (this->_internal_has_checksum()) {
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::
      InternalWriteMessage(3, _Internal::checksum(this),
        _Internal::checksum(this).GetCachedSize(), target, stream);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = ::_pbi::WireFormat::InternalSerializeUnknownFieldsToArray(
        _internal_metadata_.unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(::PROTOBUF_NAMESPACE_ID::UnknownFieldSet::default_instance), target, stream);
  }
  // @@protoc_insertion_point(serialize_to_array_end:uprotocol.v1.File)
  return target;
}

size_t File::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:uprotocol.v1.File)
  size_t total_size = 0;

  uint32_t cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  // string name = 1;
  if (!this->_internal_name().empty()) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
        this->_internal_name());
  }

  // .uprotocol.v1.Checksum checksum = 3;
  if (this->_internal_has_checksum()) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::MessageSize(
        *_impl_.checksum_);
  }

  // optional uint64 size = 2;
  cached_has_bits = _impl_._has_bits_[0];
  if (cached_has_bits & 0x00000001u) {
    total_size += ::_pbi::WireFormatLite::UInt64SizePlusOne(this->_internal_size());
  }

  return MaybeComputeUnknownFieldsSize(total_size, &_impl_._cached_size_);
}

const ::PROTOBUF_NAMESPACE_ID::Message::ClassData File::_class_data_ = {
    ::PROTOBUF_NAMESPACE_ID::Message::CopyWithSourceCheck,
    File::MergeImpl
};
const ::PROTOBUF_NAMESPACE_ID::Message::ClassData*File::GetClassData() const { return &_class_data_; }


void File::MergeImpl(::PROTOBUF_NAMESPACE_ID::Message& to_msg, const ::PROTOBUF_NAMESPACE_ID::Message& from_msg) {
  auto* const _this = static_cast<File*>(&to_msg);
  auto& from = static_cast<const File&>(from_msg);
  // @@protoc_insertion_point(class_specific_merge_from_start:uprotocol.v1.File)
  GOOGLE_DCHECK_NE(&from, _this);
  uint32_t cached_has_bits = 0;
  (void) cached_has_bits;

  if (!from._internal_name().empty()) {
    _this->_internal_set_name(from._internal_name());
  }
  if (from._internal_has_checksum()) {
    _this->_internal_mutable_checksum()->::uprotocol::v1::Checksum::MergeFrom(
        from._internal_checksum());
  }
  if (from._internal_has_size()) {
    _this->_internal_set_size(from._internal_size());
  }
  _this->_internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
}

void File::CopyFrom(const File& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:uprotocol.v1.File)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool File::IsInitialized() const {
  return true;
}

void File::InternalSwap(File* other) {
  using std::swap;
  auto* lhs_arena = GetArenaForAllocation();
  auto* rhs_arena = other->GetArenaForAllocation();
  _internal_metadata_.InternalSwap(&other->_internal_metadata_);
  swap(_impl_._has_bits_[0], other->_impl_._has_bits_[0]);
  ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::InternalSwap(
      &_impl_.name_, lhs_arena,
      &other->_impl_.name_, rhs_arena
  );
  ::PROTOBUF_NAMESPACE_ID::internal::memswap<
      PROTOBUF_FIELD_OFFSET(File, _impl_.size_)
      + sizeof(File::_impl_.size_)
      - PROTOBUF_FIELD_OFFSET(File, _impl_.checksum_)>(
          reinterpret_cast<char*>(&_impl_.checksum_),
          reinterpret_cast<char*>(&other->_impl_.checksum_));
}

::PROTOBUF_NAMESPACE_ID::Metadata File::GetMetadata() const {
  return ::_pbi::AssignDescriptors(
      &descriptor_table_file_2eproto_getter, &descriptor_table_file_2eproto_once,
      file_level_metadata_file_2eproto[0]);
}

// ===================================================================

class FileBatch::_Internal {
 public:
};

FileBatch::FileBatch(::PROTOBUF_NAMESPACE_ID::Arena* arena,
                         bool is_message_owned)
  : ::PROTOBUF_NAMESPACE_ID::Message(arena, is_message_owned) {
  SharedCtor(arena, is_message_owned);
  // @@protoc_insertion_point(arena_constructor:uprotocol.v1.FileBatch)
}
FileBatch::FileBatch(const FileBatch& from)
  : ::PROTOBUF_NAMESPACE_ID::Message() {
  FileBatch* const _this = this; (void)_this;
  new (&_impl_) Impl_{
      decltype(_impl_.files_){from._impl_.files_}
    , /*decltype(_impl_._cached_size_)*/{}};

  _internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
  // @@protoc_insertion_point(copy_constructor:uprotocol.v1.FileBatch)
}

inline void FileBatch::SharedCtor(
    ::_pb::Arena* arena, bool is_message_owned) {
  (void)arena;
  (void)is_message_owned;
  new (&_impl_) Impl_{
      decltype(_impl_.files_){arena}
    , /*decltype(_impl_._cached_size_)*/{}
  };
}

FileBatch::~FileBatch() {
  // @@protoc_insertion_point(destructor:uprotocol.v1.FileBatch)
  if (auto *arena = _internal_metadata_.DeleteReturnArena<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>()) {
  (void)arena;
    return;
  }
  SharedDtor();
}

inline void FileBatch::SharedDtor() {
  GOOGLE_DCHECK(GetArenaForAllocation() == nullptr);
  _impl_.files_.~RepeatedPtrField();
}

void FileBatch::SetCachedSize(int size) const {
  _impl_._cached_size_.Set(size);
}

void FileBatch::Clear() {
// @@protoc_insertion_point(message_clear_start:uprotocol.v1.FileBatch)
  uint32_t cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  _impl_.files_.Clear();
  _internal_metadata_.Clear<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>();
}

const char* FileBatch::_InternalParse(const char* ptr, ::_pbi::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  while (!ctx->Done(&ptr)) {
    uint32_t tag;
    ptr = ::_pbi::ReadTag(ptr, &tag);
    switch (tag >> 3) {
      // repeated .uprotocol.v1.File files = 1;
      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<uint8_t>(tag) == 10)) {
          ptr -= 1;
          do {
            ptr += 1;
            ptr = ctx->ParseMessage(_internal_add_files(), ptr);
            CHK_(ptr);
            if (!ctx->DataAvailable(ptr)) break;
          } while (::PROTOBUF_NAMESPACE_ID::internal::ExpectTag<10>(ptr));
        } else
          goto handle_unusual;
        continue;
      default:
        goto handle_unusual;
    }  // switch
  handle_unusual:
    if ((tag == 0) || ((tag & 7) == 4)) {
      CHK_(ptr);
      ctx->SetLastTag(tag);
      goto message_done;
    }
    ptr = UnknownFieldParse(
        tag,
        _internal_metadata_.mutable_unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(),
        ptr, ctx);
    CHK_(ptr != nullptr);
  }  // while
message_done:
  return ptr;
failure:
  ptr = nullptr;
  goto message_done;
#undef CHK_
}

uint8_t* FileBatch::_InternalSerialize(
    uint8_t* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {
  // @@protoc_insertion_point(serialize_to_array_start:uprotocol.v1.FileBatch)
  uint32_t cached_has_bits = 0;
  (void) cached_has_bits;

  // repeated .uprotocol.v1.File files = 1;
  for (unsigned i = 0,
      n = static_cast<unsigned>(this->_internal_files_size()); i < n; i++) {
    const auto& repfield = this->_internal_files(i);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::
        InternalWriteMessage(1, repfield, repfield.GetCachedSize(), target, stream);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = ::_pbi::WireFormat::InternalSerializeUnknownFieldsToArray(
        _internal_metadata_.unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(::PROTOBUF_NAMESPACE_ID::UnknownFieldSet::default_instance), target, stream);
  }
  // @@protoc_insertion_point(serialize_to_array_end:uprotocol.v1.FileBatch)
  return target;
}

size_t FileBatch::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:uprotocol.v1.FileBatch)
  size_t total_size = 0;

  uint32_t cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  // repeated .uprotocol.v1.File files = 1;
  total_size += 1UL * this->_internal_files_size();
  for (const auto& msg : this->_impl_.files_) {
    total_size +=
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::MessageSize(msg);
  }

  return MaybeComputeUnknownFieldsSize(total_size, &_impl_._cached_size_);
}

const ::PROTOBUF_NAMESPACE_ID::Message::ClassData FileBatch::_class_data_ = {
    ::PROTOBUF_NAMESPACE_ID::Message::CopyWithSourceCheck,
    FileBatch::MergeImpl
};
const ::PROTOBUF_NAMESPACE_ID::Message::ClassData*FileBatch::GetClassData() const { return &_class_data_; }


void FileBatch::MergeImpl(::PROTOBUF_NAMESPACE_ID::Message& to_msg, const ::PROTOBUF_NAMESPACE_ID::Message& from_msg) {
  auto* const _this = static_cast<FileBatch*>(&to_msg);
  auto& from = static_cast<const FileBatch&>(from_msg);
  // @@protoc_insertion_point(class_specific_merge_from_start:uprotocol.v1.FileBatch)
  GOOGLE_DCHECK_NE(&from, _this);
  uint32_t cached_has_bits = 0;
  (void) cached_has_bits;

  _this->_impl_.files_.MergeFrom(from._impl_.files_);
  _this->_internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
}

void FileBatch::CopyFrom(const FileBatch& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:uprotocol.v1.FileBatch)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool FileBatch::IsInitialized() const {
  return true;
}

void FileBatch::InternalSwap(FileBatch* other) {
  using std::swap;
  _internal_metadata_.InternalSwap(&other->_internal_metadata_);
  _impl_.files_.InternalSwap(&other->_impl_.files_);
}

::PROTOBUF_NAMESPACE_ID::Metadata FileBatch::GetMetadata() const {
  return ::_pbi::AssignDescriptors(
      &descriptor_table_file_2eproto_getter, &descriptor_table_file_2eproto_once,
      file_level_metadata_file_2eproto[1]);
}

// ===================================================================

class Checksum::_Internal {
 public:
};

Checksum::Checksum(::PROTOBUF_NAMESPACE_ID::Arena* arena,
                         bool is_message_owned)
  : ::PROTOBUF_NAMESPACE_ID::Message(arena, is_message_owned) {
  SharedCtor(arena, is_message_owned);
  // @@protoc_insertion_point(arena_constructor:uprotocol.v1.Checksum)
}
Checksum::Checksum(const Checksum& from)
  : ::PROTOBUF_NAMESPACE_ID::Message() {
  Checksum* const _this = this; (void)_this;
  new (&_impl_) Impl_{
      decltype(_impl_.value_){}
    , decltype(_impl_.type_){}
    , /*decltype(_impl_._cached_size_)*/{}};

  _internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
  _impl_.value_.InitDefault();
  #ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
    _impl_.value_.Set("", GetArenaForAllocation());
  #endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
  if (!from._internal_value().empty()) {
    _this->_impl_.value_.Set(from._internal_value(), 
      _this->GetArenaForAllocation());
  }
  _this->_impl_.type_ = from._impl_.type_;
  // @@protoc_insertion_point(copy_constructor:uprotocol.v1.Checksum)
}

inline void Checksum::SharedCtor(
    ::_pb::Arena* arena, bool is_message_owned) {
  (void)arena;
  (void)is_message_owned;
  new (&_impl_) Impl_{
      decltype(_impl_.value_){}
    , decltype(_impl_.type_){0}
    , /*decltype(_impl_._cached_size_)*/{}
  };
  _impl_.value_.InitDefault();
  #ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
    _impl_.value_.Set("", GetArenaForAllocation());
  #endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
}

Checksum::~Checksum() {
  // @@protoc_insertion_point(destructor:uprotocol.v1.Checksum)
  if (auto *arena = _internal_metadata_.DeleteReturnArena<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>()) {
  (void)arena;
    return;
  }
  SharedDtor();
}

inline void Checksum::SharedDtor() {
  GOOGLE_DCHECK(GetArenaForAllocation() == nullptr);
  _impl_.value_.Destroy();
}

void Checksum::SetCachedSize(int size) const {
  _impl_._cached_size_.Set(size);
}

void Checksum::Clear() {
// @@protoc_insertion_point(message_clear_start:uprotocol.v1.Checksum)
  uint32_t cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  _impl_.value_.ClearToEmpty();
  _impl_.type_ = 0;
  _internal_metadata_.Clear<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>();
}

const char* Checksum::_InternalParse(const char* ptr, ::_pbi::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  while (!ctx->Done(&ptr)) {
    uint32_t tag;
    ptr = ::_pbi::ReadTag(ptr, &tag);
    switch (tag >> 3) {
      // .uprotocol.v1.ChecksumType type = 1;
      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<uint8_t>(tag) == 8)) {
          uint64_t val = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
          _internal_set_type(static_cast<::uprotocol::v1::ChecksumType>(val));
        } else
          goto handle_unusual;
        continue;
      // bytes value = 2;
      case 2:
        if (PROTOBUF_PREDICT_TRUE(static_cast<uint8_t>(tag) == 18)) {
          auto str = _internal_mutable_value();
          ptr = ::_pbi::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
        } else
          goto handle_unusual;
        continue;
      default:
        goto handle_unusual;
    }  // switch
  handle_unusual:
    if ((tag == 0) || ((tag & 7) == 4)) {
      CHK_(ptr);
      ctx->SetLastTag(tag);
      goto message_done;
    }
    ptr = UnknownFieldParse(
        tag,
        _internal_metadata_.mutable_unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(),
        ptr, ctx);
    CHK_(ptr != nullptr);
  }  // while
message_done:
  return ptr;
failure:
  ptr = nullptr;
  goto message_done;
#undef CHK_
}

uint8_t* Checksum::_InternalSerialize(
    uint8_t* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {
  // @@protoc_insertion_point(serialize_to_array_start:uprotocol.v1.Checksum)
  uint32_t cached_has_bits = 0;
  (void) cached_has_bits;

  // .uprotocol.v1.ChecksumType type = 1;
  if (this->_internal_type() != 0) {
    target = stream->EnsureSpace(target);
    target = ::_pbi::WireFormatLite::WriteEnumToArray(
      1, this->_internal_type(), target);
  }

  // bytes value = 2;
  if (!this->_internal_value().empty()) {
    target = stream->WriteBytesMaybeAliased(
        2, this->_internal_value(), target);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = ::_pbi::WireFormat::InternalSerializeUnknownFieldsToArray(
        _internal_metadata_.unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(::PROTOBUF_NAMESPACE_ID::UnknownFieldSet::default_instance), target, stream);
  }
  // @@protoc_insertion_point(serialize_to_array_end:uprotocol.v1.Checksum)
  return target;
}

size_t Checksum::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:uprotocol.v1.Checksum)
  size_t total_size = 0;

  uint32_t cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  // bytes value = 2;
  if (!this->_internal_value().empty()) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::BytesSize(
        this->_internal_value());
  }

  // .uprotocol.v1.ChecksumType type = 1;
  if (this->_internal_type() != 0) {
    total_size += 1 +
      ::_pbi::WireFormatLite::EnumSize(this->_internal_type());
  }

  return MaybeComputeUnknownFieldsSize(total_size, &_impl_._cached_size_);
}

const ::PROTOBUF_NAMESPACE_ID::Message::ClassData Checksum::_class_data_ = {
    ::PROTOBUF_NAMESPACE_ID::Message::CopyWithSourceCheck,
    Checksum::MergeImpl
};
const ::PROTOBUF_NAMESPACE_ID::Message::ClassData*Checksum::GetClassData() const { return &_class_data_; }


void Checksum::MergeImpl(::PROTOBUF_NAMESPACE_ID::Message& to_msg, const ::PROTOBUF_NAMESPACE_ID::Message& from_msg) {
  auto* const _this = static_cast<Checksum*>(&to_msg);
  auto& from = static_cast<const Checksum&>(from_msg);
  // @@protoc_insertion_point(class_specific_merge_from_start:uprotocol.v1.Checksum)
  GOOGLE_DCHECK_NE(&from, _this);
  uint32_t cached_has_bits = 0;
  (void) cached_has_bits;

  if (!from._internal_value().empty()) {
    _this->_internal_set_value(from._internal_value());
  }
  if (from._internal_type() != 0) {
    _this->_internal_set_type(from._internal_type());
  }
  _this->_internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
}

void Checksum::CopyFrom(const Checksum& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:uprotocol.v1.Checksum)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool Checksum::IsInitialized() const {
  return true;
}

void Checksum::InternalSwap(Checksum* other) {
  using std::swap;
  auto* lhs_arena = GetArenaForAllocation();
  auto* rhs_arena = other->GetArenaForAllocation();
  _internal_metadata_.InternalSwap(&other->_internal_metadata_);
  ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::InternalSwap(
      &_impl_.value_, lhs_arena,
      &other->_impl_.value_, rhs_arena
  );
  swap(_impl_.type_, other->_impl_.type_);
}

::PROTOBUF_NAMESPACE_ID::Metadata Checksum::GetMetadata() const {
  return ::_pbi::AssignDescriptors(
      &descriptor_table_file_2eproto_getter, &descriptor_table_file_2eproto_once,
      file_level_metadata_file_2eproto[2]);
}

// @@protoc_insertion_point(namespace_scope)
}  // namespace v1
}  // namespace uprotocol
PROTOBUF_NAMESPACE_OPEN
template<> PROTOBUF_NOINLINE ::uprotocol::v1::File*
Arena::CreateMaybeMessage< ::uprotocol::v1::File >(Arena* arena) {
  return Arena::CreateMessageInternal< ::uprotocol::v1::File >(arena);
}
template<> PROTOBUF_NOINLINE ::uprotocol::v1::FileBatch*
Arena::CreateMaybeMessage< ::uprotocol::v1::FileBatch >(Arena* arena) {
  return Arena::CreateMessageInternal< ::uprotocol::v1::FileBatch >(arena);
}
template<> PROTOBUF_NOINLINE ::uprotocol::v1::Checksum*
Arena::CreateMaybeMessage< ::uprotocol::v1::Checksum >(Arena* arena) {
  return Arena::CreateMessageInternal< ::uprotocol::v1::Checksum >(arena);
}
PROTOBUF_NAMESPACE_CLOSE

// @@protoc_insertion_point(global_scope)
#include <google/protobuf/port_undef.inc>
