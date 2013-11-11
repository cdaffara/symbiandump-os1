/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies). 
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

#include "T_DataCommsBase.h"

//	Epoc includes
#include <in_sock.h>
#include <es_sock.h>
#include <es_enum.h>
#include <bt_sock.h>
#include <cs_subconevents.h>

/*@{*/
///	Constant Literals used.
_LIT(KInterfaceDownBuf,							"EInterfaceDown");

// TConnectionType
_LIT(KConnectionCSD ,							"EConnectionCSD ");
_LIT(KConnectionGPRS,							"EConnectionGPRS");
_LIT(KConnectionGPRSR97,						"EConnectionGPRSR97");
_LIT(KConnectionGPRSR99,						"EConnectionGPRSR99");
_LIT(KConnectionGPRSRel4,						"EConnectionGPRSRel4");
_LIT(KConnectionGPRSRel5,						"EConnectionGPRSRel5");
//_LIT(KConnectionCDMA,							"EConnectionCDMA");
//_LIT(KConnectionCDMA20001xRTT,					"EConnectionCDMA20001xRTT");
//_LIT(KConnectionCDMA20001xRTTDO,				"EConnectionCDMA20001xRTTDO ");
//_LIT(KConnectionCDMA20001xRTTDV,				"EConnectionCDMA20001xRTTDV");
//_LIT(KConnectionCDMA20003xRTT,					"EConnectionCDMA20003xRTT");
_LIT(KConnectionEthernet,						"EConnectionEthernet");
_LIT(KConnectionWLAN,							"EConnectionWLAN");
_LIT(KConnectionBTPAN,							"EConnectionBTPAN");

// TConnStopType
_LIT(KStopAuthoritative,						"EStopAuthoritative");

// TConnAttachType
_LIT(KAttachTypeMonitor,						"EAttachTypeMonitor");

// SockType
_LIT(KSockStreamStr,							"KSockStream");
_LIT(KSockDatagramStr,							"KSockDatagram");

// Protocol
_LIT(KProtocolInetTcpStr,						"KProtocolInetTcp");
_LIT(KProtocolInetUdpStr,						"KProtocolInetUdp");

// AddrFamily
_LIT(KAFUnspecStr,								"KAFUnspec");
_LIT(KAfInetStr,								"KAfInet");
_LIT(KAfInet6Str,								"KAfInet6");

// Capability
_LIT(ECapabilityTCBStr,							"ECapabilityTCB");
_LIT(ECapabilityCommDDStr,						"ECapabilityCommDD");
_LIT(ECapabilityPowerMgmtStr,					"ECapabilityPowerMgmt");
_LIT(ECapabilityMultimediaDDStr,				"ECapabilityMultimediaDD");
_LIT(ECapabilityReadDeviceDataStr,				"ECapabilityReadDeviceData");
_LIT(ECapabilityWriteDeviceDataStr,				"ECapabilityWriteDeviceData");
_LIT(ECapabilityDRMStr,							"ECapabilityDRM");
_LIT(ECapabilityTrustedUIStr,					"ECapabilityTrustedUI");
_LIT(ECapabilityProtServStr,					"ECapabilityProtServ");
_LIT(ECapabilityDiskAdminStr,					"ECapabilityDiskAdmin");
_LIT(ECapabilityNetworkControlStr,				"ECapabilityNetworkControl");
_LIT(ECapabilityAllFilesStr,					"ECapabilityAllFiles");
_LIT(ECapabilitySwEventStr,						"ECapabilitySwEvent");
_LIT(ECapabilityNetworkServicesStr,				"ECapabilityNetworkServices");
_LIT(ECapabilityLocalServicesStr,				"ECapabilityLocalServices");
_LIT(ECapabilityReadUserDataStr,				"ECapabilityReadUserData");
_LIT(ECapabilityWriteUserDataStr,				"ECapabilityWriteUserData");
_LIT(ECapabilityLocationStr,					"ECapabilityLocation");
_LIT(ECapabilitySurroundingsDDStr,				"ECapabilitySurroundingsDD");
_LIT(ECapabilityUserEnvironmentStr,				"ECapabilityUserEnvironment");
_LIT(ECapability_LimitStr,						"ECapability_Limit");
_LIT(ECapability_HardLimitStr,					"ECapability_HardLimit");
_LIT(ECapability_NoneStr,						"ECapability_None");
_LIT(ECapability_DeniedStr,						"ECapability_Denied");

// ProgressNotification stages
_LIT(KStartingSelectionStr,						"KStartingSelection");
_LIT(KFinishedSelectionStr,						"KFinishedSelection");
_LIT(KConnectionOpenStr,						"KConnectionOpen");
_LIT(KLinkLayerOpenStr,							"KLinkLayerOpen");
_LIT(KLinkLayerClosedStr,						"KLinkLayerClosed");
_LIT(KNonExistentStr,							"KNonExistent");

// TSubConnType
_LIT(KCreateNew,								"ECreateNew");
_LIT(KAttachToDefault,							"EAttachToDefault");

// EventGroupUid
_LIT(KSubConnGenericEventsImplUidStr,			"KSubConnGenericEventsImplUid");

// EventNotification events
_LIT(KSubConGenericEventParamsGrantedStr,		"KSubConGenericEventParamsGranted");
_LIT(KSubConGenericEventDataClientJoinedStr,	"KSubConGenericEventDataClientJoined");
_LIT(KSubConGenericEventDataClientLeftStr,		"KSubConGenericEventDataClientLeft");
_LIT(KSubConGenericEventSubConDownStr,			"KSubConGenericEventSubConDown");
_LIT(KSubConGenericEventParamsChangedStr,		"KSubConGenericEventParamsChanged");
_LIT(KSubConGenericEventParamsRejectedStr,		"KSubConGenericEventParamsRejected");

// ParameterSetType

_LIT(KAcceptable,								"EAcceptable");
_LIT(KRequested,								"ERequested");
_LIT(KGranted,									"EGranted");
/*@}*/


CT_DataCommsBase::CT_DataCommsBase()
	{
	}

CT_DataCommsBase::~CT_DataCommsBase()
	{
	}

void CT_DataCommsBase::RunL( CActive* aActive, TInt /*aIndex*/ )
	{
	ERR_PRINTF2(_L("RunL Stray signal, status = %d"), aActive->iStatus.Int());
	SetBlockResult(EFail);
	}

void CT_DataCommsBase::DoCancel( CActive* aActive, TInt /*aIndex*/ )
	{
	ERR_PRINTF2(_L("DoCancel Stray signal, status = %d"), aActive->iStatus.Int());
	SetBlockResult(EFail);
	}

void CT_DataCommsBase::MissingParamError( const TDesC& aParamName )
	{
	ERR_PRINTF2(_L("No %S"), &aParamName);
	SetBlockResult(EFail);
	}

TConnInterfaceState	CT_DataCommsBase::InterfaceNotificationState( const TDesC& aState )
	{
	TConnInterfaceState state = EInterfaceUp;
	if( aState == KInterfaceDownBuf() )
		{
		state = EInterfaceDown;
		}
	return state;
	}

TConnectionType CT_DataCommsBase::ConnectionType( const TDesC& aConnectionTypeName )
	{
	TConnectionType	connType = EConnectionGeneric;
	if( aConnectionTypeName == KConnectionCSD() )
		{
		connType = EConnectionCSD;
		}
	else if( aConnectionTypeName == KConnectionGPRS() )
		{
		connType = EConnectionGPRS;
		}
	else if( aConnectionTypeName == KConnectionGPRSR97() )
		{
		connType = EConnectionGPRSR97;
		}
	else if( aConnectionTypeName == KConnectionGPRSR99() )
		{
		connType = EConnectionGPRSR99;
		}
	else if( aConnectionTypeName == KConnectionGPRSRel4() )
		{
		connType = EConnectionGPRSRel4;
		}
	else if( aConnectionTypeName == KConnectionGPRSRel5() )
		{
		connType = EConnectionGPRSRel5;
		}
/*	else if( aConnectionTypeName == KConnectionCDMA() )
		{
		connType = EConnectionCDMA;
		}
	else if( aConnectionTypeName == KConnectionCDMA20001xRTT() )
		{
		connType = EConnectionCDMA20001xRTT;
		}
	else if( aConnectionTypeName == KConnectionCDMA20001xRTTDO() )
		{
		connType = EConnectionCDMA20001xRTTDO ;
		}
	else if( aConnectionTypeName == KConnectionCDMA20001xRTTDV() )
		{
		connType = EConnectionCDMA20001xRTTDV;
		}
	else if( aConnectionTypeName == KConnectionCDMA20003xRTT() )
		{
		connType = EConnectionCDMA20003xRTT;
		}*/
	else if( aConnectionTypeName == KConnectionEthernet() )
		{
		connType = EConnectionEthernet;
		}
	else if( aConnectionTypeName == KConnectionWLAN() )
		{
		connType = EConnectionWLAN;
		}
	else if( aConnectionTypeName == KConnectionBTPAN() )
		{
		connType = EConnectionBTPAN;
		}
	return connType;
	}

RConnection::TConnStopType CT_DataCommsBase::ConnectionStopType( const TDesC& aConnectionStopType )
	{
	RConnection::TConnStopType	stopType = RConnection::EStopNormal;
	if( aConnectionStopType == KStopAuthoritative() )
		{
		stopType = RConnection::EStopAuthoritative;
		}
	return stopType;
	}

RConnection::TConnAttachType CT_DataCommsBase::ConnectionAttachType( const TDesC& aConnectionAttachType )
	{
	RConnection::TConnAttachType	connAttachType = RConnection::EAttachTypeNormal;
	if( aConnectionAttachType == KAttachTypeMonitor() )
		{
		connAttachType = RConnection::EAttachTypeMonitor;
		}
	return connAttachType;
	}

TUint CT_DataCommsBase::ProgressNotificationExpectedStage( const TDesC& aStage)
	{
	TUint expectedStage = 0;
	if ( aStage == KStartingSelectionStr() )
		{
		expectedStage = KStartingSelection;
		}
	else if ( aStage == KFinishedSelectionStr() )
		{
		expectedStage = KFinishedSelection;
		}
	else if ( aStage == KConnectionOpenStr() )
		{
		expectedStage = KConnectionOpen;
		}
	else if ( aStage == KLinkLayerOpenStr() )
		{
		expectedStage = KLinkLayerOpen;
		}
	else if ( aStage == KLinkLayerClosedStr() )
		{
		expectedStage = KLinkLayerClosed;
		}
	else if ( aStage == KNonExistentStr() )
		{
		expectedStage = 1234;
		}
	return expectedStage;
	}

