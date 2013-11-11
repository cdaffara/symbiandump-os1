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

#include "l2capSigPacketEcho.h"
#include "l2capSignalHandler.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_L2CAP);
#endif

//                   ECHO REQUEST COMMAND 
//For outgoing echo requests driven by an L2Cap IOCTL

HEchoRequest::HEchoRequest(RMBufChain& aCommand,
		  	               TUint8 aRTXTimerDuration,
	                       TUint16 aERTXTimerDuration)
 : HL2CapCommand(aCommand, aRTXTimerDuration, aERTXTimerDuration),
   iEchoResponseHandler(NULL)
	{
	LOG_FUNC
	}

/*static*/ HEchoRequest* HEchoRequest::New(RMBufChain& aData,
		  	                               TUint8 aRTXTimerDuration,
	                                       TUint16 aERTXTimerDuration)
	{
	LOG_STATIC_FUNC
	HEchoRequest* cmd = NULL;
	RMBufChain buf;
	buf.Assign(aData);
	TRAPD(rerr, buf.PrependL(KEchoRequestEmptyLength));
	
	if(rerr == KErrNone)
		{
		cmd = new HEchoRequest(buf, aRTXTimerDuration, aERTXTimerDuration);
		if(cmd)
			{
			// Setup message contents.
			cmd->SetCode(EEchoRequest);
			cmd->SetID(KUninitialisedID);
			cmd->WriteDataLength();
			}
		else
			{
			// Free the allocated buffer.
			buf.Free();
			}
		}
	return cmd;
	}		

/*static*/ HEchoRequest* HEchoRequest::New(TUint8 aRTXTimerDuration,
	                                       TUint16 aERTXTimerDuration)
	{
	LOG_STATIC_FUNC
	HEchoRequest* cmd = NULL;
	RMBufChain buf;
	TRAPD(rerr, buf.AllocL(KEchoRequestEmptyLength));
	
	if(rerr == KErrNone)
		{
		cmd = new HEchoRequest(buf, aRTXTimerDuration, aERTXTimerDuration);
		if(cmd)
			{
			// Setup message contents.
			cmd->SetCode(EEchoRequest);
			cmd->SetID(KUninitialisedID);
			cmd->WriteDataLength();
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
TInt HEchoRequest::NewCommandIfValid(RMBufChain& aBuffer, HL2CapCommand*& aCommand)
	{
	LOG_STATIC_FUNC
	// Firstly align the MBufChain.  The maximum size we can align the 
	// MBufChain to is the maximum MBuf size
	__ASSERT_COMPILE(KEchoRequestEmptyLength <= KMBufSmallSize);
	
	// We don't align the data as that way we don't need to worry about it being 
	// larger than one MBuf in size.  It doesn't need to be aligned anyway as the
	// only way the data is accessed is by being copied out.
	TInt length = aBuffer.Length();
	if(length >= KEchoRequestEmptyLength)
		{
		// Don't need to check result as we know that the MBufChain contains
		// at least length bytes as we asked first.
		(void)aBuffer.Align(KEchoRequestEmptyLength);
		aCommand = new HEchoRequest(aBuffer);
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
	
HEchoRequest::~HEchoRequest()
	{
	LOG_FUNC
	}

TBool HEchoRequest::ProcessCommand(CL2CapSignalHandler& aSignalHandler)
	{
	LOG_FUNC
	if(aSignalHandler.HandleEchoRequest(this))
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

TInt HEchoRequest::GetData(RMBufChain& aData) const
	{
	LOG_FUNC
	TInt rerr = KErrNone;
	if(DataLength() > KEchoRequestEmptyLength)
		{
		TRAP(rerr, iCommand.CopyL(aData, KEchoRequestEmptyLength));
		}
	else
		{
		rerr = KErrNotFound;
		}
	return rerr;
	}


//					ECHO RESPONSE COMMAND 
//Constructor to deal command coming in from remote side
HEchoResponse::HEchoResponse(RMBufChain& aCommand) 
 : HL2CapCommand(aCommand)
	{
	LOG_FUNC
	}

/*static*/ HEchoResponse* HEchoResponse::New(RMBufChain& aData, TUint8 aId)
	{
	LOG_STATIC_FUNC
	HEchoResponse* cmd = NULL;
	RMBufChain buf;
	buf.Assign(aData);
	TRAPD(rerr, buf.PrependL(KEchoResponseEmptyLength));
	
	if(rerr == KErrNone)
		{
		cmd = new HEchoResponse(buf);
		if(cmd)
			{
			// Setup message contents.
			cmd->SetCode(EEchoResponse);
			cmd->SetID(aId);
			cmd->WriteDataLength();
			}
		else
			{
			// Free the allocated buffer.
			buf.Free();
			}
		}
	return cmd;
	}		

/*static*/ HEchoResponse* HEchoResponse::New(TUint8 aId)
	{
	LOG_STATIC_FUNC
	HEchoResponse* cmd = NULL;
	RMBufChain buf;
	TRAPD(rerr, buf.AllocL(KEchoResponseEmptyLength));
	
	if(rerr == KErrNone)
		{
		cmd = new HEchoResponse(buf);
		if(cmd)
			{
			// Setup message contents.
			cmd->SetCode(EEchoResponse);
			cmd->SetID(aId);
			cmd->WriteDataLength();
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
TInt HEchoResponse::NewCommandIfValid(RMBufChain& aBuffer, HL2CapCommand*& aCommand)
	{
	LOG_STATIC_FUNC
	// Firstly align the MBufChain.  The maximum size we can align the 
	// MBufChain to is the maximum MBuf size
	__ASSERT_COMPILE(KEchoResponseEmptyLength <= KMBufSmallSize);
	
	// We don't align the data as that way we don't need to worry about it being 
	// larger than one MBuf in size.  It doesn't need to be aligned anyway as the
	// only way the data is accessed is by being copied out.
	TInt length = aBuffer.Length();
	if(length >= KEchoResponseEmptyLength)
		{
		// Don't need to check result as we know that the MBufChain contains
		// at least length bytes as we asked first.
		(void)aBuffer.Align(KEchoResponseEmptyLength);
		aCommand = new HEchoResponse(aBuffer);
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

HEchoResponse::~HEchoResponse()
	{
	LOG_FUNC
	}


TBool HEchoResponse::ProcessCommand(CL2CapSignalHandler& aSignalHandler)	// Handle incoming echo response
	{
	LOG_FUNC
	if(aSignalHandler.HandleEchoResponse(this))
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
	
TInt HEchoResponse::GetData(RMBufChain& aData) const
	{
	LOG_FUNC
	TInt rerr = KErrNone;
	if(DataLength() > KEchoResponseEmptyLength)
		{
		TRAP(rerr, iCommand.CopyL(aData, KEchoResponseEmptyLength));
		}
	else
		{
		rerr = KErrNotFound;
		}
	return rerr;
	}

