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

#include <e32cons.h>
#include <f32file.h>
#include <s32mem.h>

#include <lbs/lbsprivacyextnotifiers.h>

//#include "privfwmarshallingstubs.h"
#include "lbsprivnotifier.h"


const TUid KScreenOutputChannel2 ={0x00000666};
const TUid KScreenOutputChannel3 ={0x00000667};

// The CLbsPrivacyNotifierBase class provides the base functionality for the
// privacy notifiers (notify and verify).

CLbsPrivacyNotifierBase::~CLbsPrivacyNotifierBase()
	{
	Cancel();
	iTimer.Close();
	//delete iResponseWriter;
	//delete iMsgReader;
	delete 	iHandlerChannel;
	}

CLbsPrivacyNotifierBase::CLbsPrivacyNotifierBase()
		:	CActive(EPriorityStandard)
	{
	iChannelCreated = EFalse;
	}

void CLbsPrivacyNotifierBase::Release()
	{
	delete this;
	}

TPtrC8 CLbsPrivacyNotifierBase::StartL(const TDesC8& aBuffer)
	{
	(void)aBuffer;
		return KNullDesC8();
	}

void CLbsPrivacyNotifierBase::ProcessBuffer(const TDesC8& aBuffer, TMessageType /*aMsgType*/)
	{
//	if(!iPipesCreated)
//		{ // Need to create pipes here (and NOT in constructor) to allow test code to pre-define the properties
//
//		// Attach to the write pipe.
//	 	iResponseWriter   = CPrivFwWritePropPipe::NewL(KStubToProxyPipeKey, KStubToProxyFlowKey, ETrue);
//	 	
//	 	// Only attach to the read pipe for verifiers. We don't expect to receive response data for notify notifiers.
//	 	if (iNotifierType == ENotifierTypeVerify)
//	 		{
//	 		iMsgReader = CPrivFwNotifyReadPropPipe::NewL(KProxyToStubPipeKey, KProxyToStubFlowKey, *this, ETrue);	
//	 		}
//
//	 	else
//	 		{
//	 		iMsgReader = NULL;
//	 		}
//	 	
//		iPipesCreated = ETrue;
//		}
	
	if(!iChannelCreated)
		{
		iHandlerChannel = CTestRequestChannel::NewL(this, RLbsTestChannel::EChannelHandler2Proxy);
		//iProxyChannel = CTestRequestChannel::NewL(this, RLbsTestChannel::EChannelProxy2Handler);
		iChannelCreated = ETrue;			
		}

	TLbsPrivacyNotifierParamsPckg params;
	params.Copy(aBuffer);
	TLbsExternalRequestInfo info;
	params().GetRequesterInfo(info);

	SendRequestDetails(EMessageVerifyRequest, EPosCancelReasonNotAvailable, info);	
	}

void CLbsPrivacyNotifierBase::SendRequestDetails(TMessageType aMsgType, TPosVerifyCancelReason aCancelReason, TLbsExternalRequestInfo& aRequestInfo)
	{
	//Write response to queue

	(void) aMsgType;
	(void) aCancelReason;
	
	TUint reqIdNull = 0;
	if(iNotifierType == ENotifierTypeVerify)
		{
		TLbsPrivFwVerifyRequestMsg* privReqMsg = new (ELeave) TLbsPrivFwVerifyRequestMsg(reqIdNull); //No RequestId
		ConvertToTestRequestInfo(privReqMsg->iTestRequestInfo, aRequestInfo);
		iHandlerChannel->SendPrivFwRequestMessage(*privReqMsg);
		delete privReqMsg;
		}
	else
		{
		TLbsPrivFwNotifyRequestMsg* privReqMsg = new (ELeave) TLbsPrivFwNotifyRequestMsg(reqIdNull);  //No RequestId
		ConvertToTestRequestInfo(privReqMsg->iTestRequestInfo, aRequestInfo);
		iHandlerChannel->SendPrivFwRequestMessage(*privReqMsg);
		delete privReqMsg;
		}
	}

void CLbsPrivacyNotifierBase::ConvertToTestRequestInfo(TLbsTestRequestInfo& aTestRequestInfo, TLbsExternalRequestInfo& aRequestInfo)
	{

	aTestRequestInfo.SetNetworkType(aRequestInfo.NetworkType());
	aTestRequestInfo.SetRequestType(aRequestInfo.RequestType());

	aTestRequestInfo.SetClientNameFormat(aRequestInfo.ClientNameFormat());
	TLbsClientName2 clientName;
	aRequestInfo.GetClientName(clientName);
	aTestRequestInfo.SetClientName(clientName);

	aTestRequestInfo.SetRequesterIdFormat(aRequestInfo.RequesterIdFormat());
	TLbsRequesterId2 requesterName;
	aRequestInfo.GetRequesterId(requesterName);
	aTestRequestInfo.SetRequesterId(requesterName);
			
	}

