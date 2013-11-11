/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/

// INCLUDE FILES
#include <eiknotapi.h>
//#include "privfwmarshallingstubs.h"
#include "lbsqnnnotifier.h"

const TUid KScreenOutputChannel = { 0x100065ac };

const TInt KLbsResponseTimeout = 30*1000000; // 10 secs 

// ================= MEMBER FUNCTIONS =======================


// C++ default constructor can NOT contain any code, that might leave.
CQnNPlugin::CQnNPlugin()
    {
    }

// Destructor
CQnNPlugin::~CQnNPlugin()
    {
    if (iTimer)
        {
        iTimer->Cancel();
        delete iTimer;
        iTimer = NULL;
        }
    if (iCompletionTimer)
        {
        iCompletionTimer->Cancel();
        delete iCompletionTimer;
        iCompletionTimer = NULL;
        }
    
    delete iHandlerChannel;
    }


// Two-phased constructor
CQnNPlugin* CQnNPlugin::NewL()
    {
    CQnNPlugin* self = new (ELeave) CQnNPlugin;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// EPOC default constructor can leave.
void CQnNPlugin::ConstructL()
    {
    this->BaseConstructL(KScreenOutputChannel, MEikSrvNotifierBase2::ENotifierPriorityVHigh);

    iTimer = CPeriodic::NewL(CActive::EPriorityStandard);
    iCompletionTimer = CPeriodic::NewL(CActive::EPriorityStandard);
//  iPipesCreated = EFalse;
	iChannelCreated = EFalse;
    }

// ---------------------------------------------------------
// CQnNPlugin::HandleNewRequestL
// 
// ---------------------------------------------------------
//
void CQnNPlugin::HandleNewRequestL(/*IN*/ TPosQNRequestId aRequestId)
    {
//	if(!iPipesCreated)
//		{ // Need to create pipes here (and NOT in constructor) to allow test code to pre-define the properties
//	 	iResponseWriter   = CPrivFwWritePropPipe::NewL(KStubToProxyPipeKey, KStubToProxyFlowKey, ETrue);
//		iMsgReader = CPrivFwNotifyReadPropPipe::NewL(KProxyToStubPipeKey, KProxyToStubFlowKey, *this, ETrue);
//		iPipesCreated = ETrue;
//		}

	if(!iChannelCreated)
		{
		iHandlerChannel = CTestRequestChannel::NewL(this, RLbsTestChannel::EChannelHandler2Proxy);
		//iProxyChannel = CTestRequestChannel::NewL(this, RLbsTestChannel::EChannelProxy2Handler);
		iChannelCreated = ETrue;			
		}

	iRequestCancelled = EFalse;
    SetCurrentRequestL(aRequestId);
    TRequestType requestType = RequestTypeL(aRequestId);
	TLbsTestChannelMsgBase::TLbsTestChannelMsgType	msgType;
	
    if (requestType == EQuery)
        {
        msgType = TLbsTestChannelMsgBase::EMessageVerifyRequest;
        }
    else
        {
        msgType = TLbsTestChannelMsgBase::EMessageNotifyRequest;
        }
    SendRequestDetails(aRequestId, msgType);

	// For notifications we just complete the request.
	if (msgType == TLbsTestChannelMsgBase::EMessageNotifyRequest)
		{
		iCompletionRequestId = aRequestId;
		iCompletionResponse = EResponseTypeAccepted;
		
		DismissDialog();
		}
	
    
    ResetResponseTimer();
  	}


void CQnNPlugin::HandleRequestCancelled(/*IN*/ TPosQNRequestId aRequestId)
	{
  	iRequestCancelled = ETrue;

//  if (iCompletionTimer->IsActive())
//      {
//      iCompletionTimer->Cancel();
//      }

  	SetCurrentRequestL(aRequestId);

  	SendRequestDetails(aRequestId, TLbsTestChannelMsgBase::EMessageVerifyCancelRequest);
  
  	ResetResponseTimer();
  	}


void CQnNPlugin::HandleAllRequestCancelled()
  	{
  	HandleRequestCancelled(CurrentRequest());
  	}


TInt CQnNPlugin::TimerCallBack(TAny* aPtr)
    {
    (void) aPtr;
    return 0; // callback is not called again
    }


void CQnNPlugin::SendRequestDetails(TPosQNRequestId aRequestId, TLbsTestChannelMsgBase::TLbsTestChannelMsgType aMsgType)
	{

	switch(aMsgType)
		{
		case TLbsTestChannelMsgBase::EMessageNotifyRequest:
			{
			TLbsPrivFwNotifyRequestMsg* privReqMsg = new (ELeave) TLbsPrivFwNotifyRequestMsg((TUint&)aRequestId);
			//Fill TestRequestInfo structure
			GetTestRequestInfo(privReqMsg->iTestRequestInfo);
			iHandlerChannel->SendPrivFwRequestMessage(*privReqMsg);
			delete privReqMsg;
			}
			break;
		
		case TLbsTestChannelMsgBase::EMessageVerifyRequest:
		case TLbsTestChannelMsgBase::EMessageVerifyCancelRequest: //TBD
			{
			TLbsPrivFwVerifyRequestMsg* privReqMsg = new (ELeave) TLbsPrivFwVerifyRequestMsg((TUint&)aRequestId);
			//Fill TestRequestInfo structure
			GetTestRequestInfo(privReqMsg->iTestRequestInfo);
			iHandlerChannel->SendPrivFwRequestMessage(*privReqMsg);
			delete privReqMsg;
			}
			break;
			
		}
		
	}


void CQnNPlugin::GetTestRequestInfo(TLbsTestRequestInfo& aTestRequestInfo)
	{

   	TInt numEntries = RequestorCountL();
	ASSERT(numEntries > 0);
	TLbsRequesterId2 requesterName;
	TLbsClientName2 clientName;
	CPosRequestor::TRequestorType requestorType;

	aTestRequestInfo.SetRequestSource(RequestSource());
	aTestRequestInfo.SetTimeoutStrategy(QueryTimeoutStrategy());
	aTestRequestInfo.SetRequestDecision(LocationRequestDecision());
	aTestRequestInfo.SetNotificationReason(NotificationReason());
	aTestRequestInfo.SetVerifyCancelReason(CancelReason());
    
    for (TInt i=0; i < numEntries;i++)
    	{
        CPosRequestor* req = RequestorLC(i);
    	
    	aTestRequestInfo.SetNetworkType(ConvertTNetworkType(req->NetworkType()));
    	aTestRequestInfo.SetRequestType(ConvertTRequestType(req->RequestType()));
    	requestorType = req->RequestorType();
    	if (requestorType == CPosRequestor::ERequestorService)
    		{
        	aTestRequestInfo.SetClientNameFormat(ConvertTRequestorIdFormat(static_cast<CPosRequestor::TRequestorIdFormat>(req->RequestorIdFormat())));
	        aTestRequestInfo.SetClientName(req->RequestorIdString());
    		}
    	else // (requestorType == CPosRequestor::ERequestorContact)
    		{
        	aTestRequestInfo.SetRequesterIdFormat(ConvertTRequestorIdFormat(static_cast<CPosRequestor::TRequestorIdFormat>(req->RequestorIdFormat())));
	        aTestRequestInfo.SetRequesterId(req->RequestorIdString());
    		}

     	CleanupStack::PopAndDestroy(req);
    	}
	}
  
    
TInt CQnNPlugin::CompletionTimerCallBack(TAny* aPtr)
    {
    ((CQnNPlugin*)aPtr)->DismissDialog();
    return 0; // callback is not called again
    }


void CQnNPlugin::ResetResponseTimer()
	{// Used to check for an unacceptable delay from the test program 
    if (iTimer->IsActive())
        {
        iTimer->Cancel();
        }
    TTimeIntervalMicroSeconds32 delay(KLbsResponseTimeout);
    iTimer->Start(delay, 0,TCallBack(TimerCallBack, this));
	}


void CQnNPlugin::DismissDialog()
    {
    if (iCompletionTimer)
        {
        iCompletionTimer->Cancel();
        }

	if(iRequestCancelled)
		{
		return;
		}
  
    if (iCompletionResponse == EResponseTypeAccepted)
        {
        CompleteRequest(iCompletionRequestId, KErrNone);
        }
    else if (iCompletionResponse == EResponseTypeRejected)
        {
        CompleteRequest(iCompletionRequestId, KErrAccessDenied);
        }
    else if (iCompletionResponse == EResponseTypeTimedOut)
        {
        CompleteRequest(iCompletionRequestId, KErrTimedOut);
        }
    else if (iCompletionResponse == EResponseTypeCompleteAllRequests)
        {
        CompleteAllRequests(KErrNone);
        }
    else if (iCompletionResponse < 0)
        {
        CompleteRequest(iCompletionRequestId, iCompletionResponse);
        }
    else
        {
        CompleteRequest(iCompletionRequestId, KErrNotFound);
        }
    }


void CQnNPlugin::ProcessTestChannelRequestMessage(RLbsTestChannel::TLbsTestChannelId aChannelId, const TLbsTestChannelMsgBase& aMessage)
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
			//iCompletionDelay = responseMsg.iResponseDelay;
			iCompletionRequestId = aMessage.RequestId();
			if (responseMsg.iResponseDelay.Int64() == 0)
				{
				DismissDialog();
				}
			else
				{
			    // Start timer to dismiss dialog after delay period
			    iCompletionTimer->Start(responseMsg.iResponseDelay.Int64(), 0, TCallBack(CompletionTimerCallBack, this));
				}
			
			}
			break;
			
		default:
			break;
						
		}
	}

