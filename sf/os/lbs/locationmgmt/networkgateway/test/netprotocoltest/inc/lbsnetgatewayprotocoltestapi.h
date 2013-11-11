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
// Header file for the api for talking to the stub protocol module.
// 
//

#ifndef LBSNETGATEWAYPROTOCOLTESTAPI_H_
#define LBSNETGATEWAYPROTOCOLTESTAPI_H_

#include <e32property.h>
#include <lbs/lbslocdatasourcegpsbase.h>
#include <lbs/lbsassistancedatabuilderset.h>
#include <lbs/lbsnetcommon.h>
#include <lbs/lbsnetprotocolbase.h>
#include <lbs/lbsextendedsatellite.h>

const TUint KTe_LbsNetGatewayTestProtocolIndex = 0;
const TUid KTe_LbsNetGatewayTestProtocolUid = { 0x10282250 };
const TUint KTe_LbsNetGatewayTestProtocolIndex2 = 1;
const TUid KTe_LbsNetGatewayTestProtocolUid2 = { 0x10285AB9 };

const TUint KTe_LbsNetGatewayTestProtocolIndex3 = 2;
const TUid KTe_LbsNetGatewayTestProtocolUid3 = { 0x102871C8 }; 

const TUint KTe_LbsNetGatewayTestProtocolIndex4 = 3;
const TUid KTe_LbsNetGatewayTestProtocolUid4 = { 0x102871C9 }; 

const TUint KTe_LbsNetGatewayTestProtocolIndex5 = 4;
const TUid KTe_LbsNetGatewayTestProtocolUid5 = { 0x102871CA }; 

const TUint KTe_LbsNetGatewayTestProtocolIndex6 = 5;
const TUid KTe_LbsNetGatewayTestProtocolUid6 = { 0x102871CB }; 

const TUint KTe_LbsNetGatewayTestProtocolIndex7 = 6;
const TUid KTe_LbsNetGatewayTestProtocolUid7 = { 0x102871CC }; 

const TUint KTe_LbsNetGatewayTestProtocolIndex8 = 7;
const TUid KTe_LbsNetGatewayTestProtocolUid8 = { 0x102871CD }; 

const TUint KTe_LbsNetGatewayTestProtocolIndex9 = 8;
const TUid KTe_LbsNetGatewayTestProtocolUid9 = { 0x102871CE }; 

const TUint KTe_LbsNetGatewayTestProtocolIndex10 = 9;
const TUid KTe_LbsNetGatewayTestProtocolUid10 = { 0x102871CF }; 

const TUint KTe_LbsNetGatewayTestProtocolIndex11 = 10;
const TUid KTe_LbsNetGatewayTestProtocolUid11 = { 0x102871D0 }; 
		
//10 PM uids for roaming modules
const TUint KTe_LbsNetGatewayTestProtocolIndex12 = 11;
const TUid KTe_LbsNetGatewayTestProtocolUid12 = { 0x102871D1 }; 

const TUint KTe_LbsNetGatewayTestProtocolIndex13 = 12;
const TUid KTe_LbsNetGatewayTestProtocolUid13 = { 0x102871D2 }; 

const TUint KTe_LbsNetGatewayTestProtocolIndex14 = 13;
const TUid KTe_LbsNetGatewayTestProtocolUid14 = { 0x102871D3 }; 

const TUint KTe_LbsNetGatewayTestProtocolIndex15 = 14;
const TUid KTe_LbsNetGatewayTestProtocolUid15 = { 0x102871D4 }; 

const TUint KTe_LbsNetGatewayTestProtocolIndex16 = 15;
const TUid KTe_LbsNetGatewayTestProtocolUid16 = { 0x102871D5 }; 

const TUint KTe_LbsNetGatewayTestProtocolIndex17 = 16;
const TUid KTe_LbsNetGatewayTestProtocolUid17 = { 0x102871D6 }; 

const TUint KTe_LbsNetGatewayTestProtocolIndex18 = 17;
const TUid KTe_LbsNetGatewayTestProtocolUid18 = { 0x102871D7 }; 

const TUint KTe_LbsNetGatewayTestProtocolIndex19 = 18;
const TUid KTe_LbsNetGatewayTestProtocolUid19 = { 0x102871D8 }; 

const TUint KTe_LbsNetGatewayTestProtocolIndex20 = 19;
const TUid KTe_LbsNetGatewayTestProtocolUid20 = { 0x102871D9 }; 



//
// Base Message Type
//

enum TNetGatewayMsgType
	{
	ENetMsgUnknown = 0,
	
	// Messages coming from test code to PM.
	ENetMsgProcessPrivacyRequest = 1000,
	ENetMsgProcessLocationRequest,
	ENetMsgProcessSessionComplete,
	ENetMsgProcessAssistanceData,
	ENetMsgProcessLocationUpdate,
	ENetMsgGetCurrentCapabilitiesRequest,
	ENetMsgProcessStatusUpdate,
	
	// Messages coming from PM to test code.
	ENetMsgRespondPrivacyRequest = 2000,
	ENetMsgRespondLocationRequest,
	ENetMsgRequestTransmitLocation,
	ENetMsgCancelTransmitLocation,
	ENetMsgRequestAssistanceData,
	ENetMsgRequestSelfLocation,
	ENetMsgCancelSelfLocation,
	ENetMsgRequestNetworkLocation,
	ENetMsgCancelNetworkLocation,
	ENetMsgGetCurrentCapabilitiesResponse,

	// Special test message to force the NetGateway to shutdown
	ENetMsgShutdownNetGateway,
	
	ENetMsgCancelExternalLocation=2015
	};
	