TBool CT_DataCommsBase::GetSockType( const TDesC& aSockTypeStr, TUint& aSockType )
	{
	TBool found = ETrue;

	if( aSockTypeStr == KSockStreamStr() )
		{
		aSockType = KSockStream;
		}
	else if( aSockTypeStr == KSockDatagramStr() )
		{
		aSockType = KSockDatagram;
		}
	else
		{
		found = EFalse;
		}

	return found;
	}

TBool CT_DataCommsBase::GetProtocol( const TDesC& aProtocolStr, TUint& aProtocol )
	{
	TBool found = ETrue;

	if( aProtocolStr == KProtocolInetTcpStr() )
		{
		aProtocol = KProtocolInetTcp;
		}
	else if( aProtocolStr == KProtocolInetUdpStr() )
		{
		aProtocol = KProtocolInetUdp;
		}
	else
		{
		found = EFalse;
		}

	return found;
	}

TBool CT_DataCommsBase::GetAddrFamily( const TDesC& aAddrFamilyStr, TUint& aAddrFamily )
	{
	TBool found = ETrue;

	if( aAddrFamilyStr == KAFUnspecStr() )
		{
		aAddrFamily = KAFUnspec;
		}
	else if( aAddrFamilyStr == KAfInetStr() )
		{
		aAddrFamily = KAfInet;
		}
	else if( aAddrFamilyStr == KAfInet6Str() )
		{
		aAddrFamily = KAfInet6;
		}
	else
		{
		found = EFalse;
		}

	return found;
	}

TBool CT_DataCommsBase::GetCapability( const TDesC& aCapabilityStr, TCapability& aCapability )
	{
	TBool found = ETrue;

	if( aCapabilityStr == ECapabilityTCBStr() )
		{
		aCapability = ECapabilityTCB;
		}
	else if( aCapabilityStr == ECapabilityCommDDStr() )
		{
		aCapability = ECapabilityCommDD;
		}
	else if( aCapabilityStr == ECapabilityPowerMgmtStr() )
		{
		aCapability = ECapabilityPowerMgmt;
		}
	else if( aCapabilityStr == ECapabilityMultimediaDDStr() )
		{
		aCapability = ECapabilityMultimediaDD;
		}
	else if( aCapabilityStr == ECapabilityReadDeviceDataStr() )
		{
		aCapability = ECapabilityReadDeviceData;
		}
	else if( aCapabilityStr == ECapabilityWriteDeviceDataStr() )
		{
		aCapability = ECapabilityWriteDeviceData;
		}
	else if( aCapabilityStr == ECapabilityDRMStr() )
		{
		aCapability = ECapabilityDRM;
		}
	else if( aCapabilityStr == ECapabilityTrustedUIStr() )
		{
		aCapability = ECapabilityTrustedUI;
		}
	else if( aCapabilityStr == ECapabilityProtServStr() )
		{
		aCapability = ECapabilityProtServ;
		}
	else if( aCapabilityStr == ECapabilityDiskAdminStr() )
		{
		aCapability = ECapabilityDiskAdmin;
		}
	else if( aCapabilityStr == ECapabilityNetworkControlStr() )
		{
		aCapability = ECapabilityNetworkControl;
		}
	else if( aCapabilityStr == ECapabilityAllFilesStr() )
		{
		aCapability = ECapabilityAllFiles;
		}
	else if( aCapabilityStr == ECapabilitySwEventStr() )
		{
		aCapability = ECapabilitySwEvent;
		}
	else if( aCapabilityStr == ECapabilityNetworkServicesStr() )
		{
		aCapability = ECapabilityNetworkServices;
		}
	else if( aCapabilityStr == ECapabilityLocalServicesStr() )
		{
		aCapability = ECapabilityLocalServices;
		}
	else if( aCapabilityStr == ECapabilityReadUserDataStr() )
		{
		aCapability = ECapabilityReadUserData;
		}
	else if( aCapabilityStr == ECapabilityWriteUserDataStr() )
		{
		aCapability = ECapabilityWriteUserData;
		}
	else if( aCapabilityStr == ECapabilityLocationStr() )
		{
		aCapability = ECapabilityLocation;
		}
	else if( aCapabilityStr == ECapabilitySurroundingsDDStr() )
		{
		aCapability = ECapabilitySurroundingsDD;
		}
	else if( aCapabilityStr == ECapabilityUserEnvironmentStr() )
		{
		aCapability = ECapabilityUserEnvironment;
		}
	else if( aCapabilityStr == ECapability_LimitStr() )
		{
		aCapability = ECapability_Limit;
		}
	else if( aCapabilityStr == ECapability_HardLimitStr() )
		{
		aCapability = ECapability_HardLimit;
		}
	else if( aCapabilityStr == ECapability_NoneStr() )
		{
		aCapability = ECapability_None;
		}
	else if( aCapabilityStr == ECapability_DeniedStr() )
		{
		aCapability = ECapability_Denied;
		}
	else
		{
		found = EFalse;
		}

	return found;
	}

TBool CT_DataCommsBase::SubConnectionType( const TDesC& aSubConnTypeStr, RSubConnection::TSubConnType& aSubConnType)
	{
	TBool found = ETrue;

	if( aSubConnTypeStr == KCreateNew() )
		{
		aSubConnType = RSubConnection::ECreateNew;
		}
	else if( aSubConnTypeStr == KAttachToDefault() )
		{
		aSubConnType = RSubConnection::EAttachToDefault;
		}
	else
		{
		found = EFalse;
		}

	return found;
	}

TBool CT_DataCommsBase::GetEventGroupUid(const TDesC& aEventGroupUidStr, TInt32& aEventGroupUid)
	{
	TBool found = ETrue;

	if (aEventGroupUidStr == KSubConnGenericEventsImplUidStr())
		{
		aEventGroupUid = KSubConnGenericEventsImplUid;
		}
	else
		{
		found = EFalse;
		}

	return found;
	}


TBool CT_DataCommsBase::GetEventMaskBits(const TDesC& aConstantName, TUint32& aEventMask)
	{
	TBool	ret = ETrue;

	if (aConstantName == KSubConGenericEventParamsGrantedStr())
		{
		aEventMask=KSubConGenericEventParamsGranted;
		}
	else if (aConstantName == KSubConGenericEventDataClientJoinedStr())
		{
		aEventMask=KSubConGenericEventDataClientJoined;
		}
	else if (aConstantName == KSubConGenericEventDataClientLeftStr())
		{
		aEventMask=KSubConGenericEventDataClientLeft;
		}
	else if (aConstantName == KSubConGenericEventSubConDownStr())
		{
		aEventMask=KSubConGenericEventSubConDown;
		}
	else if (aConstantName == KSubConGenericEventParamsChangedStr())
		{
		aEventMask=KSubConGenericEventParamsChanged;
		}
	else if (aConstantName == KSubConGenericEventParamsRejectedStr())
		{
		aEventMask=KSubConGenericEventParamsRejected;
		}
	else
		{
		TInt	location=aConstantName.Match(_L("*|*"));
		if( location!=KErrNotFound )
			{
			//Converting Left part of the data
			TPtrC	tempStr=aConstantName.Left(location);
			ret=GetEventMaskBits(tempStr, aEventMask);

			//Converting right data can be with another "|"
			tempStr.Set(aConstantName.Mid(location+1));

			TUint32	tmp;
			if ( GetEventMaskBits(tempStr, tmp) )
				{
				aEventMask |= tmp;
				}
			else
				{
				ret = EFalse;
				}
			}
		else
			{
			TInt	flagInt;
			TLex	lex(aConstantName);
			if (lex.Val(flagInt)==KErrNone)
				{
				aEventMask=flagInt;
				}
			else
				{
				ret=EFalse;
				ERR_PRINTF2(_L("Given flag (%S) is invalid"), &aConstantName);
				SetBlockResult(EFail);
				}
			}
		}

	return ret;
	}

TBool CT_DataCommsBase::ParameterSetType( const TDesC& aParameterSetTypeStr, CSubConParameterFamily::TParameterSetType& aParameterSetType )
	{
	TBool found = ETrue;

	if( aParameterSetTypeStr == KAcceptable() )
		{
		aParameterSetType = CSubConParameterFamily::EAcceptable;
		}
	else if( aParameterSetTypeStr == KRequested() )
		{
		aParameterSetType = CSubConParameterFamily::ERequested;
		}
	else if( aParameterSetTypeStr == KGranted() )
		{
		aParameterSetType = CSubConParameterFamily::EGranted;
		}
	else
		{
		found = EFalse;
		}

	return found;
	}

TUint32 CT_DataCommsBase::GetLowPowerMode(const TDesC& aConstantName)
	{
	_LIT(KEActiveMode,							"EActiveMode");
	_LIT(KEHoldMode,							"EHoldMode");
	_LIT(KESniffMode,							"ESniffMode");
	_LIT(KEParkMode,							"EParkMode");
	_LIT(KEScatterMode,							"EScatterMode");
	//_LIT(KEAnyLowPowerMode,						"EAnyLowPowerMode");
	
	TUint32 lowPowerMode = EAnyLowPowerMode;
	
	if (aConstantName == KEActiveMode)
		{
		lowPowerMode = EActiveMode;
		}
	else if (aConstantName == KEHoldMode)
		{
		lowPowerMode = EHoldMode;
		}
	else if (aConstantName == KESniffMode)
		{
		lowPowerMode = ESniffMode;
		}
	else if (aConstantName == KEParkMode)
		{
		lowPowerMode = EParkMode;
		}
	else if (aConstantName == KEScatterMode)
		{
		lowPowerMode = EScatterMode;
		}
	else
		{
		lowPowerMode = EAnyLowPowerMode;
		}
		
	return lowPowerMode;
	}

