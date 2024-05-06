#include <iostream>
#include "Builder.hpp"
#include "example.hpp"
#include "myidl.pb.h"

using namespace std;

template <typename T>
ostream& operator<<(ostream& os, const optional<T>& arg)
{
    if (arg) os << *arg;
    else os << "null";
    return os;
}


BUILDER_IMPLEMENT(X, tutorial::X);
SCALAR_IMPLEMENT(X, i, int)
SCALAR_IMPLEMENT(X, s, std::string)
// SCALAR_IMPLEMENT(X, os, std::optional<std::string>)
// ENUM_EXPOSE(X, e, OuterEnum)

BUILDER_IMPLEMENT(Y, tutorial::Y);
SCALAR_IMPLEMENT(Y, i, int)
SCALAR_IMPLEMENT(Y, s, std::string)
OBJECT_IMPLEMENT(Y, x, X)
// SCALAR_IMPLEMENT(X, os, std::optional<std::string>)
// ENUM_EXPOSE(e, OuterEnum)