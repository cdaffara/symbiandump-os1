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
//

/**
 @file lbsprivfwctrlstub.cpp
*/
 
#include <e32base.h>
#include <lbs/lbsloccommon.h>

#include "lbstestprivacycontroller.h"

const TInt KLbsResponseTimeout = 10*1000000; // 10 secs 

CTestPrivacyController* CTestPrivacyController::NewL()
	{
	CTestPrivacyController* self = new (ELeave) CTestPrivacyController;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CTestPrivacyController::~CTestPrivacyController()
	{
	delete iLbsPrivacyController;
	iLbsPrivacyController = NULL;
    
    iTimer->Cancel();
	delete iTimer;
	
	if(iChannelCreated)
		{
		delete 	iHandlerChannel;
		}
	iCompletionTimerArray.Close();
	iCompletionRequestIdArray.Close();
	}

CTestPrivacyController::CTestPrivacyController()
    : iLastUsedRequestId(0xFFFFFFFF)
	{
	}

void CTestPrivacyController::ConstructL()
	{
	iLbsPrivacyController = CLbsPrivacyController::NewL(*this);

	iTimer = CPeriodic::NewL(CActive::EPriorityStandard);
    
	iHandlerChannel = CTestRequestChannel::NewL(this, RLbsTestChannel::EChannelHandler2Proxy);
	iChannelCreated = ETrue;
	}

		
void CTestPrivacyController::ProcessNetworkLocationRequest(TUint aRequestId, const TLbsExternalRequestInfo& aRequestInfo, const TNotificationType& aNotificationType)
	{
	
	switch(aNotificationType)
		{
		case ENotifyAndVerifyLocationAcceptedIfNoReponse:
		case ENotifyAndVerifyLocationRejectedIfNoReponse:
			{
			TCompletionTimer	timer;
			timer.iRequestId = aRequestId;
			timer.iIsTimerCreated = ETrue;
			timer.iCompletionTimer = CPeriodic::NewL(CActive::EPriorityStandard);
			iCompletionTimerArray.Append(timer);

			TLbsPrivFwVerifyRequestMsg* privReqMsg = new (ELeave) TLbsPrivFwVerifyRequestMsg(aRequestId);
			const TLbsExternalRequestInfo2* dummy = static_cast<const TLbsExternalRequestInfo2*> (&aRequestInfo);
			ConvertToTestRequestInfo(privReqMsg->iTestRequestInfo, *dummy);
			iHandlerChannel->SendPrivFwRequestMessage(*privReqMsg);
			delete privReqMsg;
			break;
			}

		case ENotifyLocationAccepted:
		case ENotifyLocationRejected:
			{
			TLbsPrivFwNotifyRequestMsg* privReqMsg = new (ELeave) TLbsPrivFwNotifyRequestMsg(aRequestId);
			const TLbsExternalRequestInfo2* dummy = static_cast<const TLbsExternalRequestInfo2*> (&aRequestInfo);
			ConvertToTestRequestInfo(privReqMsg->iTestRequestInfo, *dummy);
			iHandlerChannel->SendPrivFwRequestMessage(*privReqMsg);
			delete privReqMsg;
			break;
			}

		default:
			//Panic
			User::Invariant();
		}
		
	ResetResponseTimer();
	}

void CTestPrivacyController::ConvertToTestRequestInfo(TLbsTestRequestInfo& aTestRequestInfo, const TLbsExternalRequestInfo2& aRequestInfo)
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

void CTestPrivacyController::ProcessNetworkPositionUpdate(TUint aRequestId, const TPositionInfo& aPosInfo)
	{
	(void) aRequestId;
	(void) aPosInfo;
	User::Invariant();
	
	// This function should never be called in standalone privacy mode.
	}

void CTestPrivacyController::ProcessRequestComplete(TUint aRequestId, TInt aReason)
	{	
	//Send Response with other information
	TInt arraySize = iCompletionTimerArray.Count();
	for(int i = 0; i < arraySize; i++)
		{
		if(iCompletionTimerArray[i].iRequestId == aRequestId)
			{
			if(iCompletionTimerArray[i].iIsTimerCreated)
				{
				iCompletionTimerArray[i].iCompletionTimer->Cancel();
				delete iCompletionTimerArray[i].iCompletionTimer;
				iCompletionTimerArray.Remove(i);
				iLastUsedRequestId = aRequestId;
				}
			break;
			}
		}

	TLbsPrivFwSessionCompleteMsg* sessionCompleteMsg = new (ELeave) TLbsPrivFwSessionCompleteMsg(aRequestId);
	sessionCompleteMsg->iCompleteReason = aReason;
	iHandlerChannel->SendPrivFwRequestMessage(*sessionCompleteMsg);
	
	delete sessionCompleteMsg;

	}



void CTestPrivacyController::ProcessTestChannelRequestMessage(RLbsTestChannel::TLbsTestChannelId aChannelId, const TLbsTestChannelMsgBase& aMessage)
	{
	__ASSERT_ALWAYS(aChannelId == RLbsTestChannel::EChannelHandler2Proxy, User::Invariant());
	
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
			iCompletionRequestIdArray.Append(aMessage.RequestId());

			if (responseMsg.iResponseDelay.Int64() == 0)
				{
				RespondNetworkLocationRequest();
				}
			else
				{
			    // Start timer to dismiss dialog after delay period
			    //iCompletionTimer->Start(responseMsg.iResponseDelay.Int64(), 0, TCallBack(CompletionTimerCallBack, this));
			    
			    //Start appropriate completion timer
				TInt arraySize = iCompletionTimerArray.Count();
				TBool	requestFound = EFalse;
				for(int i = 0; i < arraySize; i++)
					{
					if(iCompletionTimerArray[i].iRequestId == aMessage.RequestId())
						{
						iCompletionTimerArray[i].iCompletionTimer->Start(responseMsg.iResponseDelay.Int64(), 0, TCallBack(CompletionTimerCallBack, this));
						requestFound = ETrue;
						break;
						}
					}
				
				// we shouldn't panic when the request has just been closed from the other side (from NRH). It happens because of a race condition
				if(requestFound == EFalse && iLastUsedRequestId != 0xFFFFFFFF && iLastUsedRequestId != aMessage.RequestId())
					{
					//Something is wrong - ReqestId should match
					User::Invariant();
					}
			    
				}
			
			}
			break;
			
		default:
			User::Invariant();
			
		}
	
	}
	