TBool CT_DataCommsBase::GetTShutdown(const TDesC& aConstantName, RSocket::TShutdown& aShutdown)
	{
	_LIT(KNormal,								"ENormal");
	_LIT(KStopInput,							"EStopInput");
	_LIT(KStopOutput,							"EStopOutput");
	_LIT(KImmediate,							"EImmediate");

	TBool	ret=ETrue;

	if (aConstantName == KNormal)
		{
		// Complete when socket output/input stopped.
		aShutdown=RSocket::ENormal;
		}
	else if (aConstantName == KStopInput)
		{
		// Stop socket input and complete when input is stopped.
		aShutdown=RSocket::EStopInput;
		}
	else if (aConstantName == KStopOutput)
		{
		// Stop socket output and complete when output is stopped.
		aShutdown=RSocket::EStopOutput;
		}
	else if (aConstantName == KImmediate)
		{
		// Stop socket input/output and complete (abortive close).
		aShutdown=RSocket::EImmediate;
		}
	else
		{
		// If we did not find the value go for default
		// Complete when socket output/input stopped.
		ret = EFalse;
		aShutdown=RSocket::ENormal;
		}

	return ret;
	}

TInt CT_DataCommsBase::GetTBTPacketType(const TDesC& aConstantName)
	{
	_LIT(KEPacketsDM1,							"EPacketsDM1");
	_LIT(KEPacketsDH1,							"EPacketsDH1");
	_LIT(KEPacketsDM3,							"EPacketsDM1");
	_LIT(KEPacketsDH3,							"EPacketsDH1");
	_LIT(KEPacketsDM5,							"EPacketsDM1");
	_LIT(KEPacketsDH5,							"EPacketsDH1");
	_LIT(KEPacketsHV1,							"EPacketsHV1");
	_LIT(KEPacketsHV2,							"EPacketsHV2");
	_LIT(KEPacketsHV3,							"EPacketsHV3");
	_LIT(KEAnyACLPacket,						"EAnyACLPacket");
	_LIT(KEAnySCOPacket,						"EAnySCOPacket");

	TInt packetType = EAnyPacket;
	
	//Description
	//Bluetooth SIG specified values for indicating packet types.
	//
	//DM1, DH1, DM3, DH3, DM5, DH5 are ACL packet types
	//HV1, HV2, HV3 are SCO (synchronous link) packet types
	//
	//

	//EPacketsDM1
	if (aConstantName == KEPacketsDM1)
		{
		packetType = EPacketsDM1;
		}
	//DH1 ACL packet type
	else if (aConstantName == KEPacketsDH1)
		{
		packetType = EPacketsDH1;
		}
	//EPacketsDM3
	else if (aConstantName == KEPacketsDM3)
		{
		packetType = EPacketsDM3;
		}
	//DH3 ACL packet type
	else if (aConstantName == KEPacketsDH3)
		{
		packetType = EPacketsDH3;
		}
	//EPacketsDM5
	else if (aConstantName == KEPacketsDM5)
		{
		packetType = EPacketsDM5;
		}
	//DH5 ACL packet type
	else if (aConstantName == KEPacketsDH5)
		{
		packetType = EPacketsDH5;
		}
	//HV1 SCO packet type
	else if (aConstantName == KEPacketsHV1)
		{
		packetType = EPacketsHV1;
		}
	//HV2 SCO packet type
 	else if (aConstantName == KEPacketsHV2)
		{
		packetType = EPacketsHV2;
		}
	//HV3 SCO packet type
 	else if (aConstantName == KEPacketsHV3)
		{
		packetType = EPacketsHV3;
		}

	//TBTPacketTypeCombinations
	//Description
	//Bitmask values to help request combinations of packets.

	//Any ACL packet
  	else if (aConstantName == KEAnyACLPacket)
		{
		packetType = EAnyACLPacket;
		}
	//Any SCO packet
  	else if (aConstantName == KEAnySCOPacket)
		{
		packetType = EAnySCOPacket;
		}
	//Any packet
	else
		{
		packetType = EAnyPacket;
		}
		
	return packetType;
	}


TUint32  CT_DataCommsBase::GetLinkStateNotifierBits(const TDesC& aConstantName)
	{
	_LIT(KENotifyMaster,						"ENotifyMaster");
	_LIT(KENotifySlave,							"ENotifySlave");

	_LIT(KENotifyActiveMode,					"ENotifyActiveMode");
	_LIT(KENotifySniffMode,						"ENotifySniffMode");
	_LIT(KENotifyParkMode,						"ENotifyParkMode");
	_LIT(KENotifyHoldMode,						"ENotifyHoldMode");

	_LIT(KENotifyMaxSlots1,						"ENotifyMaxSlots1");
	_LIT(KENotifyMaxSlots3,						"ENotifyMaxSlots3");
	_LIT(KENotifyMaxSlots5,						"ENotifyMaxSlots5");

	_LIT(KENotifyPacketsDM1,					"ENotifyPacketsDM1");
	_LIT(KENotifyPacketsDH1,					"ENotifyPacketsDH1");
	_LIT(KENotifyPacketsDM3,					"ENotifyPacketsDM3");
	_LIT(KENotifyPacketsDH3,					"ENotifyPacketsDH3");
	_LIT(KENotifyPacketsDM5,					"ENotifyPacketsDM5");
	_LIT(KENotifyPacketsDH5,					"ENotifyPacketsDH5");

	_LIT(KENotifyPacketsHV1,					"ENotifyPacketsHV1");
	_LIT(KENotifyPacketsHV2,					"ENotifyPacketsHV2");
	_LIT(KENotifyPacketsHV3,					"ENotifyPacketsHV3");

	_LIT(KENotifyAuthenticationComplete,		"ENotifyAuthenticationComplete");
	_LIT(KENotifyEncryptionChangeOn,			"ENotifyEncryptionChangeOn");
	_LIT(KENotifyEncryptionChangeOff,			"ENotifyEncryptionChangeOff");


	_LIT(KENotifyPhysicalLinkUp,				"ENotifyPhysicalLinkUp");
	_LIT(KENotifyPhysicalLinkDown,				"ENotifyPhysicalLinkDown");
	_LIT(KENotifyPhysicalLinkError,				"ENotifyPhysicalLinkError");

	_LIT(KENotifySCOLinkUp,						"ENotifySCOLinkUp");
	_LIT(KENotifySCOLinkDown,					"ENotifySCOLinkDown");
	_LIT(KENotifySCOLinkError,					"ENotifySCOLinkError");
	
	TUint32 linkStateNotifierBits = 0;

	// This is from BTTypes.h

	// Bitmask values for notifying/requesting notification of the state of a physical link.
	//
	// These values have a two fold purpose:
	// 1) They can be used along with TBTPhysicalLinkStateNotifierCombinations to specify
	// a set of baseband change events for which notification is desired.
	// 2) They can be used to notify the state of a physical link or a subset of that state.
	// For example if only mode change events are being monitored, then at each mode change event
	// only the bit reperesenting the new mode will be returned, however if the whole link state
	// is required, then a combination of bit values indicating that state will be returned.
	//
	// THESE BIT VALUES MUST ONLY BE USED WHEN REQUESTING NOTIFICATION, OR WHEN PARSING THE
	// RESULTS OF A NOTIFICATION. All other baseband state activities should use one of the
	// appropriate Bluetooth SIG specified enums
	// TBTBasebandRole, TBTLinkMode, TBTLinkModeCombinations, TBTPacketType,
	// TBTPacketTypeCombinations
	//

	//Master
	if (aConstantName == KENotifyMaster)
		{
		linkStateNotifierBits = ENotifyMaster;
		}
	//Slave
	else if (aConstantName == KENotifySlave)
		{
		linkStateNotifierBits = ENotifySlave;
		}

	//Active Mode
	else if (aConstantName == KENotifyActiveMode)
		{
		linkStateNotifierBits = ENotifyActiveMode;
		}
	//Sniff mode
	else if (aConstantName == KENotifySniffMode)
		{
		linkStateNotifierBits = ENotifySniffMode;
		}
	//Park mode
	else if (aConstantName == KENotifyParkMode)
		{
		linkStateNotifierBits = ENotifyParkMode;
		}
	//Hold mode
	else if (aConstantName == KENotifyHoldMode)
		{
		linkStateNotifierBits = ENotifyHoldMode;
		}

	//Max slots 1
	else if (aConstantName == KENotifyMaxSlots1)
		{
		linkStateNotifierBits = ENotifyMaxSlots1;
		}
	//Max slots 3
	else if (aConstantName == KENotifyMaxSlots3)
		{
		linkStateNotifierBits = ENotifyMaxSlots3;
		}
	//Max slots 5
	else if (aConstantName == KENotifyMaxSlots5)
		{
		linkStateNotifierBits = ENotifyMaxSlots5;
		}


	//Packets DM1
	else if (aConstantName == KENotifyPacketsDM1)
		{
		linkStateNotifierBits = ENotifyPacketsDM1;
		}
	//Packets DH1
	else if (aConstantName == KENotifyPacketsDH1)
		{
		linkStateNotifierBits = ENotifyPacketsDH1;
		}
	//Packets DM3
	else if (aConstantName == KENotifyPacketsDM3)
		{
		linkStateNotifierBits = ENotifyPacketsDM3;
		}
	//Packets DH3
	else if (aConstantName == KENotifyPacketsDH3)
		{
		linkStateNotifierBits = ENotifyPacketsDH3;
		}
	//Packets DM5
	else if (aConstantName == KENotifyPacketsDM5)
		{
		linkStateNotifierBits = ENotifyPacketsDM5;
		}
	//Packets DH5
	else if (aConstantName == KENotifyPacketsDH5)
		{
		linkStateNotifierBits = ENotifyPacketsDH5;
		}

	//HV1
	else if (aConstantName == KENotifyPacketsHV1)
		{
		linkStateNotifierBits = ENotifyPacketsHV1;
		}
	//HV2
	else if (aConstantName == KENotifyPacketsHV2)
		{
		linkStateNotifierBits = ENotifyPacketsHV2;
		}
	//HV3
	else if (aConstantName == KENotifyPacketsHV3)
		{
		linkStateNotifierBits = ENotifyPacketsHV3;
		}

	//Authentication Complete
	else if (aConstantName == KENotifyAuthenticationComplete)
		{
		linkStateNotifierBits = ENotifyAuthenticationComplete;
		}
	//Encryption Change On
	else if (aConstantName == KENotifyEncryptionChangeOn)
		{
		linkStateNotifierBits = ENotifyEncryptionChangeOn;
		}
	//Encryption Change Off
	else if (aConstantName == KENotifyEncryptionChangeOff)
		{
		linkStateNotifierBits = ENotifyEncryptionChangeOff;
		}
	//Physical Link Up
	else if (aConstantName == KENotifyPhysicalLinkUp)
		{
		linkStateNotifierBits = ENotifyPhysicalLinkUp;
		}
	//Physical Link Down
	else if (aConstantName == KENotifyPhysicalLinkDown)
		{
		linkStateNotifierBits = ENotifyPhysicalLinkDown;
		}
	//Physical Link Error
	else if (aConstantName == KENotifyPhysicalLinkError)
		{
		linkStateNotifierBits = ENotifyPhysicalLinkError;
		}

	//SCO Link Up
	else if (aConstantName == KENotifySCOLinkUp)
		{
		linkStateNotifierBits = ENotifySCOLinkUp;
		}
	//SCO Link Down
	else if (aConstantName == KENotifySCOLinkDown)
		{
		linkStateNotifierBits = ENotifySCOLinkDown;
		}
	//SCO Link Error
	else if (aConstantName == KENotifySCOLinkError)
		{
		linkStateNotifierBits = ENotifySCOLinkError;
		}
	else
		{
		linkStateNotifierBits = 0;
		}
		
	return linkStateNotifierBits;
	}

