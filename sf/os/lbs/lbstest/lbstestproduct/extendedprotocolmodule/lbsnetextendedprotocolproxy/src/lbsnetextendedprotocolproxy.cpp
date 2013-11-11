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
 
#include "lbsnetextendedprotocolproxy.h"
#include <lbs/lbsnetprotocolbase.h>
#include <lbs/test/lbsmarshallingstubs.h>
#include <iniparser.h>
#include <lbs/test/memorymanager.h>

EXPORT_C CNetProtocolProxy* CNetProtocolProxy::NewL()
	{
	CNetProtocolProxy* self = new (ELeave) CNetProtocolProxy;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CNetProtocolProxy* CNetProtocolProxy::NewL(TInt aPmId)
	{
	CNetProtocolProxy* self = new (ELeave) CNetProtocolProxy;
	CleanupStack::PushL(self);
	self->ConstructL(aPmId);
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
	RDebug::Printf("CNetProtocolProxy::Call(%d)",aAction);
	
	VA_LIST list;
	VA_START(list, aAction);
	
	CWritePropPipe& r = *iActionWriter;
	TRAPD(e,r.ResetL());
	__ASSERT_ALWAYS(e == KErrNone, User::Panic(_L("ActionWriteFail"), aAction));
	
	r << aAction;
	// these are the messages that are sent by the Protocol Module (the test code)
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
	
	//Add the end of sequence marker to the end of the message
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
	
TNetProtocolResponseType CNetProtocolProxy::WaitForResponse(TInt aTimeout)
	{
	RDebug::Printf("CNetProtocolProxy::WaitForResponse(%d)",aTimeout);

	CReadPropPipe& r = *iResponseReader;	
	
	TRAPD(e, r.RefreshL(aTimeout));
	if (KErrNone == e)
		{
		TInt32 response;
		r >> response;
		iLastResponse = static_cast<TNetProtocolResponseType>(response);
		}
	else
		{
		iLastResponse = ENetMsgTimeoutExpired;
		}
	return iLastResponse;
	}

void CNetProtocolProxy::WaitForResponseL(TInt aTimeout, TRequestStatus& aStatus)
	{
	RDebug::Printf("CNetProtocolProxy::WaitForResponse(%d, aStatus)",aTimeout);

	CReadPropPipe& r = *iResponseReader;	
	
	r.RefreshL(aTimeout, aStatus);
	}

void CNetProtocolProxy::CancelWait()
	{
	iResponseReader->CancelRefresh();
	}

TInt CNetProtocolProxy::GetArgsLC(TNetProtocolResponseType aResponse, ...)
	{
	RDebug::Printf("CNetProtocolProxy::GetArgs(%d)",aResponse);

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
		case ENetMsgCancelExternalLocation:
			cleanupStackCount = GetENetMsgCancelExternalLocationArgsL(r, list);
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
	
TInt CNetProtocolProxy::GetENetMsgCancelExternalLocationArgsL(CReadPropPipe& aPipe, VA_LIST aList)
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

void CNetProtocolProxy::ConstructL(TInt aPmId)
	{
	//Check the size of the int past in is OK
	if((aPmId < 0) || (aPmId > 100))
		{
		User::Leave(KErrOverflow);
		}
	
	//Open up the PM INI file to extract the key parameters to use
	_LIT(KIniFileName, "c:\\testdata\\configs\\pminfo.ini");
	CIniData* iniFile = CIniData::NewL(KIniFileName);
	CleanupStack::PushL(iniFile);
	
	TBuf<4> pmName;
	pmName.Format(_L("PM%d"), aPmId);
	
	//Response Pipe Key
	TUint32 responsePipeKey;
	TPtrC responsePipeKeyBuf;
	iniFile->FindVar(pmName, _L("ResponsePipeKey"), responsePipeKeyBuf);
	TLex lex(responsePipeKeyBuf);
	User::LeaveIfError(lex.Val(responsePipeKey, EHex));
	
	//Response Flow Key
	TUint32 responseFlowKey;
	TPtrC responseFlowKeyBuf;
	iniFile->FindVar(pmName, _L("ResponseFlowKey"), responseFlowKeyBuf);
	TLex lex2(responseFlowKeyBuf);
	User::LeaveIfError(lex2.Val(responseFlowKey, EHex));
	
	//Action Pipe Key
	TUint32 actionPipeKey;
	TPtrC actionPipeKeyBuf;
	iniFile->FindVar(pmName, _L("ActionPipeKey"), actionPipeKeyBuf);
	TLex lex3(actionPipeKeyBuf);
	User::LeaveIfError(lex3.Val(actionPipeKey, EHex));
	
	//Action Flow Key
	TUint32 actionFlowKey;
	TPtrC actionFlowKeyBuf;
	iniFile->FindVar(pmName, _L("ActionFlowKey"), actionFlowKeyBuf);
	TLex lex4(actionFlowKeyBuf);
	User::LeaveIfError(lex4.Val(actionFlowKey, EHex));
	
	iActionWriter   = CWritePropPipe::NewL(actionPipeKey, actionFlowKey);
	iResponseReader = CReadPropPipe::NewL(responsePipeKey, responseFlowKey);
	
	iLastResponse = ENetMsgNullResponse;
	
	CleanupStack::PopAndDestroy(iniFile);
	}
