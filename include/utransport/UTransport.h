#ifndef _UTRANSPORT_
#define _UTRANSPORT_

#include <stdint.h>
#include <cstddef>
//#include <uAttributes.h>
#include <uListener.h>
#include <uPayload.h>
#include <include/uri/up_uri.h>
#include <include/model/attributes.h>

using uUri = uri_datamodel::up_uri;
using uAttributes = cloudevents::format::CE_Attributes;

class uTransport 
{
    public:

        virtual uStatus send(
            uUri &uri, 
            uPayload &payload,
            uAttributes &attributes) = 0;

       virtual uStatus registerListener(
            uUri &uri,
            uListener &listner) = 0;

       virtual uStatus unregisterListener(
            uUri &uri, 
            uListener &listner) = 0;

       virtual uStatus receive(
            uUri &uri, 
            uPayload &payload, 
            uAttributes &attributes) = 0;
        
        virtual ~uTransport() {} 

};

#endif /*_UTRANSPORT_*/