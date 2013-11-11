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


#include <e32base.h>
#include <e32debug.h>
#include <lbs/lbsnetprotocolbase.h>
#include "tlbsprivfwtestchannel.h"
#include <lbs/test/tlbsprivfwtestchannelmsgtypes.h>

/** @see RLbsTestChannel::TLbsTestChannelId */
const TInt KLbsNetChannelIdMax = RLbsTestChannel::EChannelProxy2Handler;
const TInt KCategoryKeyIndex	 = 0;
const TInt KReadPropKeyIndex 	 = 1;
const TInt KReadPropAckKeyIndex  = 2;
const TInt KWritePropKeyIndex 	 = 3;
const TInt KWritePropAckKeyIndex = 4;


const TInt KResendingAttempts = 3; 
const TInt KTimeoutBetweenAttempts = 500000;
const TInt KWaitingForAcknowledgment = 1;


/** The array contains description (property keys and Ids) of all the channels supported by 
    the RLbsTestChannel interface */
const TUint KPropTable[KLbsNetChannelIdMax+1][5]=
	{
	{KUidSystemCategoryValue, KProxyToStubPipeKey, KProxyToStubFlowKey, KStubToProxyPipeKey, KStubToProxyFlowKey},  //EChannelHandler2Proxy
	{KUidSystemCategoryValue, KStubToProxyPipeKey, KStubToProxyFlowKey, KProxyToStubPipeKey, KProxyToStubFlowKey},  //EChannelProxy2Handler
	};

_LIT_SECURITY_POLICY_PASS(KSecurityPolicy); // Test code, so msecurity is a non-issue

//
// RLbsTestChannel
//
/** Initialise the resources needed for the channel.

This must be called before a client can connect to the channel
by calling RLbsNetRequestChannel::OpenL().

This may do tasks such as creating the RMsgQueue used internally (or 
RProperties if publish and subscribe is being used).
*/
EXPORT_C void RLbsTestChannel::InitializeL(TLbsTestChannelId aChannelId)
	{
	TSecurityPolicy readPolicy(ECapabilityLocation);
	TSecurityPolicy writePolicy(ECapabilityWriteDeviceData);	 
	
	TInt err = RProperty::Define(TUid::Uid(KPropTable[aChannelId][KCategoryKeyIndex]),
								 KPropTable[aChannelId][KWritePropKeyIndex],
								 RProperty::ELargeByteArray, 
								 KSecurityPolicy, KSecurityPolicy, 
								 KMaximumMessageSizeBytes);
	if (err != KErrNone && err != KErrAlreadyExists)
		{
		User::Leave(err);
		}
						 
	err = RProperty::Define(TUid::Uid(KPropTable[aChannelId][KCategoryKeyIndex]),
							KPropTable[aChannelId][KWritePropAckKeyIndex],
							RProperty::EInt,
							KSecurityPolicy, KSecurityPolicy);
	if (err != KErrNone && err != KErrAlreadyExists)
		{
		User::Leave(err);
		}
	User::LeaveIfError(RProperty::Set(TUid::Uid(KPropTable[aChannelId][KCategoryKeyIndex]), KPropTable[aChannelId][KWritePropAckKeyIndex], 0));
	}

/**
*/
EXPORT_C void RLbsTestChannel::ShutDownL(TLbsTestChannelId aChannelId)
	{
	User::LeaveIfError(RProperty::Delete(TUid::Uid(KPropTable[aChannelId][KCategoryKeyIndex]), KPropTable[aChannelId][KWritePropKeyIndex]));
	User::LeaveIfError(RProperty::Delete(TUid::Uid(KPropTable[aChannelId][KCategoryKeyIndex]), KPropTable[aChannelId][KWritePropAckKeyIndex]));	
	}

EXPORT_C RLbsTestChannel::RLbsTestChannel()
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
EXPORT_C void RLbsTestChannel::OpenL(TLbsTestChannelId aChannelId, MLbsTestChannelObserver& aObserver)
	{	
	__ASSERT_DEBUG(iSendProperty.Handle()==NULL, User::Invariant());		//Handle open.
	__ASSERT_DEBUG(iSendMsgReadProperty.Handle()==NULL, User::Invariant());		//Handle open.
	
	// Get the CategoryUid from the cenrep file owned by LbsRoot.

	User::LeaveIfError(iSendProperty.Attach(TUid::Uid(KPropTable[aChannelId][KCategoryKeyIndex]), KPropTable[aChannelId][KWritePropKeyIndex]));
	User::LeaveIfError(iSendMsgReadProperty.Attach(TUid::Uid(KPropTable[aChannelId][KCategoryKeyIndex]), KPropTable[aChannelId][KWritePropAckKeyIndex]));
	
	iMonitor = CLbsTestChannelMonitor::NewL(aObserver, aChannelId, TUid::Uid(KPropTable[aChannelId][KCategoryKeyIndex]), 
										KPropTable[aChannelId][KReadPropKeyIndex], 
										KPropTable[aChannelId][KReadPropAckKeyIndex]);
	}

/** Close the connection to the Network Request Handler Channel.

This function closes the connection and frees any resources 
created in RLbsTestChannel::OpenL().
*/	
EXPORT_C void RLbsTestChannel::Close()
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

