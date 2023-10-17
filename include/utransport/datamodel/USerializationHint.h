#ifndef _USERIALIZATIONHINT_H_
#define _USERIALIZATIONHINT_H_

#include <iostream>
#include <optional>

namespace uprotocol::utransport {
    // Define an enumeration for USerializationHint
    enum class USerializationHint {
        UNKNOWN = 0,    
        PROTOBUF = 1,   // data is a Base64 encoded protobuf string
        JSON = 2,       // data is a UTF-8 string containing a JSON structure
        SOMEIP = 3,     // data is a UTF-8 string containing a JSON structure
        RAW = 4         // data is a Base64 encoded protobuf string of an Any object with the payload inside
    };

    inline std::optional<USerializationHint> USerializationHintFromInt(int32_t value) {

        switch (value) {
            case 0: {
                return USerializationHint::UNKNOWN;
            }
            case 1: {
                return USerializationHint::PROTOBUF;
            }
            case 2: {
                return USerializationHint::JSON;
            }
            case 3: {
                return USerializationHint::SOMEIP;
            }
            case 4: {
                return USerializationHint::RAW;
            }
            default: {
                    return std::nullopt; 
            }
        }
    }

    inline std::optional<USerializationHint> USerializationHintFromString(const std::string& value) {

        if (value == "") {
            return USerializationHint::UNKNOWN;
        }
        
        if (value == "application/x-protobuf") {
            return USerializationHint::PROTOBUF;
        }
        
        if (value == "application/json") {
            return USerializationHint::JSON;
        }

        if (value == "application/x-someip") {
            return USerializationHint::SOMEIP;
        }

        if (value == "application/octet-stream") {
            return USerializationHint::RAW;
        }

        return std::nullopt; 
    }

    inline std::optional<std::string> USerializationHintToString(USerializationHint value) {

        switch (value) {
            case USerializationHint::UNKNOWN: {
                return "";
            }
            case USerializationHint::PROTOBUF: {
                return "application/x-protobuf";
            }
            case USerializationHint::JSON: {
                return "application/json";
            }
            case USerializationHint::SOMEIP: {
                return "application/x-someip";
            }
            case USerializationHint::RAW: {
                return "application/octet-stream";
            }
            default: {
                    return std::nullopt; 
            }
        }
    }

    inline std::optional<int32_t> USerializationHintToInt(USerializationHint value) {

        switch (value) {
            case USerializationHint::UNKNOWN: {
                return 0;
            }
            case USerializationHint::PROTOBUF: {
                return 1;
            }
            case USerializationHint::JSON: {
                return 2;
            }
            case USerializationHint::SOMEIP: {
                return 3;
            }
            case USerializationHint::RAW: {
                return 4;
            }
            default: {
                    return std::nullopt; 
            }
        }
    }
}

#endif 