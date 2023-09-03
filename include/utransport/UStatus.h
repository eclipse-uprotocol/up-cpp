#ifndef _USTATUS_H_
#define _USTATUS_H_

namespace uprotocol 
{
    namespace utransport
    {
        namespace datamodel
        {
            enum UStatus 
            { 
                OK  = 1,
                CANCELLED = 2,
                UNKNOWN = 3, 
                INVALID_ARGUMENT = 4,
            
                // DEADLINE_EXCEEDED(com.google.rpc.Code.DEADLINE_EXCEEDED_VALUE),
                // NOT_FOUND(com.google.rpc.Code.NOT_FOUND_VALUE),
                // ALREADY_EXISTS(com.google.rpc.Code.ALREADY_EXISTS_VALUE),
                // PERMISSION_DENIED(com.google.rpc.Code.PERMISSION_DENIED_VALUE),
                // UNAUTHENTICATED(com.google.rpc.Code.UNAUTHENTICATED_VALUE),
                // RESOURCE_EXHAUSTED(com.google.rpc.Code.RESOURCE_EXHAUSTED_VALUE),
                // FAILED_PRECONDITION(com.google.rpc.Code.FAILED_PRECONDITION_VALUE),
                // ABORTED(com.google.rpc.Code.ABORTED_VALUE),
                // OUT_OF_RANGE(com.google.rpc.Code.OUT_OF_RANGE_VALUE),
                // UNIMPLEMENTED(com.google.rpc.Code.UNIMPLEMENTED_VALUE),
                // INTERNAL(com.google.rpc.Code.INTERNAL_VALUE),
                // UNAVAILABLE(com.google.rpc.Code.UNAVAILABLE_VALUE),
                // DATA_LOSS(com.google.rpc.Code.DATA_LOSS_VALUE),
                UNSPECIFIED = 5
            };
        }
    }
}

#endif /* _USTATUS_H_*/