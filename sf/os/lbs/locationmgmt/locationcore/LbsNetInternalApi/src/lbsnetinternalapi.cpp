// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @released
*/

#include <e32base.h>
#include <e32debug.h>
#include "lbsnetinternalapi.h"
#include "lbsnetinternalcommon.h"
#include "lbsnetinternalmsgtypes.h"
#include "lbspropertykeydefs.h"
#include "lbsrootcenrepdefs.h"

/** @see RLbsNetChannel::TLbsNetChannelId */
const TInt KLbsNetChannelIdMax = RLbsNetChannel::EChannelNG2SUPLPSY;
const TInt KCategoruKeyIndex	 = 0;
const TInt KReadPropKeyIndex 	 = 1;
const TInt KReadPropAckKeyIndex  = 2;
const TInt KWritePropKeyIndex 	 = 3;
const TInt KWritePropAckKeyIndex = 4;

/** The array contains description (property keys and Ids) of all the channels supported by 
    the RLbsNetChannel interface */
const TUint KPropTable[KLbsNetChannelIdMax+1][5]=
	{
	{KNetGatewayChannelCategoryKey, KNetGatewayMessageKey, KNetGatewayMessageReadAckKey, KNetRequestMessageKey, KNetRequestMessageReadAckKey},  //EChannelNRH2NG
	{KNetRequestChannelCategoryKey, KNetRequestMessageKey, KNetRequestMessageReadAckKey, KNetGatewayMessageKey, KNetGatewayMessageReadAckKey},  //EChannelNG2NRH
	{KNetAGPS2NGChannelCategoryKey, KNetAGPS2NGMessageKey, KNetAGPS2NGMessageReadAckKey, KNetNG2AGPSMessageKey, KNetNG2AGPSMessageReadAckKey},  //EChannelAGPS2NG
	{KNetNG2AGPSChannelCategoryKey, KNetNG2AGPSMessageKey, KNetNG2AGPSMessageReadAckKey, KNetAGPS2NGMessageKey, KNetAGPS2NGMessageReadAckKey},  //EChannelNG2AGPS
	{KNetSUPLPSY2NGChannelCategoryKey, KNetSUPLPSY2NGMessageKey, KNetSUPLPSY2NGMessageReadAckKey, KNetNG2SUPLPSYMessageKey, KNetNG2SUPLPSYMessageReadAckKey},  //EChannelSUPLPSY2NG
	{KNetNG2SUPLPSYChannelCategoryKey, KNetNG2SUPLPSYMessageKey, KNetNG2SUPLPSYMessageReadAckKey, KNetSUPLPSY2NGMessageKey, KNetSUPLPSY2NGMessageReadAckKey},  //EChannelNG2SUPLPSY
	};

//
// RLbsNetChannel
//
/** Initialise the resources needed for the channel.

This must be called before a client can connect to the channel
by calling RLbsNetRequestChannel::OpenL().

This may do tasks such as creating the RMsgQueue used internally (or 
RProperties if publish and subscribe is being used).
*/
EXPORT_C void RLbsNetChannel::InitializeL(TLbsNetChannelId aChannelId)
	{
	TSecurityPolicy readPolicy(ECapabilityLocation);
	TSecurityPolicy writePolicy(ECapabilityWriteDeviceData);
	
	TInt err = RProperty::Define(KPropTable[aChannelId][KWritePropKeyIndex],
								 RProperty::ELargeByteArray, 
								 readPolicy, writePolicy, 
								 KMaximumMessageSizeBytes);
	if (err != KErrNone && err != KErrAlreadyExists)
		{
		User::Leave(err);
		}
						 
	err = RProperty::Define(KPropTable[aChannelId][KWritePropAckKeyIndex],
							RProperty::EInt,
							readPolicy, writePolicy);
	if (err != KErrNone && err != KErrAlreadyExists)
		{
		User::Leave(err);
		}
										 
	User::LeaveIfError(RProperty::Set(RProcess().SecureId(), KPropTable[aChannelId][KWritePropAckKeyIndex], 0));
	
	// Save the Uid of the process which called InitializeL()
	// in the cenrep file owned by LbsRoot.
	LbsNetInternalCommon::SetCategoryUidL(KPropTable[aChannelId][KCategoruKeyIndex],
										  RProcess().SecureId());
	}

/**
*/
EXPORT_C void RLbsNetChannel::ShutDownL(TLbsNetChannelId aChannelId)
	{
	User::LeaveIfError(RProperty::Delete(KPropTable[aChannelId][KWritePropKeyIndex]));
	User::LeaveIfError(RProperty::Delete(KPropTable[aChannelId][KWritePropAckKeyIndex]));
	}

EXPORT_C RLbsNetChannel::RLbsNetChannel()
	{
	}
	
/** Connect to the Network Request Handler Channel.

The channel must first be initialised before this is called, otherwise the
resources needed by the channel will not have been allocated.

@param aObserver Reference to the observer class used to notify of new messages 
                 from the Network Request Handler.
@param aChannelId Uid of the RProperty category used for this channel.

@leave
*/
EXPORT_C void RLbsNetChannel::OpenL(TLbsNetChannelId aChannelId, MLbsNetChannelObserver& aObserver)
	{	
	__ASSERT_DEBUG(iSendProperty.Handle()==NULL, User::Invariant());		//Handle open.
	__ASSERT_DEBUG(iSendMsgReadProperty.Handle()==NULL, User::Invariant());		//Handle open.
	
	// Get the CategoryUid from the cenrep file owned by LbsRoot.
	TUid categoryUid;
	LbsNetInternalCommon::GetCategoryUidL(KPropTable[aChannelId][KCategoruKeyIndex],
										  categoryUid);
	
	User::LeaveIfError(iSendProperty.Attach(categoryUid, KPropTable[aChannelId][KWritePropKeyIndex]));
	User::LeaveIfError(iSendMsgReadProperty.Attach(categoryUid, KPropTable[aChannelId][KWritePropAckKeyIndex]));
	
	iMonitor = CNetChannelMonitor::NewL(aObserver, aChannelId, categoryUid, 
										KPropTable[aChannelId][KReadPropKeyIndex], 
										KPropTable[aChannelId][KReadPropAckKeyIndex]);
	}

