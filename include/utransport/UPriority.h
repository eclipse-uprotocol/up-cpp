#ifndef _UPRIORITY_H_
#define _UPRIORITY_H_

namespace uprotocol 
{
    namespace utransport
    {
        namespace datamodel
        {
            enum UPriority
            {
                // Low Priority. No bandwidth assurance such as File Transfer.
                LOW =  0,
                // Standard, undifferentiated application such as General (unclassified).
                STANDARD = 1,
                // Operations, Administration, and Management such as Streamer messages (sub, connect, etc…)
                OPERATIONS =  2,
                // Multimedia streaming such as Video Streaming
                MULTIMEDIA_STREAMING = 3,
                // Real-time interactive such as High priority (rpc events)
                REALTIME_INTERACTIVE = 4,
                // Signaling such as Important
                SIGNALING =  5,
                // Network control such as Safety Critical
                NETWORK_CONTROL =  6,
                // UNKOWN
                UNDEFINED = 7
            };
        }
    }
}

#endif /* _UPRIORITY_H_ */