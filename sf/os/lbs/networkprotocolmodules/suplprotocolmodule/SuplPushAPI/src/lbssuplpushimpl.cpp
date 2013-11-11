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
// Internal implementation of the SUPL Push API
// 
//

/**
 @file
 @internalComponent
 @deprecated
*/

#include <e32base.h>
#include <centralrepository.h>
#include "lbsdevloggermacros.h"

#include "lbssuplpushimpl.h"
#include "lbssuplpushprops.h"
#include "lbssystemcontroller.h"
#include "lbsrootcenrepdefs.h"


/** The time to live for a message in seconds. After a message is submitted via the SuplInit method,
it must be delivered to the SPM in this time. Otherwise, it is failed and 
MLbsSuplPushObserver::OnSuplInitComplete with aError==KErrTimedOut is called.*/ 
#ifdef TE_LBSSUPLPUSHIMPL_ON
const TInt KMsgTimeoutInterval = 5;
#else
const TInt KMsgTimeoutInterval = 30;
#endif //TE_LBSSUPLPUSHIMPL_ON

/** Maximumal possible length of the SUPL INIT message. 
   	This value is bigger and not exactly equal to the real maximal length of an ASN.1 encoded SUPL INIT message.
   	It is used only for cutting off mailformed extra large SUPL INIT messages. */
const TInt KLbsMaxSuplMsgLength = 2048;




/**
Constructor. 

@param aReqId [In] The message unique request id.
@param aMsg [In] The message with the request id record in the start of the buffer. The ownership is passed to CLbsSuplPushMsgInfo here.
*/  
CLbsSuplPushMsgInfo::CLbsSuplPushMsgInfo(TInt aReqId, HBufC8* aMsg) :
iReqId(aReqId), iMsg(aMsg)	
	{
	LBSLOG(ELogP1, "CLbsSuplPushMsgInfo::CLbsSuplPushMsgInfo() Begin\n");
	LBSLOG(ELogP1, "CLbsSuplPushMsgInfo::CLbsSuplPushMsgInfo() End\n");
	}


/**
Destructor. The iMsg buffer is deleted here.
*/ 
CLbsSuplPushMsgInfo::~CLbsSuplPushMsgInfo()
	{
	LBSLOG(ELogP1, "CLbsSuplPushMsgInfo::~CLbsSuplPushMsgInfo() Begin\n");
	delete iMsg;
	LBSLOG(ELogP1, "CLbsSuplPushMsgInfo::~CLbsSuplPushMsgInfo() End\n");
	}
			

/**
Creates an instance of the CLbsSuplPushImpl class.

@param aChannel [In] The id of the channel to be opened.
@param aObserver [In] A reference to an observer waiting for request completion call-backs.

@return An instance of the class. The calling application becomes the
owner of the returned instance and is responsible its disposal.

@see CLbsSuplPush::ConstructL
*/
CLbsSuplPushImpl* CLbsSuplPushImpl::NewL(TLbsSuplPushChannel aChannel, MLbsSuplPushObserver& aObserver)
	{
	LBSLOG(ELogP1, "CLbsSuplPushImpl::NewL() Begin\n");
	CLbsSuplPushImpl* newObj = new (ELeave) CLbsSuplPushImpl(aChannel, aObserver);
	CleanupStack::PushL(newObj);
	newObj->ConstructL(aChannel);
	CleanupStack::Pop(newObj);
	LBSLOG(ELogP1, "CLbsSuplPushImpl::NewL() End\n");
	return newObj;
	}
	
/**
Closes the interface and disposes all open or used resources.
*/
CLbsSuplPushImpl::~CLbsSuplPushImpl()
	{
	LBSLOG(ELogP1, "CLbsSuplPushImpl::~CLbsSuplPushImpl() Begin\n");
	Cancel();
	if(iState==EWaitingAck)
		{
		ReleaseBusyProp();
		}
	delete iTimer;
	
	iMsgQueue.ResetAndDestroy();
	iAckProperty.Close();
	iBusyProperty.Close();
	LBSLOG(ELogP1, "CLbsSuplPushImpl::~CLbsSuplPushImpl() End\n");
	}
	
