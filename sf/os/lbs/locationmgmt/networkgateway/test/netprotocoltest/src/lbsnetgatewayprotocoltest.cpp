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
// Source file for the 'stub' network protocol module used in
// the Network Gateway unit tests.
// 
//

/**
 @file
 @internalTechnology
 @released
*/

#include <e32base.h>
#include <e32debug.h>
#include <ecom/implementationproxy.h>
#include <lbs.h>
#include "lbsdevloggermacros.h"
#include "lbsnetgatewayprotocoltest.h"


_LIT(KLbsTestNetworkProtocol,"LbsTestNetworkProtocol");


	
//
// ECom instantiation functions
//

const TImplementationProxy KImplementationTable[] = 
	{
	IMPLEMENTATION_PROXY_ENTRY(0x10282250, CTestLbsNetworkProtocol::New0L),
	IMPLEMENTATION_PROXY_ENTRY(0x10285AB9, CTestLbsNetworkProtocol::New1L),
	// Entry points for multiple PM test
	IMPLEMENTATION_PROXY_ENTRY(0x102871C8, CTestLbsNetworkProtocol::New2L),
	IMPLEMENTATION_PROXY_ENTRY(0x102871C9, CTestLbsNetworkProtocol::New3L),
	IMPLEMENTATION_PROXY_ENTRY(0x102871CA, CTestLbsNetworkProtocol::New4L),
	IMPLEMENTATION_PROXY_ENTRY(0x102871CB, CTestLbsNetworkProtocol::New5L),
	IMPLEMENTATION_PROXY_ENTRY(0x102871CC, CTestLbsNetworkProtocol::New6L),
	IMPLEMENTATION_PROXY_ENTRY(0x102871CD, CTestLbsNetworkProtocol::New7L),
	IMPLEMENTATION_PROXY_ENTRY(0x102871CE, CTestLbsNetworkProtocol::New8L),
	IMPLEMENTATION_PROXY_ENTRY(0x102871CF, CTestLbsNetworkProtocol::New9L),

	// Another 10 entry points for the roaming PMs 
	IMPLEMENTATION_PROXY_ENTRY(0x102871D0, CTestLbsNetworkProtocol::New10L),
	IMPLEMENTATION_PROXY_ENTRY(0x102871D1, CTestLbsNetworkProtocol::New11L),
	IMPLEMENTATION_PROXY_ENTRY(0x102871D2, CTestLbsNetworkProtocol::New12L),
	IMPLEMENTATION_PROXY_ENTRY(0x102871D3, CTestLbsNetworkProtocol::New13L),
	IMPLEMENTATION_PROXY_ENTRY(0x102871D4, CTestLbsNetworkProtocol::New14L),
	IMPLEMENTATION_PROXY_ENTRY(0x102871D5, CTestLbsNetworkProtocol::New15L),
	IMPLEMENTATION_PROXY_ENTRY(0x102871D6, CTestLbsNetworkProtocol::New16L),
	IMPLEMENTATION_PROXY_ENTRY(0x102871D7, CTestLbsNetworkProtocol::New17L),
	IMPLEMENTATION_PROXY_ENTRY(0x102871D8, CTestLbsNetworkProtocol::New18L),
	IMPLEMENTATION_PROXY_ENTRY(0x102871D9, CTestLbsNetworkProtocol::New19L),
	};
	
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = (sizeof(KImplementationTable) / sizeof(TImplementationProxy));
	return KImplementationTable;
	}

//
// CTestLbsNetworkProtocol Implementation
//

CTestLbsNetworkProtocol::CTestLbsNetworkProtocol(TUint aModuleIndex, 
		TLbsNetProtocolModuleParams& aParams) : iObserver(aParams.Observer()),
		iTestChannel(aModuleIndex),
		iModuleIndex(aModuleIndex)
	{
	LBSLOG(ELogP1, "CTestLbsNetworkProtocol::CTestLbsNetworkProtocol:");
	}

CTestLbsNetworkProtocol::~CTestLbsNetworkProtocol()
	{
	LBSLOG(ELogP1, "CTestLbsNetworkProtocol::~CTestLbsNetworkProtocol:");
	iTestChannel.Close();
	RNetGatewayProtocolTestChannel::Shutdown(iModuleIndex);
	}