If the test code already deleted all P&S properties used for delivery of the message,
then the function is completed  straight away, with the completion code KErrNone.
    
@param aMessage The message to be sent to the Network Request Handler.
@param aStatus Request status that is completed when the message has
			   been read. If there is an error sending the message
			   it will be stored in aStatus.Int().
*/
EXPORT_C void RLbsTestChannel::SendMessage(const TLbsTestChannelMsgBase& aMessage, 
										  TRequestStatus& aStatus)
	{
	//We need to resubscribe for notifications in case we stopped to listen for new messages
	//after the test P&S properties were deleted by the previous test and CLbsTestChannelMonitor::RunL
	//failed with KErrNotFound(-1).
	iMonitor->NotifyTestChannelMessage();
	
	// Must check that the previous message was read
	TInt sendAck = KWaitingForAcknowledgment;
	
	//We make three attempts waiting by 0.5 sec between each
	//If the other side closed the channel we consider that it is not interesred in this message 
	//and no ACK is required. So, the message is completed with KErrNone straight away. 

	for(TInt i=0; i<KResendingAttempts; i++)
		{
		TInt err = iSendMsgReadProperty.Get(sendAck);
		if(KErrNotFound==err)
			{
			TRequestStatus* statPtr = &aStatus;
			User::RequestComplete(statPtr, KErrNone);
			return;
			}
		if(sendAck!=1)
			{
			break;
			}
		User::After(KTimeoutBetweenAttempts);
		}
	
	if (KWaitingForAcknowledgment == sendAck)
		{
		// Pending message has not been read yet; flag an error
		TRequestStatus* statPtr = &aStatus;
		User::RequestComplete(statPtr, KErrInUse);
		}
	else
		{
		// Publish the new message
		iSendMsgReadProperty.Set(KWaitingForAcknowledgment);
		iSendMsgReadProperty.Subscribe(aStatus);
		TPtrC8 ptr(reinterpret_cast<const TUint8*>(&aMessage), aMessage.Size());
		iSendProperty.Set(ptr);
		}
	}
	
/** Cancel the current request status for SendMessage().
*/
EXPORT_C void RLbsTestChannel::CancelSendMessageNotification()
	{
	iSendMsgReadProperty.Cancel();
	}	


//
// CLbsTestChannelMonitor - used internally by the RLbsTestChannel class
//
CLbsTestChannelMonitor* CLbsTestChannelMonitor::NewL(MLbsTestChannelObserver& aObserver, RLbsTestChannel::TLbsTestChannelId aChannelId, TUid aPropId, TUint aPropKey, TUint aAckPropKey)
	{
	CLbsTestChannelMonitor* self = new (ELeave) CLbsTestChannelMonitor(aObserver, aChannelId);
	CleanupStack::PushL(self);
	self->ConstructL(aPropId, aPropKey, aAckPropKey);
	CleanupStack::Pop();
	return self;
	}

CLbsTestChannelMonitor::CLbsTestChannelMonitor(MLbsTestChannelObserver& aObserver, RLbsTestChannel::TLbsTestChannelId aChannelId) :
	CActive(EPriorityStandard),
	iObserver(aObserver),
	iChannelId(aChannelId)
	{
	CActiveScheduler::Add(this);
	}

CLbsTestChannelMonitor::~CLbsTestChannelMonitor()
	{
	Cancel();
	iReceiveProperty.Close();
	iReceiveMsgReadProperty.Close();	
	}

void CLbsTestChannelMonitor::ConstructL(TUid aPropId, TUint aPropKey, TUint aAckPropKey)
	{
	__ASSERT_DEBUG(iReceiveProperty.Handle()==NULL, User::Invariant());		//Handle open.
	__ASSERT_DEBUG(iReceiveMsgReadProperty.Handle()==NULL, User::Invariant());		//Handle open.
	
	User::LeaveIfError(iReceiveProperty.Attach(aPropId, aPropKey));
	User::LeaveIfError(iReceiveMsgReadProperty.Attach(aPropId, aAckPropKey));
	
	NotifyTestChannelMessage();
	// flag we are ready for receiving messages
	iReceiveMsgReadProperty.Set(0);
	}
	
void CLbsTestChannelMonitor::NotifyTestChannelMessage()
	{
	if(!IsActive())
		{
		iReceiveProperty.Subscribe(iStatus);
		SetActive();
		}
	}

void CLbsTestChannelMonitor::RunL()
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
			NotifyTestChannelMessage();
			
			// Notify observer
			iObserver.ProcessTestChannelMessage(iChannelId, 
											   reinterpret_cast<TLbsTestChannelMsgBase&>(*&buf));

			// flag message as read
			iReceiveMsgReadProperty.Set(0);
			break;	
			}
			
		case KErrNotFound:
			
			break;
			
		default:
			{
			User::Leave(result);
			break;
			}
		}
	}
	
void CLbsTestChannelMonitor::DoCancel()
	{
	iReceiveProperty.Cancel();

    if (iStatus == KRequestPending) 
    	{
    	TRequestStatus* status = &iStatus;
 		User::RequestComplete(status,KErrCancel);    	
    	}
	}
	
TInt CLbsTestChannelMonitor::RunError(TInt /*aError*/)
	{
	return KErrNone;
	}