void CLbsPrivacyNotifierBase::StartTimer(const TTimeIntervalMicroSeconds& aCompletionDelay)
	{
	// Start a timer to complete message very soon.
	iTimer.After(iStatus, aCompletionDelay.Int64());
	SetActive();
	}

void CLbsPrivacyNotifierBase::DoCancel()
	{
	iTimer.Cancel();
	}

void CLbsPrivacyNotifierBase::RunL()
	{
	// "User response" is ready - so complete the Message.
	CompleteMessage(KErrNone);
	
	}

void CLbsPrivacyNotifierBase::Cancel()
/**
 * The notifier has been deactivated so resources can be freed and outstanding messages completed.
 */
	{
	CActive::Cancel();
	TLbsExternalRequestInfo info;
	//We will not use info in case of cancel so we can send default information
	//We are sending EPosCancelReasonNotAvailable all the time - probably we should find the 
	//actual cancel reason and send it across - TBD
	SendRequestDetails(EMessageVerifyCancelRequest, EPosCancelReasonNotAvailable, info);	
	CompleteMessage(KErrCancel);
	}

TPtrC8 CLbsPrivacyNotifierBase::UpdateL(const TDesC8& /*aBuffer*/)
/**
 * Update a currently active notifier with data aBuffer.
 */
	{
	return TPtrC8();
	}

void CLbsPrivacyNotifierBase::CompleteMessage(TInt aRet)
	{
	TLbsPrivacyResponseParams response;
	if (iCompletionResponse != EResponseTypeUnknown)
		{
		// A "user response" is expected (accept or reject)
		// Decision based on presence of named files
		// Absence of either file indicates that no response should be returned
		// (to simulate user ignoring dialog)
		switch(iCompletionResponse)
			{
			case EResponseTypeAccepted:
				response.SetResponseType(EResponseAccepted);
				break;

			case EResponseTypeRejected:
				response.SetResponseType(EResponseRejected);
				break;
			
			case EResponseTypeTimedOut:
				response.SetResponseType(EResponseTimedOut);
				break;
				
			default:
				{
				break;
				}
			}
		}
	else
		{
		response.SetResponseType(EResponseTimedOut);
		}

	TLbsPrivacyResponseParamsPckg pckg(response);
	if(iMessage.Handle())
		{
		iMessage.Write(2, pckg); // response buffer is 2nd param
		iMessage.Complete(aRet);
		}
	}

void CLbsPrivacyNotifierBase::ProcessTestChannelRequestMessage(RLbsTestChannel::TLbsTestChannelId aChannelId, const TLbsTestChannelMsgBase& aMessage)
	{
	__ASSERT_ALWAYS(aChannelId == RLbsTestChannel::EChannelHandler2Proxy, User::Invariant());
	
	//If we are getting response from Notifier, we do not need to send any response
	
	if(iNotifierType == ENotifierTypeNotify)
		{
		return;
		}
		
	switch(aMessage.Type())
		{
		case TLbsTestChannelMsgBase::EMessageNotifyResponse:
			{
			//We should not response in case of Notify request	
			}
			break;
			
		case TLbsTestChannelMsgBase::EMessageVerifyResponse:
			{
			const TLbsPrivFwVerifyResponseMsg& responseMsg = static_cast<const TLbsPrivFwVerifyResponseMsg&> (aMessage);	

			iCompletionResponse = responseMsg.iResponse;
			//iCompletionDelay = responseMsg.iResponseDelay;
			
			if (responseMsg.iResponseDelay.Int64() == 0)
				{
				CompleteMessage(KErrNone);
				}
			else
				{
			    // Start timer to dismiss dialog after delay period
			    //iCompletionTimer->Start(responseMsg.iResponseDelay.Int64(), 0, TCallBack(CompletionTimerCallBack, this));
				StartTimer(responseMsg.iResponseDelay);
				}
			
			}
			break;
			
		default:
			break;
			
		}
	}

//void CLbsPrivacyNotifierBase::OnReadyToReadL(CPrivFwReadPropPipe& aPipe)
//	{
//	TInt32 msgType;
//
//	aPipe.RefreshL(0);
//	aPipe >> msgType;
//	
//	// Not expecting any notify messages from the test request object, ever. However if we
//	// do get a msg, read and do nothing.
//	if ((msgType == EMessageNotifyRequest) || (iNotifierType == ENotifierTypeNotify))
//		{
//		TInt32 tempCompletionResponse;
//		aPipe >> iCompletionRequestId;
//		aPipe >> tempCompletionResponse;
//		PrivFwTestMarshaller::ReadL(aPipe, iCompletionDelay);
//		}
//
//	// Process a valid verify message.
//	else
//		{
//		TInt32 tempCompletionResponse;
//		aPipe >> iCompletionRequestId;
//		aPipe >> tempCompletionResponse;
//		PrivFwTestMarshaller::ReadL(aPipe, iCompletionDelay);
//		iCompletionResponse = static_cast<TResponseType>(tempCompletionResponse);
//		if (iCompletionDelay->Int64() == 0)
//			{
//			CompleteMessage(KErrNone);
//			}
//		else
//			{
//		    // Start timer to dismiss dialog after delay period
//			StartTimer();
//			}
//		}	
//
//	//TBD::: We might need to cancel timer 	- BRAJ
//	}


