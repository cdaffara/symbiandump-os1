// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// @file T_Lbschannel.cpp
// This is the Cpp file which contains the channel for communication between
// the test harness and the test AGps module.
// 
//

// User includes
#include "tlbschannel.h"

const TInt KLbsChannelIdMax = RT_LbsChannel::EChannelTAGPS2TH;
//const TInt KCategoryKeyIndex	 = 0;
const TInt KReadPropKeyIndex 	 = 1;
const TInt KReadPropAckKeyIndex  = 2;
const TInt KWritePropKeyIndex 	 = 3;
const TInt KWritePropAckKeyIndex = 4;

/** The array contains description (property size, keys and Ids) of all the channels supported by 
    the RT_LbsChannel interface */
const TUint KPropTable[KLbsChannelIdMax+1][5]=
	{
	{0, KTH2TAGPSMessageKey, KTH2TAGPSMessageReadAckKey, KTAGPS2THMessageKey, KTAGPS2THMessageReadAckKey}, 
	{0, KTAGPS2THMessageKey, KTAGPS2THMessageReadAckKey, KTH2TAGPSMessageKey, KTH2TAGPSMessageReadAckKey}, 
	};

//
// RT_LbsChannel
//

EXPORT_C void RT_LbsChannel::InitializeL(TT_LbsChannelId aChannelId)
	{
	const TSecurityPolicy KReadPolicy(ECapability_None);
	const TSecurityPolicy KWritePolicy(ECapabilityWriteDeviceData);
	
	TInt err = RProperty::Define(KUidSystemCategory,
								 KPropTable[aChannelId][KWritePropKeyIndex],
								 RProperty::ELargeByteArray, 
								 KReadPolicy, KWritePolicy, 
								 sizeof(TT_LbsMsgBase));
	if (err != KErrNone && err != KErrAlreadyExists)
		{
		User::Leave(err);
		}
						 
	err = RProperty::Define(KUidSystemCategory,
							KPropTable[aChannelId][KWritePropAckKeyIndex],
							RProperty::EInt,
							KReadPolicy, KWritePolicy);
	if (err != KErrNone && err != KErrAlreadyExists)
		{
		User::Leave(err);
		}
										 
	User::LeaveIfError(RProperty::Set(KUidSystemCategory, KPropTable[aChannelId][KWritePropAckKeyIndex], 0));
	}


EXPORT_C void RT_LbsChannel::ShutDownL(TT_LbsChannelId aChannelId)
	{
	User::LeaveIfError(RProperty::Delete(KPropTable[aChannelId][KWritePropKeyIndex]));
	User::LeaveIfError(RProperty::Delete(KPropTable[aChannelId][KWritePropAckKeyIndex]));
	}


EXPORT_C RT_LbsChannel::RT_LbsChannel()
	{
	}
	

/** 

The channel must first be initialised before this is called, otherwise the
resources needed by the channel will not have been allocated.

@param aObserver Reference to the observer class used to notify of new messages. 
@param aChannelId Uid of the RProperty category used for this channel.

@leave
*/
EXPORT_C void RT_LbsChannel::OpenL(TT_LbsChannelId aChannelId, MT_LbsChannelObserver& aObserver)
	{	
	__ASSERT_DEBUG(iSendProperty.Handle()==NULL, User::Invariant());
	__ASSERT_DEBUG(iSendMsgReadProperty.Handle()==NULL, User::Invariant());
	
	User::LeaveIfError(iSendProperty.Attach(KUidSystemCategory, KPropTable[aChannelId][KWritePropKeyIndex]));
	User::LeaveIfError(iSendMsgReadProperty.Attach(KUidSystemCategory, KPropTable[aChannelId][KWritePropAckKeyIndex]));
	
	iMonitor = CT_ChannelMonitor::NewL(aObserver, aChannelId, KUidSystemCategory, 
										KPropTable[aChannelId][KReadPropKeyIndex], 
										KPropTable[aChannelId][KReadPropAckKeyIndex]);
	}

