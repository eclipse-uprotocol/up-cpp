#ifndef _ULISTENER_H_
#define _ULISTENER_H_

#include <uprotocol-cpp/uri/datamodel/UUri.h>
#include <uprotocol-cpp/transport/datamodel/UAttributes.h>
#include <src/main/proto/ustatus.pb.h>
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
			virtual UCode onReceive(const UUri &uri, 
									const UPayload &payload, 
									const UAttributes &attributes) const = 0; 

			virtual ~UListener() {} 
	};
}


#endif /* _ULISTENER_H_*/