CTestLbsNetworkProtocol* CTestLbsNetworkProtocol::New0L(TAny* aData)
	{
	return NewL(KTe_LbsNetGatewayTestProtocolIndex, aData);
	}

CTestLbsNetworkProtocol* CTestLbsNetworkProtocol::New1L(TAny* aData)
	{
	return NewL(KTe_LbsNetGatewayTestProtocolIndex2, aData);
	}

CTestLbsNetworkProtocol* CTestLbsNetworkProtocol::New2L(TAny* aData)
	{
	return NewL(KTe_LbsNetGatewayTestProtocolIndex3, aData);
	}

CTestLbsNetworkProtocol* CTestLbsNetworkProtocol::New3L(TAny* aData)
	{
	return NewL(KTe_LbsNetGatewayTestProtocolIndex4, aData);
	}

CTestLbsNetworkProtocol* CTestLbsNetworkProtocol::New4L(TAny* aData)
	{
	return NewL(KTe_LbsNetGatewayTestProtocolIndex5, aData);
	}

CTestLbsNetworkProtocol* CTestLbsNetworkProtocol::New5L(TAny* aData)
	{
	return NewL(KTe_LbsNetGatewayTestProtocolIndex6, aData);
	}

CTestLbsNetworkProtocol* CTestLbsNetworkProtocol::New6L(TAny* aData)
	{
	return NewL(KTe_LbsNetGatewayTestProtocolIndex7, aData);
	}

CTestLbsNetworkProtocol* CTestLbsNetworkProtocol::New7L(TAny* aData)
	{
	return NewL(KTe_LbsNetGatewayTestProtocolIndex8, aData);
	}

CTestLbsNetworkProtocol* CTestLbsNetworkProtocol::New8L(TAny* aData)
	{
	return NewL(KTe_LbsNetGatewayTestProtocolIndex9, aData);
	}

CTestLbsNetworkProtocol* CTestLbsNetworkProtocol::New9L(TAny* aData)
	{
	return NewL(KTe_LbsNetGatewayTestProtocolIndex10, aData);
	}

CTestLbsNetworkProtocol* CTestLbsNetworkProtocol::New10L(TAny* aData)
	{
	return NewL(KTe_LbsNetGatewayTestProtocolIndex11, aData);
	}

// 10 entry points for roaming modules
CTestLbsNetworkProtocol* CTestLbsNetworkProtocol::New11L(TAny* aData)
	{
	return NewL(KTe_LbsNetGatewayTestProtocolIndex12, aData);
	}
CTestLbsNetworkProtocol* CTestLbsNetworkProtocol::New12L(TAny* aData)
	{
	return NewL(KTe_LbsNetGatewayTestProtocolIndex13, aData);
	}
CTestLbsNetworkProtocol* CTestLbsNetworkProtocol::New13L(TAny* aData)
	{
	return NewL(KTe_LbsNetGatewayTestProtocolIndex14, aData);
	}
CTestLbsNetworkProtocol* CTestLbsNetworkProtocol::New14L(TAny* aData)
	{
	return NewL(KTe_LbsNetGatewayTestProtocolIndex15, aData);
	}
CTestLbsNetworkProtocol* CTestLbsNetworkProtocol::New15L(TAny* aData)
	{
	return NewL(KTe_LbsNetGatewayTestProtocolIndex16, aData);
	}
CTestLbsNetworkProtocol* CTestLbsNetworkProtocol::New16L(TAny* aData)
	{
	return NewL(KTe_LbsNetGatewayTestProtocolIndex17, aData);
	}
CTestLbsNetworkProtocol* CTestLbsNetworkProtocol::New17L(TAny* aData)
	{
	return NewL(KTe_LbsNetGatewayTestProtocolIndex18, aData);
	}
CTestLbsNetworkProtocol* CTestLbsNetworkProtocol::New18L(TAny* aData)
	{
	return NewL(KTe_LbsNetGatewayTestProtocolIndex19, aData);
	}
CTestLbsNetworkProtocol* CTestLbsNetworkProtocol::New19L(TAny* aData)
	{
	return NewL(KTe_LbsNetGatewayTestProtocolIndex20, aData);
	}



