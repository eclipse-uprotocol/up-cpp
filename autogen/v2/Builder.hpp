#pragma once

#include <memory>
#include <optional>
#include <variant>
#include <vector>

struct BuilderEnum {
    struct Rec {
        int inner_value;
        int value;
        std::string name;
    };
    typedef std::vector<Rec>    Table;

    int inner_value;
    int value;
    std::string name;

    BuilderEnum(const std::variant<int,std::string>& arg, const Table& enum_map);
    BuilderEnum(int inner_value, const Table& enum_map);
};

#define BUILDER_EXPOSE(cls) \
    struct Impl; \
    std::shared_ptr<Impl> pImpl; \
    cls(); \
    cls(std::shared_ptr<Impl>); \
    std::string debug();

#define BUILDER_IMPLEMENT(cls, store_type) \
struct cls::Impl { \
    store_type* storage; \
    bool no_delete; \
    Impl() { storage = new store_type(); no_delete = false; } \
    ~Impl() { if (!no_delete) delete storage; } \
}; \
\
cls::cls() : pImpl(new Impl()) {} \
cls::cls(std::shared_ptr<Impl> arg) : pImpl(arg) {} \
std::string cls::debug() { return pImpl->storage->DebugString(); }



#define SCALAR_EXPOSE(cls, member, member_type) \
    cls& member(const std::optional<member_type>& = std::nullopt); \
    member_type member##_get(const std::optional<member_type>& = std::nullopt);

#define SCALAR_IMPLEMENT(cls, member, member_type) \
cls& cls::member(const std::optional<member_type>& arg) \
{ \
    if (arg) pImpl->storage->set_##member(*arg); \
    return *this; \
} \
member_type cls::member##_get(const std::optional<member_type>& arg) \
{ \
    if (arg) pImpl->storage->set_##member(*arg); \
    return pImpl->storage->member(); \
};



#define ENUM_EXPOSE(cls, member, member_type) \
    cls& member(const std::optional<member_type>& = std::nullopt); \
    member_type member##_get(const std::optional<member_type>& = std::nullopt);

#define ENUM_IMPLEMENT(cls, member, member_type) \
cls& cls::member(const std::optional<member_type>& arg) \
{ \
    if (arg) pImpl->storage->set_##member(static_cast<inner_type>(member_type(*arg).inner_value)); \
    return *this; \
} \
member_type cls::member##_get(const std::optional<member_type>& arg) \
{ \
    if (arg) pImpl->storage->set_##member(static_cast<inner_type>(member_type(*arg).inner_value)); \
    return member_type(pImpl->storage->member()); \
};



#define OBJECT_EXPOSE(cls, member, member_type) \
    member_type member(const std::optional<member_type>& = std::nullopt);

#define OBJECT_IMPLEMENT(cls, member, member_type) \
member_type cls::member(const std::optional<member_type>& arg) \
{ \
    if (arg) *pImpl->storage->mutable_##member() = *arg->pImpl->storage; \
    member_type ret; \
    delete ret.pImpl->storage; \
    ret.pImpl->storage = pImpl->storage->mutable_##member(); \
    ret.pImpl->no_delete = true; \
    return ret; \
};
