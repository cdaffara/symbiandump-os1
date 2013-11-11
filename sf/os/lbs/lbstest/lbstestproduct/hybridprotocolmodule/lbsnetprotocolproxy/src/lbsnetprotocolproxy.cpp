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
 @file lbsnetprotocolproxy.cpp
*/
 
#include <lbs/test/lbsnetprotocolproxy.h>
#include <lbs/lbsnetprotocolbase.h>
#include <lbs/test/lbsmarshallingstubs.h>
#include <lbs/test/memorymanager.h>
#include <lbs/test/lbstestlogger.h>
#include <lbs/test/lbsparamlogger.h>

EXPORT_C CNetProtocolProxy* CNetProtocolProxy::NewL()
	{
	CNetProtocolProxy* self = new (ELeave) CNetProtocolProxy;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CNetProtocolProxy::~CNetProtocolProxy()
	{
	delete iActionWriter;
	delete iResponseReader;
	}

void CNetProtocolProxy::CallL(TNetProtocolActionType aAction,...)
	{
	VA_LIST list;
	VA_START(list, aAction);
	
	CWritePropPipe& r = *iActionWriter;
	TRAPD(e,r.ResetL());
	__ASSERT_ALWAYS(e == KErrNone, User::Panic(_L("ActionWriteFail"), aAction));
	
	r << aAction;
	
//	LBSTESTLOG_METHOD1(_L("--"), "CNetProtocolProxy::CallL", (TUint32)aAction); 

	switch(aAction)
		{
		case ENetMsgProcessPrivacyRequest:  CallProcessPrivacyRequestL(r, list);
										    break;	
		case ENetMsgProcessLocationRequest:	CallProcessLocationRequestL(r, list);
										    break;	
		case ENetMsgProcessSessionComplete:	CallProcessSessionCompleteL(r, list);
										    break;	
		case ENetMsgProcessAssistanceData:  CallProcessAssistanceDataL(r, list);
										    break;	
		case ENetMsgProcessLocationUpdate:  CallProcessLocationUpdateL(r, list);
										    break;	
		case ENetMsgGetCurrentCapabilitiesRequest: CallGetCurrentCapabilitiesL(r, list);
										    break;	
		case ENetMsgProcessStatusUpdate:    CallProcessStatusUpdateL(r, list);
										    break;
		
		default:							User::Invariant();
											break;
		}
	
	TRAP(e, r.CommitAndWaitL(KActionChannelWriteTimeout));
	__ASSERT_ALWAYS(e == KErrNone, User::Panic(_L("ActionWriteFail"), aAction));
	
	VA_END(list);
	}
		
void CNetProtocolProxy::CallGetCurrentCapabilitiesL(CWritePropPipe& aPipe, VA_LIST aList)		
	{
	TLbsNetPosCapabilities* pCaps = VA_ARG(aList, TLbsNetPosCapabilities*);
	
	LBSTestMarshaller::WriteL(aPipe, *pCaps);
	}


void CNetProtocolProxy::CallProcessStatusUpdateL(CWritePropPipe& aPipe, VA_LIST aList)
	{	
	MLbsNetworkProtocolObserver::TLbsNetProtocolServiceMask* pMask = VA_ARG(aList, 	MLbsNetworkProtocolObserver::TLbsNetProtocolServiceMask*);
	
	LBSTestMarshaller::WriteL(aPipe, *pMask);
	}


void CNetProtocolProxy::CallProcessPrivacyRequestL(CWritePropPipe& aPipe, VA_LIST aList)
	{

    TLbsNetSessionId* aSessionId		= VA_ARG(aList, TLbsNetSessionId*); 
	TBool* aEmergency					= VA_ARG(aList, TBool*); 
	TLbsNetPosRequestPrivacy* pPrivacy 	= VA_ARG(aList, TLbsNetPosRequestPrivacy*);
	TLbsExternalRequestInfo* pReqInfo 	= VA_ARG(aList, TLbsExternalRequestInfo*);
	
  
	LBSTestMarshaller::WriteL(aPipe, *aSessionId);
	LBSTestMarshaller::WriteL(aPipe, *aEmergency);
	LBSTestMarshaller::WriteL(aPipe, *pPrivacy);
	LBSTestMarshaller::WriteL(aPipe, *pReqInfo);
	}
	
void CNetProtocolProxy::CallProcessLocationRequestL(CWritePropPipe& aPipe, VA_LIST aList)		
	{
	TLbsNetSessionId* pSessionId 	 	= VA_ARG(aList, TLbsNetSessionId*);
	TBool* pEmergency			 		= VA_ARG(aList, TBool*);
	MLbsNetworkProtocolObserver::TLbsNetProtocolService* pService 	= VA_ARG(aList, MLbsNetworkProtocolObserver::TLbsNetProtocolService*);
	TLbsNetPosRequestQuality* pQuality	= VA_ARG(aList, TLbsNetPosRequestQuality*);
	TLbsNetPosRequestMethod* pMethod	= VA_ARG(aList, TLbsNetPosRequestMethod*);

	LBSTestMarshaller::WriteL(aPipe, *pSessionId);
	LBSTestMarshaller::WriteL(aPipe, *pEmergency);
	LBSTestMarshaller::WriteL(aPipe, *pService);
	LBSTestMarshaller::WriteL(aPipe, *pQuality);
	LBSTestMarshaller::WriteL(aPipe, *pMethod);
	}
	
void CNetProtocolProxy::CallProcessSessionCompleteL(CWritePropPipe& aPipe, VA_LIST aList)
	{
	TLbsNetSessionId* pSessionId	= VA_ARG(aList, TLbsNetSessionId*);
	TInt*  pReason					= VA_ARG(aList, TInt*);
	
	LBSTestMarshaller::WriteL(aPipe, *pSessionId);
	LBSTestMarshaller::WriteL(aPipe, *pReason);
	}
	
void CNetProtocolProxy::CallProcessAssistanceDataL(CWritePropPipe& aPipe, VA_LIST aList)
	{
	TLbsAsistanceDataGroup* pMask		= VA_ARG(aList, TLbsAsistanceDataGroup*);
	RLbsAssistanceDataBuilderSet* pData	= VA_ARG(aList, RLbsAssistanceDataBuilderSet*);
	TInt* pReason						= VA_ARG(aList, TInt*);

	LBSTestMarshaller::WriteL(aPipe, *pMask);
	LBSTestMarshaller::WriteL(aPipe, *pData);
	LBSTestMarshaller::WriteL(aPipe, *pReason);
	}
	
void CNetProtocolProxy::CallProcessLocationUpdateL(CWritePropPipe& aPipe, VA_LIST aList)
	{
	TLbsNetSessionId* pSessionId	= VA_ARG(aList, TLbsNetSessionId*);
	TPositionInfoBase* pPosInfo		= VA_ARG(aList, TPositionInfoBase*);
	
	LBSTestMarshaller::WriteL(aPipe, *pSessionId);
	LBSTestMarshaller::WriteL(aPipe, *pPosInfo);
	}

/**
Returns true if the response should be discarded

@param aResponseType specifies the response type.
@param aResponseFilterMask defines which responses should be filtered out (discarded). 
 */
TBool CNetProtocolProxy::Filtered(TNetProtocolResponseType aResponseType, TUint aResponseFilterMask)
	{
	// here need to look at the bit masks to decide which Response types should be discarded.
	
	TUint shift = aResponseType - KProtocolResponseTypeBase;
	TUint bitmask = 1 << shift;
	
	if (bitmask & aResponseFilterMask)
		{
		return ETrue;
		}

	return EFalse;
	}
		
/**
Waits for a response form the Network Gateway.
If no response is received before the timeout then ENetMsgTimeoutExpired is returned.
aResponseFilterMask is optional and defaults to 0.
Responses may be filtered (discarded) by passing in a aResponseFilterMask 
with the relevant TNetProtocolResponseFilter bit set to a 1.
For example, to filter out both Requests for Assistance Data AND 
Current Capabilities Responses then 
aResponseFilterMask should be set to:
 (EResponseFilterMsgRequestAssistanceData | EResponseFilterMsgGetCurrentCapabilitiesResponse)
@param aTimeout specifies the timeout.
@param aResponseFilterMask is optional and defaults to 0. 
 */

TNetProtocolResponseType CNetProtocolProxy::WaitForResponse(TInt aTimeout, TUint aResponseFilterMask)
	{
    TUint32 temp = (TUint32)aResponseFilterMask;
    
    LBSTESTLOG_METHOD(_L("--"), "jcm testing"); 

 //   LBSTESTLOG_METHOD2(_L("--"), "CNetProtocolProxy::WaitForResponse", aTimeout, temp); 

	CReadPropPipe& r = *iResponseReader;	
	
 	TTime timeStart;
	timeStart.UniversalTime();

	TBool done(EFalse);
	
	while (!done)
		{
		TRAPD(e, r.RefreshL(aTimeout));
       LBSTESTLOG_METHOD1(_L("--"), "TRAPD(e, r.RefreshL(aTimeout));",e); 

		if (KErrNone == e)
			{
			TInt32 response;
			r >> response;
			
			TNetProtocolResponseType protocolResponse = static_cast<TNetProtocolResponseType>(response);
	 //       LBSTESTLOG_METHOD1(_L("--"), "response has been returned",protocolResponse); 
			   LBSTESTLOG_METHOD(_L("--"), "jcm testing 1");
			if (Filtered(protocolResponse, aResponseFilterMask))
				{
		//	    LBSTESTLOG_METHOD(_L("--"), "CNetProtocolProxy::WaitForResponse - response is being discarded because of filter"); 
			    LBSTESTLOG_METHOD(_L("--"), "jcm testing 2");
			 	TTime timeNow;
				timeNow.UniversalTime();
				TTimeIntervalMicroSeconds delay(0);
				delay = timeNow.MicroSecondsFrom(timeStart);
				TInt64 interval64 = delay.Int64();
				timeStart = timeNow;
				aTimeout = aTimeout - interval64;
				if (aTimeout <= 0)
					{
					iLastResponse = ENetMsgTimeoutExpired;
					done = ETrue;
			//        LBSTESTLOG_METHOD1(_L("--"), "returning ENetMsgTimeoutExpired",iLastResponse); 
					   LBSTESTLOG_METHOD1(_L("--"), "jcm testing 3",iLastResponse);
					}
				}
			else
				{
				iLastResponse = protocolResponse;
           //     LBSTESTLOG_METHOD1(_L("--"), "CNetProtocolProxy::WaitForResponse - no filering here, response is ", iLastResponse); 
				   LBSTESTLOG_METHOD1(_L("--"), "jcm testing 4",iLastResponse);
				done = ETrue;
				}
			}
		else
			{
       //     LBSTESTLOG_METHOD1(_L("--"), "RefreshL(aTimeout) returned erorr",e); 
		    LBSTESTLOG_METHOD(_L("--"), "jcm testing 5");
			iLastResponse = ENetMsgTimeoutExpired;
			done = ETrue;
       //    LBSTESTLOG_METHOD1(_L("--"), "returning ENetMsgTimeoutExpired",iLastResponse); 
			   LBSTESTLOG_METHOD1(_L("--"), "jcm testing 6",iLastResponse);
			}
		
		}

   // LBSTESTLOG_METHOD1(_L("--"), "CNetProtocolProxy::WaitForResponse returning", iLastResponse); 
	   LBSTESTLOG_METHOD1(_L("--"), "jcm testing 7",iLastResponse);
	return iLastResponse;
	}

void CNetProtocolProxy::WaitForResponseL(TInt aTimeout, TRequestStatus& aStatus)
	{
	CReadPropPipe& r = *iResponseReader;	
	
	r.RefreshL(aTimeout, aStatus);
	}

void CNetProtocolProxy::CancelWait()
	{
	iResponseReader->CancelRefresh();
	}

TInt CNetProtocolProxy::GetArgsLC(TNetProtocolResponseType aResponse, ...)
	{
	VA_LIST list;
	VA_START(list, aResponse);

	CReadPropPipe& r = *iResponseReader;
	
	TInt cleanupStackCount = 0;
	switch(aResponse)
		{
		case ENetMsgRespondPrivacyRequest:	
			cleanupStackCount = GetENetMsgRespondPrivacyRequestArgsL(r, list);
			break;
		case ENetMsgRespondLocationRequest: 
			cleanupStackCount = GetENetMsgRespondLocationRequestArgsL(r, list);
			break;
		case ENetMsgRequestTransmitLocation:
			cleanupStackCount = GetENetMsgRequestTransmitLocationArgsL(r, list);
			break;
		case ENetMsgCancelTransmitLocation:
			cleanupStackCount = GetENetMsgCancelTransmitLocationArgsL(r, list);
			break;
		case ENetMsgRequestAssistanceData:  
			cleanupStackCount = GetENetMsgRequestAssistanceDataArgsL(r, list);
			break;
		case ENetMsgRequestSelfLocation: 
			cleanupStackCount = GetENetMsgRequestSelfLocationArgsL(r, list);
			break;
		case ENetMsgCancelSelfLocation:
			cleanupStackCount = GetENetMsgCancelSelfLocationArgsL(r, list);
			break;
		case ENetMsgRequestNetworkLocation:
			cleanupStackCount = GetENetMsgRequestNetworkLocationArgsL(r, list);
											break;
		case ENetMsgCancelNetworkLocation:
			cleanupStackCount = GetENetMsgCancelNetworkLocationArgsL(r, list);
			break;
		case ENetMsgGetCurrentCapabilitiesResponse:
			cleanupStackCount = GetENetMsgAdviceSystemStatusArgsL(r, list);
			break;
		default:
			User::Invariant();
			break;

		};
	VA_END(list);

	return cleanupStackCount;
	}

TInt CNetProtocolProxy::GetENetMsgRespondPrivacyRequestArgsL(CReadPropPipe& aPipe, VA_LIST aList)
	{
	TLbsNetSessionId** pSessionId   = VA_ARG(aList, TLbsNetSessionId**);
    CLbsNetworkProtocolBase::TLbsPrivacyResponse* pResponse = VA_ARG(aList, CLbsNetworkProtocolBase::TLbsPrivacyResponse*);
    CMemoryManager* memManager = aPipe.iMemoryManager;
	TInt i = 0;
	LBSTestMarshaller::ReadL(aPipe, *pSessionId,memManager);
	LBSTestMarshaller::ReadL(aPipe, i);
	
	*pResponse = (CLbsNetworkProtocolBase::TLbsPrivacyResponse)i;	
	return 0;
	}

TInt CNetProtocolProxy::GetENetMsgRespondLocationRequestArgsL(CReadPropPipe& aPipe, VA_LIST aList)
	{
	TLbsNetSessionId**  pSessionId = VA_ARG(aList, TLbsNetSessionId**);
    TInt*  			    pReason    = VA_ARG(aList, TInt*);	
    TPositionInfoBase** pPosInfo   = VA_ARG(aList, TPositionInfoBase**);
    CMemoryManager* memManager = aPipe.iMemoryManager;
    LBSTestMarshaller::ReadL(aPipe, *pSessionId,memManager);
    LBSTestMarshaller::ReadL(aPipe, *pReason);
    LBSTestMarshaller::ReadL(aPipe, *pPosInfo,memManager);
    return 0;    
	}

TInt CNetProtocolProxy::GetENetMsgRequestTransmitLocationArgsL(CReadPropPipe& aPipe, VA_LIST aList)
	{
	TLbsNetSessionId** pSessionId   = VA_ARG(aList, TLbsNetSessionId**);
    HBufC16**		   pDest		= VA_ARG(aList, HBufC16**);
    TInt* 			   pPriority	 = VA_ARG(aList, TInt*);
    CMemoryManager* memManager = aPipe.iMemoryManager;
	LBSTestMarshaller::ReadL(aPipe, *pSessionId,memManager);
    LBSTestMarshaller::ReadL(aPipe, *pDest);
    CleanupStack::PushL(*pDest);
   	LBSTestMarshaller::ReadL(aPipe,	*pPriority);
   	return 1;
	}

TInt CNetProtocolProxy::GetENetMsgCancelTransmitLocationArgsL(CReadPropPipe& aPipe, VA_LIST aList)
	{
	TLbsNetSessionId** pSessionId	= VA_ARG(aList, TLbsNetSessionId**);
    TInt*  pReason  				= VA_ARG(aList, TInt*);
    CMemoryManager* memManager = aPipe.iMemoryManager;
	LBSTestMarshaller::ReadL(aPipe, *pSessionId,memManager);
	LBSTestMarshaller::ReadL(aPipe, *pReason);
	return 0;
	}
	
TInt CNetProtocolProxy::GetENetMsgRequestAssistanceDataArgsL(CReadPropPipe& aPipe, VA_LIST aList)
	{
	TLbsAsistanceDataGroup* pDataMask  = VA_ARG(aList,TLbsAsistanceDataGroup*);
	
	LBSTestMarshaller::ReadL(aPipe, *pDataMask);
	return 0;
	}

TInt CNetProtocolProxy::GetENetMsgRequestSelfLocationArgsL(CReadPropPipe& aPipe, VA_LIST aList)
	{
	TLbsNetSessionId**	pSessionId	 = VA_ARG(aList, TLbsNetSessionId**);
    TLbsNetPosRequestOptionsBase** pOptions  = VA_ARG(aList, TLbsNetPosRequestOptionsBase**);	
    CMemoryManager* memManager = aPipe.iMemoryManager;
	LBSTestMarshaller::ReadL(aPipe, *pSessionId,memManager);
    LBSTestMarshaller::ReadL(aPipe, *pOptions,memManager);
    return 0;
	}

TInt CNetProtocolProxy::GetENetMsgCancelSelfLocationArgsL(CReadPropPipe& aPipe, VA_LIST aList)
	{
	TLbsNetSessionId** pSessionId  = VA_ARG(aList, TLbsNetSessionId**);
    TInt*  pReason				   = VA_ARG(aList, TInt*);
    CMemoryManager* memManager = aPipe.iMemoryManager;
    LBSTestMarshaller::ReadL(aPipe, *pSessionId,memManager);
    LBSTestMarshaller::ReadL(aPipe, *pReason);
   	return 0;
	}

TInt CNetProtocolProxy::GetENetMsgRequestNetworkLocationArgsL(CReadPropPipe& aPipe, VA_LIST aList)
	{
	TLbsNetSessionId** pSessionId			 = VA_ARG(aList, TLbsNetSessionId**);
    TLbsNetPosRequestOptionsBase** pOptions  = VA_ARG(aList, TLbsNetPosRequestOptionsBase**);
    CMemoryManager* memManager = aPipe.iMemoryManager;
    LBSTestMarshaller::ReadL(aPipe, *pSessionId,memManager);
    LBSTestMarshaller::ReadL(aPipe, *pOptions,memManager);
    return 0;
	}

TInt CNetProtocolProxy::GetENetMsgCancelNetworkLocationArgsL(CReadPropPipe& aPipe, VA_LIST aList)
	{
	TLbsNetSessionId** pSessionId = VA_ARG(aList, TLbsNetSessionId**);
    TInt*  pReason				  = VA_ARG(aList, TInt*);
    CMemoryManager* memManager =aPipe.iMemoryManager;
    LBSTestMarshaller::ReadL(aPipe, *pSessionId,memManager);
    LBSTestMarshaller::ReadL(aPipe, *pReason);
    return 0;
	}
                        
TInt CNetProtocolProxy::GetENetMsgAdviceSystemStatusArgsL(CReadPropPipe& aPipe, VA_LIST aList)
	{
	CLbsNetworkProtocolBase::TLbsSystemStatus* pStatus = VA_ARG(aList, CLbsNetworkProtocolBase::TLbsSystemStatus*);
	
	TInt i = 0;
	LBSTestMarshaller::ReadL(aPipe, i);
	*pStatus = (CLbsNetworkProtocolBase::TLbsSystemStatus)i;
	return 0;
	}
                        
CNetProtocolProxy::CNetProtocolProxy()
	{
	
	}

void CNetProtocolProxy::ConstructL()
	{
	iActionWriter   = CWritePropPipe::NewL(KActionChannelPipeKey, KActionChannelFlowKey);
	iResponseReader = CReadPropPipe::NewL(KResponseChannelPipeKey, KResponseChannelFlowKey);
	
	iLastResponse = ENetMsgNullResponse;
	}
