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

#include "l2capSigPacketConnection.h"
#include "l2capSignalHandler.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_L2CAP);
#endif

//					CONNECTION REQUEST COMMAND 

/*static*/ HConnectionRequest* HConnectionRequest::New(TL2CAPPort aLocalPort, TL2CAPPort aRemotePort,
                       								   TUint8 aRTXTimerDuration,
	                                                   TUint16 aERTXTimerDuration)
	{
	LOG_STATIC_FUNC
	HConnectionRequest* cmd = NULL;
	RMBufChain buf;
	TRAPD(rerr, buf.AllocL(KConnectRequestLength));
	if(rerr == KErrNone)
		{
		cmd = new HConnectionRequest(buf, aRTXTimerDuration, aERTXTimerDuration);
		if(cmd)
			{
			// Setup message contents.
			cmd->SetCode(EConnectionRequest);
			cmd->SetID(KUninitialisedID);
			cmd->WriteDataLength();
			cmd->SetPSM(aRemotePort);
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
TInt HConnectionRequest::NewCommandIfValid(RMBufChain& aBuffer, HL2CapCommand*& aCommand)
	{
	LOG_STATIC_FUNC
	// Firstly align the MBufChain.  The maximum size we can align the 
	// MBufChain to is the maximum MBuf size
	__ASSERT_COMPILE(KConnectRequestLength <= KMBufSmallSize);
	
	TInt length = aBuffer.Length();
	if(length == KConnectRequestLength)
		{
		// Don't need to check result as we know that the MBufChain contains
		// at least length bytes as we asked first.
		(void)aBuffer.Align(length);
		aCommand = new HConnectionRequest(aBuffer);
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

HConnectionRequest::HConnectionRequest(RMBufChain& aCommand,
                       				   TUint8 aRTXTimerDuration,
	                                   TUint16 aERTXTimerDuration)
 : HL2CapCommand(aCommand, aRTXTimerDuration, aERTXTimerDuration)
	{
	LOG_FUNC
	}

HConnectionRequest::~HConnectionRequest()
	{
	LOG_FUNC
	}

		
TBool HConnectionRequest::ProcessCommand(CL2CapSignalHandler& aSignalHandler)
	{
	LOG_FUNC
	if(aSignalHandler.HandleConnectionRequest(this))
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

//					CONNECTION RESPONSE COMMAND 
//Constructor to deal command coming in from remote side
HConnectionResponse::HConnectionResponse(RMBufChain& aCommand) 
 : HL2CapCommand(aCommand)
	{
	LOG_FUNC
	}

/*static*/ HConnectionResponse* HConnectionResponse::New(TUint8 aId, TL2CAPPort aLocalPort, TL2CAPPort aRemotePort, TConnectResponseResult aResult, TConnectResponseStatus aStatus)
	{
	LOG_STATIC_FUNC
	HConnectionResponse* cmd = NULL;
	RMBufChain buf;
	TRAPD(rerr, buf.AllocL(KConnectResponseLength));
	if(rerr == KErrNone)
		{
		cmd = new HConnectionResponse(buf);
		if(cmd)
			{
			// Setup message contents.
			cmd->SetCode(EConnectionResponse);
			cmd->SetID(aId);
			cmd->WriteDataLength();
			
			cmd->SetDestinationCID(aLocalPort);
			cmd->SetSourceCID(aRemotePort);
			cmd->SetResult(aResult);
			cmd->SetStatus(aStatus);
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
		KErrCorrupt if the command structure is invalid.valid command.
*/
TInt HConnectionResponse::NewCommandIfValid(RMBufChain& aBuffer, HL2CapCommand*& aCommand)
	{
	LOG_STATIC_FUNC
	// Firstly align the MBufChain.  The maximum size we can align the 
	// MBufChain to is the maximum MBuf size
	__ASSERT_COMPILE(KConnectResponseLength <= KMBufSmallSize);
	
	TInt length = aBuffer.Length();
	if(length == KConnectResponseLength)
		{
		// Don't need to check result as we know that the MBufChain contains
		// at least length bytes as we asked first.
		(void)aBuffer.Align(length);
		aCommand = new HConnectionResponse(aBuffer);
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

HConnectionResponse::~HConnectionResponse()
	{
	LOG_FUNC
	}

TBool HConnectionResponse::ProcessCommand(CL2CapSignalHandler& aSignalHandler)	
	{
	LOG_FUNC
	if(aSignalHandler.HandleConnectionResponse(this))
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