/**
Constructor.

@param aChannel [In] The id of the channel to be opened.
@param aObserver [In] A reference to an observer waiting for request completion call-backs.
*/
CLbsSuplPushImpl::CLbsSuplPushImpl(TLbsSuplPushChannel aChannel, MLbsSuplPushObserver& aObserver):
CActive(CActive::EPriorityUserInput),
iChannel(aChannel),
iState(ECreated),
iObserver(aObserver)
	{
	LBSLOG(ELogP1, "CLbsSuplPushImpl::CLbsSuplPushImpl() Begin\n");
	CActiveScheduler::Add(this);
	LBSLOG(ELogP1, "CLbsSuplPushImpl::CLbsSuplPushImpl() End\n");
	}

/**
2nd phase constructor. 
Creates and assigns all the required internal resources.

@param aChannel [In] An id of the channel to be opened.
*/
void CLbsSuplPushImpl::ConstructL(TLbsSuplPushChannel aChannel)
	{
	LBSLOG(ELogP1, "CLbsSuplPushImpl::ConstructL() Begin\n");
	//There are only two channels available currently.
	__ASSERT_DEBUG(aChannel==ELbsSuplPushChannelSMS || aChannel==ELbsSuplPushChannelWAP,
			User::Invariant());
	
	if(!(aChannel==ELbsSuplPushChannelSMS || aChannel==ELbsSuplPushChannelWAP))
		{
		User::Leave(KErrGeneral);
		}
		
	if(aChannel==ELbsSuplPushChannelSMS)
		{
		iBusyPropKey=KLbsSuplPushSmsBusyKey;
		iInitPropKey=KLbsSuplPushSmsInitKey;
		iAckPropKey=KLbsSuplPushSmsAckKey;
		}
	else
		{	
		iBusyPropKey=KLbsSuplPushWapBusyKey;
		iInitPropKey=KLbsSuplPushWapInitKey;
		iAckPropKey=KLbsSuplPushWapAckKey;
		}
	
	iTimer = CLbsCallbackTimer::NewL(*this);
	LBSLOG(ELogP1, "CLbsSuplPushImpl::ConstructL() End\n");
	}

/**
Provides an internal implementation of CLbsSuplPush::SuplInit

@param aReqId [Out] A reference on the TLbsSuplPushRequestId variable where the assigned request id is written.
@param aMsg [In] The message buffer.
@param aReserved [In] Reserved for future use.

@return An error code related to the synchronous part of the request - KErrNone if successful, KErrArgument if 
the message has a wrong size, KErrNotReady if the LBS can't be started, KErrPermissionDenied if the calling process
has not enough capabilities, or another system error code.

@see MLbsSuplPushObserver::OnSuplInitComplete
@see CLbsSuplPush::SuplInit
*/
TInt CLbsSuplPushImpl::SuplInit(TLbsSuplPushRequestId& aReqId, const TDesC8& aMsg, TInt /*aReserved*/)
	{
	LBSLOG(ELogP1, "CLbsSuplPushImpl::ConstructL() Begin\n");
	TRAPD(err, SuplInitL(aReqId, aMsg));
	LBSLOG(ELogP1, "CLbsSuplPushImpl::ConstructL() End\n");
	return err;
	}