TLbsExternalRequestInfo::TFormatIndicator CQnNPlugin::ConvertTRequestorIdFormat(CPosRequestor::TRequestorIdFormat aRequestorIdFormat)
    {
    switch (aRequestorIdFormat)
        {
	    case CPosRequestor::EIdFormatGenericName :
    		return TLbsExternalRequestInfo::EFormatLogicalName;
	    case CPosRequestor::EIdFormatPhoneNumber :
    		return TLbsExternalRequestInfo::EFormatMSISDN;
	    case CPosRequestor::EIdFormatUrl :
    		return TLbsExternalRequestInfo::EFormatURL;
	    case CPosRequestor::EIdFormatEmail :
	    	return TLbsExternalRequestInfo::EFormatEmailAddress;
	    case CPosRequestor::EIdFormatSIPUrl :
	    	return TLbsExternalRequestInfo::EFormatSIPURL;
	    case CPosRequestor::EIdFormatIMSPublicIdentity :
	    	return TLbsExternalRequestInfo::EFormatIMSPublicIdentity;
	    case CPosRequestor::EIdFormatMIN :
	    	return TLbsExternalRequestInfo::EFormatMIN;
	    case CPosRequestor::EIdFormatMDN :
	    	return TLbsExternalRequestInfo::EFormatMDN;
	    default:
	    	return TLbsExternalRequestInfo::EFormatUnknown;
        }
    }


