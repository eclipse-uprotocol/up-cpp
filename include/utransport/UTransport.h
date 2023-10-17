#ifndef _UTRANSPORT_
#define _UTRANSPORT_

#include <stdint.h>
#include <cstddef>
#include "datamodel/UListener.h"
#include "datamodel/UPayload.h"
#include "datamodel/UAttributes.h"
#include <include/uri/up_uri.h>

using namespace uprotocol::utransport;
using namespace uri_datamodel;

namespace uprotocol::utransport
{
     class UTransport 
     {
          public:

               virtual UStatus send(
                    const UUri &uri, 
                    const UPayload &payload,
                    const UAttributes &attributes) = 0;

               virtual UStatus registerListener(
                    UUri &uri,
                    UListener &listner) = 0;

               virtual UStatus unregisterListener(
                    UUri &uri, 
                    UListener &listner) = 0;

               virtual UStatus receive(
                    const UUri &uri, 
                    const UPayload &payload, 
                    const UAttributes &attributes) = 0;
               
               virtual ~UTransport() {} 
     };
}

#endif /*_UTRANSPORT_*/