// method splitted in blocks to save stack space
TBool CT_DataCommsBase::GetIntValue(const TDesC& aConstantName, TInt& aValue)
	{
	TBool	ret= ETrue;
	// ************************************************************************
	// bt_sock.h
	// ************************************************************************
	_LIT(KBTAddrFamilyName,							"KBTAddrFamily");
	_LIT(KBTLinkManagerName,						"KBTLinkManager");
	_LIT(KL2CAPName,								"KL2CAP");
	_LIT(KRFCOMMName,								"KRFCOMM");
	_LIT(KSDPName,									"KSDP");
	_LIT(KAVCTPName,								"KAVCTP");
	_LIT(KTCIL2CAPName,								"KTCIL2CAP");
	_LIT(KBTMajorName,								"KBTMajor");
	_LIT(KBTMinorName,								"KBTMinor");
	_LIT(KBTBuildName,								"KBTBuild");
	_LIT(KSolInetIpName,							"KSolInetIp");
	_LIT(KSolBtBlogName,							"KSolBtBlog");
	_LIT(KSolBtHCIName,								"KSolBtHCI");
	_LIT(KSolBtLMName,								"KSolBtLM");
	_LIT(KSolBtL2CAPName,							"KSolBtL2CAP");
	_LIT(KSolBtRFCOMMName,							"KSolBtRFCOMM");
	_LIT(KSolBtAVCTPName,							"KSolBtAVCTP");
	_LIT(KSolBtACLName,								"KSolBtACL");
	_LIT(KSolBtAVDTPSignallingName,					"KSolBtAVDTPSignalling");
	_LIT(KSolBtAVDTPMediaName,						"KSolBtAVDTPMedia");
	_LIT(KSolBtAVDTPReportingName,					"KSolBtAVDTPReporting");
	_LIT(KSolBtAVDTPRecoveryName,					"KSolBtAVDTPRecovery");
	_LIT(KSolBtAVDTPInternalName,					"KSolBtAVDTPInternal");
	_LIT(KSolBtLMProxyName,							"KSolBtLMProxy");
	_LIT(KSolBtSAPBaseName,							"KSolBtSAPBase");
	_LIT(KSCOListenQueSizeName,						"KSCOListenQueSize");
	_LIT(KL2MinMTUName,								"KL2MinMTU");
	_LIT(KMaxPSMName,								"KMaxPSM");
	_LIT(KMinUserPSMName,							"KMinUserPSM");
	_LIT(KMinPSMName,								"KMinPSM");
	_LIT(KHostResInquiryName,						"KHostResInquiry");
	_LIT(KHostResNameName,							"KHostResName");
	_LIT(KHostResIgnoreCacheName,					"KHostResIgnoreCache");
	_LIT(KHostResCacheName,							"KHostResCache");
	_LIT(KL2CAPEchoRequestIoctlName,				"KL2CAPEchoRequestIoctl");
	_LIT(KL2CAPIncomingMTUIoctlName,				"KL2CAPIncomingMTUIoctl");
	_LIT(KL2CAPOutgoingMTUIoctlName,				"KL2CAPOutgoingMTUIoctl");
	_LIT(KL2CAPUpdateChannelConfigIoctlName,		"KL2CAPUpdateChannelConfigIoctl");
	_LIT(KDefaultBasebandConnectionTimeoutName,		"KDefaultBasebandConnectionTimeout");
	_LIT(KLMAddSCOConnIoctlName,					"KLMAddSCOConnIoctl");
	_LIT(KHCIRemoveSCOConnIoctlName,				"KHCIRemoveSCOConnIoctl");
	_LIT(KHCIChangePacketTypeIoctlName,				"KHCIChangePacketTypeIoctl");
	_LIT(KHCIAuthRequestIoctlName,					"KHCIAuthRequestIoctl");
	_LIT(KHCIEncryptIoctlName,						"KHCIAuthRequestIoctl");
	_LIT(KHCIChangeLinkKeyIoctlName,				"KHCIChangeLinkKeyIoctl");
	_LIT(KHCIMasterLinkKeyIoctlName,				"KHCIMasterLinkKeyIoctl");
	_LIT(KHCIHoldModeIoctlName,						"KHCIHoldModeIoctl");
	_LIT(KHCISniffModeIoctlName,					"KHCISniffModeIoctl");
	_LIT(KHCIExitSniffModeIoctlName,				"KHCIExitSniffModeIoctl");
	_LIT(KHCIParkModeIoctlName,						"KHCIParkModeIoctl");
	_LIT(KHCIExitParkModeIoctlName,					"KHCIExitParkModeIoctl");
	_LIT(KHCIReadPageTimeoutIoctlName,				"KHCIReadPageTimeoutIoctl");
	_LIT(KHCIWritePageTimeoutIoctlName,				"KHCIWritePageTimeoutIoctl");
	_LIT(KHCIReadScanEnableIoctlName,				"KHCIReadScanEnableIoctl");
	_LIT(KHCIWriteScanEnableIoctlName,				"KHCIWriteScanEnableIoctl");
	_LIT(KHCIReadDeviceClassIoctlName,				"KHCIReadDeviceClassIoctl");
	_LIT(KHCIWriteDeviceClassIoctlName,				"KHCIWriteDeviceClassIoctl");
	_LIT(KHCIReadVoiceSettingIoctlName,				"KHCIReadVoiceSettingIoctl");
	_LIT(KHCIWriteVoiceSettingIoctlName,			"KHCIWriteVoiceSettingIoctl");
	_LIT(KHCIReadHoldModeActivityIoctlName,			"KHCIReadHoldModeActivityIoctl");
	_LIT(KHCIWriteHoldModeActivityIoctlName,		"KHCIWriteHoldModeActivityIoctl");
	_LIT(KHCILocalVersionIoctlName,					"KHCILocalVersionIoctl");
	_LIT(KHCILocalFeaturesIoctlName,				"KHCILocalFeaturesIoctl");
	_LIT(KHCICountryCodeIoctlName,					"KHCICountryCodeIoctl");
	_LIT(KHCILocalAddressIoctlName,					"KHCILocalAddressIoctl");
	_LIT(KHCIWriteDiscoverabilityIoctlName,			"KHCIWriteDiscoverabilityIoctl");
	_LIT(KHCIReadDiscoverabilityIoctlName,			"KHCIReadDiscoverabilityIoctl");
	_LIT(KHCIReadAuthenticationEnableIoctlName,		"KHCIReadAuthenticationEnableIoctl");
	_LIT(KHCIWriteAuthenticationEnableIoctlName,	"KHCIWriteAuthenticationEnableIoctl");
	_LIT(KL2CAPPassiveAutoBindName,					"KL2CAPPassiveAutoBind");
	_LIT(KRFCOMMModemStatusCmdIoctlName,			"KRFCOMMModemStatusCmdIoctl");
	_LIT(KRFCOMMRemoteLineStatusCmdIoctlName,		"KRFCOMMRemoteLineStatusCmdIoctl");
	_LIT(KRFCOMMRemotePortNegCmdIoctlName,			"KRFCOMMRemotePortNegCmdIoctl");
	_LIT(KRFCOMMRemotePortNegRequestIoctlName,		"KRFCOMMRemotePortNegRequestIoctl");
	_LIT(KRFCOMMConfigChangeIndicationIoctlName,	"KRFCOMMConfigChangeIndicationIoctl");
	_LIT(KRFCOMMLocalPortParameterName,				"KRFCOMMLocalPortParameter");
	_LIT(KRFCOMMGetAvailableServerChannelName,		"KRFCOMMGetAvailableServerChannel");
	_LIT(KRFCOMMMaximumSupportedMTUName,			"KRFCOMMMaximumSupportedMTU");
	_LIT(KRFCOMMGetDebug1Name,						"KRFCOMMGetDebug1");
	_LIT(KRFCOMMGetRemoteModemStatusName,			"KRFCOMMGetRemoteModemStatus");
	_LIT(KRFCOMMGetTransmitCreditName,				"KRFCOMMGetTransmitCredit");
	_LIT(KRFCOMMGetReceiveCreditName,				"KRFCOMMGetReceiveCredit");
	_LIT(KRFCOMMGetReUsedCountName,					"KRFCOMMGetReUsedCount");
	_LIT(KRFCOMMFlowTypeCBFCName,					"KRFCOMMFlowTypeCBFC");
	_LIT(KRFCOMMLocalModemStatusName,				"KRFCOMMLocalModemStatus");
	_LIT(KErrNoneName,								"KErrNone");
	_LIT(KErrCancelName,							"KErrCancel");
	_LIT(KErrBadHandleName,							"KErrBadHandle");
	_LIT(KZero,										"Zero");
	_LIT(KOne,										"One");
	_LIT(ELMOutboundACLSizeName,					"ELMOutboundACLSize");
	_LIT(ELMInboundACLSizeName,						"ELMInboundACLSize");
	_LIT(KLMGetACLHandleName,						"KLMGetACLHandle");
	_LIT(KLMGetACLLinkCountName,					"KLMGetACLLinkCount");
	_LIT(KLMGetACLLinkArrayName,					"KLMGetACLLinkArray");
	_LIT(KLMSetBasebandConnectionPolicyName,		"KLMSetBasebandConnectionPolicy");
	_LIT(KLMGetBasebandHandleName,					"KLMGetBasebandHandle");
	_LIT(EBBSubscribePhysicalLinkName,				"EBBSubscribePhysicalLink");
	_LIT(EBBBeginRawName,							"EBBBeginRaw");
	_LIT(EBBRequestRoleMasterName,					"EBBRequestRoleMaster");
	_LIT(EBBRequestRoleSlaveName,					"EBBRequestRoleSlave");
	_LIT(EBBCancelModeRequestName,					"EBBCancelModeRequest");
	_LIT(EBBRequestSniffName,						"EBBRequestSniff");
	_LIT(EBBRequestParkName,						"EBBRequestPark");
	_LIT(EBBRequestPreventRoleChangeName,			"EBBRequestPreventRoleChange");
	_LIT(EBBRequestAllowRoleChangeName,				"EBBRequestAllowRoleChange");
	_LIT(EBBRequestChangeSupportedPacketTypesName,	"EBBRequestChangeSupportedPacketTypes");
	_LIT(EBBEnumeratePhysicalLinksName,				"EBBEnumeratePhysicalLinks");
	_LIT(EBBGetPhysicalLinkStateName,				"EBBGetPhysicalLinkState");
	_LIT(EBBGetSniffIntervalName,					"EBBGetSniffInterval");
	_LIT(EBBRequestLinkAuthenticationName,			"EBBRequestLinkAuthentication");
	_LIT(EBBRequestPreventSniffName,				"EBBRequestPreventSniff");
	_LIT(EBBRequestPreventHoldName,					"EBBRequestPreventHold");
	_LIT(EBBRequestPreventParkName,					"EBBRequestPreventPark");
	_LIT(EBBRequestPreventAllLowPowerModesName,		"EBBRequestPreventAllLowPowerModes");
	_LIT(EBBRequestAllowSniffName,					"EBBRequestAllowSniff");
	_LIT(EBBRequestAllowHoldName,					"EBBRequestAllowHold");
	_LIT(EBBRequestAllowParkName,					"EBBRequestAllowPark");
	_LIT(EBBRequestAllowAllLowPowerModesName,		"EBBRequestAllowAllLowPowerModes");
	_LIT(KBTRegisterCodServiceName,					"KBTRegisterCodService");
	_LIT(KBTSecurityDeviceOverrideName,				"KBTSecurityDeviceOverride");
	_LIT(KESockDefaultMessageSlotsName,				"KESockDefaultMessageSlots");
	_LIT(KUseEmbeddedUniqueIdName,					"KUseEmbeddedUniqueId");
	_LIT(KConnProgressDefaultName,					"KConnProgressDefault");
	_LIT(KMaxSockAddrSizeName,						"KMaxSockAddrSize");
	_LIT(KAfInetName,								"KAfInet");
	_LIT(KAfInet6Name,								"KAfInet6");
	_LIT(KAFUnspecName,								"KAFUnspec");
	_LIT(KSockStreamName,							"KSockStream");
	_LIT(KSockDatagramName,							"KSockDatagram");
	_LIT(KSockSeqPacketName,						"KSockSeqPacket");
	_LIT(KSockRawName,								"KSockRaw");
	_LIT(KSOLSocketName,							"KSOLSocket");
	_LIT(KLevelUnspecifiedName,						"KLevelUnspecified");
	_LIT(KSODebugName,								"KSODebug");
	_LIT(KSORecvBufName,							"KSORecvBuf");
	_LIT(KSOSendBufName,							"KSOSendBuf");
	_LIT(KSoReuseAddrName,							"KSoReuseAddr");
	_LIT(KSONonBlockingIOName,						"KSONonBlockingIO");
	_LIT(KSOBlockingIOName,							"KSOBlockingIO");
	_LIT(KSOSelectPollName,							"KSOSelectPoll");
	_LIT(KSOReadBytesPendingName,					"KSOReadBytesPending");
	_LIT(KSOUrgentDataOffsetName,					"KSOUrgentDataOffset");
	_LIT(KSOSelectLastErrorName,					"KSOSelectLastError");
	_LIT(KSOEnableTransferName,						"KSOEnableTransfer");
	_LIT(KSODisableTransferName,					"KSODisableTransfer");
	_LIT(KSocketBufSizeUndefinedName,				"KSocketBufSizeUndefined");
	_LIT(KSocketDefaultBufferSizeName,				"KSocketDefaultBufferSize");
	_LIT(KSocketInternalOptionBitName,				"KSocketInternalOptionBit");
	_LIT(KIOctlSelectName,							"KIOctlSelect");
	_LIT(KSockSelectReadName,						"KSockSelectRead");
	_LIT(KSockSelectWriteName,						"KSockSelectWrite");
	_LIT(KSockSelectExceptName,						"KSockSelectExcept");
	_LIT(KSockSelectReadContinuationName,			"KSockSelectReadContinuation");
	_LIT(KSockWriteUrgentName,						"KSockWriteUrgent");
	_LIT(KSocketInternalWriteBitName,				"KSocketInternalWriteBit");
	_LIT(KSockWriteSystemMaskName,					"KSockWriteSystemMask");
	_LIT(KSockReadPeekName,							"KSockReadPeek");
	_LIT(KSocketInternalReadBitName,				"KSocketInternalReadBit");
	_LIT(KSockReadContinuationName,					"KSockReadContinuation");
	_LIT(KSIConnectionLessName,						"KSIConnectionLess");
	_LIT(KSIReliableName,							"KSIReliable");
	_LIT(KSIInOrderName,							"KSIInOrder");
	_LIT(KSIMessageBasedName,						"KSIMessageBased");
	_LIT(KSIDatagramName,							"KSIDatagram");
	_LIT(KSIStreamBasedName,						"KSIStreamBased");
	_LIT(KSIPseudoStreamName,						"KSIPseudoStream");
	_LIT(KSIUrgentDataName,							"KSIUrgentData");
	_LIT(KSIConnectDataName,						"KSIConnectData");
	_LIT(KSIDisconnectDataName,						"KSIDisconnectData");
	_LIT(KSIBroadcastName,							"KSIBroadcast");
	_LIT(KSIMultiPointName,							"KSIMultiPoint");
	_LIT(KSIQOSName,								"KSIQOS");
	_LIT(KSIWriteOnlyName,							"KSIWriteOnly");
	_LIT(KSIReadOnlyName,							"KSIReadOnly");
	_LIT(KSIGracefulCloseName,						"KSIGracefulClose");
	_LIT(KSICanReconnectName,						"KSICanReconnect");
	_LIT(KSIPeekDataName,							"KSIPeekData");
	_LIT(KSIRequiresOwnerInfoName,					"KSIRequiresOwnerInfo");
	_LIT(KNSNameResolutionName,						"KNSNameResolution");
	_LIT(KNSHeirarchicalNamingName,					"KNSHeirarchicalNaming");
	_LIT(KNSRemoteDiscoveryName,					"KNSRemoteDiscovery");
	_LIT(KNSServiceResolutionName,					"KNSServiceResolution");
	_LIT(KNSNameRegistrationName,					"KNSNameRegistration");
	_LIT(KNSServiceRegistrationName,				"KNSServiceRegistration");
	_LIT(KNSDynamicAddressingName,					"KNSDynamicAddressing");
	_LIT(KNSInfoDatabaseName,						"KNSInfoDatabase");
	_LIT(KNSRequiresConnectionStartupName,			"KNSRequiresConnectionStartup");
	_LIT(KSocketNoSecurityName,						"KSocketNoSecurity");
	_LIT(KSecureSocketsName,						"KSecureSockets");
	_LIT(KSocketMessageSizeIsStreamName,			"KSocketMessageSizeIsStream");
	_LIT(KSocketMessageSizeUndefinedName,			"KSocketMessageSizeUndefined");
	_LIT(KSocketMessageSizeNoLimitName,				"KSocketMessageSizeNoLimit");
	_LIT(KUndefinedSockTypeName,					"KUndefinedSockType");
	_LIT(KUndefinedProtocolName,					"KUndefinedProtocol");
	_LIT(KErrWouldBlockName,						"KErrWouldBlock");
	_LIT(KErrConnectionTerminatedName,				"KErrConnectionTerminated");
	_LIT(KErrCannotFindProtocolName,				"KErrCannotFindProtocol");
	_LIT(KCOLConnectionName,						"KCOLConnection");
	_LIT(KCOLProviderName,							"KCOLProvider");
	_LIT(KConnInternalOptionBitName,				"KConnInternalOptionBit");
	_LIT(KConnWriteUserDataBitName,					"KConnWriteUserDataBit");
	_LIT(KConnReadUserDataBitName,					"KConnReadUserDataBit");
	_LIT(KCoEnumerateConnectionClientsName,			"KCoEnumerateConnectionClients");
	_LIT(KCoGetConnectionClientInfoName,			"KCoGetConnectionClientInfo");
	_LIT(KCoGetConnectionSocketInfoName,			"KCoGetConnectionSocketInfo");
	_LIT(KConnectionTypeDefaultName,				"KConnectionTypeDefault");
	_LIT(KSubConnParamsInterfaceUidName,			"KSubConnParamsInterfaceUid");
	_LIT(KSubConnEventInterfaceUidName,				"KSubConnEventInterfaceUid");
	_LIT(KSubConnGenericParamsImplUidName,			"KSubConnGenericParamsImplUid");
	_LIT(KSubConnGenericEventsImplUidName,			"KSubConnGenericEventsImplUid");
	_LIT(KSubConGlobalFamilyName,					"KSubConGlobalFamily");
	_LIT(KSubConQoSFamilyName,						"KSubConQoSFamily");
	_LIT(KSubConAuthorisationFamilyName,			"KSubConAuthorisationFamily");
	_LIT(KNotificationEventMaxSizeName,				"KNotificationEventMaxSize");

	// BT Address Family
	if (aConstantName==KBTAddrFamilyName)
		{
		aValue=KBTAddrFamily;
		}
	else if (aConstantName==KBTLinkManagerName)
		{
		aValue=KBTLinkManager;
		}
	else if (aConstantName==KL2CAPName)
		{
		aValue=KL2CAP;
		}
	else if (aConstantName==KRFCOMMName)
		{
		aValue=KRFCOMM;
		}
	else if (aConstantName==KSDPName)
		{
		aValue=KSDP;
		}
	else if (aConstantName==KAVCTPName)
		{
		aValue=KAVCTP;
		}
	else if (aConstantName==KTCIL2CAPName)
		{
		aValue=KTCIL2CAP;
		}
	else if (aConstantName==KBTMajorName)
		{
		aValue=KBTMajor;
		}
	else if (aConstantName==KBTMinorName)
		{
		aValue=KBTMinor;
		}
	else if (aConstantName==KBTBuildName)
		{
		aValue=KBTBuild;
		}
	else if (aConstantName==KSolInetIpName)
		{
		aValue=KSolInetIp;
		}
	else if (aConstantName==KSolBtBlogName)
		{
		aValue=KSolBtBlog;
		}
	else if (aConstantName==KSolBtHCIName)
		{
		aValue=KSolBtHCI;
		}
	else if (aConstantName==KSolBtLMName)
		{
		aValue=KSolBtLM;
		}
	else if (aConstantName==KSolBtL2CAPName)
		{
		aValue=KSolBtL2CAP;
		}
	else if (aConstantName==KSolBtRFCOMMName)
		{
		aValue=KSolBtRFCOMM;
		}
	else if (aConstantName==KSolBtAVCTPName)
		{
		aValue=KSolBtAVCTP;
		}
	else if (aConstantName==KSolBtACLName)
		{
		aValue=KSolBtACL;
		}
	else if (aConstantName==KSolBtAVDTPSignallingName)
		{
		aValue=KSolBtAVDTPSignalling;
		}
	else if (aConstantName==KSolBtAVDTPMediaName)
		{
		aValue=KSolBtAVDTPMedia;
		}
	else if (aConstantName==KSolBtAVDTPReportingName)
		{
		aValue=KSolBtAVDTPReporting;
		}
	else if (aConstantName==KSolBtAVDTPRecoveryName)
		{
		aValue=KSolBtAVDTPRecovery;
		}
	else if (aConstantName==KSolBtAVDTPInternalName)
		{
		aValue=KSolBtAVDTPInternal;
		}
	else if (aConstantName==KSolBtLMProxyName)
		{
		aValue=KSolBtLMProxy;
		}
	else if (aConstantName==KSolBtSAPBaseName)
		{
		aValue=KSolBtSAPBase;
		}
	else if (aConstantName==KSCOListenQueSizeName)
		{
		aValue=KSCOListenQueSize;
		}
	else if (aConstantName==KL2MinMTUName)
		{
		aValue=KL2MinMTU;
		}
	else if (aConstantName==KMaxPSMName)
		{
		aValue=KMaxPSM;
		}
	else if (aConstantName==KMinUserPSMName)
		{
		aValue=KMinUserPSM;
		}
	else if (aConstantName==KMinPSMName)
		{
		aValue=KMinPSM;
		}
	else if (aConstantName==KHostResInquiryName)
		{
		aValue=KHostResInquiry;
		}
	else if (aConstantName==KHostResNameName)
		{
		aValue=KHostResName;
		}
	else if (aConstantName==KHostResIgnoreCacheName)
		{
		aValue=KHostResIgnoreCache;
		}
	else if (aConstantName==KHostResCacheName)
		{
		aValue=KHostResCache;
		}
	else if (aConstantName==KL2CAPEchoRequestIoctlName)
		{
		aValue=KL2CAPEchoRequestIoctl;
		}
	else if (aConstantName==KL2CAPIncomingMTUIoctlName)
		{
		aValue=KL2CAPIncomingMTUIoctl;
		}
	else if (aConstantName==KL2CAPOutgoingMTUIoctlName)
		{
		aValue=KL2CAPOutgoingMTUIoctl;
		}
	else if (aConstantName==KL2CAPUpdateChannelConfigIoctlName)
		{
		aValue=KL2CAPUpdateChannelConfigIoctl;
		}
	else if (aConstantName==KDefaultBasebandConnectionTimeoutName)
		{
		aValue=KDefaultBasebandConnectionTimeout;
		}
	else if (aConstantName==KLMAddSCOConnIoctlName)
		{
		aValue=KLMAddSCOConnIoctl;
		}
	else if (aConstantName==KHCIRemoveSCOConnIoctlName)
		{
		aValue=KHCIRemoveSCOConnIoctl;
		}
	else if (aConstantName==KHCIChangePacketTypeIoctlName)
		{
		aValue=KHCIChangePacketTypeIoctl;
		}
	else if (aConstantName==KHCIAuthRequestIoctlName)
		{
		aValue=KHCIAuthRequestIoctl;
		}
	else if (aConstantName==KHCIEncryptIoctlName)
		{
		aValue=KHCIEncryptIoctl;
		}
	else if (aConstantName==KHCIChangeLinkKeyIoctlName)
		{
		aValue=KHCIChangeLinkKeyIoctl;
		}
	else if (aConstantName==KHCIMasterLinkKeyIoctlName)
		{
		aValue=KHCIMasterLinkKeyIoctl;
		}
	else if (aConstantName==KHCIHoldModeIoctlName)
		{
		aValue=KHCIHoldModeIoctl;
		}
	else if (aConstantName==KHCISniffModeIoctlName)
		{
		aValue=KHCISniffModeIoctl;
		}
	else if (aConstantName==KHCIExitSniffModeIoctlName)
		{
		aValue=KHCIExitSniffModeIoctl;
		}
	else if (aConstantName==KHCIParkModeIoctlName)
		{
		aValue=KHCIParkModeIoctl;
		}
	else if (aConstantName==KHCIExitParkModeIoctlName)
		{
		aValue=KHCIExitParkModeIoctl;
		}
	else if (aConstantName==KHCIReadPageTimeoutIoctlName)
		{
		aValue=KHCIReadPageTimeoutIoctl;
		}
	else if (aConstantName==KHCIWritePageTimeoutIoctlName)
		{
		aValue=KHCIWritePageTimeoutIoctl;
		}
	else if (aConstantName==KHCIReadScanEnableIoctlName)
		{
		aValue=KHCIReadScanEnableIoctl;
		}
	else if (aConstantName==KHCIWriteScanEnableIoctlName)
		{
		aValue=KHCIWriteScanEnableIoctl;
		}
	else if (aConstantName==KHCIReadDeviceClassIoctlName)
		{
		aValue=KHCIReadDeviceClassIoctl;
		}
	else if (aConstantName==KHCIWriteDeviceClassIoctlName)
		{
		aValue=KHCIWriteDeviceClassIoctl;
		}
	else if (aConstantName==KHCIReadVoiceSettingIoctlName)
		{
		aValue=KHCIReadVoiceSettingIoctl;
		}
	else if (aConstantName==KHCIWriteVoiceSettingIoctlName)
		{
		aValue=KHCIWriteVoiceSettingIoctl;
		}
	else if (aConstantName==KHCIReadHoldModeActivityIoctlName)
		{
		aValue=KHCIReadHoldModeActivityIoctl;
		}
	else if (aConstantName==KHCIWriteHoldModeActivityIoctlName)
		{
		aValue=KHCIWriteHoldModeActivityIoctl;
		}
	else if (aConstantName==KHCILocalVersionIoctlName)
		{
		aValue=KHCILocalVersionIoctl;
		}
	else if (aConstantName==KHCILocalFeaturesIoctlName)
		{
		aValue=KHCILocalFeaturesIoctl;
		}
	else if (aConstantName==KHCICountryCodeIoctlName)
		{
		aValue=KHCICountryCodeIoctl;
		}
	else if (aConstantName==KHCILocalAddressIoctlName)
		{
		aValue=KHCILocalAddressIoctl;
		}
	else if (aConstantName==KHCIWriteDiscoverabilityIoctlName)
		{
		aValue=KHCIWriteDiscoverabilityIoctl;
		}
	else if (aConstantName==KHCIReadDiscoverabilityIoctlName)
		{
		aValue=KHCIReadDiscoverabilityIoctl;
		}
	else if (aConstantName==KHCIReadAuthenticationEnableIoctlName)
		{
		aValue=KHCIReadAuthenticationEnableIoctl;
		}
	else if (aConstantName==KHCIWriteAuthenticationEnableIoctlName)
		{
		aValue=KHCIWriteAuthenticationEnableIoctl;
		}
	else if (aConstantName==KL2CAPPassiveAutoBindName)
		{
		aValue=KL2CAPPassiveAutoBind;
		}
	else if (aConstantName==KRFCOMMModemStatusCmdIoctlName)
		{
		aValue=KRFCOMMModemStatusCmdIoctl;
		}
	else if (aConstantName==KRFCOMMRemoteLineStatusCmdIoctlName)
		{
		aValue=KRFCOMMRemoteLineStatusCmdIoctl;
		}
	else if (aConstantName==KRFCOMMRemotePortNegCmdIoctlName)
		{
		aValue=KRFCOMMRemotePortNegCmdIoctl;
		}
	else if (aConstantName==KRFCOMMRemotePortNegRequestIoctlName)
		{
		aValue=KRFCOMMRemotePortNegRequestIoctl;
		}
	else if (aConstantName==KRFCOMMConfigChangeIndicationIoctlName)
		{
		aValue=KRFCOMMConfigChangeIndicationIoctl;
		}
	else if (aConstantName==KRFCOMMLocalPortParameterName)
		{
		aValue=KRFCOMMLocalPortParameter;
		}
	else if (aConstantName==KRFCOMMGetAvailableServerChannelName)
		{
		aValue=KRFCOMMGetAvailableServerChannel;
		}
	else if (aConstantName==KRFCOMMMaximumSupportedMTUName)
		{
		aValue=KRFCOMMMaximumSupportedMTU;
		}
	else if (aConstantName==KRFCOMMGetDebug1Name)
		{
		aValue=KRFCOMMGetDebug1;
		}
	else if (aConstantName==KRFCOMMGetRemoteModemStatusName)
		{
		aValue=KRFCOMMGetRemoteModemStatus;
		}
	else if (aConstantName==KRFCOMMGetTransmitCreditName)
		{
		aValue=KRFCOMMGetTransmitCredit;
		}
	else if (aConstantName==KRFCOMMGetReceiveCreditName)
		{
		aValue=KRFCOMMGetReceiveCredit;
		}
	else if (aConstantName==KRFCOMMGetReUsedCountName)
		{
		aValue=KRFCOMMGetReUsedCount;
		}
	else if (aConstantName==KRFCOMMFlowTypeCBFCName)
		{
		aValue=KRFCOMMFlowTypeCBFC;
		}
	else if (aConstantName==KRFCOMMLocalModemStatusName)
		{
		aValue=KRFCOMMLocalModemStatus;
		}
	else if (aConstantName==KErrNoneName)
		{
		aValue=KErrNone;
		}
	else if (aConstantName==KErrCancelName)
		{
		aValue=KErrCancel;
		}
	else if (aConstantName==KErrBadHandleName)
		{
		aValue=KErrBadHandle;
		}
	else if (aConstantName==KZero)
		{
		aValue=0;
		}
	else if (aConstantName==KOne)
		{
		aValue=1;
		}
	else if (aConstantName==ELMOutboundACLSizeName)
		{
		aValue=ELMOutboundACLSize;
		}
	else if (aConstantName==ELMInboundACLSizeName)
		{
		aValue=ELMInboundACLSize;
		}
	else if (aConstantName==KLMGetACLHandleName)
		{
		aValue=KLMGetACLHandle;
		}
	else if (aConstantName==KLMGetACLLinkCountName)
		{
		aValue=KLMGetACLLinkCount;
		}
	else if (aConstantName==KLMGetACLLinkArrayName)
		{
		aValue=KLMGetACLLinkArray;
		}
	else if (aConstantName==KLMSetBasebandConnectionPolicyName)
		{
		aValue=KLMSetBasebandConnectionPolicy;
		}
	else if (aConstantName==KLMGetBasebandHandleName)
		{
		aValue=KLMGetBasebandHandle;
		}
	else if (aConstantName==EBBSubscribePhysicalLinkName)
		{
		aValue=EBBSubscribePhysicalLink;
		}
	else if (aConstantName==EBBBeginRawName)
		{
		aValue=EBBBeginRaw;
		}
	else if (aConstantName==EBBRequestRoleMasterName)
		{
		aValue=EBBRequestRoleMaster;
		}
	else if (aConstantName==EBBRequestRoleSlaveName)
		{
		aValue=EBBRequestRoleSlave;
		}
	else if (aConstantName==EBBCancelModeRequestName)
		{
		aValue=EBBCancelModeRequest;
		}
	else if (aConstantName==EBBRequestSniffName)
		{
		aValue=EBBRequestSniff;
		}
	else if (aConstantName==EBBRequestParkName)
		{
		aValue=EBBRequestPark;
		}
	else if (aConstantName==EBBRequestPreventRoleChangeName)
		{
		aValue=EBBRequestPreventRoleChange;
		}
	else if (aConstantName==EBBRequestAllowRoleChangeName)
		{
		aValue=EBBRequestAllowRoleChange;
		}
	else if (aConstantName==EBBRequestChangeSupportedPacketTypesName)
		{
		aValue=EBBRequestChangeSupportedPacketTypes;
		}
	else if (aConstantName==EBBEnumeratePhysicalLinksName)
		{
		aValue=EBBEnumeratePhysicalLinks;
		}
	else if (aConstantName==EBBGetPhysicalLinkStateName)
		{
		aValue=EBBGetPhysicalLinkState;
		}
	else if (aConstantName==EBBGetSniffIntervalName)
		{
		aValue=EBBGetSniffInterval;
		}
	else if (aConstantName==EBBRequestLinkAuthenticationName)
		{
		aValue=EBBRequestLinkAuthentication;
		}
	else if (aConstantName==EBBRequestPreventSniffName)
		{
		aValue=EBBRequestPreventSniff;
		}
	else if (aConstantName==EBBRequestPreventHoldName)
		{
		aValue=EBBRequestPreventHold;
		}
	else if (aConstantName==EBBRequestPreventParkName)
		{
		aValue=EBBRequestPreventPark;
		}
	else if (aConstantName==EBBRequestPreventAllLowPowerModesName)
		{
		aValue=EBBRequestPreventAllLowPowerModes;
		}
	else if (aConstantName==EBBRequestAllowSniffName)
		{
		aValue=EBBRequestAllowSniff;
		}
	else if (aConstantName==EBBRequestAllowHoldName)
		{
		aValue=EBBRequestAllowHold;
		}
	else if (aConstantName==EBBRequestAllowParkName)
		{
		aValue=EBBRequestAllowPark;
		}
	else if (aConstantName==EBBRequestAllowAllLowPowerModesName)
		{
		aValue=EBBRequestAllowAllLowPowerModes;
		}
	else if (aConstantName==KBTRegisterCodServiceName)
		{
		aValue=KBTRegisterCodService;
		}
	else if (aConstantName==KBTSecurityDeviceOverrideName)
		{
		aValue=KBTSecurityDeviceOverride;
		}
	else if (aConstantName==KESockDefaultMessageSlotsName)
		{
		aValue=KESockDefaultMessageSlots;
		}
	else if (aConstantName==KUseEmbeddedUniqueIdName)
		{
		aValue=KUseEmbeddedUniqueId;
		}
	else if (aConstantName==KConnProgressDefaultName)
		{
		aValue=KConnProgressDefault;
		}
	else if (aConstantName==KMaxSockAddrSizeName)
		{
		aValue=KMaxSockAddrSize;
		}
	else if (aConstantName==KAfInetName)
		{
		aValue=KAfInet;
		}
	else if (aConstantName==KAfInet6Name)
		{
		aValue=KAfInet6;
		}
	else if (aConstantName==KAFUnspecName)
		{
		aValue=KAFUnspec;
		}
	else if (aConstantName==KSockStreamName)
		{
		aValue=KSockStream;
		}
	else if (aConstantName==KSockDatagramName)
		{
		aValue=KSockDatagram;
		}
	else if (aConstantName==KSockSeqPacketName)
		{
		aValue=KSockSeqPacket;
		}
	else if (aConstantName==KSockRawName)
		{
		aValue=KSockRaw;
		}
	else if (aConstantName==KSOLSocketName)
		{
		aValue=KSOLSocket;
		}
	else if (aConstantName==KLevelUnspecifiedName)
		{
		aValue=KLevelUnspecified;
		}
	else if (aConstantName==KSODebugName)
		{
		aValue=KSODebug;
		}
	else if (aConstantName==KSORecvBufName)
		{
		aValue=KSORecvBuf;
		}
	else if (aConstantName==KSOSendBufName)
		{
		aValue=KSOSendBuf;
		}
	else if (aConstantName==KSoReuseAddrName)
		{
		aValue=KSoReuseAddr;
		}
	else if (aConstantName==KSONonBlockingIOName)
		{
		aValue=KSONonBlockingIO;
		}
	else if (aConstantName==KSOBlockingIOName)
		{
		aValue=KSOBlockingIO;
		}
	else if (aConstantName==KSOSelectPollName)
		{
		aValue=KSOSelectPoll;
		}
	else if (aConstantName==KSOReadBytesPendingName)
		{
		aValue=KSOReadBytesPending;
		}
	else if (aConstantName==KSOUrgentDataOffsetName)
		{
		aValue=KSOUrgentDataOffset;
		}
	else if (aConstantName==KSOSelectLastErrorName)
		{
		aValue=KSOSelectLastError;
		}
	else if (aConstantName==KSOEnableTransferName)
		{
		aValue=KSOEnableTransfer;
		}
	else if (aConstantName==KSODisableTransferName)
		{
		aValue=KSODisableTransfer;
		}
	else if (aConstantName==KSocketBufSizeUndefinedName)
		{
		aValue=KSocketBufSizeUndefined;
		}
	else if (aConstantName==KSocketDefaultBufferSizeName)
		{
		aValue=KSocketDefaultBufferSize;
		}
	else if (aConstantName==KSocketInternalOptionBitName)
		{
		aValue=KSocketInternalOptionBit;
		}
	else if (aConstantName==KIOctlSelectName)
		{
		aValue=KIOctlSelect;
		}
	else if (aConstantName==KSockSelectReadName)
		{
		aValue=KSockSelectRead;
		}
	else if (aConstantName==KSockSelectWriteName)
		{
		aValue=KSockSelectWrite;
		}
	else if (aConstantName==KSockSelectExceptName)
		{
		aValue=KSockSelectExcept;
		}
	else if (aConstantName==KSockSelectReadContinuationName)
		{
		aValue=KSockSelectReadContinuation;
		}
	else if (aConstantName==KSockWriteUrgentName)
		{
		aValue=KSockWriteUrgent;
		}
	else if (aConstantName==KSocketInternalWriteBitName)
		{
		aValue=KSocketInternalWriteBit;
		}
	else if (aConstantName==KSockWriteSystemMaskName)
		{
		aValue=KSockWriteSystemMask;
		}
	else if (aConstantName==KSockReadPeekName)
		{
		aValue=KSockReadPeek;
		}
	else if (aConstantName==KSocketInternalReadBitName)
		{
		aValue=KSocketInternalReadBit;
		}
	else if (aConstantName==KSockReadContinuationName)
		{
		aValue=KSockReadContinuation;
		}
	else if (aConstantName==KSIConnectionLessName)
		{
		aValue=KSIConnectionLess;
		}
	else if (aConstantName==KSIReliableName)
		{
		aValue=KSIReliable;
		}
	else if (aConstantName==KSIInOrderName)
		{
		aValue=KSIInOrder;
		}
	else if (aConstantName==KSIMessageBasedName)
		{
		aValue=KSIMessageBased;
		}
	else if (aConstantName==KSIDatagramName)
		{
		aValue=KSIDatagram;
		}
	else if (aConstantName==KSIStreamBasedName)
		{
		aValue=KSIStreamBased;
		}
	else if (aConstantName==KSIPseudoStreamName)
		{
		aValue=KSIPseudoStream;
		}
	else if (aConstantName==KSIUrgentDataName)
		{
		aValue=KSIUrgentData;
		}
	else if (aConstantName==KSIConnectDataName)
		{
		aValue=KSIConnectData;
		}
	else if (aConstantName==KSIDisconnectDataName)
		{
		aValue=KSIDisconnectData;
		}
	else if (aConstantName==KSIBroadcastName)
		{
		aValue=KSIBroadcast;
		}
	else if (aConstantName==KSIMultiPointName)
		{
		aValue=KSIMultiPoint;
		}
	else if (aConstantName==KSIQOSName)
		{
		aValue=KSIQOS;
		}
	else if (aConstantName==KSIWriteOnlyName)
		{
		aValue=KSIWriteOnly;
		}
	else if (aConstantName==KSIReadOnlyName)
		{
		aValue=KSIReadOnly;
		}
	else if (aConstantName==KSIGracefulCloseName)
		{
		aValue=KSIGracefulClose;
		}
	else if (aConstantName==KSICanReconnectName)
		{
		aValue=KSICanReconnect;
		}
	else if (aConstantName==KSIPeekDataName)
		{
		aValue=KSIPeekData;
		}
	else if (aConstantName==KSIRequiresOwnerInfoName)
		{
		aValue=KSIRequiresOwnerInfo;
		}
	else if (aConstantName==KNSNameResolutionName)
		{
		aValue=KNSNameResolution;
		}
	else if (aConstantName==KNSHeirarchicalNamingName)
		{
		aValue=KNSHeirarchicalNaming;
		}
	else if (aConstantName==KNSRemoteDiscoveryName)
		{
		aValue=KNSRemoteDiscovery;
		}
	else if (aConstantName==KNSServiceResolutionName)
		{
		aValue=KNSServiceResolution;
		}
	else if (aConstantName==KNSNameRegistrationName)
		{
		aValue=KNSNameRegistration;
		}
	else if (aConstantName==KNSServiceRegistrationName)
		{
		aValue=KNSServiceRegistration;
		}
	else if (aConstantName==KNSDynamicAddressingName)
		{
		aValue=KNSDynamicAddressing;
		}
	else if (aConstantName==KNSInfoDatabaseName)
		{
		aValue=KNSInfoDatabase;
		}
	else if (aConstantName==KNSRequiresConnectionStartupName)
		{
		aValue=KNSRequiresConnectionStartup;
		}
	else if (aConstantName==KSocketNoSecurityName)
		{
		aValue=KSocketNoSecurity;
		}
	else if (aConstantName==KSecureSocketsName)
		{
		aValue=KSecureSockets;
		}
	else if (aConstantName==KSocketMessageSizeIsStreamName)
		{
		aValue=KSocketMessageSizeIsStream;
		}
	else if (aConstantName==KSocketMessageSizeUndefinedName)
		{
		aValue=KSocketMessageSizeUndefined;
		}
	else if (aConstantName==KSocketMessageSizeNoLimitName)
		{
		aValue=KSocketMessageSizeNoLimit;
		}
	else if (aConstantName==KUndefinedSockTypeName)
		{
		aValue=KUndefinedSockType;
		}
	else if (aConstantName==KUndefinedProtocolName)
		{
		aValue=KUndefinedProtocol;
		}
	else if (aConstantName==KErrWouldBlockName)
		{
		aValue=KErrWouldBlock;
		}
	else if (aConstantName==KErrConnectionTerminatedName)
		{
		aValue=KErrConnectionTerminated;
		}
	else if (aConstantName==KErrCannotFindProtocolName)
		{
		aValue=KErrCannotFindProtocol;
		}
	else if (aConstantName==KCOLConnectionName)
		{
		aValue=KCOLConnection;
		}
	else if (aConstantName==KCOLProviderName)
		{
		aValue=KCOLProvider;
		}
	else if (aConstantName==KConnInternalOptionBitName)
		{
		aValue=KConnInternalOptionBit;
		}
	else if (aConstantName==KConnWriteUserDataBitName)
		{
		aValue=KConnWriteUserDataBit;
		}
	else if (aConstantName==KConnReadUserDataBitName)
		{
		aValue=KConnReadUserDataBit;
		}
	else if (aConstantName==KCoEnumerateConnectionClientsName)
		{
		aValue=KCoEnumerateConnectionClients;
		}
	else if (aConstantName==KCoGetConnectionClientInfoName)
		{
		aValue=KCoGetConnectionClientInfo;
		}
	else if (aConstantName==KCoGetConnectionSocketInfoName)
		{
		aValue=KCoGetConnectionSocketInfo;
		}
	else if (aConstantName==KConnectionTypeDefaultName)
		{
		aValue=KConnectionTypeDefault;
		}
	else if (aConstantName==KSubConnParamsInterfaceUidName)
		{
		aValue=KSubConnParamsInterfaceUid;
		}
	else if (aConstantName==KSubConnEventInterfaceUidName)
		{
		aValue=KSubConnEventInterfaceUid;
		}
	else if (aConstantName==KSubConnGenericParamsImplUidName)
		{
		aValue=KSubConnGenericParamsImplUid;
		}
	else if (aConstantName==KSubConnGenericEventsImplUidName)
		{
		aValue=KSubConnGenericEventsImplUid;
		}
	else if (aConstantName==KSubConGlobalFamilyName)
		{
		aValue=KSubConGlobalFamily;
		}
	else if (aConstantName==KSubConQoSFamilyName)
		{
		aValue=KSubConQoSFamily;
		}
	else if (aConstantName==KSubConAuthorisationFamilyName)
		{
		aValue=KSubConAuthorisationFamily;
		}
	else if (aConstantName==KNotificationEventMaxSizeName)
		{
		aValue=KNotificationEventMaxSize;
		}
	else
		{
		ret=EFalse;
		aValue=-1;
		}

	return ret;
	}