/**
A leaving implementation of the CLbsSuplPushImpl::SuplInit. 

@param aReqId [Out] A reference on the TLbsSuplPushRequestId variable where the assigned request id is written.
@param aMsg [In] The message buffer.

@leave If a error happens, it leaves with the correspondent error code.

@see CLbsSuplPushImpl::SuplInit
*/
void CLbsSuplPushImpl::SuplInitL(TLbsSuplPushRequestId& aReqId, const TDesC8& aMsg)
	{
	LBSLOG(ELogP1, "CLbsSuplPushImpl::SuplInitL() Begin\n");
	aReqId = 0;
	
	if(aMsg.Length()>KLbsMaxSuplMsgLength || aMsg.Length()==0)
		{
		//The messages out of these bounds must not arrive. If they do please check the source of the messages
		//and make sure they are correct.
		__ASSERT_DEBUG(0, User::Invariant());
		User::Leave(KErrArgument);
		}
	
	if(iState==ECreated)
		{
				
		// Get the CategoryUid from the cenrep file owned by LbsRoot.
		TInt category;
		CRepository* rep = CRepository::NewLC(KLbsCenRepUid);
		User::LeaveIfError(rep->Get(KSuplPushAPIKey, category));
		CleanupStack::PopAndDestroy(rep);
		iPropOwnerSecureId = TUid::Uid(category);
		
		//Initializing the BUSY property if it was not initialized before
		TInt val;
		User::LeaveIfError(RProperty::Get(iPropOwnerSecureId, iBusyPropKey, val));
		if(val==0)
			{
			User::LeaveIfError(RProperty::Set(iPropOwnerSecureId, iBusyPropKey, 1));
			}
		
		//Attaching to the BUSY and ACK properties
		User::LeaveIfError(iBusyProperty.Attach(iPropOwnerSecureId, iBusyPropKey, EOwnerThread));
		User::LeaveIfError(iAckProperty.Attach(iPropOwnerSecureId, iAckPropKey, EOwnerThread));
		
		iState=EInitialized; 
		}
		
	TBool isBusy;
	TInt reqId;
	User::LeaveIfError(GetBusyData(isBusy, reqId));
	User::LeaveIfError(SetBusyData(isBusy, reqId+1));
	
	HBufC8* buf=HBufC8::NewL(2*sizeof(TInt)+aMsg.Length());
	CleanupStack::PushL(buf);
	TPtr8 ptr=buf->Des();
	TInt length = aMsg.Length(); 
	ptr.Append(TPckgC<TInt>(length));
	ptr.Append(TPckgC<TInt>(reqId));
	ptr.Append(aMsg);
	
	CLbsSuplPushMsgInfo* msg = new(ELeave) CLbsSuplPushMsgInfo(reqId, buf);
	CleanupStack::Pop(buf);
	
	CleanupStack::PushL(msg);
	iMsgQueue.AppendL(msg);
	CleanupStack::Pop(msg);

	if(iState==EInitialized)
		{
		//if not leave, we switch either to EWaitingAck or EWaitingBusy state here
		User::LeaveIfError(SendMessage(EFalse)); 		
		}
	aReqId = reqId;
	LBSLOG(ELogP1, "CLbsSuplPushImpl::SuplInitL() End\n");
	}


/**
Tries to send the next message in the queue. If the channel is busy, it subscribes for the BUSY property
and wait asynchronously when the property is free.

@param aNotifyObserver [In] If the function must notify the observer when a error happens and the message fails.

@return A error code if error happens, KErrNone - otherwise.

@see CLbsSuplPushImpl::SuplInitL
@see CLbsSuplPushImpl::OnTimerEventL
@see CLbsSuplPushImpl::RunL
*/
TInt CLbsSuplPushImpl::SendMessage(TBool aNotifyObserver)
	{
	LBSLOG(ELogP1, "CLbsSuplPushImpl::SendMessage() Begin\n");
	//A message can be sent only when we are in the EInitialized state. And it must be in the queue, so the queue
	//can't be empty.
	__ASSERT_DEBUG(iState==EInitialized && iMsgQueue.Count()>0, User::Invariant());	
		
	CLbsSuplPushMsgInfo* msg=iMsgQueue[0];
	TBool isBusy;
	TInt reqId;
	
	TInt err = GetBusyData(isBusy, reqId);
	if(err==KErrNone)
		{
		if(isBusy)
			{
			iBusyProperty.Subscribe(iStatus);
			SetActive();
			iState = EWaitingBusy;
			LBSLOG(ELogP1, "CLbsSuplPushImpl::SendMessage() End\n");
			return KErrNone;
			}
	
		iAckProperty.Subscribe(iStatus);
		SetActive();
	
		err = RProperty::Set(iPropOwnerSecureId, iInitPropKey, *msg->iMsg);
		if(err==KErrNone)
			{
			err=SetBusyData(ETrue, reqId);
			if(err==KErrNone)
				{
				iState=EWaitingAck;
				iTimer->EventAfter(TTimeIntervalSeconds(KMsgTimeoutInterval), 0);
				LBSLOG(ELogP1, "CLbsSuplPushImpl::SendMessage() End\n");
				return KErrNone;
				}
			}
		//err!=KErrNone)
		Cancel();
		}
	
	//err!=KErrNone)
	if(aNotifyObserver)
		{
		iObserver.OnSuplInitComplete(iChannel, msg->iReqId, err, 0);
		LBSLOG(ELogP9, "<-S MLbsSuplPushObserver::OnSuplInitComplete() SuplPush\n");
		LBSLOG2(ELogP9, "  > TLbsSuplPushChannel aChannel = %d\n", iChannel);
		LBSLOG2(ELogP9, "  > TLbsSuplPushRequestId aReqId = %d\n", msg->iReqId);
		LBSLOG2(ELogP9, "  > TInt aError = %d\n", err);
		}
	iMsgQueue.Remove(0);
	delete msg;
	LBSLOG(ELogP1, "CLbsSuplPushImpl::SendMessage() End\n");
	return err;
	}

