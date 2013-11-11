// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <bluetooth/logger.h>
#include "avctpreceiver.h"
#include "avctpcommon.h"
#include "avctpbody.h"
#include "avctpPriorities.h"
#include "avctpserviceutils.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_AVCTP_SERVICES);
#endif

using namespace SymbianAvctp;

/**
two-phase constructor
@param aAvctpBody The class that provides access to the AVCTP client
@leave systemwide reason
@return pointer to newly created object
@internalTechnology
*/
CAvctpReceiver* CAvctpReceiver::NewL(MAvctpEventNotify& aNotify, RSocket& aAvctpSocket, TPid aPid)
	{
	LOG_STATIC_FUNC

	CAvctpReceiver* self = CAvctpReceiver::NewLC(aNotify, aAvctpSocket, aPid);
	CleanupStack::Pop(self);
	return self;
	}

/**
two-phase constructor, leaves object on cleanupstack
@param aAvctpBody The class that provides access to the AVCTP client
@leave systemwide reason
@return pointer to newly created object
@internalTechnology
*/
CAvctpReceiver* CAvctpReceiver::NewLC(MAvctpEventNotify& aNotify, RSocket& aAvctpSocket, TPid aPid)
	{
	LOG_STATIC_FUNC

	CAvctpReceiver* self = new(ELeave) CAvctpReceiver(aNotify, aAvctpSocket, aPid);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

/**
c'tor
*/
CAvctpReceiver::CAvctpReceiver(MAvctpEventNotify& aNotify, RSocket& aAvctpSocket, TPid aPid) : 
	CActive(KDataPlanePriority), 
	iNotify(aNotify),
	iAvctpSocket(aAvctpSocket),
	iRecvState(EWaitingForPktHeader),
	iRecvPtr(NULL, 0),
	iPid(aPid)
	{
	LOG_FUNC
	}

void CAvctpReceiver::ConstructL()
	{
	LOG_FUNC
	iRecvBuf = HBufC8::NewMaxL(ENormalHeaderLength);
	iRecvBuf->Des().SetMax();
	CActiveScheduler::Add(this);
	ReceiveL(EWaitingForPktHeader);
	}

/**
d'tor
*/
CAvctpReceiver::~CAvctpReceiver()
	{
	LOG_FUNC

	Cancel();
	delete iRecvBuf;
	}

inline void CAvctpReceiver::NotifyMessageReceived(const TBTDevAddr& aBTDevice,
									   		  SymbianAvctp::TTransactionLabel aTransactionLabel,
									   		  SymbianAvctp::TMessageType aType,
									   		  TBool aIpidBitSet,
							 		   		  const TDesC8& aMessageInformation)
	{
	LOG_FUNC
	__ASSERT_BTADDR(aBTDevice);
	SymbianAvctp::AssertValidTransactionLabel(aTransactionLabel);
	__ASSERT_DEBUG(aType == SymbianAvctp::ECommand || aType == SymbianAvctp::EResponse, Panic(::EAvctpInvalidMessageType));
	
	iNotify.MaenMessageReceivedIndicate(aBTDevice, aTransactionLabel, aType, aIpidBitSet, aMessageInformation);
	}
	
//
// Function that issues RecvFrom 

/**
Requests data from the Avctp Sap
When the async Recv completes, iXfrLength will contain the remaining amount of data
in the same packet that we've not yet got.
*/
void CAvctpReceiver::ReceiveL(TRecvState aNextState, TInt aBufLen)
	{
	LOG_FUNC
	//Make sure iRecvBuf isn't NULL before using it
	if((!iRecvBuf) || (iRecvBuf->Des().MaxLength() < aBufLen)) 
		{
		// Don't ReAlloc cause that copies across the old data which we don't want
		delete iRecvBuf; // free previous data
		iRecvBuf = NULL;
		iRecvBuf = HBufC8::NewMaxL(aBufLen);
		iRecvBuf->Des().SetMax();
		}
	iRecvPtr.Set(iRecvBuf->Des().MidTPtr(0, aBufLen));
	iAvctpSocket.RecvFrom(iRecvPtr, iAddr, KSockReadContinuation, iStatus, iXfrLength);	
	ChangeState(aNextState);
	}

//
// Functions From CActive
	
void CAvctpReceiver::DoCancel()
	{
	LOG_FUNC

	iAvctpSocket.CancelRecv();
	}

void CAvctpReceiver::RunL()
	{
	LOG_FUNC
	LOG1(_L("iStatus.Int(): %d"), iStatus.Int())
	
	if (iStatus.Int() == KErrNone)
		{
		switch (iRecvState)
			{
			case EWaitingForPktHeader:
				SetPacketInfo(iRecvPtr);
				if (iXfrLength())
					{
					ReceiveL(EWaitingForMsgInfo, iXfrLength());												 
					}
				else // there was no message data
					{
					NotifyMessageReceived(TAvctpSockAddr::Cast(iAddr).BTAddr(), 
													 iTransactionLabel, 
													 iMessageType, 
													 iIpidBitSet, 
													 KNullDesC8);
					ReceiveL(EWaitingForPktHeader);	
					}
				break;
				
			case EWaitingForMsgInfo:
				NotifyMessageReceived(TAvctpSockAddr::Cast(iAddr).BTAddr(), 
												 iTransactionLabel, 
												 iMessageType, 
												 iIpidBitSet, 
												 iRecvPtr);
				ReceiveL(EWaitingForPktHeader);
				break;		
					
			default:
				__ASSERT_ALWAYS(NULL, Panic(EAvctpUnrecognisedState));
				break;
			}
		}
	else if (iStatus.Int() != KErrCancel)
		{
		iNotify.MaenErrorNotify(TAvctpSockAddr::Cast(iAddr).BTAddr(), iStatus.Int());
		}
	// else ignore the KErrCancel since the cancelled request wasn't directly asked for by the client of RAvctp
	}
	
TInt CAvctpReceiver::RunError(TInt aError)
	{
	LOG_FUNC

	iNotify.MaenErrorNotify(TAvctpSockAddr::Cast(iAddr).BTAddr(), aError);
	return KErrNone;	
	}

//
// Other Functions

void CAvctpReceiver::ChangeState(TRecvState aNextState)
	{
	LOG_FUNC
	LOG2(_L("current State: %d, aNextState: %d"), iRecvState, aNextState)
	
	__ASSERT_DEBUG((iRecvState == EWaitingForPktHeader && aNextState == EWaitingForMsgInfo)   ||
				   (iRecvState == EWaitingForPktHeader && aNextState == EWaitingForPktHeader) ||
				   (iRecvState == EWaitingForMsgInfo &&   aNextState == EWaitingForPktHeader),
				   Panic(EAvctpRecvBadStateTransition));
	__ASSERT_ALWAYS(!IsActive(), Panic(EAvctpRecvBadStateTransition));
	iRecvState = aNextState;
	SetActive();
	}
    
void CAvctpReceiver::SetPacketInfo(TDes8& aHeader)
	{
	LOG_FUNC

	__ASSERT_DEBUG(Pid(aHeader) == iPid, Panic(EAvctpIncorrectPid));
	__ASSERT_DEBUG(PacketType(aHeader) == ENormalPkt, Panic(EAvctpPacketTypeNotNormal));
				
	iTransactionLabel = TransactionLabel(aHeader);
	iMessageType = MessageType(aHeader);
	iIpidBitSet = !IsValidPid(aHeader);
	
  __DEBUG_ONLY
  		(
  		TBuf<KBTAddressLength> address;
  		TAvctpSockAddr::Cast(iAddr).BTAddr().GetReadable(address);
  		)
  	
  	LOG1(_L("BT Device 0x%S"), &address);
	
	LOG3(_L("iTransactionLabel: %d, iMessageType: %d, iIpidBitSet: %d"), iTransactionLabel, iMessageType, iIpidBitSet)
	}
        	
// EOF