TInt CTestPrivacyController::TimerCallBack(TAny* aPtr)
    {
    (void) aPtr;
    //User::Invariant(); // No response from test program
    return 0; // callback is not called again
    }

TInt CTestPrivacyController::CompletionTimerCallBack(TAny* aPtr)
    {
    ((CTestPrivacyController*)aPtr)->RespondNetworkLocationRequest();
    return 0; // callback is not called again
    }


void CTestPrivacyController::ResetResponseTimer()
	{// Used to check for an unacceptable delay from the test program 
    if (iTimer->IsActive())
        {
        iTimer->Cancel();
        }
    TTimeIntervalMicroSeconds32 delay(KLbsResponseTimeout);
    iTimer->Start(delay, 0,TCallBack(TimerCallBack, this));
	}


void CTestPrivacyController::RespondNetworkLocationRequest()
    {
    //if (iCompletionTimer)
    //    {
    //    iCompletionTimer->Cancel();
    //    }
	TInt32 completionRequestId = -1;
	
	TInt arraySize = iCompletionTimerArray.Count();
	TInt compArraySize = iCompletionRequestIdArray.Count();
	TBool	requestFound = EFalse;
	for(int i = 0; i < arraySize; i++)
		{
		for(int j=0; j<compArraySize;j++)
			{
			if(iCompletionTimerArray[i].iRequestId == iCompletionRequestIdArray[j])
				{
//				iCompletionTimerArray[i].iCompletionTimer->Cancel();
//				delete iCompletionTimerArray[i].iCompletionTimer;
//				iCompletionTimerArray.Remove(i);
//				
				completionRequestId = iCompletionRequestIdArray[j];
				iCompletionRequestIdArray.Remove(j);
				
				requestFound = ETrue;
				break;
				}
			}
		if(requestFound)
			{
			break;
			}
		}
	
	if(requestFound == EFalse)
		{
		//Something is wrong - ReqestId should match
	//	User::Invariant();
		}
		
//	if(iRequestCancelled)
//		{
//		iRequestCancelled = EFalse;
//		return;	
//		}
	
	if(iCompletionResponse == EResponseTypeAccepted)
		{
		iLbsPrivacyController->RespondNetworkLocationRequest(completionRequestId, CLbsPrivacyController::ERequestAccepted);
		}
	else if (iCompletionResponse == EResponseTypeRejected)
		{
		iLbsPrivacyController->RespondNetworkLocationRequest(completionRequestId, CLbsPrivacyController::ERequestRejected);
		}
	else if (iCompletionResponse == EResponseTypeIgnore)
		{
		iLbsPrivacyController->RespondNetworkLocationRequest(completionRequestId, CLbsPrivacyController::ERequestIgnore);
		}
	else
		{
		iLbsPrivacyController->RespondNetworkLocationRequest(completionRequestId, CLbsPrivacyController::ERequestUnknown);
		}
    }

void CTestPrivacyController::ProcessCloseDownNotification(TInt32 /*aReservedParam*/)
	{
	//Send CloseDown information to test program
	
	TLbsPrivFwCtrlCloseDownMsg* closeDownMsg = new (ELeave) TLbsPrivFwCtrlCloseDownMsg();
	iHandlerChannel->SendPrivFwRequestMessage(*closeDownMsg);
	delete closeDownMsg;	

	//Wait some time to propagate controller shutdown message to proxy	
  	User::After(1000);
	
	CActiveScheduler::Stop();

	}
	
	
	
	



//From MReadPropPipeObserver
//void CTestPrivacyController::OnReadyToReadL(CPrivFwReadPropPipe& aPipe)
//	{
//	TInt32 action;
//	aPipe.RefreshL(0);
//	aPipe >> action;
//	
//	switch(action)
//		{
//		case EMessageVerifyRequest:
//			{
//			//Read response
//			TInt32 tempCompletionResponse;
//			aPipe >> iCompletionRequestId;
//			aPipe >> tempCompletionResponse;
//
//		    TTimeIntervalMicroSeconds* 	completionDelay;
//
//			PrivFwTestMarshaller::ReadL(aPipe, completionDelay);
//			iCompletionResponse = static_cast<TResponseType>(tempCompletionResponse);
//			if (completionDelay->Int64() == 0)
//				{
//				RespondNetworkLocationRequest();
//				}
//			else
//				{
//			    // Start timer to dismiss dialog after delay period
//			    iCompletionTimer->Start(completionDelay->Int64(), 0, TCallBack(CompletionTimerCallBack, this));
//				}
//			
//			delete completionDelay;
//
//			break;
//			}
//		case EMessageNotifyRequest:
//			//We should not respond in case of Notification request
//			break;
//						
//		default:
//			User::Invariant();
//		
//		}
//	}

