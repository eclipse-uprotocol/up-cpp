#ifndef _UTRANSPORT_
#define _UTRANSPORT_

#include <stdint.h>
#include <cstddef>
#include <uprotocol/transport/datamodel/UListener.h>
#include <uprotocol/transport/datamodel/UPayload.h>
#include <uprotocol/transport/datamodel/UAttributes.h>
#include <uri/datamodel/UUri.h>

using namespace uprotocol::utransport;
using namespace uprotocol::uri;

namespace uprotocol::utransport
{
     class UTransport 
     {
          public:

               /**
               * API used to authenticate with the underlining transport layer that the uEntity passed
               * matches the transport specific identity. MUST pass a resolved UUri.
               * @param uEntity Resolved UEntity
               * @return Returns OKSTATUS if authenticate was successful, FAILSTATUS if the calling uE 
               * is not authenticated.
               */
               virtual UStatus authenticate(const UEntity &uEntity) = 0;
               
               /**
               * Transmit UPayload to the topic using the attributes defined in UTransportAttributes.
               * @param topic Resolved UUri topic to send the payload to.
               * @param payload Actual payload.
               * @param attributes Additional transport attributes.
               * @return Returns OKSTATUS if the payload has been successfully sent (ACK'ed), otherwise it
               * returns FAILSTATUS with the appropriate failure.
               */
               virtual UStatus send(const UUri &uri, 
                                    const UPayload &payload,
                                    const UAttributes &attributes) = 0;

               /**
               * Register listener to be called when UPayload is received for the specific topic.
               * @param topic Resolved UUri for where the message arrived via the underlying transport technology.
               * @param listener The method to execute to process the date for the topic.
               * @return Returns OKSTATUS if the listener is unregistered correctly, otherwise it returns FAILSTATUS
               * with the appropriate failure.
               */      
               virtual UStatus registerListener(const UUri &uri,
                                                const UListener &listner) = 0;

               /**
               * Unregister a listener for a given topic. Messages arriving on this topic will no longer be processed
               * by this listener.
               * @param topic Resolved UUri for where the listener was registered to receive messages from.
               * @param listener The method to execute to process the date for the topic.
               * @return Returns OKSTATUS if the listener is unregistered correctly, otherwise it returns FAILSTATUS
               * with the appropriate failure.
               * 
               */
               virtual UStatus unregisterListener(const UUri &uri, 
                                                  const UListener &listner) = 0;

               virtual UStatus receive(const UUri &uri, 
                                       const UPayload &payload, 
                                       const UAttributes &attributes) = 0;
               
               virtual ~UTransport() {} 
     };
}

#endif /*_UTRANSPORT_*/