/** Close the connection.

This function closes the connection and frees any resources 
created in RTbsChannel::OpenL().
*/	
EXPORT_C void RT_LbsChannel::Close()
	{
	CancelSendMessageNotification();
	
	delete iMonitor;
	iSendProperty.Close();
	iSendMsgReadProperty.Close();
	}

/** Send a message through the channel.
    
This is an asynchronous function that only completes when the message 
has been read. 

If another call is made to this function before a previous one has 
completed then aStatus will be completed straight away, with the 
completion code KErrInUse.
    
@param aMessage The message to be sent.
@param aStatus Request status that is completed when the message has
			   been read. If there is an error sending the message
			   it will be stored in aStatus.Int().
*/
EXPORT_C void RT_LbsChannel::SendMessage(const TT_LbsMsgBase& aMessage, 
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
		TPckgC<TT_LbsMsgBase> pckg(aMessage);
		iSendProperty.Set(pckg);
		}	
	}
			
/** Cancel the current request status for SendMessage().
*/
EXPORT_C void RT_LbsChannel::CancelSendMessageNotification()
	{
	iSendMsgReadProperty.Cancel();
	}	


//
// CT_ChannelMonitor - used to listen for incoming messages 
//
CT_ChannelMonitor* CT_ChannelMonitor::NewL(MT_LbsChannelObserver& aObserver, RT_LbsChannel::TT_LbsChannelId aChannelId, TUid aPropId, TUint aPropKey, TUint aAckPropKey)
	{
	CT_ChannelMonitor* self = new (ELeave) CT_ChannelMonitor(aObserver, aChannelId);
	CleanupStack::PushL(self);
	self->ConstructL(aPropId, aPropKey, aAckPropKey);
	CleanupStack::Pop();
	return self;
	}

CT_ChannelMonitor::CT_ChannelMonitor(MT_LbsChannelObserver& aObserver, RT_LbsChannel::TT_LbsChannelId aChannelId) :
	CActive(EPriorityStandard),
	iObserver(aObserver),
	iChannelId(aChannelId)
	{
	CActiveScheduler::Add(this);
	}

CT_ChannelMonitor::~CT_ChannelMonitor()
	{
	Cancel();
	iReceiveProperty.Close();
	iReceiveMsgReadProperty.Close();	
	}

void CT_ChannelMonitor::ConstructL(TUid aPropId, TUint aPropKey, TUint aAckPropKey)
	{
	__ASSERT_DEBUG(iReceiveProperty.Handle()==NULL, User::Invariant());		//Handle open.
	__ASSERT_DEBUG(iReceiveMsgReadProperty.Handle()==NULL, User::Invariant());		//Handle open.
	
	User::LeaveIfError(iReceiveProperty.Attach(aPropId, aPropKey));
	User::LeaveIfError(iReceiveMsgReadProperty.Attach(aPropId, aAckPropKey));
	
	NotifyChannelMessage();
	// flag we are ready for receiving messages
	iReceiveMsgReadProperty.Set(0);
	}
	
void CT_ChannelMonitor::NotifyChannelMessage()
	{
	iReceiveProperty.Subscribe(iStatus);
	SetActive();	
	}

void CT_ChannelMonitor::RunL()
	{
	TInt result = iStatus.Int();
	switch (result)
		{
		case KErrNone:
			{
			// Read property
			TT_LbsMsgBase msg;
			TPckg<TT_LbsMsgBase> pckg(msg);
			iReceiveProperty.Get(pckg);
			
			// resubscribe for further messages
			NotifyChannelMessage();
			
			// Notify observer
			iObserver.ProcessChannelMessage(iChannelId, msg);

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
	
void CT_ChannelMonitor::DoCancel()
	{
	iReceiveProperty.Cancel();
	}
	
TInt CT_ChannelMonitor::RunError(TInt /*aError*/)
	{
	return KErrNone;
	}





