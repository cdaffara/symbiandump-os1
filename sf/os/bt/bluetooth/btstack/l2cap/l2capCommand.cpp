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
// Implements the handling of l2cap signal packets (both inbound and outbound).
// 
//

/**
 @file
 @internalComponent
*/

#include <bluetooth/logger.h>
#include "l2capCommand.h"

#include "l2capSigPacketConnection.h"
#include "l2capSigPacketConfigure.h"
#include "l2capSigPacketDisconnection.h"
#include "l2capSigPacketEcho.h"
#include "l2capSigPacketCommandReject.h"
#include "l2capSigPacketInformation.h"
#include "l2capSignalHandler.h"

#include "btsockettimer.h"

#include "l2signalmgr.h"

#include "L2CapDebugControlInterface.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_L2CAP);
#endif

// Static function that decodes the RMBufChain into the correct command returns pointer to base class
TInt HL2CapCommand::DecodeCommand(RMBufChain& aCommandString, HL2CapCommand*& aCommand)
	{
	LOG_STATIC_FUNC
	aCommand = NULL;
	TInt rerr = KErrNone;
	RMBufChain remainingBuffer;
	
	if(aCommandString.Length() >= KL2CapCommandHeaderLength)
		{
		aCommandString.Align(KL2CapCommandHeaderLength);
		TUint8 code = (aCommandString.First())->Get(KCodeOffset);
		TUint16 len = LittleEndian::Get16((aCommandString.First())->Ptr()+KLengthOffset);

		// Check the command has a valid length.
		if(len + KL2CapCommandHeaderLength <= aCommandString.Length())
			{
			TRAP(rerr, aCommandString.SplitL(len + KL2CapCommandHeaderLength, remainingBuffer));
			if(rerr == KErrNone)
				{
				TInt cmdErr = KErrNone; 
				switch(code)
					{
					case ECommandReject:
						(void)HCommandReject::NewCommandIfValid(aCommandString, aCommand);
						break;
					case EConnectionRequest:
						cmdErr = HConnectionRequest::NewCommandIfValid(aCommandString, aCommand);
						break;
					case EConnectionResponse:
						(void)HConnectionResponse::NewCommandIfValid(aCommandString, aCommand);
						break;
					case EConfigureRequest:
						cmdErr = HConfigureRequest::NewCommandIfValid(aCommandString, aCommand);
						break;
					case EConfigureResponse:
						(void)HConfigureResponse::NewCommandIfValid(aCommandString, aCommand);
						break;
					case EDisconnectionRequest:
						cmdErr = HDisconnectRequest::NewCommandIfValid(aCommandString, aCommand);
						break;
					case EDisconnectionResponse: 
						(void)HDisconnectResponse::NewCommandIfValid(aCommandString, aCommand);
						break;
					case EEchoRequest:
						cmdErr = HEchoRequest::NewCommandIfValid(aCommandString, aCommand);
						break;
					case EEchoResponse:
						(void)HEchoResponse::NewCommandIfValid(aCommandString, aCommand);
						break;
					case EInformationRequest:
						cmdErr = HInformationRequest::NewCommandIfValid(aCommandString, aCommand);
						break;
					case EInformationResponse:
						(void)HInformationResponse::NewCommandIfValid(aCommandString, aCommand);
						break;

					default:
						// This is not a recognised command.
						LOG1(_L("DecodeCommand Unrecognised Command, code = %d"), code);
						aCommand = new HInvalidCommand(aCommandString);
						break;
					};
					
				if(cmdErr == KErrCorrupt)
					{
					aCommand = new HInvalidCommand(aCommandString);
					}

				if(!aCommand)
					{
					aCommandString.Free();
					}

				aCommandString.Assign(remainingBuffer);
				}
			}
		else
			{
			// Invalid command length.
			rerr = KErrCorrupt;
			}
		}
	else
		{
		rerr = KErrCompletion;
		}
	return rerr; 
	}
	
	
HL2CapCommand::~HL2CapCommand()
	{
	LOG_FUNC
	CancelResponseTimer();
	iLink.Deque();
	iCommand.Free();

	L2CAP_DEBUG(ObjectAllocation(L2capDebugInfo::EL2CapCommand,
	                             L2capDebugInfo::EDeleted));
	}

HL2CapCommand::HL2CapCommand(RMBufChain& aCommand, 
                             TUint8 aRTXTimerDuration,
	                         TUint16 aERTXTimerDuration)
 : iResponseTimerRunning(EFalse),
   iRetransmissionCounter(0),
   iRTXTimerDuration(aRTXTimerDuration),
   iERTXTimerDuration(aERTXTimerDuration)
	{
	LOG_FUNC
	TInt length;
	iCommand.Assign(aCommand);
	length = iCommand.Length();

	if (length > KMBufSmallSize)
		{
		length = KMBufSmallSize;	// alignment of longer packets, causes problems
		}
	iCommand.Align(length);


	L2CAP_DEBUG(ObjectAllocation(L2capDebugInfo::EL2CapCommand,
	                             L2capDebugInfo::EAllocated));
	}

TInt HL2CapCommand::GetCommand(CL2CAPMux& aMuxer, RMBufChain& aBuffer)
	{
	LOG_FUNC
	// Set the ID if this is a request.  If the ID has already
	// been initialised do not overwrite it.  This could be
	// a retransmission.
	if(IsRequest() && ID() == KUninitialisedID)
		{
		SetID(aMuxer.NextSigId());
		}
		
	TRAPD(rerr, iCommand.CopyL(aBuffer));
	return rerr;
	}