//
// CLbsNotifyNotifier													//
//
CLbsPrivacyNotifierBase* CLbsNotifyNotifier::NewL()
	{
	CLbsNotifyNotifier* self=new (ELeave) CLbsNotifyNotifier();
	CleanupStack::PushL(self);
	self->ConstructL();
	CActiveScheduler::Add(self);
	CleanupStack::Pop();
	return self;
	}

void CLbsNotifyNotifier::ConstructL()
	{
	//User::LeaveIfError(iTimer.CreateLocal());
	iInfo.iUid=KLbsExtLocationRequestNotifyUid;
	iInfo.iChannel=KScreenOutputChannel2;
	iInfo.iPriority=ENotifierPriorityLow;

	iNotifierType = ENotifierTypeNotify;
	}
	
	
CLbsNotifyNotifier::CLbsNotifyNotifier()
	{
	}

CLbsNotifyNotifier::TNotifierInfo CLbsNotifyNotifier::RegisterL()
/**
 * Called when a notifier is first loaded to allow any initial construction that is required.
 */
	{
	iInfo.iUid=KLbsExtLocationRequestNotifyUid;
	iInfo.iChannel=KScreenOutputChannel2;
	iInfo.iPriority=ENotifierPriorityLow;
	return iInfo;
	}

CLbsNotifyNotifier::TNotifierInfo CLbsNotifyNotifier::Info() const
	{
	return iInfo;
	}

void CLbsNotifyNotifier::RunL()
	{
	// Timer completed, indicating notifier is no longer active.
	Cancel();
	}

void CLbsNotifyNotifier::Cancel()
/**
 * The notifier has been deactivated
 */
	{
	CActive::Cancel();
	}

TPtrC8 CLbsNotifyNotifier::StartL(const TDesC8& aBuffer)
	{
	(void)aBuffer;
	
	ProcessBuffer(aBuffer, EMessageNotifyRequest);
	
	return KNullDesC8();
	}

void CLbsNotifyNotifier::StartL(const TDesC8& aBuffer, TInt aReplySlot, const RMessagePtr2& aMessage)
/**
 * Start the notifier with data aBuffer.  aMessage should be completed when the notifier is deactivated.
 * May be called multiple times if more than one client starts the notifier.  The notifier is immediately
 * responsible for completing aMessage.
 */
	{
	(void) aReplySlot;

	ProcessBuffer(aBuffer, EMessageNotifyRequest);

	iMessage = aMessage; // message completed when timer expires
	}


//
// CLbsVerifyNotifier													//
//
CLbsPrivacyNotifierBase* CLbsVerifyNotifier::NewL()
	{
	CLbsVerifyNotifier* self=new (ELeave) CLbsVerifyNotifier();
	CleanupStack::PushL(self);
	self->ConstructL();
	CActiveScheduler::Add(self);
	CleanupStack::Pop();
	return self;
	}

void CLbsVerifyNotifier::ConstructL()
	{
	User::LeaveIfError(iTimer.CreateLocal());
	iInfo.iUid=KLbsExtLocationRequestVerifyUid;
	iInfo.iChannel=KScreenOutputChannel3;
	iInfo.iPriority=ENotifierPriorityLow;

	iNotifierType = ENotifierTypeVerify;
	}
	
CLbsVerifyNotifier::CLbsVerifyNotifier()
	{
	}

CLbsVerifyNotifier::TNotifierInfo CLbsVerifyNotifier::RegisterL()
/**
 * Called when a notifier is first loaded to allow any initial construction that is required.
 */
	{
	iInfo.iUid=KLbsExtLocationRequestVerifyUid;
	iInfo.iChannel=KScreenOutputChannel3;
	iInfo.iPriority=ENotifierPriorityLow;
	return iInfo;
	}

CLbsVerifyNotifier::TNotifierInfo CLbsVerifyNotifier::Info() const
	{
	return iInfo;
	}

void CLbsVerifyNotifier::StartL(const TDesC8& aBuffer, TInt aReplySlot, const RMessagePtr2& aMessage)
/**
 * Start the notifier with data aBuffer.  aMessage should be completed when the notifier is deactivated.
 * May be called multiple times if more than one client starts the notifier.  The notifier is immediately
 * responsible for completing aMessage.
 */
	{
	(void) aReplySlot;

	ProcessBuffer(aBuffer, EMessageVerifyRequest);

	iMessage = aMessage; // message completed when timer expires
	}