/**
From MLbsCallbackTimerObserver. Called when a message timeout error happens. It removes the outdated message 
from the queue, notifies the observer that that message fails. Then it tries to deliver the next message if there is any.
If there is no messages to deliver and the object is in the EWaitingBusy state, it unsubscribes 
from the BUSY property.

@see CLbsCallbackTimer
@see CLbsSuplPushImpl::OnTimerError
*/
void CLbsSuplPushImpl::OnTimerEventL(TInt /*aTimerId*/)
	{
	LBSLOG(ELogP1, "CLbsSuplPushImpl::SendMessage() Begin\n");
	//A timer event can occur only when we are trying to deliver a message. That is we must be in 
	//EWaitingAck state.
	__ASSERT_DEBUG((iMsgQueue.Count()>0 && iState==EWaitingAck), User::Invariant());
			
	//Remove the expired message from the queue and notify the observer
	CLbsSuplPushMsgInfo* msg=iMsgQueue[0];
	iObserver.OnSuplInitComplete(iChannel, msg->iReqId, KErrTimedOut, 0);
	LBSLOG(ELogP9, "<-S MLbsSuplPushObserver::OnSuplInitComplete() SuplPush\n");
	LBSLOG2(ELogP9, "  > TLbsSuplPushChannel aChannel = %d\n", iChannel);
	LBSLOG2(ELogP9, "  > TLbsSuplPushRequestId aReqId = %d\n", msg->iReqId);
	LBSLOG(ELogP9, "  > TInt aError = KErrTimedOut\n");
	delete msg;
	iMsgQueue.Remove(0);
	
	
		
	//end the ACK subscription
	Cancel();
			
	//Release the BUSY property. 
	//The iState is set to EInitialized
	ReleaseBusyProp(); 
			
	//Schedule the next timeout event and start to deliver next message
	while(iMsgQueue.Count()>0)
		{
		//The message is deleted in the SendMessage if the sending attempt fails
		if(SendMessage(ETrue)==KErrNone)
			{
			break;
			}
		}
	LBSLOG(ELogP1, "CLbsSuplPushImpl::SendMessage() End\n");
	}


