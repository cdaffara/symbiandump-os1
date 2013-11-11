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
// The class provides an implementation of the SUPL Push API SMS or WAP Push channel for 
// the receiver's part of the interface.
// 
//

/**
 @file
 @internalComponent
 @deprecated
*/

#include <e32base.h>
#include <e32property.h>

#include <lbs/lbssuplpushreceiver.h>
#include "lbsdevloggermacros.h"

#include "lbssuplpushprops.h"
#include "lbssuplpushreceiverchannel.h"


/**
Creates the channel and starts listening for incoming SUPL INIT messages.

@param aChannel [In] The id of the channel to listen messages on. 
@param aObserver [In] A reference to an observer waiting for SUPL INIT messages.
@param aPropOwnerSecureId [In] The secure id of the process that owns the P&S propertirs used.

@return an instance of the channel. The calling class becomes the
owner of the returned instance and is responsible its disposal.

@leave If a error happens, it leaves with the correspondent error code.

@see CLbsSuplPushRecImpl::NewL
*/
CLbsSuplPushRecChannel* CLbsSuplPushRecChannel::NewL(TLbsSuplPushChannel aChannel, 
		MLbsSuplPushRecObserver& aObserver, TUid aPropOwnerSecureId)
	{
	LBSLOG(ELogP1, "CLbsSuplPushRecChannel::NewL() Begin\n");
	CLbsSuplPushRecChannel* self = new (ELeave) CLbsSuplPushRecChannel(aChannel, aObserver, aPropOwnerSecureId);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	LBSLOG(ELogP1, "CLbsSuplPushRecChannel::NewL() End\n");
	return self;
	}
	
/**
Cancels the subscription for new messages and close the property used.
  
@see CLbsSuplPushRecImpl::~CLbsSuplPushRecImpl
*/
CLbsSuplPushRecChannel::~CLbsSuplPushRecChannel()
	{
	LBSLOG(ELogP1, "CLbsSuplPushRecChannel::~CLbsSuplPushRecChannel() Begin\n");
	Cancel();
	iInitProperty.Close();
	LBSLOG(ELogP1, "CLbsSuplPushRecChannel::~CLbsSuplPushRecChannel() End\n");
	}
	
/**
Constructor.

@param aChannel [In] The id of the channel to listen messages on. 
@param aObserver [In] A reference to an observer waiting for SUPL INIT messages.
@param aPropOwnerSecureId [In] The secure id of the process that owns the P&S propertirs used.

@see CLbsSuplPushRecChannel::NewL
*/
CLbsSuplPushRecChannel::CLbsSuplPushRecChannel(TLbsSuplPushChannel aChannel, MLbsSuplPushRecObserver& aObserver, 
		TUid aPropOwnerSecureId) : CActive(EPriorityStandard), 
		iObserver(aObserver), iChannel(aChannel), iPropOwnerSecureId(aPropOwnerSecureId)
	{
	LBSLOG(ELogP1, "CLbsSuplPushRecChannel::CLbsSuplPushRecChannel() Begin\n");
	if(aChannel==ELbsSuplPushChannelSMS)
		{
		iInitPropKey=KLbsSuplPushSmsInitKey;
		iAckPropKey=KLbsSuplPushSmsAckKey;
		}
	else
		{	
		iInitPropKey=KLbsSuplPushWapInitKey;
		iAckPropKey=KLbsSuplPushWapAckKey;
		}	
	CActiveScheduler::Add(this);
	LBSLOG(ELogP1, "CLbsSuplPushRecChannel::CLbsSuplPushRecChannel() End\n");
	}

/**
The 2nd phase constructor. Attaches to the property, completes the async request to cause the RunL 
method to be called for the first time and check if already there is a message set to the INIT property.

@leave If a error happens, it leaves with one of the system error codes.

@see CLbsSuplPushRecChannel::NewL
*/
void CLbsSuplPushRecChannel::ConstructL()
	{
	LBSLOG(ELogP1, "CLbsSuplPushRecChannel::ConstructL() Begin\n");
	User::LeaveIfError(iInitProperty.Attach(iPropOwnerSecureId, iInitPropKey, EOwnerThread));
	
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, KErrNone);
	SetActive();
	LBSLOG(ELogP1, "CLbsSuplPushRecChannel::ConstructL() End\n");
	}


