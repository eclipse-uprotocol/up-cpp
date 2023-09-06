#ifndef _USTATUS_H_
#define _USTATUS_H_

#include <google/rpc/code.pb.h>

using namespace google::rpc;

namespace uprotocol 
{
    namespace utransport
    {
        namespace datamodel
        {
            enum UStatus 
            { 
                OK =                    Code::OK,
                CANCELLED =             Code::CANCELLED,
                UNKNOWN =               Code::UNKNOWN,
                INVALID_ARGUMENT =      Code::INVALID_ARGUMENT,
                DEADLINE_EXCEEDED =     Code::DEADLINE_EXCEEDED,
                NOT_FOUND =             Code::NOT_FOUND,
                ALREADY_EXISTS =        Code::ALREADY_EXISTS,
                PERMISSION_DENIED =     Code::PERMISSION_DENIED,
                UNAUTHENTICATED =       Code::UNAUTHENTICATED,
                RESOURCE_EXHAUSTED =    Code::RESOURCE_EXHAUSTED,
                FAILED_PRECONDITION =   Code::FAILED_PRECONDITION,
                ABORTED =               Code::ABORTED,
                OUT_OF_RANGE =          Code::OUT_OF_RANGE,
                UNIMPLEMENTED =         Code::UNIMPLEMENTED,
                INTERNAL =              Code::INTERNAL,
                UNAVAILABLE =           Code::UNAVAILABLE,
                DATA_LOSS =             Code::DATA_LOSS,
                UNSPECIFIED =           -1
            };
        }
    }
}

#endif /* _USTATUS_H_*/