void CT_DataCommsBase::ConvertWhiteSpace(const TDesC & inputData, TDes & convertedData)
	{
	for (int pos = 0; pos < inputData.Length(); ++pos)
		{
		if (inputData[pos] == '\r')
			{
			convertedData.Append(_L("\\r"));
			}
		else if (inputData[pos] == '\n')
			{
			convertedData.Append(_L("\\n"));
			}
		else
			{
			convertedData.Append(inputData[pos]);
			}
		}
	}

void CT_DataCommsBase::PrintProtocol(const TDesC& aDesc, const TProtocolDesc& aProtocol, TBool aShowServiceInfoFlags /* = EFalse */ )
	{
	TBuf<KMaxVersionName> versionName(aProtocol.iVersion.Name());
	INFO_PRINTF3(_L("%S.iAddrFamily              = %d"), &aDesc, aProtocol.iAddrFamily);
	INFO_PRINTF3(_L("%S.iByteOrder               = %d"), &aDesc, aProtocol.iByteOrder);
	INFO_PRINTF3(_L("%S.iMessageSize             = %d"), &aDesc, aProtocol.iMessageSize);
	INFO_PRINTF3(_L("%S.iName                    = %S"), &aDesc, &aProtocol.iName);
	INFO_PRINTF3(_L("%S.iNamingServices          = %d"), &aDesc, aProtocol.iNamingServices);
	INFO_PRINTF3(_L("%S.iProtocol                = %d"), &aDesc, aProtocol.iProtocol);
	INFO_PRINTF3(_L("%S.iSecurity                = %d"), &aDesc, aProtocol.iSecurity);
	INFO_PRINTF3(_L("%S.iServiceInfo             = %d"), &aDesc, aProtocol.iServiceInfo);

	if( aShowServiceInfoFlags )
		{
		INFO_PRINTF3(_L("%S        KSIConnectionLess = %d"), &aDesc, (aProtocol.iServiceInfo & KSIConnectionLess) ? 1 : 0);
		INFO_PRINTF3(_L("%S        KSIConnectData    = %d"), &aDesc, (aProtocol.iServiceInfo & KSIConnectData) ? 1 : 0);
		INFO_PRINTF3(_L("%S        KSIDisconnectData = %d"), &aDesc, (aProtocol.iServiceInfo & KSIDisconnectData) ? 1 : 0);
		INFO_PRINTF3(_L("%S        KSIUrgentData     = %d"), &aDesc, (aProtocol.iServiceInfo & KSIUrgentData) ? 1 : 0);
		INFO_PRINTF3(_L("%S        KSIPeekData       = %d"), &aDesc, (aProtocol.iServiceInfo & KSIPeekData) ? 1 : 0);
		INFO_PRINTF3(_L("%S        KSIGracefulClose  = %d"), &aDesc, (aProtocol.iServiceInfo & KSIGracefulClose) ? 1 : 0);
		}

	INFO_PRINTF3(_L("%S.iSockType                = %d"), &aDesc, aProtocol.iSockType);
	INFO_PRINTF3(_L("%S.iVersion                 = %S"), &aDesc, &versionName);
	INFO_PRINTF1(_L("------------------------------------------------------------"));
	}