/** Close the connection to the Network Request Handler Channel.

This function closes the connection and frees any resources 
created in RLbsNetChannel::OpenL().
*/	
EXPORT_C void RLbsNetChannel::Close()
	{
	CancelSendMessageNotification();
	
	delete iMonitor;
	iSendProperty.Close();
	iSendMsgReadProperty.Close();
	}

/** Send a message to the Network Request Handler. 
    
This is an asynchronous function that only completes when the message 
has been read. 

If another call is made to this function before a previous one has 
completed then aStatus will be completed straight away, with the 
completion code KErrInUse.
    
@param aMessage The message to be sent to the Network Request Handler.
@param aStatus Request status that is completed when the message has
			   been read. If there is an error sending the message
			   it will be stored in aStatus.Int().
*/
EXPORT_C void RLbsNetChannel::SendMessage(const TLbsNetInternalMsgBase& aMessage, 
										  TRequestStatus& aStatus)
	{
	// Must check that the previous message was read
	TInt sendAck;
	iSendMsgReadProperty.Get(sendAck);
	if (sendAck == 1)
		{
		// Pending message has not been read yet; flag an error
		TRequestStatus* statPtr = &aStatus;
		User::RequestComplete(statPtr, KErrInUse);
		}
	else
		{
		// Publish the new message
		iSendMsgReadProperty.Set(1);
		iSendMsgReadProperty.Subscribe(aStatus);
		TPtrC8 ptr(reinterpret_cast<const TUint8*>(&aMessage), aMessage.Size());
		iSendProperty.Set(ptr);
		}	
	}
	
/** Cancel the current request status for SendMessage().
*/
EXPORT_C void RLbsNetChannel::CancelSendMessageNotification()
	{
	iSendMsgReadProperty.Cancel();
	iSendMsgReadProperty.Set(0);
	}	


//
// CNetChannelMonitor - used internally by the RLbsNetChannel class
//
CNetChannelMonitor* CNetChannelMonitor::NewL(MLbsNetChannelObserver& aObserver, RLbsNetChannel::TLbsNetChannelId aChannelId, TUid aPropId, TUint aPropKey, TUint aAckPropKey)
	{
	CNetChannelMonitor* self = new (ELeave) CNetChannelMonitor(aObserver, aChannelId);
	CleanupStack::PushL(self);
	self->ConstructL(aPropId, aPropKey, aAckPropKey);
	CleanupStack::Pop();
	return self;
	}

CNetChannelMonitor::CNetChannelMonitor(MLbsNetChannelObserver& aObserver, RLbsNetChannel::TLbsNetChannelId aChannelId) :
	CActive(EPriorityStandard),
	iObserver(aObserver),
	iChannelId(aChannelId)
	{
	CActiveScheduler::Add(this);
	}

CNetChannelMonitor::~CNetChannelMonitor()
	{
	Cancel();
	iReceiveProperty.Close();
	iReceiveMsgReadProperty.Close();	
	}

void CNetChannelMonitor::ConstructL(TUid aPropId, TUint aPropKey, TUint aAckPropKey)
	{
	__ASSERT_DEBUG(iReceiveProperty.Handle()==NULL, User::Invariant());		//Handle open.
	__ASSERT_DEBUG(iReceiveMsgReadProperty.Handle()==NULL, User::Invariant());		//Handle open.
	
	User::LeaveIfError(iReceiveProperty.Attach(aPropId, aPropKey));
	User::LeaveIfError(iReceiveMsgReadProperty.Attach(aPropId, aAckPropKey));
	
	NotifyNetChannelMessage();
	// flag we are ready for receiving messages
	iReceiveMsgReadProperty.Set(0);
	}
	
void CNetChannelMonitor::NotifyNetChannelMessage()
	{
	iReceiveProperty.Subscribe(iStatus);
	SetActive();	
	}

void CNetChannelMonitor::RunL()
	{
	TInt result = iStatus.Int();
	switch (result)
		{
		case KErrNone:
			{
			// Read property
			TUint8 buf[KMaximumMessageSizeBytes];
			TPtr8 ptr(buf, sizeof(buf));
			iReceiveProperty.Get(ptr);
			
			// resubscribe for further messages
			NotifyNetChannelMessage();
			
			// Notify observer
			iObserver.ProcessNetChannelMessage(iChannelId, 
											   reinterpret_cast<TLbsNetInternalMsgBase&>(*&buf));

			// flag message as read
			iReceiveMsgReadProperty.Set(0);
			break;	
			}
		default:
			{
			User::Leave(result);
			break;
			}
		}
	}
	
void CNetChannelMonitor::DoCancel()
	{
	iReceiveProperty.Cancel();
	}
	
TInt CNetChannelMonitor::RunError(TInt /*aError*/)
	{
	return KErrNone;
	}
