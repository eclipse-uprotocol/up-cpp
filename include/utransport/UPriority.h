#ifndef _UPRIORITY_H_
#define _UPRIORITY_H_

#include <iostream>
#include <optional>

namespace uprotocol 
{
    namespace utransport
    {
        namespace datamodel
        {
            enum UPriority
            {
                LOW =  0,                   // Low Priority. No bandwidth assurance such as File Transfer.
                STANDARD = 1,               // Standard, undifferentiated application such as General (unclassified).
                OPERATIONS =  2,            // Operations, Administration, and Management such as Streamer messages (sub, connect, etc…)
                MULTIMEDIA_STREAMING = 3,   // Multimedia streaming such as Video Streaming
                REALTIME_INTERACTIVE = 4,   // Real-time interactive such as High priority (rpc events)
                SIGNALING =  5,             // Signaling such as Important
                NETWORK_CONTROL =  6,       // Network control such as Safety Critical
                UNDEFINED = 7               // UNDEFINED

            };

            std::optional<UPriority> UPriorityFromInt(int32_t value) 
            {
                switch (value) 
                {
                    case 0:
                    {
                        return UPriority::LOW;
                    }
                    case 1: 
                    {
                        return UPriority::STANDARD;
                    }
                    case 2: 
                    {
                        return UPriority::OPERATIONS;
                    }
                    case 3:
                    {
                        return UPriority::MULTIMEDIA_STREAMING;
                    }
                    case 4:
                    {
                        return UPriority::REALTIME_INTERACTIVE;
                    }
                    case 5:
                    {
                        return UPriority::SIGNALING;
                    }
                    case 6:
                    {
                        return UPriority::NETWORK_CONTROL;
                    }
                    case 7:
                    {
                        return UPriority::UNDEFINED;
                    }
                    default: 
                    {
                         return std::nullopt; 
                    }
                }
            }

            std::optional<UPriority> UPriorityFromString(const std::string& value)
            {
                if (value == "CS0") 
                {
                    return UPriority::LOW;
                }
               
                if (value == "CS1") 
                {
                    return UPriority::STANDARD;
                }
               
                if (value == "CS2") 
                {
                    return UPriority::OPERATIONS;
                }

                if (value == "CS3") 
                {
                    return UPriority::MULTIMEDIA_STREAMING;
                }

                if (value == "CS4") 
                {
                    return UPriority::REALTIME_INTERACTIVE;
                }

                if (value == "CS5") 
                {
                    return UPriority::SIGNALING;
                }
                
                if (value == "CS6") 
                {
                    return UPriority::NETWORK_CONTROL;
                }             

                return std::nullopt; 
            }

            std::optional<std::string> UPriorityToString(UPriority value)
            {
                switch (value) 
                {
                    case UPriority::LOW:
                    {
                        return "CS0";
                    }
                    case UPriority::STANDARD: 
                    {
                        return "CS1";
                    }
                    case UPriority::OPERATIONS: 
                    {
                        return "CS2";
                    }
                    case UPriority::MULTIMEDIA_STREAMING: 
                    {
                        return "CS3";
                    }
                    case UPriority::REALTIME_INTERACTIVE: 
                    {
                        return "CS4";
                    }
                    case UPriority::SIGNALING: 
                    {
                        return "CS5";
                    }
                    case UPriority::NETWORK_CONTROL: 
                    {
                        return "CS6";
                    }
                    default: 
                    {
                         return std::nullopt; 
                    }
                }
            }

            std::optional<int32_t> UPriorityToInt(UPriority value)
            {
                switch (value) 
                {
                    case UPriority::LOW:
                    {
                        return 0;
                    }
                    case UPriority::STANDARD: 
                    {
                        return 1;
                    }
                    case UPriority::OPERATIONS: 
                    {
                        return 2;
                    }
                    case UPriority::MULTIMEDIA_STREAMING: 
                    {
                        return 3;
                    }
                    case UPriority::REALTIME_INTERACTIVE: 
                    {
                        return 4;
                    }
                    case UPriority::SIGNALING: 
                    {
                        return 5;
                    }
                    case UPriority::NETWORK_CONTROL: 
                    {
                        return 6;
                    }
                    default: 
                    {
                         return std::nullopt; 
                    }
                }
            }
        }
    }
}

#endif /* _UPRIORITY_H_ */