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
 @file lbsprivfwproxy.cpp
*/

#include <lbs/lbslocclasstypes.h>

//#include "privfwmarshallingstubs.h"

#include "lbsprivfwproxy.h"



//------------------------------------------------------------------------------
// CPrivFwProxy 
//------------------------------------------------------------------------------

EXPORT_C CPrivFwProxy* CPrivFwProxy::NewL(TPrivacyHandlerType aPrivHandlerType, MPrivFwProxyObserver& aObserver)
	{
	CPrivFwProxy* self = new (ELeave) CPrivFwProxy(aPrivHandlerType, aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CPrivFwProxy::~CPrivFwProxy()
	{
//	RLbsTestChannel::ShutDownL(RLbsTestChannel::EChannelHandler2Proxy);
//	RLbsTestChannel::ShutDownL(RLbsTestChannel::EChannelProxy2Handler);
	
	delete iProxyChannel;
	}

CPrivFwProxy::CPrivFwProxy(TPrivacyHandlerType aPrivHandlerType, MPrivFwProxyObserver& aObserver):
	iPrivHandlerType(aPrivHandlerType),
	iObserver(aObserver)
	{
	
	}

void CPrivFwProxy::ConstructL()
	{
	ASSERT(iPrivHandlerType != EPrivacyHandlerUnknown);
	
//	RLbsTestChannel::InitializeL(RLbsTestChannel::EChannelHandler2Proxy);
//	RLbsTestChannel::InitializeL(RLbsTestChannel::EChannelProxy2Handler);

	//iHandlerChannel = CTestRequestChannel::NewL(this, RLbsTestChannel::EChannelHandler2Proxy);
	iProxyChannel = CTestRequestChannel::NewL(this, RLbsTestChannel::EChannelProxy2Handler);
	
//	iMsgType = TLbsTestChannelMsgBase::EMessageNull;
	}

EXPORT_C void CPrivFwProxy::SendNotifierResponse(TRequestType aRequestType, TInt aRequestId, TResponseType aResponse, TTimeIntervalMicroSeconds& aResponseDelay)
	{
	TUint requestId = static_cast<TUint> (aRequestId);
	
	switch(aRequestType)
		{
		case ERequestVerify:
			{
			TLbsPrivFwVerifyResponseMsg *responseMsg = new (ELeave) TLbsPrivFwVerifyResponseMsg(requestId);
			responseMsg->iResponse = aResponse;
			responseMsg->iResponseDelay = aResponseDelay;
			iProxyChannel->SendPrivFwRequestMessage(*responseMsg);
			delete responseMsg;
			}
			break;
			
		case ERequestNotify:
			{
			TLbsPrivFwNotifyResponseMsg *responseMsg = new (ELeave) TLbsPrivFwNotifyResponseMsg(requestId);
			responseMsg->iResponse = aResponse;
			responseMsg->iResponseDelay = aResponseDelay;
			iProxyChannel->SendPrivFwRequestMessage(*responseMsg);
			delete responseMsg;
			}
			break;
			
		default:
			break;
			
		}
		
	}
	

EXPORT_C void CPrivFwProxy::SendNotifierTestData(TInt aRequestId, const TLbsNotifierTestData& aPrivacyTestData)
	{
	(void) aRequestId;
	(void)aPrivacyTestData;	
	}
	

TLbsExternalRequestInfo::TFormatIndicator CPrivFwProxy::ConvertTRequestorIdFormat(CPosRequestor::TRequestorIdFormat aRequestorIdFormat)
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


TLbsExternalRequestInfo::TNetworkType CPrivFwProxy::ConvertTNetworkType(CPosRequestor::TNetworkType aNetworkType)
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


TLbsExternalRequestInfo::TRequestType CPrivFwProxy::ConvertTRequestType(CPosRequestor::TRequestType aRequestType)
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

	
void CPrivFwProxy::ProcessTestChannelRequestMessage(RLbsTestChannel::TLbsTestChannelId aChannelId, const TLbsTestChannelMsgBase& aMessage)
	{
	__ASSERT_ALWAYS(aChannelId == RLbsTestChannel::EChannelProxy2Handler, User::Invariant());
	
	switch(aMessage.Type())	
		{
		case TLbsTestChannelMsgBase::EPrivacyMsgSessionComplete:
			{
			const TLbsPrivFwSessionCompleteMsg& reqCompleteMsg = static_cast<const TLbsPrivFwSessionCompleteMsg&> (aMessage);
			iObserver.NotifierRequestComplete(aMessage.RequestId(), reqCompleteMsg.iCompleteReason);
			}
			break;
			
		case TLbsTestChannelMsgBase::EMessageCtrlCloseDown:
			{
			iObserver.NotiferCtrlCloseDown();
			}
			break;
			
		case TLbsTestChannelMsgBase::EMessageNotifyRequest:
			{
			//TLbsTestRequestInfo *pTestRequestInfo = new TLbsTestRequestInfo();
			const TLbsPrivFwNotifyRequestMsg& dummy = static_cast<const TLbsPrivFwNotifyRequestMsg&> (aMessage);
			//ConvertToTestRequestInfo(&dummy.iTestRequestInfo, pTestRequestInfo);
			//iObserver.NotifierDataAvailable(aMessage.RequestId(), *pTestRequestInfo);
			TLbsTestRequestInfo tempRequestInfo = dummy.iTestRequestInfo;
			iObserver.NotifierDataAvailable(aMessage.RequestId(), tempRequestInfo);
			//delete pTestRequestInfo;
			}
			break;
			
		case TLbsTestChannelMsgBase::EMessageVerifyRequest:
			{
			//TLbsTestRequestInfo *pTestRequestInfo = new TLbsTestRequestInfo();
			const TLbsPrivFwVerifyRequestMsg& dummy = static_cast<const TLbsPrivFwVerifyRequestMsg&> (aMessage);
			//ConvertToTestRequestInfo(&dummy.iTestRequestInfo, pTestRequestInfo);
			TLbsTestRequestInfo tempRequestInfo = dummy.iTestRequestInfo;
			iObserver.NotifierDataAvailable(aMessage.RequestId(), tempRequestInfo);
			//delete pTestRequestInfo;
			}
			break;
			
		default:
			{
			//TLbsTestRequestInfo *pTestRequestInfo = new TLbsTestRequestInfo();
			//GetParamInfo(aMessage, pTestRequestInfo);	
			//iObserver.NotifierDataAvailable(aMessage.RequestId(), *pTestRequestInfo);
			}
			break;
		}
	}

//void CPrivFwProxy::ConvertToTestRequestInfo(const TLbsTestRequestInfo* aSrcRequestInfo, TLbsTestRequestInfo* aDestRequestInfo)
//	{
//	switch(iPrivHandlerType)
//		{
//		case EPrivacyHandleByQnN:	
//			{
//			*aDestRequestInfo = *aSrcRequestInfo;
//			}
//			break;
//			
//		case EPrivacyHandleByNotifier:
//		case EPrivacyHandleByController:	
//			{
//			const TLbsExternalRequestInfo& requestInfo = static_cast<const TLbsExternalRequestInfo&>(*aSrcRequestInfo);
//			
//			aDestRequestInfo->SetNetworkType(requestInfo.NetworkType());
//			aDestRequestInfo->SetRequestType(requestInfo.RequestType());
//			aDestRequestInfo->SetClientNameFormat(requestInfo.ClientNameFormat());
//
//			TLbsClientName2 clientName;
//			requestInfo.GetClientName(clientName);
//			aDestRequestInfo->SetClientName(clientName);
//			aDestRequestInfo->SetRequesterIdFormat(requestInfo.RequesterIdFormat());
//
//			TLbsRequesterId2 requesterName;
//			requestInfo.GetRequesterId(requesterName);
//			aDestRequestInfo->SetRequesterId(requesterName);
//		    
//			}
//			break;
//
//		default:
//			User::Invariant();
//			break;
//		}
//		
//	}









/*	
void CPrivFwProxy::GetParamInfo(TLbsTestChannelMsgBase& aMessage, TLbsTestRequestInfo* aTestRequestInfo)
	{
	
	switch(aMessage.Type())	
		{
		case EMessageNotifyRequest:
			{
			const TLbsPrivFwNotifyRequestMsg& dummy = static_cast<const TLbsPrivFwNotifyRequestMsg&> (aMessage);
			if(iPrivHandlerType == EPrivacyHandleByQnN)
				{
				*aTestRequestInfo = *(dummy.iTestRequestInfo);
				}
			else
				{
				ConvertToTestRequestInfo()	
				}
			}			
			break;

		case EMessageVerifyRequest:
			{
		
			}	
			break;		

		}
		
	}
*/
	
/*	
void CPrivFwProxy::GetArgs(const TLbsTestChannelMsgBase& aMessage)
	{
	switch(iPrivHandlerType)
		{
		case EPrivacyHandleByNotifier:	
			cleanupStackCount = GetPrivFwNotifierParamInfo(aMessage);
			break;
			
		case EPrivacyHandleByQnN:	
			cleanupStackCount = GetPrivFwQnNParamInfo(aMessage);
			break;
			
		case EPrivacyHandleByController:	
			cleanupStackCount = GetPrivFwCtrlParamInfo(aMessage);
			break;

		default:
			User::Invariant();
			break;
		}
	}
*/



/*	
TInt CPrivFwProxy::ReadPipe(TInt aTimeout)
	{
	TInt err = KErrNone;
	RDebug::Printf("CPrivFwProxy::ReadPipe(%d)", aTimeout);
	TRAP(err, iResponseReader->RefreshL(aTimeout));
	if (KErrNone == err)
		{
		TInt32 msgTypeVal;
		TInt32 requestIdVal;

		// Read msg type.
		*iResponseReader >> msgTypeVal;
		iMsgType = static_cast<TMessageType>(msgTypeVal);

		if(iMsgType != EMessageCtrlCloseDown)
			{
			// Read the request id.
			*iResponseReader >> requestIdVal;
			iRequestId = requestIdVal;	
			}
		}
	else
		{
		iMsgType = EPrivacyMsgTimeoutExpired;
		iRequestId = 0;
		}
	return err;
	}


TInt CPrivFwProxy::GetArgsLC(TInt aDummy, ...)
	{
	TInt cleanupStackCount = 0;

	RDebug::Printf("CPrivFwProxy::GetArgs(%d)", iMsgType);

	VA_LIST list;
	VA_START(list, aDummy);

	
	CPrivFwReadPropPipe& r = *iResponseReader;
	
	switch(iPrivHandlerType)
		{
		case EPrivacyHandleByNotifier:	
			cleanupStackCount = GetEPrivacyMsgNotifierParamInfoArgsL(r, list);
			break;
			
		case EPrivacyHandleByQnN:	
			cleanupStackCount = GetEPrivacyMsgQnNParamInfoArgsL(r, list);
			break;
			
		case EPrivacyHandleByController:	
			cleanupStackCount = GetEPrivacyMsgCtrlParamInfoArgsL(r, list);
			break;

		default:
			User::Invariant();
			break;
		}
	
	VA_END(list);
	return cleanupStackCount;
	}

TInt CPrivFwProxy::GetEPrivacyMsgQnNParamInfoArgsL(CPrivFwReadPropPipe& aPipe, VA_LIST aList)
	{
	TLbsTestRequestInfo**	pRequesterInfo = VA_ARG(aList, TLbsTestRequestInfo**);
	*pRequesterInfo = new (ELeave) TLbsTestRequestInfo();
	
	TInt nextPipeValue;
	PrivFwTestMarshaller::ReadL(aPipe, nextPipeValue);
	(*pRequesterInfo)->SetRequestSource(static_cast<TPosRequestSource>(nextPipeValue));
	PrivFwTestMarshaller::ReadL(aPipe, nextPipeValue);
	(*pRequesterInfo)->SetTimeoutStrategy(static_cast<TPosRequestDecision>(nextPipeValue));
	PrivFwTestMarshaller::ReadL(aPipe, nextPipeValue);
	(*pRequesterInfo)->SetRequestDecision(static_cast<TPosRequestDecision>(nextPipeValue));
	PrivFwTestMarshaller::ReadL(aPipe, nextPipeValue);
	(*pRequesterInfo)->SetNotificationReason(static_cast<TPosNotificationReason>(nextPipeValue));
	PrivFwTestMarshaller::ReadL(aPipe, nextPipeValue);
	(*pRequesterInfo)->SetVerifyCancelReason(static_cast<TPosVerifyCancelReason>(nextPipeValue));
	
	TInt numEntries;
	PrivFwTestMarshaller::ReadL(aPipe, numEntries);
    for (TInt i=0; i < numEntries;i++)
        {
    	PrivFwTestMarshaller::ReadL(aPipe, nextPipeValue);
    	(*pRequesterInfo)->SetNetworkType(ConvertTNetworkType(static_cast<CPosRequestor::TNetworkType>(nextPipeValue)));
    	PrivFwTestMarshaller::ReadL(aPipe, nextPipeValue);
    	(*pRequesterInfo)->SetRequestType(ConvertTRequestType(static_cast<CPosRequestor::TRequestType>(nextPipeValue)));
    	PrivFwTestMarshaller::ReadL(aPipe, nextPipeValue);
    	CPosRequestor::TRequestorType requestorType = static_cast<CPosRequestor::TRequestorType>(nextPipeValue);
    	if (requestorType == CPosRequestor::ERequestorService)
    		{
        	PrivFwTestMarshaller::ReadL(aPipe, nextPipeValue);
        	(*pRequesterInfo)->SetClientNameFormat(ConvertTRequestorIdFormat(static_cast<CPosRequestor::TRequestorIdFormat>(nextPipeValue)));
        	TLbsClientName2 clientName;
        	aPipe >> clientName;
        	(*pRequesterInfo)->SetClientName(clientName);
    		}
    	else // (requestorType == CPosRequestor::ERequestorContact)
    		{
        	PrivFwTestMarshaller::ReadL(aPipe, nextPipeValue);
        	(*pRequesterInfo)->SetRequesterIdFormat(ConvertTRequestorIdFormat(static_cast<CPosRequestor::TRequestorIdFormat>(nextPipeValue)));
        	TLbsRequesterId2 requesterName;
        	aPipe >> requesterName;
    		(*pRequesterInfo)->SetRequesterId(requesterName);
    		}
        }
    CleanupStack::PushL(*pRequesterInfo);	
	return 1;
	}

TInt CPrivFwProxy::GetEPrivacyMsgNotifierParamInfoArgsL(CPrivFwReadPropPipe& aPipe, VA_LIST aList)
	{
    //We would expect to get QnN buffer data structure
	TLbsTestRequestInfo**	pRequesterInfo = VA_ARG(aList, TLbsTestRequestInfo**);
	*pRequesterInfo = new (ELeave) TLbsTestRequestInfo();
	TLbsExternalRequestInfo* dummy = NULL;

	//First read cancel reason
	TInt	cancelReason;
    PrivFwTestMarshaller::ReadL(aPipe, cancelReason);
	(*pRequesterInfo)->SetVerifyCancelReason(static_cast<TPosVerifyCancelReason>(cancelReason));

    PrivFwTestMarshaller::ReadL(aPipe, dummy);

	(*pRequesterInfo)->SetNetworkType(dummy->NetworkType());
	(*pRequesterInfo)->SetRequestType(dummy->RequestType());
	(*pRequesterInfo)->SetClientNameFormat(dummy->ClientNameFormat());
	TLbsClientName2 clientName;
	dummy->GetClientName(clientName);
	(*pRequesterInfo)->SetClientName(clientName);
	(*pRequesterInfo)->SetRequesterIdFormat(dummy->RequesterIdFormat());
	TLbsRequesterId2 requesterName;
	dummy->GetRequesterId(requesterName);
	(*pRequesterInfo)->SetRequesterId(requesterName);
    
    CleanupStack::PushL(*pRequesterInfo);
    
    
    delete dummy;
	return 1;
	}
 
TInt CPrivFwProxy::GetEPrivacyMsgCtrlParamInfoArgsL(CPrivFwReadPropPipe& aPipe, VA_LIST aList)
	{
    //We would expect to get QnN buffer data structure
	TLbsTestRequestInfo**	pRequesterInfo = VA_ARG(aList, TLbsTestRequestInfo**);
	*pRequesterInfo = new (ELeave) TLbsTestRequestInfo();
	TLbsExternalRequestInfo2* dummy = NULL;

    PrivFwTestMarshaller::ReadL(aPipe, dummy);

	(*pRequesterInfo)->SetNetworkType(dummy->NetworkType());
	(*pRequesterInfo)->SetRequestType(dummy->RequestType());
	(*pRequesterInfo)->SetClientNameFormat(dummy->ClientNameFormat());
	TLbsClientName2 clientName;
	dummy->GetClientName(clientName);
	(*pRequesterInfo)->SetClientName(clientName);
	(*pRequesterInfo)->SetRequesterIdFormat(dummy->RequesterIdFormat());
	TLbsRequesterId2 requesterName;
	dummy->GetRequesterId(requesterName);
	(*pRequesterInfo)->SetRequesterId(requesterName);
    
    CleanupStack::PushL(*pRequesterInfo);
    
    delete dummy;
	return 1;
	}
*/



/*	
void CPrivFwProxy::OnReadyToReadL(CPrivFwReadPropPipe& aPipe)
	{
	(void)aPipe;
	ASSERT(iResponseReader == &aPipe);

	const TInt KTimeOut = 1000000;
	ReadPipe(KTimeOut);

	if(iMsgType == EPrivacyMsgSessionComplete) //We would get this message in case of controller
		{ //Read requestid and complete reason
		TInt32 aCompletionCode;

		// Read requestId and completion code.
		*iResponseReader >> aCompletionCode;
					
		iObserver.NotifierRequestComplete(iRequestId, aCompletionCode);
		}
    else if(iMsgType == EMessageCtrlCloseDown) //We would get this message in case of controller close down
		{ 
		iObserver.NotiferCtrlCloseDown();
		}
	else
		{
		TLbsTestRequestInfo* privNotifierData;
		TInt latestMsgType = 1;
		CPrivFwProxy::GetArgsLC(latestMsgType, &privNotifierData);
		iObserver.NotifierDataAvailable(iRequestId, *privNotifierData);
		CleanupStack::PopAndDestroy(privNotifierData);
		}
	}
*/
