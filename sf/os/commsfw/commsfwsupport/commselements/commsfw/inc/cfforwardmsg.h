// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//



/**
@file 
@internalTechnology
*/


#ifndef __CFFORWARDMSG_H__
#define __CFFORWARDMSG_H__

#include <cfmsgs.h>

namespace CommsFW
{
class TCFForwardMsg : public TCFCommsMessage
/** To forwards the root server client message (of type RMessage2) to a module worker thread.
The module is expected to complete message.
The CCFModuleChannelHandler subclass i.e. CWorkerRootServChannelHandler::CFMessageForward calls
CWorkerThread::CFMessageForward that serves the following message types 
@see TCFMessageType in cfshared.h
A factory message
	EFactoryMsg,	//message for factories' instances residing in the module thread
The format of the RMessage2 for any forward message is expected as follows
	Ptr2[in/out]== ptr to serialised instance of a class CNetMessage:
	class CNetMessage : public SMetaData
		{
		}

@internalTechnology
@released
*/
	{
public:
    /** Code identifying this message type. Will be first parameter in
    the physical message.
    @see TCFCommsMessage
    */
	enum {KCode = TCFCommsMessage::ECodeForward};

	typedef struct TParamsTAG
	/** Structure defining the layout of the parameters in the message.
	*/
		{
		/** The shutdown behaviour expected.
		@see TCFShutdownType
		*/
		RMessage2 iMsg;
		} TParams;
	
	inline TCFForwardMsg( const RMessage2& aMsg ) :
		TCFCommsMessage( TCFCommsMessage::ECodeForward )
		{
		TParams* params = const_cast<TParams*>(
		                  reinterpret_cast<const TParams*>(
		                      TCFMessage::ParamBlock() ) );
		params->iMsg = aMsg;
		}
	inline const RMessage2& Msg() const
		{
		TParams* params = const_cast<TParams*>(
		                  reinterpret_cast<const TParams*>(
		                      TCFMessage::ParamBlock() ) );
		return params->iMsg;
		}
	};

} // namespace CommsFW

#endif // __CFFORWARDMSG_H__