/* Base message class used to pass data.
*/
class TNetGatewayMsg
	{
public:
	TNetGatewayMsg() : iType(ENetMsgUnknown) {}
	
	TAny* Data() { return reinterpret_cast<TAny*>(iBuffer); }
	const TAny* Data() const { return reinterpret_cast<const TAny*>(iBuffer); }
	
public:
	TNetGatewayMsgType iType;
	
private:
	TUint32 iBuffer[1024 - sizeof(TNetGatewayMsgType)];
	};

//
// Channel interface
//

/* Definition of keys use for each property
*/
const TUint KChannelAckOffset = 0x800;
const TUint KNetProtocolChannelKey = 0x10001000;
const TUint KNetProtocolChannelAckKey = (KNetProtocolChannelKey + KChannelAckOffset);
const TUint KTestAppChannelKey = 0x10002000;
const TUint KTestAppChannelAckKey = (KTestAppChannelKey + KChannelAckOffset);
const TUint KAgpsDataSetKey = 0x10003000;
	
class CNetProtocolMessageListener;
class CNetProtocolMessageTransmitter;

/* Obsever for messages arriving.
*/
class MNetGatewayProtocolTestObserver
	{
public:
	virtual void ProcessNetProtocolMessage(const TNetGatewayMsg& aMessage) = 0;
	};
	
/* Interface to the channel.
*/
class RNetGatewayProtocolTestChannel
	{
public:
	IMPORT_C RNetGatewayProtocolTestChannel(TUint aModuleIndex);
	IMPORT_C RNetGatewayProtocolTestChannel();
	
	IMPORT_C static void InitialiseL(TUint aModuleIndex);
	IMPORT_C static void Shutdown(TUint aModuleIndex);
	
	IMPORT_C void OpenL(TUint aTxKey, TUint aRxKey, MNetGatewayProtocolTestObserver& aRxObserver);
	IMPORT_C void OpenL(TUint aTxKey, TUint aRxKey, MNetGatewayProtocolTestObserver& aRxObserver, TUint aModuleIndex);
	IMPORT_C void Close();
	
	IMPORT_C void SendNetProtocolMessageL(const TNetGatewayMsg& aMessage);

private:
	CNetProtocolMessageListener* iListener;
	CNetProtocolMessageTransmitter* iTransmitter;
	TUint iModuleIndex; 
	};
	
//
// Message Data Types
//

struct SNetMsgProcessPrivacyRequest
	{
	TLbsNetSessionId iSessionId;
	TBool iEmergency;
	TLbsNetPosRequestPrivacy iPrivacy;
	TLbsExternalRequestInfo iRequestInfo;
	};
	
struct SNetMsgProcessLocationRequest
	{
	TLbsNetSessionId iSessionId;
	TBool iEmergency;
	MLbsNetworkProtocolObserver::TLbsNetProtocolService iService;
	TLbsNetPosRequestQuality iQuality;
	TLbsNetPosRequestMethod iMethod;
	};
	
struct SNetMsgProcessSessionComplete
	{
	TLbsNetSessionId iSessionId;
	TInt iReason;
	};

struct SNetMsgProcessAssistanceData
	{
	TLbsAssistanceDataGroup iDataMask;
	TInt iReason;
	RLbsAssistanceDataBuilderSet iData;
	};
	
struct SNetMsgProcessLocationUpdate
	{
	TLbsNetSessionId iSessionId;
	TPositionExtendedSatelliteInfo iReferenceLocation;
	};

struct SNetMsgRespondPrivacyRequest
	{
	TLbsNetSessionId iSessionId;
	CLbsNetworkProtocolBase::TLbsPrivacyResponse iResponse;
	};
	
struct SNetMsgRespondLocationRequest
	{
	TLbsNetSessionId iSessionId;
	TInt iReason;
	TPositionExtendedSatelliteInfo iPositionInfo;
	};
	
struct SNetMsgRequestTransmitLocation
	{
	TLbsNetSessionId iSessionId;
	TInt iPriority;
	TBuf<128> iDestination;
	TLbsNetPosRequestOptionsTechnology iOptions;
	};
	
struct SNetMsgCancelTransmitLocation
	{
	TLbsNetSessionId iSessionId;
	TInt iReason;
	};

struct SNetMsgCancelExternalLocation
	{
	TLbsNetSessionId iSessionId;
	TInt iReason;
	};
	
struct SNetMsgRequestAssistanceData
	{
	TLbsAssistanceDataGroup iDataRequestMask;	
	};
	
struct SNetMsgRequestSelfLocation
	{
	TLbsNetSessionId iSessionId;
	TBool iNewClient;	
	TLbsAssistanceDataGroup iDataRequestMask;	
	};
	
struct SNetMsgCancelSelfLocation
	{
	TLbsNetSessionId iSessionId;
	TInt iReason;
	};
	
struct SNetMsgRequestNetworkLocation
	{
	TLbsNetSessionId iSessionId;
	TLbsNetPosRequestQuality iQuality;
	};
	
struct SNetMsgCancelNetworkLocation
	{
	TLbsNetSessionId iSessionId;
	TInt iReason;
	};

struct SNetMsgGetCurrentCapabilitiesRequest
	{
	};

struct SNetMsgGetCurrentCapabilitiesResponse
	{
	TLbsNetPosCapabilities iCapabilities;
	};
	
struct SNetMsgProcessStatusUpdate
	{
	MLbsNetworkProtocolObserver::TLbsNetProtocolServiceMask iActiveServiceMask;
	};
	
#endif // LBSNETGATEWAYPROTOCOLTESTAPI_H_
