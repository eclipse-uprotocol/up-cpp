#ifndef _UTRANSPORT_
#define _UTRANSPORT_

#include <stdint.h>
#include <cstddef>
#include "UListener.h"
#include "UPayload.h"
#include "UAttributes.h"
#include <include/uri/up_uri.h>

using namespace uprotocol::utransport::datamodel;
using namespace uri_datamodel;

namespace uprotocol 
{
    namespace utransport
    {
          class UTransport 
          {
               public:

                    virtual UStatus send(
                         UUri &uri, 
                         UPayload &payload,
                         UAttributes &attributes) = 0;

                    virtual UStatus registerListener(
                         UUri &uri,
                         UListener &listner) = 0;

                    virtual UStatus unregisterListener(
                         UUri &uri, 
                         UListener &listner) = 0;

                    virtual UStatus receive(
                         UUri &uri, 
                         UPayload &payload, 
                         UAttributes &attributes) = 0;
                    
                    virtual ~UTransport() {} 
          };
    }
}

#endif /*_UTRANSPORT_*/