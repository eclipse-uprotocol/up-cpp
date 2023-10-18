#ifndef _ULISTENER_H_
#define _ULISTENER_H_

#include <include/uri/up_uri.h>
#include <include/uri/up_uri_factory.h>
#include <include/model/attributes.h>
#include "UStatus.h"
#include "UPayload.h"
#include "UAttributes.h"

namespace uprotocol::utransport
{
	class UListener 
	{
		public:

			virtual UStatus onReceive(const UUri &uri, 
									  const UPayload &payload, 
									  const UAttributes &attributes) = 0; 

			virtual ~UListener() {} 
	};
}


#endif /* _ULISTENER_H_*/