/**
Called when the INIT property is changed and once on the construction of the object. Extracts 
the message from the INIT property, notifies both the observer by calling the 
MLbsSuplPushRecObserver::OnSuplInit function and the sender by setting the ACK property.
Then it resubscribes for the INIT property to listen for next messages.

@see CSuplPushImpl
@see MLbsSuplPushRecObserver::OnSuplInit
@see CActive::RunL
*/
void CLbsSuplPushRecChannel::RunL()
	{
	LBSLOG(ELogP1, "CLbsSuplPushRecChannel::RunL() Begin\n");
	if(iStatus==KErrNone)
		{
		iInitProperty.Subscribe(iStatus);
		SetActive();
		
		TPckgBuf<TInt> length;
		TInt err = RProperty::Get(iPropOwnerSecureId, iInitPropKey, length);
		
		//The INIT propery has been defined but not set yet.
		if(err==KErrNone && length.Length()==0)
			{
			LBSLOG(ELogP1, "CLbsSuplPushRecChannel::RunL() - err==KErrNone && length.Length()==0 End\n");
			return;
			}
		
		//Can't do anything here, just resubscribe
		if(err!=KErrOverflow || length()<=0)
			{
			//The property must be set in the correct value.
			__ASSERT_DEBUG(0, User::Invariant());
			LBSLOG(ELogP1, "CLbsSuplPushRecChannel::RunL() - err!=KErrOverflow || length()<=0 End\n");
			return;
			}

		TInt len = length() + 2*sizeof(TInt);
		
		HBufC8* msg = HBufC8::New(len);
		
		//Not enough memory to handle the message, just resubscribe 
		if(msg==0)
			{
			LBSLOG(ELogP1, "CLbsSuplPushRecChannel::RunL() - msg==0 End\n");
			return;
			}
		
		TPtr8 ptr = msg->Des();
		err = RProperty::Get(iPropOwnerSecureId, iInitPropKey, ptr);
		if(err!=KErrNone || ptr.Length()!=len)
			{
			delete msg;
			//Unexpected environment error or the length of the message is not equal the length declared in
			//the header.
			__ASSERT_DEBUG(0, User::Invariant());
			LBSLOG(ELogP1, 
				"CLbsSuplPushRecChannel::RunL() RProperty::Get(iPropOwnerSecureId, iInitPropKey, ptr)|| ptr.Length()!=len End\n");
			return;
			}
		
		TPckgBuf<TInt> reqId;
		reqId.Copy(msg->Mid(sizeof(TInt), sizeof(TInt))); 
		
		if(reqId()<=0)
			{
			delete msg;
			//Message is corrupted, the reqId must be > 0. 
			__ASSERT_DEBUG(0, User::Invariant());
			LBSLOG(ELogP1, "CLbsSuplPushRecChannel::RunL() - reqId()<=0 End\n");
			return;
			}

			
		TInt ackId = 0;
		err = RProperty::Get(iPropOwnerSecureId, iAckPropKey, ackId);

		if(err!=KErrNone || ackId<0)
			{
			delete msg;
			if(ackId<0)
				{
				RProperty::Set(iPropOwnerSecureId, iAckPropKey, 0);
				}
			//Unexpected environment error or wrong ackId, it must be > 0.
			__ASSERT_DEBUG(0, User::Invariant());
			LBSLOG(ELogP1, 
					"CLbsSuplPushRecChannel::RunL() - RProperty::Get(iPropOwnerSecureId, iAckPropKey, ackId)!=KErrNone || ackId<0 End\n");
			return;
			}
		

		//We notify the observer only if message was not delivered before
		if(ackId!=reqId())
			{
			TLbsSuplPushRequestId reqIdUnsgn = reqId(); 
			TPtrC8 msgPtr = msg->Right(length());
			err = RProperty::Set(iPropOwnerSecureId, iAckPropKey, reqId());
			//Unexpected environment error.
			__ASSERT_DEBUG(err==KErrNone, User::Invariant());
			iObserver.OnSuplInit(iChannel, reqIdUnsgn , msgPtr);
			LBSLOG(ELogP9, "<-S MLbsSuplPushRecObserver::OnSuplInit() SuplPush\n");
			LBSLOG2(ELogP9, "  > TLbsSuplPushChannel aChannel = %d\n", iChannel);
			LBSLOG2(ELogP9, "  > TLbsSuplPushRequestId aReqId = %d\n", reqIdUnsgn);
			
			}
						
		delete msg;
		}
	else
		{
		iInitProperty.Subscribe(iStatus);
		SetActive();
		}
	LBSLOG(ELogP1, "CLbsSuplPushRecChannel::RunL() End\n");
	}
/**
Cancels the subscription for the INIT property.

@see CActive::Cancel
@see CLbsSuplPushRecChannel::~CLbsSuplPushRecChannel
*/
void CLbsSuplPushRecChannel::DoCancel()
	{
	LBSLOG(ELogP1, "CLbsSuplPushRecChannel::DoCancel() Begin\n");
	iInitProperty.Cancel();
	LBSLOG(ELogP1, "CLbsSuplPushRecChannel::DoCancel() End\n");
	}