/**
Called either when in the EWaitingAck state and  the ACK property is changed or when in the EWaitingBusy
state and the BUSY property is changed. 

In the EWaitingAck state it checks that it is really the notification on 
the message delivering now. If it is, it notifies the observer about the successful delivery. If not,
it resubscribes, returns immediately and the object continues to wait for the correct notification 
asynchronously.

In the EWaitingBusy state it checks if the channel is free. If it is not, it resubscribes, returns immideatelly,
and the object continues to wait when the channel is free asynchronously.

In both states if the object is in the EInitialized state and ready to deliver the next message, the function
tries to deliver the message if there is any.

@leave The function does not leave despite it has the L suffix. So, we do not need to imlement 
the CActive::RunError function.

@see CActive::RunL
*/
/*virtual*/ void CLbsSuplPushImpl::RunL()
	{
	LBSLOG(ELogP1, "CLbsSuplPushImpl::SendMessage() Begin\n");
	//The asyncronous request can be only either in EWaitingAck or EWaitingBusy state.
	__ASSERT_DEBUG(iState==EWaitingAck || iState==EWaitingBusy, User::Invariant());
	if(EWaitingAck==iState)
		{
		//At least the message being delivered must be in the queue.
		__ASSERT_DEBUG(iMsgQueue.Count()>0, User::Invariant());
			
		CLbsSuplPushMsgInfo* msg = iMsgQueue[0];			
		TInt err = iStatus.Int();
		if(err==KErrNone)
			{
			TInt ack;
			err=RProperty::Get(iPropOwnerSecureId, iAckPropKey, ack);
			if(err==KErrNone && ack!=msg->iReqId) 
				{
				//It was not our message. It must not happen.
				__ASSERT_DEBUG(0, User::Invariant());
				//We continue to listen for our message
				iAckProperty.Subscribe(iStatus);
				SetActive();
				LBSLOG(ELogP1, "CLbsSuplPushImpl::SendMessage() End\n");
				return;
				}
			}

		iObserver.OnSuplInitComplete(iChannel, msg->iReqId, err, 0);
		LBSLOG(ELogP9, "<-S MLbsSuplPushObserver::OnSuplInitComplete() SuplPush\n");
		LBSLOG2(ELogP9, "  > TLbsSuplPushChannel aChannel = %d\n", iChannel);
		LBSLOG2(ELogP9, "  > TLbsSuplPushRequestId aReqId = %d\n", msg->iReqId);
		LBSLOG2(ELogP9, "  > TInt aError = %d\n", err);
			
		delete msg;
		iMsgQueue.Remove(0);
		ReleaseBusyProp();
		iTimer->Cancel();  //cancel timeout event here
		
		}
	else//	EWaitingBusy:
		{
		TBool isBusy;
		TInt reqId;
		TInt err= GetBusyData(isBusy, reqId);
		if(err!=KErrNone || isBusy) //Still waiting
			{
			iBusyProperty.Subscribe(iStatus);
			SetActive();
			LBSLOG(ELogP1, "CLbsSuplPushImpl::SendMessage() End\n");
			return;
			}
		iState = EInitialized;
		}
	
	//If there's been no error, we must be ready for sending messages and switch to EInitialized state.
	__ASSERT_DEBUG(EInitialized == iState, User::Invariant());
	
	//Start to deliver next message
	while(iMsgQueue.Count()>0)
		{
		//The message is deleted in the SendMessage if the sending attempt fails
		if(SendMessage(ETrue)==KErrNone)
			{
			break;
			}
		}
	
	LBSLOG(ELogP1, "CLbsSuplPushImpl::SendMessage() End\n");
	}
	
/**
Called when CActive::Cancel method is called. It cancels both the ACK and BUSY subscriptions. Only one of
the two subscriptions can be actually active at every moment. But it is harmless to cancel inactive
subscription.

@see CActive::DoCancel
@see CLbsSuplPushImpl::~CLbsSuplPushImpl
@see CLbsSuplPushImpl::SendMessage
@see CLbsSuplPushImpl::OnTimerEventL
*/
/*virtual*/ void CLbsSuplPushImpl::DoCancel()
	{
	LBSLOG(ELogP1, "CLbsSuplPushImpl::SendMessage() Begin\n");
	if(iState!=ECreated)
		{
		iAckProperty.Cancel();
		iBusyProperty.Cancel();
		}
	LBSLOG(ELogP1, "CLbsSuplPushImpl::SendMessage() End\n");
	}