CTestLbsNetworkProtocol* CTestLbsNetworkProtocol::NewL(TUint aModuleIndex, TAny* aData)
	{
	LBSLOG(ELogP1, "CTestLbsNetworkProtocol::NewL:");
	TLbsNetProtocolModuleParams* params = reinterpret_cast<TLbsNetProtocolModuleParams*>(aData);
	CTestLbsNetworkProtocol* self = new (ELeave) CTestLbsNetworkProtocol(aModuleIndex, *params);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

void CTestLbsNetworkProtocol::ConstructL()
	{
	LBSLOG(ELogP1, "CTestLbsNetworkProtocol::ConstructL:");
	RNetGatewayProtocolTestChannel::InitialiseL(iModuleIndex);
	iTestChannel.OpenL(KTestAppChannelKey, KNetProtocolChannelKey, *this);	
	}

void CTestLbsNetworkProtocol::RespondPrivacyRequest(const TLbsNetSessionId& aSessionId,
							   						const TLbsPrivacyResponse& aResponse,
							   						TInt /* aReason */)
	{
	LBSLOG(ELogP2, "CTestLbsNetworkProtocol::RespondPrivacyRequest:");
	LBSLOG3(ELogP2, "SessionId : (%d, %d)\n", aSessionId.SessionOwner().iUid, 
											  aSessionId.SessionNum());	
	LBSLOG2(ELogP2, "Response : %d", aResponse);
	
	TNetGatewayMsg msg;
	SNetMsgRespondPrivacyRequest* response;
	response = reinterpret_cast<SNetMsgRespondPrivacyRequest*>(msg.Data());	
	msg.iType = ENetMsgRespondPrivacyRequest;
	response->iSessionId = aSessionId;
	response->iResponse = aResponse;
	
	TInt err = KErrNone;
	TRAP(err, iTestChannel.SendNetProtocolMessageL(msg);)
	if(err)
		User::Panic(KLbsTestNetworkProtocol, err);
	}
	
void CTestLbsNetworkProtocol::RespondLocationRequest(const TLbsNetSessionId& aSessionId,
													 TInt  aReason,
													 const TPositionInfoBase& aPosInfo)
	{
	const TPositionInfo& posInfo = static_cast<const TPositionInfo&>(aPosInfo);
	TPosition position;
	posInfo.GetPosition(position);
	
	LBSLOG(ELogP2, "CTestLbsNetworkProtocol::RespondLocationRequest:");
	LBSLOG3(ELogP2, "SessionId : (%d, %d)\n", aSessionId.SessionOwner().iUid, 
											  aSessionId.SessionNum());
	LBSLOG2(ELogP2, "Reason : %d", aReason);
	LBSLOG3(ELogP2, "Position : %LF lat, %LF long", position.Latitude(), 
													position.Longitude());
	
	SNetMsgRespondLocationRequest* response;
	TNetGatewayMsg msg;
	response = reinterpret_cast<SNetMsgRespondLocationRequest*>(msg.Data());
	new (response) SNetMsgRespondLocationRequest;
	msg.iType = ENetMsgRespondLocationRequest;
	response->iSessionId = aSessionId;
	response->iReason = aReason;
	// This assumes that aPosInfo is really a reference to 
	// a TPositionSatelliteInfo. If this isn't true then
	// the last part will be junk.
	Mem::Copy(&response->iPositionInfo, &aPosInfo, response->iPositionInfo.PositionClassSize());
	
	TInt err = KErrNone;
	TRAP(err, iTestChannel.SendNetProtocolMessageL(msg);)
	if(err)
		User::Panic(KLbsTestNetworkProtocol, err);
	
	}

void CTestLbsNetworkProtocol::RequestAssistanceData(TLbsAssistanceDataGroup aDataRequestMask, const TLbsNetSessionIdArray& /*aSessionIdArray*/)
	{
	LBSLOG(ELogP2, "CTestLbsNetworkProtocol::RequestAssistanceData:");
	LBSLOG2(ELogP2, "Data Mask : 0x%x", aDataRequestMask);
	
	TNetGatewayMsg msg;
	SNetMsgRequestAssistanceData* request;
	request = reinterpret_cast<SNetMsgRequestAssistanceData*>(msg.Data());
	msg.iType = ENetMsgRequestAssistanceData;
	request->iDataRequestMask = aDataRequestMask;
	
	TInt err = KErrNone;
	TRAP(err, iTestChannel.SendNetProtocolMessageL(msg);)
	if(err)
		User::Panic(KLbsTestNetworkProtocol, err);
	}

void CTestLbsNetworkProtocol::RequestSelfLocation(const TLbsNetSessionId& aSessionId,
													 const TLbsNetPosRequestOptionsBase& aOptions)
	{
	LBSLOG(ELogP2, "CTestLbsNetworkProtocol::RequestNetworkBasedLocation:");
	LBSLOG3(ELogP2, "SessionId : (%d, %d)\n", aSessionId.SessionOwner().iUid, 
											  aSessionId.SessionNum());
	
	TNetGatewayMsg msg;
	SNetMsgRequestSelfLocation* request;
	request = reinterpret_cast<SNetMsgRequestSelfLocation*>(msg.Data());
	msg.iType = ENetMsgRequestSelfLocation;
	const TLbsNetPosRequestOptionsAssistance assistOptions = reinterpret_cast<const TLbsNetPosRequestOptionsAssistance&>(aOptions);
	request->iSessionId = aSessionId;
	request->iNewClient = assistOptions.NewClientConnected();
	request->iDataRequestMask = assistOptions.DataRequestMask();
	TInt err = KErrNone;
	TRAP(err, iTestChannel.SendNetProtocolMessageL(msg);)
	if(err)
		User::Panic(KLbsTestNetworkProtocol, err);
	}
	
void CTestLbsNetworkProtocol::CancelSelfLocation(const TLbsNetSessionId& aSessionId,
													 TInt aReason)
	{
	LBSLOG(ELogP2, "CTestLbsNetworkProtocol::CancelSelfLocation:");
	LBSLOG3(ELogP2, "SessionId : (%d, %d)\n", aSessionId.SessionOwner().iUid, 
											  aSessionId.SessionNum());
	LBSLOG2(ELogP2, "Reason : %d", aReason);
	
	TNetGatewayMsg msg;
	SNetMsgCancelSelfLocation* cancel;
	cancel = reinterpret_cast<SNetMsgCancelSelfLocation*>(msg.Data());
	msg.iType = ENetMsgCancelSelfLocation;
	cancel->iSessionId = aSessionId;
	cancel->iReason = aReason;

	TInt err = KErrNone;
	TRAP(err, iTestChannel.SendNetProtocolMessageL(msg);)
	if(err)
		User::Panic(KLbsTestNetworkProtocol, err);
	}

void CTestLbsNetworkProtocol::RequestNetworkLocation(const TLbsNetSessionId& aSessionId,
													 const TLbsNetPosRequestOptionsBase& aOptions)
	{
	LBSLOG(ELogP2, "CTestLbsNetworkProtocol::RequestNetworkLocation:");
	LBSLOG3(ELogP2, "SessionId : (%d, %d)\n", aSessionId.SessionOwner().iUid, 
											  aSessionId.SessionNum());
	
	TNetGatewayMsg msg;
	SNetMsgRequestNetworkLocation* request;
	request = reinterpret_cast<SNetMsgRequestNetworkLocation*>(msg.Data());
	msg.iType = ENetMsgRequestNetworkLocation;
	const TLbsNetPosRequestOptions netOptions = reinterpret_cast<const TLbsNetPosRequestOptions&>(aOptions);
	netOptions.GetRequestQuality(request->iQuality);
	request->iSessionId = aSessionId;

	TInt err = KErrNone;
	TRAP(err, iTestChannel.SendNetProtocolMessageL(msg);)
	if(err)
		User::Panic(KLbsTestNetworkProtocol, err);
	}
	
void CTestLbsNetworkProtocol::CancelNetworkLocation(const TLbsNetSessionId& aSessionId,
													 TInt aReason)
	{
	LBSLOG(ELogP2, "CTestLbsNetworkProtocol::CancelNetworkLocation:");
	LBSLOG3(ELogP2, "SessionId : (%d, %d)\n", aSessionId.SessionOwner().iUid, 
											  aSessionId.SessionNum());
	LBSLOG2(ELogP2, "Reason : %d", aReason);
	
	TNetGatewayMsg msg;
	SNetMsgCancelNetworkLocation* cancel;
	cancel = reinterpret_cast<SNetMsgCancelNetworkLocation*>(msg.Data());
	msg.iType = ENetMsgCancelNetworkLocation;
	cancel->iSessionId = aSessionId;
	cancel->iReason = aReason;

	TInt err = KErrNone;
	TRAP(err, iTestChannel.SendNetProtocolMessageL(msg);)
	if(err)
		User::Panic(KLbsTestNetworkProtocol, err);
	}

void CTestLbsNetworkProtocol::RequestTransmitLocation(const TLbsNetSessionId& aSessionId,
													  const TDesC& aDestination,
													  TInt aPriority,
													  const TLbsNetPosRequestOptionsBase& aOptions)
	{
	LBSLOG(ELogP2, "CTestLbsNetworkProtocol::RequestTransmitLocation:");
	LBSLOG3(ELogP2, "SessionId : (%d, %d)\n", aSessionId.SessionOwner().iUid, 
											  aSessionId.SessionNum());
	LBSLOG2(ELogP2, "Destination : %S", &aDestination);
	LBSLOG2(ELogP2, "Priority : %d", aPriority);
					
	TNetGatewayMsg msg;
	SNetMsgRequestTransmitLocation* request;
	request = reinterpret_cast<SNetMsgRequestTransmitLocation*>(msg.Data());
	new (request) SNetMsgRequestTransmitLocation; // in-place construction is needed to construct the descriptors (iDestination)
	msg.iType = ENetMsgRequestTransmitLocation;
	request->iSessionId = aSessionId;
	request->iPriority = aPriority;
	request->iDestination = aDestination;
	request->iOptions = reinterpret_cast<const TLbsNetPosRequestOptionsTechnology&>(aOptions);

	TInt err = KErrNone;
	TRAP(err, iTestChannel.SendNetProtocolMessageL(msg);)
	if(err)
		User::Panic(KLbsTestNetworkProtocol, err);
	}
	
void CTestLbsNetworkProtocol::CancelTransmitLocation(const TLbsNetSessionId& aSessionId,
													 TInt  aReason)
	{
	LBSLOG(ELogP2, "CTestLbsNetworkProtocol::CancelTransmitLocation:");
	LBSLOG3(ELogP2, "SessionId : (%d, %d)\n", aSessionId.SessionOwner().iUid, 
											  aSessionId.SessionNum());
	LBSLOG2(ELogP2, "Reason : %d", aReason);
	
	TNetGatewayMsg msg;
	SNetMsgCancelTransmitLocation* cancel;
	cancel = reinterpret_cast<SNetMsgCancelTransmitLocation*>(msg.Data());
	msg.iType = ENetMsgCancelTransmitLocation;
	cancel->iSessionId = aSessionId;
	cancel->iReason = aReason;

	TInt err = KErrNone;
	TRAP(err, iTestChannel.SendNetProtocolMessageL(msg);)
	if(err)
		User::Panic(KLbsTestNetworkProtocol, err);
	}

void CTestLbsNetworkProtocol::CancelExternalLocation(
									const TLbsNetSessionId& aSessionId,
									TInt aReason)
	{
	LBSLOG(ELogP2, "CTestLbsNetworkProtocol::CancelExternalLocation:");
	LBSLOG3(ELogP2, "SessionId : (%d, %d)\n", aSessionId.SessionOwner().iUid, 
											  aSessionId.SessionNum());
	LBSLOG2(ELogP2, "Reason : %d", aReason);
	
	TNetGatewayMsg msg;
	SNetMsgCancelExternalLocation* cancel;
	cancel = reinterpret_cast<SNetMsgCancelExternalLocation*>(msg.Data());
	msg.iType = ENetMsgCancelExternalLocation;
	cancel->iSessionId = aSessionId;
	cancel->iReason = aReason;

	TInt err = KErrNone;
	TRAP(err, iTestChannel.SendNetProtocolMessageL(msg);)
	if(err)
		User::Panic(KLbsTestNetworkProtocol, err);
	}

	
void CTestLbsNetworkProtocol::AdviceSystemStatus(TLbsSystemStatus /*aStatus*/)
	{
#pragma message("NOTE: CTestLbsNetworkProtocol::AdviceSystemStatus method not yet implemented. TBD")
	}

void CTestLbsNetworkProtocol::ProcessNetProtocolMessage(const TNetGatewayMsg& aMessage)
	{
	LBSLOG(ELogP1, "CTestLbsNetworkProtocol::ProcessNetProtocolMessage:");
	
	switch (aMessage.iType)
		{
		case ENetMsgProcessPrivacyRequest:
			{
			const SNetMsgProcessPrivacyRequest* request;
			request = reinterpret_cast<const SNetMsgProcessPrivacyRequest*>(aMessage.Data());
			iObserver.ProcessPrivacyRequest(request->iSessionId,
											request->iEmergency,
											request->iPrivacy,
											request->iRequestInfo);
			break;
			}
		case ENetMsgProcessLocationRequest:
			{
			const SNetMsgProcessLocationRequest* request;
			request = reinterpret_cast<const SNetMsgProcessLocationRequest*>(aMessage.Data());
			iObserver.ProcessLocationRequest(request->iSessionId,
											  request->iEmergency,
											  request->iService,
											  request->iQuality,
											  request->iMethod);
			break;
			}
		case ENetMsgProcessSessionComplete:
			{
			const SNetMsgProcessSessionComplete* complete;
			complete = reinterpret_cast<const SNetMsgProcessSessionComplete*>(aMessage.Data());
			iObserver.ProcessSessionComplete(complete->iSessionId,
											  complete->iReason);
			break;
			}
		case ENetMsgProcessAssistanceData:
			{
			const SNetMsgProcessAssistanceData* data;
			data = reinterpret_cast<const SNetMsgProcessAssistanceData*>(aMessage.Data());
			iObserver.ProcessAssistanceData(data->iDataMask,
											data->iData, 
											data->iReason);
			break;
			}
		case ENetMsgProcessLocationUpdate:
			{
			const SNetMsgProcessLocationUpdate* location;
			location = reinterpret_cast<const SNetMsgProcessLocationUpdate*>(aMessage.Data());
			iObserver.ProcessLocationUpdate(location->iSessionId, location->iReferenceLocation);
			break;
			}
		case ENetMsgGetCurrentCapabilitiesRequest:
			{
			TNetGatewayMsg msg;
			msg.iType = ENetMsgGetCurrentCapabilitiesResponse;
			SNetMsgGetCurrentCapabilitiesResponse* response;
			response = reinterpret_cast<SNetMsgGetCurrentCapabilitiesResponse*>(msg.Data());
			iObserver.GetCurrentCapabilities(response->iCapabilities);

			TInt err = KErrNone;
			TRAP(err, iTestChannel.SendNetProtocolMessageL(msg);)
			if(err)
				User::Panic(KLbsTestNetworkProtocol, err);
			break;
			}
		case ENetMsgProcessStatusUpdate:
			{
			const SNetMsgProcessStatusUpdate* status;
			status = reinterpret_cast<const SNetMsgProcessStatusUpdate*>(aMessage.Data());
			iObserver.ProcessStatusUpdate(status->iActiveServiceMask);
			break;
			}
		case ENetMsgRespondPrivacyRequest:
		case ENetMsgRespondLocationRequest:
		case ENetMsgRequestTransmitLocation:
		case ENetMsgCancelTransmitLocation:
		case ENetMsgCancelExternalLocation:
		case ENetMsgRequestAssistanceData:
		case ENetMsgRequestSelfLocation:
		case ENetMsgCancelSelfLocation:
		case ENetMsgRequestNetworkLocation:
		case ENetMsgCancelNetworkLocation:
		case ENetMsgGetCurrentCapabilitiesResponse:
			{
			// The message go from the the plugin to the test code,
			// so they should never be received here.
			break;
			}
		case ENetMsgShutdownNetGateway:
			{
			CActiveScheduler::Stop();
			break;
			}
		default:
			{
			LBSLOG_ERR2(ELogP3, "Unhandled message type ; %d\n", aMessage.iType);
			break;
			}
		}
	}


