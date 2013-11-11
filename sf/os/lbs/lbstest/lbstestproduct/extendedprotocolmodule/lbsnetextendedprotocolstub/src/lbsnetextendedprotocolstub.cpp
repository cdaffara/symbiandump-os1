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
 @file lbsnetextendedprotocolstub.cpp
*/
 
#include <lbs/lbsloccommon.h>
#include <lbs/lbsassistancedatabuilderset.h>
#include "lbsnetextendedprotocolstub.h"
#include <lbs/test/lbsmarshallingstubs.h>
#include <lbs/test/lbsparamlogger.h>
#include <lbs/test/lbstestlogger.h>
#include <iniparser.h>
#include <lbs/test/memorymanager.h>
/** The unique ID of this plug-in interface.
This corresponds to the interface UID specified in the .rss file
for this protocol module.
*/
//const TInt KPluginUidValue = 0x10282293;
#ifdef HYBRID_TEST_PM1
	const TInt KPluginUidValue = 0x10285AB7;
#endif

#ifdef HYBRID_TEST_PM2
	const TInt KPluginUidValue = 0x102869ED;
#endif

/** Defines the plug-in interface implementation UIDs, required by ECOM.
*/
const TImplementationProxy ImplementationTable[] = 
	{
	IMPLEMENTATION_PROXY_ENTRY(KPluginUidValue, CNetExtendedProtocolStub::NewL)
	};


/** Exported function for ECOM to retrieve implementations table
*/
EXPORT_C 
const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

	return ImplementationTable;
	}

//-------------------------------------------------------------------------------------------------