TBool CT_DataCommsBase::CompareProtocolInfo(TProtocolDesc& aLhs, TProtocolDesc& aRhs)
	{
	TBool returnVal = ETrue;
	if (aLhs.iAddrFamily != aRhs.iAddrFamily ||
		aLhs.iByteOrder	!= aRhs.iByteOrder	||
	    aLhs.iMessageSize != aRhs.iMessageSize ||
	    aLhs.iName != aRhs.iName	||
	    aLhs.iNamingServices != aRhs.iNamingServices ||
	    aLhs.iProtocol != aRhs.iProtocol	||
	    aLhs.iSecurity != aRhs.iSecurity	||
	    aLhs.iServiceInfo != aRhs.iServiceInfo ||
		aLhs.iSockType != aRhs.iSockType	||
		aLhs.iVersion.Name() !=   aRhs.iVersion.Name())
		{
		returnVal = EFalse;
		}

	return returnVal;
	}

TInt CT_DataCommsBase::GetStringAsInt( const TDesC& aSection, const TDesC& aStrParameter )
	{
	
	TPtrC	valueStr(_L(""));
	TInt	valueInt;
	// First look for an integer value
	if (!GetIntFromConfig(aSection, aStrParameter, valueInt))
		{
		// Didn't find an integer value - look for a string
		TBool	hasStringVal = GetStringFromConfig(aSection, aStrParameter, valueStr);
		if (hasStringVal)
			{
			// Found a string error - convert into an integer
			if ( !GetIntValue(valueStr, valueInt) )
				{
				ERR_PRINTF2(_L("%S constant was not found"), &valueStr);
				SetBlockResult(EFail);
				}
			}
		else
			{
			valueInt = KErrNone;
			}
		}

	return valueInt;
	}
