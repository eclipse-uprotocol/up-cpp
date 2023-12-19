#ifndef _USTATUS_H_
#define _USTATUS_H_

namespace uprotocol::utransport {
    
    enum class UStatus 
    { 
        OK =                    0,
        CANCELLED =             1,
        UNKNOWN =               2,
        INVALID_ARGUMENT =      3,
        DEADLINE_EXCEEDED =     4,
        NOT_FOUND =             5,
        ALREADY_EXISTS =        6,
        PERMISSION_DENIED =     7,
        UNAUTHENTICATED =       8,
        RESOURCE_EXHAUSTED =    9,
        FAILED_PRECONDITION =   10,
        ABORTED =               11,
        OUT_OF_RANGE =          12,
        UNIMPLEMENTED =         13,
        INTERNAL =              14,
        UNAVAILABLE =           15,
        DATA_LOSS =             16,
        UNSPECIFIED =           -1
    };
}

#endif /* _USTATUS_H_*/
