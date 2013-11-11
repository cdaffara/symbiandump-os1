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

#include "l2capSigPacketCommandReject.h"
#include "l2capSignalHandler.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_L2CAP);
#endif


//					COMMAND REJECT COMMAND 


HCommandReject::HCommandReject(RMBufChain& aCommand) 
 : HL2CapCommand(aCommand, KDefaultRTXTimerDuration, KDefaultERTXTimerDuration)
	{
	LOG_FUNC
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
TInt HCommandReject::NewCommandIfValid(RMBufChain& aBuffer, HL2CapCommand*& aCommand)
	{
	LOG_STATIC_FUNC
	// Firstly align the MBufChain.  The maximum size we can align the 
	// MBufChain to is the maximum MBuf size
	__ASSERT_COMPILE(KL2CapCommandRejectMaxLength <= KMBufSmallSize);
	
	TInt length = aBuffer.Length();
	if((length >= KL2CapCommandRejectMinLength) && (length <= KL2CapCommandRejectMaxLength))
		{
		// Don't need to check result as we know that the MBufChain contains
		// at least length bytes as we asked first.
		(void)aBuffer.Align(length);
		}
	else
		{
		// Dodge length!
		return KErrCorrupt;
		}
		
	if(VerifyStructure(aBuffer))
		{
		// Top!  The structure's fine, go ahead and create the command.
		aCommand = new HCommandReject(aBuffer);
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
		// We'll not have any of this nonsense!
		return KErrCorrupt;
		}	
	}

/*static*/ HCommandReject* HCommandReject::New(TL2CAPCommandRejectData aRejectData, TUint8 aId)
	{
	LOG_STATIC_FUNC
	HCommandReject* cmd = NULL;
	RMBufChain buf;
	TInt rerr = KErrNone;
	
	switch(aRejectData.iReason)
		{
		case ECommandNotUnderstood:
			TRAP(rerr, buf.AllocL(KL2CapCommandRejectMinLength));
			break;
			
		case EMTUExceeded:
			TRAP(rerr, buf.AllocL(KL2CapCommandRejectMinLength + KMTUExceededLength));
			break;

		case EInvalidCID:
			TRAP(rerr, buf.AllocL(KL2CapCommandRejectMinLength + KInvalidRequestedCIDLength));
			break;

		default:
			// Invalid reject reason.
			rerr = KErrArgument;
			break;
		};

	if(rerr == KErrNone)
		{
		// Create the command - the RMBufChain ownership will pass
		// to the new object.
		cmd = new HCommandReject(buf);
		if(cmd)
			{
			// Setup message contents.
			cmd->SetCode(ECommandReject);
			cmd->SetID(aId);
			cmd->WriteDataLength();
			cmd->SetRejectData(aRejectData);	
			}
		else
			{
			// Free the allocated buffer.
			buf.Free();
			}
		}
	return cmd;
	}

void HCommandReject::SetRejectData(TL2CAPCommandRejectData aRejectData)
	{
	LOG_FUNC
	switch(aRejectData.iReason)
		{
		case ECommandNotUnderstood:
			PutLittleEndian16(ECommandNotUnderstood, KReasonOffset, iCommand);
			break;
			
		case EMTUExceeded:
			PutLittleEndian16(EMTUExceeded, KReasonOffset, iCommand);
			PutLittleEndian16(aRejectData.iMTUExceeded, KMTUExceededOffset, iCommand);
			break;

		case EInvalidCID:
			PutLittleEndian16(EInvalidCID, KReasonOffset, iCommand);
			PutLittleEndian16(aRejectData.iLocalEndpoint, KLocalEndpointOffset, iCommand);
			PutLittleEndian16(aRejectData.iRemoteEndpoint, KRemoteEndpointOffset, iCommand);
			break;

		default:
			Panic(EL2CAPInvalidCommandRejectReason);
			break;
		};
	}
	


HCommandReject::~HCommandReject()
	{
	LOG_FUNC
	}

		
TBool HCommandReject::ProcessCommand(CL2CapSignalHandler& aSignalHandler)
	{
	LOG_FUNC
	if(aSignalHandler.HandleCommandReject(this))
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

TBool HCommandReject::VerifyStructure(const RMBufChain& aBuffer)
	{
	LOG_STATIC_FUNC
	TInt valid = ETrue;
	// We have a reject reason, check there is enough data for that reason
	TL2CAPCommandRejectReason reason = TL2CAPCommandRejectReason(GetLittleEndian16(KReasonOffset, aBuffer));
	switch(reason)
		{
		case ECommandNotUnderstood:
			{
			if(aBuffer.Length() != KL2CapCommandRejectMinLength)
				{
				valid = EFalse;
				}
			}
			break;
		case EMTUExceeded:
			{
			if(aBuffer.Length() != (KL2CapCommandRejectMinLength + KMTUExceededLength))
				{
				valid = EFalse;
				}
			break;
			}
		case EInvalidCID:
			{
			if(aBuffer.Length() != (KL2CapCommandRejectMinLength + KInvalidRequestedCIDLength))
				{
				valid = EFalse;
				}
			}
			break;
		default:
			{
			// Unknown reason code
			valid = EFalse;
			break;
			}
		};
		
	return valid;
	}

TInt HCommandReject::RejectData(TL2CAPCommandRejectData& aRejectData) const
	{
	LOG_FUNC
	TInt rcode = KErrNone;
	
	aRejectData.iReason = TL2CAPCommandRejectReason(GetLittleEndian16(KReasonOffset, iCommand));
	
	switch(aRejectData.iReason)
		{
		case EMTUExceeded:
			aRejectData.iMTUExceeded = GetLittleEndian16(KMTUExceededOffset, iCommand);	
			break;
		case EInvalidCID:
			aRejectData.iLocalEndpoint = GetLittleEndian16(KLocalEndpointOffset, iCommand);
			aRejectData.iRemoteEndpoint = GetLittleEndian16(KRemoteEndpointOffset, iCommand);			
			break;
		default:
			break;
		};
	
	return rcode;
	}
