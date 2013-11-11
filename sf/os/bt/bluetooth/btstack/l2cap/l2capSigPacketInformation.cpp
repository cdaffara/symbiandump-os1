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

#include "l2capSigPacketInformation.h"
#include "l2capSignalHandler.h"
#include "l2constants.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_L2CAP);
#endif

//					INFORMATION REQUEST COMMAND 

/*static*/ HInformationRequest* HInformationRequest::New(TInfoType aInfoType,
		  	               								 TUint8 aRTXTimerDuration,
	                       								 TUint16 aERTXTimerDuration)
	{
	LOG_STATIC_FUNC
	HInformationRequest* cmd = NULL;
	RMBufChain buf;
	TRAPD(rerr, buf.AllocL(KInformationRequestLength));
	if(rerr == KErrNone)
		{
		cmd = new HInformationRequest(buf, aRTXTimerDuration, aERTXTimerDuration);
		if(cmd)
			{
			// Setup message contents.
			cmd->SetCode(EInformationRequest);
			cmd->SetID(KUninitialisedID);
			cmd->WriteDataLength();
			
			cmd->SetInfoType(aInfoType);
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
TInt HInformationRequest::NewCommandIfValid(RMBufChain& aBuffer, HL2CapCommand*& aCommand)
	{
	LOG_STATIC_FUNC
	// Firstly align the MBufChain.  The maximum size we can align the 
	// MBufChain to is the maximum MBuf size
	__ASSERT_COMPILE(KInformationRequestLength <= KMBufSmallSize);
	
	TInt length = aBuffer.Length();
	if(length == KInformationRequestLength)
		{
		// Don't need to check result as we know that the MBufChain contains
		// at least length bytes as we asked first.
		(void)aBuffer.Align(length);
		aCommand = new HInformationRequest(aBuffer);
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

HInformationRequest::HInformationRequest(RMBufChain& aCommand, 
          								 TUint8 aRTXTimerDuration,
          								 TUint16 aERTXTimerDuration)
 : HL2CapCommand(aCommand, aRTXTimerDuration, aERTXTimerDuration)
	{
	LOG_FUNC
	}

HInformationRequest::~HInformationRequest()
	{
	LOG_FUNC
	}
		
TBool HInformationRequest::ProcessCommand(CL2CapSignalHandler& aSignalHandler)
	{
	LOG_FUNC
	if(aSignalHandler.HandleInformationRequest(this))
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




//					INFORMATION RESPONSE COMMAND 
//Constructor to deal command coming in from remote side
//
HInformationResponse::HInformationResponse(RMBufChain& aCommand) 
 : HL2CapCommand(aCommand)
	{
	LOG_FUNC
	}

/*static*/ HInformationResponse* HInformationResponse::New(TInfoType aInfoType, TInfoReqResult aResult, TUint8 aId, TUint32 aExtendedFeatures)
	{
	LOG_STATIC_FUNC
	HInformationResponse* cmd = NULL;
	RMBufChain buf;
	TRAPD(rerr, buf.AllocL(KInformationResponseMinLength + KExtendedFeatureMaskLength));
	if(rerr == KErrNone)
		{
		cmd = new HInformationResponse(buf);
		if(cmd)
			{
			// Setup message contents.
			cmd->SetCode(EInformationResponse);
			cmd->SetID(aId);
			cmd->WriteDataLength();
			
			cmd->SetInfoType(aInfoType);
			cmd->SetResult(aResult);
			
			cmd->SetExtendedFeatureMask(aExtendedFeatures);
			}
		else
			{
			// Free the allocated buffer.
			buf.Free();
			}
		}
	return cmd;
	}		

/*static*/ HInformationResponse* HInformationResponse::New(TInfoType aInfoType, TInfoReqResult aResult, TUint8 aId)
	{
	LOG_STATIC_FUNC
	HInformationResponse* cmd = NULL;
	RMBufChain buf;
	TRAPD(rerr, buf.AllocL(KInformationResponseMinLength));
	if(rerr == KErrNone)
		{
		cmd = new HInformationResponse(buf);
		if(cmd)
			{
			// Setup message contents.
			cmd->SetCode(EInformationResponse);
			cmd->SetID(aId);
			cmd->WriteDataLength();
			
			cmd->SetInfoType(aInfoType);
			cmd->SetResult(aResult);
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
TInt HInformationResponse::NewCommandIfValid(RMBufChain& aBuffer, HL2CapCommand*& aCommand)
	{
	LOG_STATIC_FUNC
	// Firstly align the MBufChain.  The maximum size we can align the 
	// MBufChain to is the maximum MBuf size
	__ASSERT_COMPILE(KInformationResponseMaxLength <= KMBufSmallSize);
	
	TInt length = aBuffer.Length();
	if((length >= KInformationResponseMinLength) && (length <= KInformationResponseMaxLength))
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
		aCommand = new HInformationResponse(aBuffer);
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

/* static */ TBool HInformationResponse::VerifyStructure(const RMBufChain& aCommand)
	{
	LOG_STATIC_FUNC
	TBool valid = ETrue;
	TInfoType infoType = static_cast<TInfoType>(GetLittleEndian16(KInfoTypeOffset, aCommand));
	TInfoReqResult result = static_cast<TInfoReqResult>(GetLittleEndian16(KResultOffset, aCommand));
	
	if(result == ESuccess)
		{
		switch(infoType)
			{ 
			case EConnectionlessMTU:
				{
				if(aCommand.Length() != KInformationResponseMinLength + KConnectionlessMTULength)
					{
					valid = EFalse;
					}

				break;
				}
			case EExtendedFeaturesSupported:
				{
				if(aCommand.Length() != KInformationResponseMinLength + KExtendedFeatureMaskLength)
					{
					valid = EFalse;
					}
				break;
				}
			default:
				{
				// Unknown info type
				// Although this is dodgy as the info type should be copied from the request
				// we accept this for the sake of interop with legacy devices that deal
				// incorrectly with extended features.
				break;
				}
			};
		}
	else if(result == ENotsupported)
		{
		if(aCommand.Length() != KInformationResponseMinLength)
			{
			valid = EFalse;
			}
		}
	else
		{
		valid = EFalse;
		}
	
	return valid;
	}

HInformationResponse::~HInformationResponse()
	{
	LOG_FUNC
	}

TBool HInformationResponse::ProcessCommand(CL2CapSignalHandler& aSignalHandler)	
	{
	LOG_FUNC
	if(aSignalHandler.HandleInformationResponse(this))
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

TL2CapEntityInfo HInformationResponse::RemoteExtendedFeatureMask() const
	{
	LOG_FUNC
	TL2CapEntityInfo peerExtendedFeatures;
	
	// Check that the features are supported, and the message is a valid length.  If not return the default
	// remote entity information.
	if(InfoType() == EExtendedFeaturesSupported && Result() == ESuccess)
		{
		TUint32 featureMask = GetLittleEndian32(KDataOffset, iCommand);

		if(featureMask & EFlowControlMode)
			{
			peerExtendedFeatures.SetSupportFlowControl();
			}
			
		if(featureMask & ERetransmissionMode)
			{
			peerExtendedFeatures.SetSupportRetranmission();
			}
			
		if(featureMask & EBiDirectionalQOS)
			{
			peerExtendedFeatures.SetSupportBiDirectionalQos();
			}

		if(featureMask & EEnhancedRetransmissionMode)
			{
			peerExtendedFeatures.SetSupportEnhancedRetransmissionMode();
			}

		if(featureMask & EStreamingMode)
			{
			peerExtendedFeatures.SetSupportStreamingMode();
			}

		if(featureMask & EFCSOption)
			{
			peerExtendedFeatures.SetSupportFCSOption();
			}

		peerExtendedFeatures.SetLinkInfoState(EL2CapEntityInfoDefined);
		}
		
	return peerExtendedFeatures;
	}

