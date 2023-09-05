#ifndef _USERIALIZATIONHINT_H_
#define _USERIALIZATIONHINT_H_

namespace uprotocol 
{
    namespace utransport
    {
        namespace datamodel
        {
            // Define an enumeration for USerializationHint
            enum class USerializationHint 
            {
                UNKNOWN = 0,
                PROTOBUF = 1,
                JSON = 2,
                SOMEIP = 3,
                RAW = 4
            };
        }
    }
}

#endif 