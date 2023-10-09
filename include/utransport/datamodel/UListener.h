#ifndef _ULISTENER_H_
#define _ULISTENER_H_

#include <include/uri/up_uri.h>
#include <include/uri/up_uri_factory.h>
#include <include/model/attributes.h>
#include "UStatus.h"
#include "UPayload.h"
#include "UAttributes.h"

using namespace uprotocol::utransport::datamodel;

namespace uprotocol 
{
    namespace utransport
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