/*static*/ TInt HL2CapCommand::ResponseTimerExpired(TAny* aL2CapCommand)
	{
	LOG_STATIC_FUNC
	HL2CapCommand* cmd = reinterpret_cast<HL2CapCommand*>(aL2CapCommand);
	cmd->HandleResponseTimerExpiry();
	return EFalse;
	}

void HL2CapCommand::HandleResponseTimerExpiry()
	{
	LOG_FUNC
	__ASSERT_DEBUG(iResponseSignalHandler != 0, Panic(EL2CAPNoResponseSignalHandler));
	// Deque the command from the response queue.
	iLink.Deque();
	iResponseTimerRunning = EFalse;
	
	if(iRetransmissionCounter >= KNumberOfRetransmissions)
		{
		iResponseSignalHandler->CommandResponseFailure(this);
		}
	else
		{
		iResponseSignalHandler->AddToOutgoingQueue(this);
		iRetransmissionCounter++;
		}	
	}
	
void HL2CapCommand::StartRTXTimer(TRTXTimerType aType, CL2CapSignalHandler& aSignalHandler)
	{
	LOG_FUNC
	// Check that it is valid to start a timer on this message.
	__ASSERT_DEBUG(IsRequest(), Panic(EL2CAPAttemptToStartRTxTimerForResponseCommand));

	// Cancel any existing timers.
	CancelResponseTimer();		
		
	TCallBack cb(ResponseTimerExpired, this);
	iResponseTimerEntry.Set(cb);

	TUint16 timerDuration;
	if(aType == ERTXTimer)
		{
		timerDuration = static_cast<TUint16>((iRetransmissionCounter + 1) * aSignalHandler.Mux().AdjustRTXTimerForSniffMode(iRTXTimerDuration));
		}
	else
		{
		timerDuration = iERTXTimerDuration;
		iRetransmissionCounter = KNumberOfRetransmissions;
		}

	BTSocketTimer::Queue(timerDuration * KL2ProtocolSecondTimerMultiplier, iResponseTimerEntry);
	iResponseTimerRunning = ETrue;
	
	iResponseSignalHandler = &aSignalHandler;
	}
	
void HL2CapCommand::CancelResponseTimer()
	{
	LOG_FUNC
	if(iResponseTimerRunning)
		{
		BTSocketTimer::Remove(iResponseTimerEntry);
		}
	}

TBool HL2CapCommand::StatefulResponse()
	{
	LOG_FUNC
	return (Code() == EConfigureResponse) || (Code() == EConnectionResponse) || (Code() == EDisconnectionResponse);
	}

HInvalidCommand::HInvalidCommand(RMBufChain& aCommand)
 : HL2CapCommand(aCommand)
	{
	LOG_FUNC
	}

HInvalidCommand::~HInvalidCommand()
	{
	LOG_FUNC
	}

TBool HInvalidCommand::ProcessCommand(CL2CapSignalHandler& aSignalHandler)
	{
	LOG_FUNC
	if(aSignalHandler.HandleInvalidCommand(this))
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
	

/*static*/ void HL2CapCommand::PutByte(TUint8 aByte, TInt aOffset, RMBufChain& aCommand)
	{
	LOG_STATIC_FUNC
	__ASSERT_ALWAYS(aCommand.First() && (aCommand.First())->Length() > aOffset,
		            Panic(EL2CapPDUInvalidLength));
	aCommand.First()->Put(aByte, aOffset); 
	}

/*static*/ void HL2CapCommand::PutLittleEndian16(TUint16 aShort, TInt aOffset, RMBufChain& aCommand)
	{
	LOG_STATIC_FUNC
	__ASSERT_ALWAYS(aCommand.First() && (aCommand.First())->Length() > aOffset+1,
		            Panic(EL2CapPDUInvalidLength));
	LittleEndian::Put16((aCommand.First())->Ptr()+aOffset, aShort);	
	}
	

/*static*/ void HL2CapCommand::PutLittleEndian32(TUint32 aLong, TInt aOffset, RMBufChain& aCommand)
	{
	LOG_STATIC_FUNC
	__ASSERT_ALWAYS(aCommand.First() && (aCommand.First())->Length() > aOffset+3,
		            Panic(EL2CapPDUInvalidLength));
	LittleEndian::Put32((aCommand.First())->Ptr()+aOffset, aLong);	
	}

/*static*/ TUint8 HL2CapCommand::GetByte(TInt aOffset, const RMBufChain& aCommand)
	{
	LOG_STATIC_FUNC
	__ASSERT_ALWAYS(aCommand.First() && (aCommand.First())->Length() > aOffset,
		            Panic(EL2CapPDUInvalidLength));
	return aCommand.First()->Get(aOffset); 
	}
	
/*static*/ TUint16 HL2CapCommand::GetLittleEndian16(TInt aOffset, const RMBufChain& aCommand)
	{
	LOG_STATIC_FUNC
	__ASSERT_ALWAYS(aCommand.First() && (aCommand.First())->Length() > aOffset+1,
		            Panic(EL2CapPDUInvalidLength));
	return (LittleEndian::Get16((aCommand.First())->Ptr()+aOffset)); 
	}
	
/*static*/ TUint32 HL2CapCommand::GetLittleEndian32(TInt aOffset, const RMBufChain& aCommand)
	{
	LOG_STATIC_FUNC
	__ASSERT_ALWAYS(aCommand.First() && (aCommand.First())->Length() > aOffset+3,
		            Panic(EL2CapPDUInvalidLength));
	return(LittleEndian::Get32((aCommand.First())->Ptr()+aOffset)); 
	}