/**
Tries to release the BUSY property. Switches to the EInitialized state in any case even though a error happens.

@return A error code if error happens, KErrNone - otherwise.

@see CLbsSuplPushImpl::~CLbsSuplPushImpl
@see CLbsSuplPushImpl::OnTimerEventL
@see CLbsSuplPushImpl::RunL
*/
TInt CLbsSuplPushImpl::ReleaseBusyProp()
	{
	LBSLOG(ELogP1, "CLbsSuplPushImpl::ReleaseBusyProp() Begin\n");
	//We must have grabbed the BUSY property before trying to release it.
	__ASSERT_DEBUG(iState==EWaitingAck, User::Invariant());
	
	TBool isBusy;
	TInt reqId;
	TInt err=GetBusyData(isBusy, reqId);
	if(err==KErrNone)
		{
		//As we have grabbed the BUSY property, it must be in the BUSY state.
		__ASSERT_DEBUG(isBusy, User::Invariant());
		err=SetBusyData(EFalse, reqId);
		}
	
	if(err!=KErrNone)
		{
		//We do not expect any environment errors here. The properties must be in correct state and
		//RProperty::Get & Set methods must not fail here.
		__ASSERT_DEBUG(0, User::Invariant());
		}
	
	//We switch to EInitialized, even though error happened and we failed to release the BUSY property
	iState = EInitialized;
	
	LBSLOG(ELogP1, "CLbsSuplPushImpl::ReleaseBusyProp() End\n");
	return err;
	}


/**
Reads the busy flag and the next free request id from the BUSY property.

@param aBusy [Out] The busy flag.
@param aChannel [Out] The next free request id.

@return A error code if error happens, KErrNone - otherwise.

@see CLbsSuplPushImpl::SuplInitL
@see CLbsSuplPushImpl::SendMessage
@see CLbsSuplPushImpl::ReleaseBusyProp
*/
TInt CLbsSuplPushImpl::GetBusyData(TBool& aBusy, TInt& aNextReqId) const
	{
	LBSLOG(ELogP1, "CLbsSuplPushImpl::GetBusyData() Begin\n");
	TInt val;
	TInt err = RProperty::Get(iPropOwnerSecureId, iBusyPropKey, val);
	if(err!=KErrNone)
		{
		LBSLOG(ELogP1, "CLbsSuplPushImpl::GetBusyData() End\n");
		return err;
		}
	
	if(val>0)
		{
		aNextReqId = val;
		aBusy = EFalse;	
		}
	else if(val<0)
		{
		aNextReqId = -val;
		aBusy = ETrue;	
		}
	
	else 
		{
		//val==0. This must not happen - the property is initialized with positive value and then is changed by our
		//code only to either postive or negative values.
		__ASSERT_DEBUG(0, User::Invariant());
		LBSLOG(ELogP1, "CLbsSuplPushImpl::GetBusyData() End\n");
		return KErrGeneral;
		}
	LBSLOG(ELogP1, "CLbsSuplPushImpl::GetBusyData() End\n");
	return KErrNone;
	}


/**
Writes the busy flag and the next free request id from the BUSY property.

@param aBusy [In] The busy flag.
@param aChannel [In] The next free request id.

@return A error code if error happens, KErrNone - otherwise.

@see CLbsSuplPushImpl::SuplInitL
@see CLbsSuplPushImpl::SendMessage
@see CLbsSuplPushImpl::ReleaseBusyProp
*/
TInt CLbsSuplPushImpl::SetBusyData(TBool aBusy, TInt aNextReqId) const
	{
	LBSLOG(ELogP1, "CLbsSuplPushImpl::SetBusyData() Begin\n");
	//The BUSY property must not be already grabbed.
	__ASSERT_DEBUG(aNextReqId>0, User::Invariant());
	
	//reseting to one and start from the very beginning
	if(aNextReqId==KMaxTInt)
		{
		aNextReqId=1;
		}
		
	TInt val=aNextReqId;
	if(aBusy)
		{
		val=-val;
		}
	LBSLOG(ELogP1, "CLbsSuplPushImpl::SetBusyData() End\n");
	return RProperty::Set(iPropOwnerSecureId, iBusyPropKey, val);
	}

/**
From MLbsCallbackTimerObserver. Normally it is called when OnTimerEventL leaves. Our implementation 
of OnTimerEventL does not leave, so this function must not be called.

@see MLbsCallbackTimerObserver
@see CLbsCallbackTimer
*/
TInt CLbsSuplPushImpl::OnTimerError(TInt /*aTimerId*/, TInt /*aError*/)
	{
	LBSLOG(ELogP1, "CLbsSuplPushImpl::SetBusyData() Begin\n");
	__ASSERT_DEBUG(0, User::Invariant());
	LBSLOG(ELogP1, "CLbsSuplPushImpl::SetBusyData() End\n");
	return KErrNone;
	}



