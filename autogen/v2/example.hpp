#pragma once

#include "Builder.hpp"

struct OuterEnum : public BuilderEnum {
    static Table enum_map;
    OuterEnum(std::variant<int,std::string> arg) : BuilderEnum(arg, enum_map) {}
    OuterEnum(int inner_value) : BuilderEnum(inner_value, enum_map) { }
};

#ifdef DECLARE_TABLES
OuterEnum::Table OuterEnum::enum_map = {
    { 1, 1, "one" },
    { 2, 2, "two" },
    { 3, 3, "three" }
};
#endif

struct X {
    BUILDER_EXPOSE(X);
    SCALAR_EXPOSE(X, i, int)
    SCALAR_EXPOSE(X, s, std::string)
    // SCALAR_EXPOSE(X, os, std::optional<std::string>)
    ENUM_EXPOSE(X, e, OuterEnum)
    // VAR_EXPOSE(X, data, std::variant<std::string, double, int>)
};

struct Y {
    BUILDER_EXPOSE(Y)
    SCALAR_EXPOSE(Y, i, int)
    SCALAR_EXPOSE(Y, s, std::string)
    // SCALAR_EXPOSE(os, optional<string>)
    OBJECT_EXPOSE(Y, x, X)
};