TLbsExternalRequestInfo::TNetworkType CQnNPlugin::ConvertTNetworkType(CPosRequestor::TNetworkType aNetworkType)
    {
	switch (aNetworkType)
	    {
	    case CPosRequestor::ENetworkTypeUnknown :
	    	return TLbsExternalRequestInfo::ENetworkTypeUnknown;
	    case CPosRequestor::ENetworkGSM :
	    	return TLbsExternalRequestInfo::ENetworkGSM;
	    case CPosRequestor::ENetworkSUPL :
	    	return TLbsExternalRequestInfo::ENetworkSUPL;
	    default:
			return TLbsExternalRequestInfo::ENetworkTypeUnknown;
	    }
    }


TLbsExternalRequestInfo::TRequestType CQnNPlugin::ConvertTRequestType(CPosRequestor::TRequestType aRequestType)
    {
    switch (aRequestType)
	    {
	    case CPosRequestor::ERequestTypeUnknown :
	    	return TLbsExternalRequestInfo::ERequestTypeUnknown;
	    case CPosRequestor::ERequestSingleShot :
	    	return TLbsExternalRequestInfo::ERequestSingleShot;
	    case CPosRequestor::ERequestPeriodic :
	    	return TLbsExternalRequestInfo::ERequestPeriodic;
	    case CPosRequestor::ERequestAreaEvent :
	    	return TLbsExternalRequestInfo::ERequestAreaEvent;
	    default:
			return TLbsExternalRequestInfo::ERequestTypeUnknown;
	   }
    }

//From MReadPropPipeObserver
//void CQnNPlugin::OnReadyToReadL(CPrivFwReadPropPipe& aPipe)
//	{
//	TInt32 action;
//	aPipe.RefreshL(0);
//	aPipe >> action;
//	
//	switch(action)
//		{
//		case EMessageNotifyRequest:
//			{
//			// here we just read data, and do nothing, in new req complete req if notify
//			TInt32 tempCompletionResponse;
//			aPipe >> iCompletionRequestId;
//			aPipe >> tempCompletionResponse;
//			PrivFwTestMarshaller::ReadL(aPipe, iCompletionDelay);
//			
//			break;
//			}
//	
//		case EMessageVerifyRequest:
//			{
//			//Read response
//			TInt32 tempCompletionResponse;
//			aPipe >> iCompletionRequestId;
//			aPipe >> tempCompletionResponse;
//			PrivFwTestMarshaller::ReadL(aPipe, iCompletionDelay);
//			iCompletionResponse = static_cast<TResponseType>(tempCompletionResponse);
//			if (iCompletionDelay->Int64() == 0)
//				{
//				DismissDialog();
//				}
//			else
//				{
//			    // Start timer to dismiss dialog after delay period
//			    iCompletionTimer->Start(iCompletionDelay->Int64(), 0, TCallBack(CompletionTimerCallBack, this));
//				}
//
//			break;
//			}
//			
//		default:
//			{
//			break;
//			}
//		
//		}
//	}
//
