#ifndef _RPC_CLIENT_H_
#define _RPC_CLIENT_H_

#include <future>
#include <include/utransport/datamodel/UPayload.h>
#include <include/utransport/datamodel/UAttributes.h>
#include <include/utransport/datamodel/UStatus.h>

using namespace uprotocol::utransport;
using namespace uprotocol::uri;

/**
 * RpcClient is an interface used by code generators for uProtocol services defined in proto files such as
 * the core uProtocol services found in https://github.com/eclipse-uprotocol/uprotocol-core-api. the interface 
 * provides a clean contract for all transports to implement to be able to support RPC on their platform. Each
 * platform MUST implement this interface. For more details please refer to
 * https://github.com/eclipse-uprotocol/uprotocol-spec/blob/main/up-l2/README.adoc[RpcClient Specifications]
 */
class RpcClient 
{
    public:
        /**
        * Support for RPC method invocation.
        * @param topic topic of the method to be invoked (i.e. the name of the API we are calling).
        * @param payload The request message to be sent to the server.
        * @param attributes Metadata for the method invocation (i.e. priority, timeout, etc.)
        * @return Returns the CompletableFuture with the result or exception.
        */
        virtual std::future<UPayload> invokeMethod(const UUri &topic, 
                                                   const UPayload &payload, 
                                                   const UAttributes &attributes) = 0;
        
        virtual ~RpcClient() {} 
};

#endif /*_RPC_CLIENT_H_*/