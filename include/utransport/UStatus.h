#ifndef _USTATUS_H_
#define _USTATUS_H_

#include <google/rpc/code.pb.h>

namespace uprotocol 
{
    namespace utransport
    {
        namespace datamodel
        {
            enum UStatus 
            { 
                OK =                    com.google.rpc.Code.OK_VALUE,
                CANCELLED =             com.google.rpc.Code.CANCELLED_VALUE,
                UNKNOWN =               com.google.rpc.Code.UNKNOWN_VALUE,
                INVALID_ARGUMENT =      com.google.rpc.Code.INVALID_ARGUMENT_VALUE,
                DEADLINE_EXCEEDED =     com.google.rpc.Code.DEADLINE_EXCEEDED_VALUE,
                NOT_FOUND =             com.google.rpc.Code.NOT_FOUND_VALUE,
                ALREADY_EXISTS =        com.google.rpc.Code.ALREADY_EXISTS_VALUE,
                PERMISSION_DENIED =     com.google.rpc.Code.PERMISSION_DENIED_VALUE,
                UNAUTHENTICATED =       com.google.rpc.Code.UNAUTHENTICATED_VALUE,
                RESOURCE_EXHAUSTED =    com.google.rpc.Code.RESOURCE_EXHAUSTED_VALUE,
                FAILED_PRECONDITION =   com.google.rpc.Code.FAILED_PRECONDITION_VALUE,
                ABORTED =               com.google.rpc.Code.ABORTED_VALUE,
                OUT_OF_RANGE =          com.google.rpc.Code.OUT_OF_RANGE_VALUE,
                UNIMPLEMENTED =         com.google.rpc.Code.UNIMPLEMENTED_VALUE,
                INTERNAL =              com.google.rpc.Code.INTERNAL_VALUE,
                UNAVAILABLE =           com.google.rpc.Code.UNAVAILABLE_VALUE,
                DATA_LOSS =             com.google.rpc.Code.DATA_LOSS_VALUE,
                UNSPECIFIED =           -1
            };
        }
    }
}

#endif /* _USTATUS_H_*/