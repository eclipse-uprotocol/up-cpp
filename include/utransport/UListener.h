#ifndef _ULISTENER_H_
#define _ULISTENER_H_

#include <include/uri/up_uri.h>
#include <include/uri/up_uri_factory.h>
#include <include/model/attributes.h>
#include <uStatus.h>
#include <uPayload.h>

using uUri = uri_datamodel::UURI;
using uAttributes = cloudevents::format::UAttributes;

namespace uprotocol 
{
    namespace Utransport
    {
		class UListener 
		{
			public:

				virtual UStatus onReceive(
					UUri &uri, 
					UPayload &payload, 
					UAttributes &attributes) = 0; 

				virtual ~UListener() {} 
		};
	}
}


#endif /* _ULISTENER_H_*/