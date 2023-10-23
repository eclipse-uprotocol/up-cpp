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

			/**
			* Method called to handle/process events.
			* @param topic Topic the underlying source of the message.
			* @param payload Payload of the message.
			* @param attributes Transportation attributes
			* @return Returns an Ack every time a message is received and processed.
			*/
			virtual UStatus onReceive(const UUri &uri, 
									  const UPayload &payload, 
									  const UAttributes &attributes) = 0; 

			virtual ~UListener() {} 
	};
}


#endif /* _ULISTENER_H_*/