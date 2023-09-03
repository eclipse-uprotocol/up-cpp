#ifndef _UTRANSPORT_
#define _UTRANSPORT_

#include <stdint.h>
#include <cstddef>
//#include <UAttributes.h>
#include <UListener.h>
#include <UPayload.h>
#include <include/uri/up_uri.h>
#include <include/model/attributes.h>

namespace uprotocol 
{
    namespace Utransport
    {
          class UTransport 
          {
               public:

                    virtual UStatus send(
                         UURI &uri, 
                         UPayload &payload,
                         UAttributes &attributes) = 0;

                    virtual UStatus registerListener(
                         UURI &uri,
                         uListener &listner) = 0;

                    virtual UStatus unregisterListener(
                         UURI &uri, 
                         uListener &listner) = 0;

                    virtual UStatus receive(
                         UURI &uri, 
                         UPayload &payload, 
                         UAttributes &attributes) = 0;
                    
                    virtual ~uTransport() {} 
          };
    }
}

#endif /*_UTRANSPORT_*/