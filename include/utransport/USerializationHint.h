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
                UNKNOWN,
                PROTOBUF,
                JSON,
                SOMEIP,
                RAW
            };
        }
    }
}

#endif 