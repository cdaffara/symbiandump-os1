// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <bluetooth/logger.h>

#include "l2capSigPacketDisconnection.h"
#include "l2capSignalHandler.h"
#include "l2constants.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_L2CAP);
#endif

//					DISCONNECT REQUEST COMMAND 

/*static*/ HDisconnectRequest* HDisconnectRequest::New(TL2CAPPort aLocalPort, TL2CAPPort aRemotePort,
		  	                                           TUint8 aRTXTimerDuration,
	                                                   TUint16 aERTXTimerDuration)
	{
	LOG_STATIC_FUNC
	HDisconnectRequest* cmd = NULL;
	RMBufChain buf;
	TRAPD(rerr, buf.AllocL(KDisconnectRequestLength));
	if(rerr == KErrNone)
		{
		cmd = new HDisconnectRequest(buf, aRTXTimerDuration, aERTXTimerDuration);
		if(cmd)
			{
			// Setup message contents.
			cmd->SetCode(EDisconnectionRequest);
			cmd->SetID(KUninitialisedID);
			cmd->WriteDataLength();
			
			cmd->SetDestinationCID(aRemotePort);
			cmd->SetSourceCID(aLocalPort);
			}
		else
			{
			// Free the allocated buffer.
			buf.Free();
			}
		}
	return cmd;
	}		

/**
Verifies that the buffer contains a structurally correct command. 
This does not ensure that the content of the command is semantically valid.

@param aCommmand A new HL2CapCommand if the buffer contains a structurally valid command.
@param aBuffer The buffer containing the command
@return KErrNone if the command if created.
		KErrNoMemory if the command structure is valid but cannot be created.
		KErrCorrupt if the command structure is invalid.
*/
TInt HDisconnectRequest::NewCommandIfValid(RMBufChain& aBuffer, HL2CapCommand*& aCommand)
	{
	LOG_STATIC_FUNC
	// Firstly align the MBufChain.  The maximum size we can align the 
	// MBufChain to is the maximum MBuf size
	__ASSERT_COMPILE(KDisconnectRequestLength <= KMBufSmallSize);
	
	TInt length = aBuffer.Length();
	if(length == KDisconnectRequestLength)
		{
		// Don't need to check result as we know that the MBufChain contains
		// at least length bytes as we asked first.
		(void)aBuffer.Align(length);
		aCommand = new HDisconnectRequest(aBuffer);
		if(aCommand)
			{
			return KErrNone;
			}
		else
			{
			return KErrNoMemory;
			}
		}
	else
		{
		// Dodge length!
		return KErrCorrupt;
		}
	}

HDisconnectRequest::HDisconnectRequest(RMBufChain& aCommand,
                                       TUint8 aRTXTimerDuration,
                                       TUint16 aERTXTimerDuration)
 : HL2CapCommand(aCommand, aRTXTimerDuration, aERTXTimerDuration)
	{
	LOG_FUNC
	}

HDisconnectRequest::~HDisconnectRequest()
	{
	LOG_FUNC
	}

TBool HDisconnectRequest::ProcessCommand(CL2CapSignalHandler& aSignalHandler)
	{
	LOG_FUNC
	if(aSignalHandler.HandleDisconnectRequest(this))
		{
		// The command has been handled.  Delete it.
		delete this;
		return ETrue;
		}
	else
		{
		return EFalse;
		}
	}



//					DISCONNECT RESPONSE COMMAND 
/*static*/ HDisconnectResponse* HDisconnectResponse::New(TUint8 aId, TL2CAPPort aLocalPort, TL2CAPPort aRemotePort)
	{
	LOG_STATIC_FUNC
	HDisconnectResponse* cmd = NULL;
	RMBufChain buf;
	TRAPD(rerr, buf.AllocL(KDisconnectResponseLength));
	if(rerr == KErrNone)
		{
		cmd = new HDisconnectResponse(buf);
		if(cmd)
			{
			// Setup message contents.
			cmd->SetCode(EDisconnectionResponse);
			cmd->SetID(aId);
			cmd->WriteDataLength();
			
			cmd->SetDestinationCID(aLocalPort);
			cmd->SetSourceCID(aRemotePort);
			}
		else
			{
			// Free the allocated buffer.
			buf.Free();
			}
		}
	return cmd;
	}	
	
/**
Verifies that the buffer contains a structurally correct command. 
This does not ensure that the content of the command is semantically valid.

@param aCommmand A new HL2CapCommand if the buffer contains a structurally valid command.
@param aBuffer The buffer containing the command
@return KErrNone if the command if created.
		KErrNoMemory if the command structure is valid but cannot be created.
		KErrCorrupt if the command structure is invalid.
*/
TInt HDisconnectResponse::NewCommandIfValid(RMBufChain& aBuffer, HL2CapCommand*& aCommand)
	{
	LOG_STATIC_FUNC
	// Firstly align the MBufChain.  The maximum size we can align the 
	// MBufChain to is the maximum MBuf size
	__ASSERT_COMPILE(KDisconnectResponseLength <= KMBufSmallSize);
	
	TInt length = aBuffer.Length();
	if(length == KDisconnectResponseLength)
		{
		// Don't need to check result as we know that the MBufChain contains
		// at least length bytes as we asked first.
		(void)aBuffer.Align(length);
		aCommand = new HDisconnectResponse(aBuffer);
		if(aCommand)
			{
			return KErrNone;
			}
		else
			{
			return KErrNoMemory;
			}
		}
	else
		{
		// Dodge length!
		return KErrCorrupt;
		}
	}

HDisconnectResponse::HDisconnectResponse(RMBufChain& aCommand) 
 : HL2CapCommand(aCommand)
	{
	LOG_FUNC
	}

HDisconnectResponse::~HDisconnectResponse()
	{
	LOG_FUNC
	}

TBool HDisconnectResponse::ProcessCommand(CL2CapSignalHandler& aSignalHandler)	
	{
	LOG_FUNC
	if(aSignalHandler.HandleDisconnectResponse(this))
		{
		// The command has been handled.  Delete it.
		delete this;
		return ETrue;
		}
	else
		{
		return EFalse;
		}
	}