CNetExtendedProtocolStub* CNetExtendedProtocolStub::NewL(TAny* aParam)
	{
//	LBSTESTLOG(ELogP2, "CNetExtendedProtocolStub::NewL");

	TLbsNetProtocolModuleParams* param = reinterpret_cast<TLbsNetProtocolModuleParams*>(aParam);
	CNetExtendedProtocolStub* self = new (ELeave) CNetExtendedProtocolStub(param->Observer());
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CNetExtendedProtocolStub::~CNetExtendedProtocolStub()
	{
//	LBSTESTLOG(ELogP2, "CNetExtendedProtocolStub::~CNetExtendedProtocolStub");
	delete iResponseWriter;
	delete iActionChannelWatcher;
	}


CNetExtendedProtocolStub::CNetExtendedProtocolStub(MLbsNetworkProtocolObserver& aObserver)
: iObserver(aObserver), iActionPerformer(aObserver)
	{
//	LBSTESTLOG(ELogP2, "CNetExtendedProtocolStub::CNetExtendedProtocolStub");	
	}

void CNetExtendedProtocolStub::ConstructL()
	{
//	LBSTESTLOG_METHOD("CNetExtendedProtocolStub::ConstructL");
	
	_LIT(KIniFileName, "c:\\testdata\\configs\\pminfo.ini");
	CIniData* iniFile = CIniData::NewL(KIniFileName);
	CleanupStack::PushL(iniFile);
	
	TBuf<10> uid;
	uid.Format(_L("%x"), KPluginUidValue);
	
	TInt uidPosition;
	TBool found = iniFile->FindVar(uid, uidPosition);
	
	TUint32 responsePipeKey;
	TUint32 responseFlowKey;
	TUint32 actionPipeKey;
	TUint32 actionFlowKey;
	
	if((found) && (uidPosition < 100) && (uidPosition >= 0))
		{
		TBuf<4> pmName;
		pmName.Format(_L("PM%d"), uidPosition);
		
		//Response Pipe Key
		TPtrC responsePipeKeyBuf;
		iniFile->FindVar(pmName, _L("ResponsePipeKey"), responsePipeKeyBuf);
		TLex lex(responsePipeKeyBuf);
		User::LeaveIfError(lex.Val(responsePipeKey, EHex));
		
		//Response Flow Key
		
		TPtrC responseFlowKeyBuf;
		iniFile->FindVar(pmName, _L("ResponseFlowKey"), responseFlowKeyBuf);
		TLex lex2(responseFlowKeyBuf);
		User::LeaveIfError(lex2.Val(responseFlowKey, EHex));
		
		//Action Pipe Key
		TPtrC actionPipeKeyBuf;
		iniFile->FindVar(pmName, _L("ActionPipeKey"), actionPipeKeyBuf);
		TLex lex3(actionPipeKeyBuf);
		User::LeaveIfError(lex3.Val(actionPipeKey, EHex));
		
		//Action Flow Key
		TPtrC actionFlowKeyBuf;
		iniFile->FindVar(pmName, _L("ActionFlowKey"), actionFlowKeyBuf);
		TLex lex4(actionFlowKeyBuf);
		User::LeaveIfError(lex4.Val(actionFlowKey, EHex));
		}
	else
		{
		actionPipeKey = KActionChannelPipeKey;
		actionFlowKey = KActionChannelFlowKey;
		responsePipeKey = KResponseChannelPipeKey;
		responseFlowKey = KResponseChannelFlowKey;
		}
	
	iResponseWriter = CWritePropPipe::NewL(responsePipeKey, responseFlowKey);
	iActionChannelWatcher = CNotifyReadPropPipe::NewL(actionPipeKey, actionFlowKey, iActionPerformer);
	
	CleanupStack::PopAndDestroy(iniFile);
	}
		

/** Send a response to a privacy request to the network.
@see CLbsNetworkProtocolBase::RespondPrivacyRequest()
*/


void CNetExtendedProtocolStub::RespondPrivacyRequest(const TLbsNetSessionId& aSessionId, 
													const TLbsPrivacyResponse& aResponse,
													TInt aReason)
	{
	LBSTESTLOG_METHOD2(INBOUND, "CNetExtendedProtocolStub::RespondPrivacyRequest", aSessionId, aResponse);

	(void) aReason; // JCM todo - need to send this over interface to test code

	//Write to Response Channel
	CWritePropPipe& r = *iResponseWriter;
	TNetProtocolResponseType response = ENetMsgRespondPrivacyRequest;

	LBSTestMarshaller::WriteL(r, response);
	LBSTestMarshaller::WriteL(r, aSessionId);
	LBSTestMarshaller::WriteL(r, aResponse);
	// jcm LBSTestMarshaller::WriteL(r, aReason);
	r << KEoSMarker;
	
	TRAPD(e, r.CommitL(KResponseChannelWriteTimeout));
	__ASSERT_ALWAYS(e == KErrNone, User::Panic(_L("RespondWriteFail"), response));
	}


/** Send a response to a location request to the network.
@see CLbsNetworkProtocolBase::RespondLocationRequest()
*/
void CNetExtendedProtocolStub::RespondLocationRequest(const TLbsNetSessionId& aSessionId, TInt aReason, const TPositionInfoBase& aPosInfo)
	{
	LBSTESTLOG_METHOD3(INBOUND, "CNetExtendedProtocolStub::RespondLocationRequest", aSessionId, aReason, aPosInfo);	

	//Write to Response Channel
	CWritePropPipe& r = *iResponseWriter;
	TNetProtocolResponseType response = ENetMsgRespondLocationRequest;
	
	LBSTestMarshaller::WriteL(r, response);
	LBSTestMarshaller::WriteL(r, aSessionId);
	LBSTestMarshaller::WriteL(r, aReason);
	LBSTestMarshaller::WriteL(r, aPosInfo);		
	r << KEoSMarker;
	
	TRAPD(e, r.CommitL(KResponseChannelWriteTimeout));
	__ASSERT_ALWAYS(e == KErrNone, User::Panic(_L("RespondWriteFail"), response));
	}


/** Send a request for GPS assistance data to the network.
@see CLbsNetworkProtocolBase::RequestAssistanceData()
*/

void CNetExtendedProtocolStub::RequestAssistanceData(TLbsAsistanceDataGroup aDataRequestMask,
							const TLbsNetSessionIdArray& aSessionIdArray)
	{
	LBSTESTLOG_METHOD1(INBOUND, "CNetExtendedProtocolStub::RequestAssistanceData", aDataRequestMask);
(void) aSessionIdArray; // JCM todo
	//Write to Response Channel
	CWritePropPipe& r = *iResponseWriter;
	TNetProtocolResponseType response = ENetMsgRequestAssistanceData;
	
	LBSTestMarshaller::WriteL(r, response);
	LBSTestMarshaller::WriteL(r, aDataRequestMask);
	r << KEoSMarker;
	
	TRAPD(e, r.CommitL(KResponseChannelWriteTimeout));
	__ASSERT_ALWAYS(e == KErrNone, User::Panic(_L("RespondWriteFail"), response));
	}

/** Send a request for an approximate location based on the current network cell to the network.
@see CLbsNetworkProtocolBase::RequestNetworkLocation()
*/
void CNetExtendedProtocolStub::RequestNetworkLocation(const TLbsNetSessionId& aSessionId,
							const TLbsNetPosRequestOptionsBase& aOptions)
	{
	LBSTESTLOG_METHOD2(INBOUND, "CNetExtendedProtocolStub::RequestNetworkLocation", aSessionId, aOptions);

	//Write to Response Channel
	CWritePropPipe& r = *iResponseWriter;
	TNetProtocolResponseType response = ENetMsgRequestNetworkLocation;

	LBSTestMarshaller::WriteL(r, response);
	LBSTestMarshaller::WriteL(r, aSessionId);
	LBSTestMarshaller::WriteL(r, aOptions);
	r << KEoSMarker;
	
	TRAPD(e, r.CommitL(KResponseChannelWriteTimeout));
	__ASSERT_ALWAYS(e == KErrNone, User::Panic(_L("RespondWriteFail"), response));
	}


/** Tell the protocol module to finish the current network based location request.
@see CLbsNetworkProtocolBase::CancelNetworkLocation()
*/
void CNetExtendedProtocolStub::CancelNetworkLocation(const TLbsNetSessionId& aSessionId, TInt aReason)
	{
	LBSTESTLOG_METHOD2(INBOUND, "CNetExtendedProtocolStub::CancelNetworkLocation", aSessionId, aReason);

	//Write to Response Channel
	CWritePropPipe& r = *iResponseWriter;
	TNetProtocolResponseType response = ENetMsgCancelNetworkLocation;
	
	LBSTestMarshaller::WriteL(r, response);
	LBSTestMarshaller::WriteL(r, aSessionId);
	LBSTestMarshaller::WriteL(r, aReason);
	r << KEoSMarker;
	
	TRAPD(e, r.CommitL(KResponseChannelWriteTimeout));
	__ASSERT_ALWAYS(e == KErrNone, User::Panic(_L("RespondWriteFail"), response));
	}


/** Request self locate.
@see CLbsNetworkProtocolBase::RequestSelfLocation()
*/
void CNetExtendedProtocolStub::RequestSelfLocation(const TLbsNetSessionId& aSessionId,
							const TLbsNetPosRequestOptionsBase& aOptions)	
	{
	LBSTESTLOG_METHOD2(INBOUND, "CNetExtendedProtocolStub::RequestSelfLocation", aSessionId, aOptions);	

	//Write to Response Channel
	CWritePropPipe& r = *iResponseWriter;
	TNetProtocolResponseType response = ENetMsgRequestSelfLocation;
	
	LBSTestMarshaller::WriteL(r, response);
	LBSTestMarshaller::WriteL(r, aSessionId);
	LBSTestMarshaller::WriteL(r, aOptions);
	r << KEoSMarker;
	
	TRAPD(e, r.CommitL(KResponseChannelWriteTimeout));
	__ASSERT_ALWAYS(e == KErrNone, User::Panic(_L("RespondWriteFail"), response));
	}


/** Cancel request for self locate.
@see CLbsNetworkProtocolBase::CancelSelfLocation()
*/
void CNetExtendedProtocolStub::CancelSelfLocation(const TLbsNetSessionId& aSessionId,
							TInt  aReason)
	{
	LBSTESTLOG_METHOD2(INBOUND, "CNetExtendedProtocolStub::CancelSelfLocation", aSessionId, aReason);
	
	//Write to Response Channel
	CWritePropPipe& r = *iResponseWriter;
	TNetProtocolResponseType response = ENetMsgCancelSelfLocation;

	LBSTestMarshaller::WriteL(r, response);
	LBSTestMarshaller::WriteL(r, aSessionId);
	LBSTestMarshaller::WriteL(r, aReason);
	r << KEoSMarker;
	
	TRAPD(e, r.CommitL(KResponseChannelWriteTimeout));
	__ASSERT_ALWAYS(e == KErrNone, User::Panic(_L("RespondWriteFail"), response));
	}

/** Lbs System Status information.
@see CLbsNetworkProtocolBase::AdviceSystemStatus()
*/
void CNetExtendedProtocolStub::AdviceSystemStatus(TLbsSystemStatus aStatus)
	{
	LBSTESTLOG_METHOD1(INBOUND, "CNetExtendedProtocolStub::AdviceSystemStatus", aStatus);

	//Write to Response Channel
	CWritePropPipe& r = *iResponseWriter;
	TNetProtocolResponseType response = ENetMsgGetCurrentCapabilitiesResponse;

	r << response;
	LBSTestMarshaller::WriteL(r, aStatus);
	r << KEoSMarker;
	
	TRAPD(e, r.CommitL(KResponseChannelWriteTimeout));
	__ASSERT_ALWAYS(e == KErrNone, User::Panic(_L("RespondWriteFail"), response));
	}


void CNetExtendedProtocolStub::RequestTransmitLocation(const TLbsNetSessionId& aSessionId,
													  const TDesC& aDestination,
													  TInt aPriority,
													  const TLbsNetPosRequestOptionsBase& aOptions)
	{
	LBSTESTLOG_METHOD3(INBOUND, "CNetExtendedProtocolStub::RequestTransmitLocation", aSessionId, aDestination, aPriority);
	
	//Write to Response Channel
	CWritePropPipe& r = *iResponseWriter;
	TNetProtocolResponseType response = ENetMsgRequestTransmitLocation;

	r << response;
	LBSTestMarshaller::WriteL(r, aSessionId);
	LBSTestMarshaller::WriteL(r, aDestination);
	LBSTestMarshaller::WriteL(r, aPriority);
	r << KEoSMarker;

	TLbsNetPosRequestOptionsTechnology opts;
	opts = reinterpret_cast<const TLbsNetPosRequestOptionsTechnology&>(aOptions);
	TRAPD(e, r.CommitL(KResponseChannelWriteTimeout));
	__ASSERT_ALWAYS(e == KErrNone, User::Panic(_L("RespondWriteFail"), response));
	}


/** Tell the protocol module to end the current send location to third party request.
@see CLbsNetworkProtocolBase::CancelTransmitLocation()
*/
void CNetExtendedProtocolStub::CancelTransmitLocation(const TLbsNetSessionId& aSessionId, TInt aReason)
	{
	LBSTESTLOG_METHOD2(INBOUND, "CNetExtendedProtocolStub::CancelTransmitLocation", aSessionId, aReason);
	
	//Write to Response Channel
	CWritePropPipe& r = *iResponseWriter;
	TNetProtocolResponseType response = ENetMsgCancelTransmitLocation;
	
	LBSTestMarshaller::WriteL(r, response);
	LBSTestMarshaller::WriteL(r, aSessionId);
	LBSTestMarshaller::WriteL(r, aReason);
	r << KEoSMarker;
	
	TRAPD(e, r.CommitL(KResponseChannelWriteTimeout));
	__ASSERT_ALWAYS(e == KErrNone, User::Panic(_L("RespondWriteFail"), response));
	}
/** Tell the protocol module to end the current send location to third party request.
@see CLbsNetworkProtocolBase::CancelTransmitLocation()
*/
void CNetExtendedProtocolStub::CancelExternalLocation(const TLbsNetSessionId& aSessionId, TInt aReason)
	{
	LBSTESTLOG_METHOD2(INBOUND, "CNetExtendedProtocolStub::CancelExternalLocation", aSessionId, aReason);
	
	//Write to Response Channel
	CWritePropPipe& r = *iResponseWriter;
	TNetProtocolResponseType response = ENetMsgCancelExternalLocation;
	
	LBSTestMarshaller::WriteL(r, response);
	LBSTestMarshaller::WriteL(r, aSessionId);
	LBSTestMarshaller::WriteL(r, aReason);
	r << KEoSMarker;
	
	TRAPD(e, r.CommitL(KResponseChannelWriteTimeout));
	__ASSERT_ALWAYS(e == KErrNone, User::Panic(_L("RespondWriteFail"), response));
	}

//-------------------------------------------------------------------------------------------------

TActionPerformer::TActionPerformer(MLbsNetworkProtocolObserver& aObserver)
 : iObserver(aObserver)
 	{
 	}

void TActionPerformer::OnReadyToReadL(CReadPropPipe& aPipe)
	{
	TInt32 action;
	aPipe.RefreshL(0);
	aPipe >> action;
    CMemoryManager* memManager =aPipe.iMemoryManager;

 	switch(action)
		{
		case ENetMsgProcessPrivacyRequest:		doNetMsgProcessPrivacyRequestL(aPipe,memManager);
												break;			
		case ENetMsgProcessLocationRequest:		doNetMsgProcessLocationRequestL(aPipe,memManager);
												break;			
		case ENetMsgProcessSessionComplete:		doNetMsgProcessSessionCompleteL(aPipe,memManager);
												break;			
		case ENetMsgProcessAssistanceData:		doNetMsgProcessAssistanceDataL(aPipe,memManager);
												break;			
		case ENetMsgProcessLocationUpdate:		doNetMsgProcessLocationUpdateL(aPipe,memManager);
												break;			
		case ENetMsgGetCurrentCapabilitiesRequest: doNetMsgGetCurrentCapabilitiesRequestL(aPipe,memManager);
												break;			
		case ENetMsgProcessStatusUpdate:		doNetMsgProcessStatusUpdateL(aPipe);
												break;			
		default :
			User::Panic(_L("ActionInvalid"), action);
		}
	}



void TActionPerformer::doNetMsgProcessPrivacyRequestL(CReadPropPipe& aPipe,CMemoryManager*aMemManager)
	{
	TLbsNetSessionId* pSessionId = NULL;
	TBool emergency = EFalse;
	TLbsNetPosRequestPrivacy* pPrivacy = NULL;
	TLbsExternalRequestInfo* pRequestInfo = NULL;

	LBSTestMarshaller::ReadL(aPipe, pSessionId,aMemManager);
	LBSTestMarshaller::ReadL(aPipe, emergency);
	LBSTestMarshaller::ReadL(aPipe, pPrivacy,aMemManager);
	LBSTestMarshaller::ReadL(aPipe, pRequestInfo,aMemManager);
	
	LBSTESTLOG_METHOD4(OUTBOUND, "MLbsNetworkProtocolObserver::ProcessPrivacyRequest", *pSessionId, emergency,	*pPrivacy, *pRequestInfo);
	iObserver.ProcessPrivacyRequest(*pSessionId, emergency, *pPrivacy, *pRequestInfo);
	}
	
void TActionPerformer::doNetMsgProcessLocationRequestL (CReadPropPipe& aPipe,CMemoryManager*aMemManager)
	{
	TLbsNetSessionId* pSessionId = NULL;
	TBool emergency = EFalse;
	TInt  service = MLbsNetworkProtocolObserver::EServiceMobileTerminated;
	TLbsNetPosRequestQuality* pQuality = NULL;
	TLbsNetPosRequestMethod* pMethod = NULL;
	
	LBSTestMarshaller::ReadL(aPipe, pSessionId,aMemManager);
	LBSTestMarshaller::ReadL(aPipe, emergency);
	LBSTestMarshaller::ReadL(aPipe, service);
	LBSTestMarshaller::ReadL(aPipe, pQuality,aMemManager);
	LBSTestMarshaller::ReadL(aPipe, pMethod,aMemManager);

	LBSTESTLOG_METHOD5(OUTBOUND, "MLbsNetworkProtocolObserver::ProcessLocationRequest", *pSessionId, emergency, service, *pQuality, *pMethod);
	iObserver.ProcessLocationRequest(*pSessionId, emergency, (MLbsNetworkProtocolObserver::TLbsNetProtocolService)service, *pQuality, *pMethod);
	}

void TActionPerformer::doNetMsgProcessSessionCompleteL(CReadPropPipe& aPipe,CMemoryManager*aMemManager)
	{
	TLbsNetSessionId* pSessionId = NULL;
	TInt reason = 0;
	
	LBSTestMarshaller::ReadL(aPipe, pSessionId,aMemManager);
	LBSTestMarshaller::ReadL(aPipe, reason);

	LBSTESTLOG_METHOD2(OUTBOUND, "MLbsNetworkProtocolObserver::ProcessSessionComplete", *pSessionId, reason);
	iObserver.ProcessSessionComplete(*pSessionId, reason);
	
    }

void TActionPerformer::doNetMsgProcessAssistanceDataL(CReadPropPipe& aPipe,CMemoryManager*aMemManager)
	{
	TLbsAsistanceDataGroup dataMask;
	RLbsAssistanceDataBuilderSet* pData = NULL;
	TInt reason = 0;
	
	LBSTestMarshaller::ReadL(aPipe, dataMask);
	LBSTestMarshaller::ReadL(aPipe, pData,aMemManager);
	LBSTestMarshaller::ReadL(aPipe, reason);
	
	LBSTESTLOG_METHOD3(OUTBOUND, "MLbsNetworkProtocolObserver::ProcessAssistanceData", dataMask, *pData, reason);
	iObserver.ProcessAssistanceData(dataMask, *pData, reason);
	
	pData->Close();
	}

void TActionPerformer::doNetMsgProcessLocationUpdateL(CReadPropPipe& aPipe,CMemoryManager*aMemManager)
	{
	TLbsNetSessionId* pSessionId = NULL;
	TPositionInfoBase* pPosInfo = NULL;

	LBSTestMarshaller::ReadL(aPipe, pSessionId,aMemManager);
	LBSTestMarshaller::ReadL(aPipe,pPosInfo,aMemManager);

	LBSTESTLOG_METHOD2(OUTBOUND, "MLbsNetworkProtocolObserver::ProcessLocationUpdate", *pSessionId, *pPosInfo);
	iObserver.ProcessLocationUpdate(*pSessionId, *pPosInfo);
	}

void TActionPerformer::doNetMsgGetCurrentCapabilitiesRequestL(CReadPropPipe& aPipe,CMemoryManager*aMemManager)
	{
	TLbsNetPosCapabilities *pCaps = NULL;
	LBSTestMarshaller::ReadL(aPipe, pCaps,aMemManager);
	
	LBSTESTLOG_METHOD1(OUTBOUND, "MLbsNetworkProtocolObserver::GetCurrentCapabilities", *pCaps);
	iObserver.GetCurrentCapabilities(*pCaps);
	}

void TActionPerformer::doNetMsgProcessStatusUpdateL(CReadPropPipe& aPipe)
	{
	MLbsNetworkProtocolObserver::TLbsNetProtocolServiceMask mask = 0x0100;
	LBSTestMarshaller::ReadL(aPipe, mask);	
	
	LBSTESTLOG_METHOD1(OUTBOUND, "MLbsNetworkProtocolObserver::ProcessStatusUpdate", mask);
	iObserver.ProcessStatusUpdate(mask);
	}

//-------------------------------------------------------------------------------------------------



