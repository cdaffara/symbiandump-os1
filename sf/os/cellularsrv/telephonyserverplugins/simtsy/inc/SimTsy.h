// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// External header file for the Simulator TSY.
// 
//

/**
 @file
 @publishedPartner
 @test
*/

#ifndef __SIMTSY_H__
#define __SIMTSY_H__

#include <e32property.h>


//
// Uids used for The Simulator TSY System Agent.
// 20 Uids have been allocated for this purpose. Some of these are unused at present.
//
// 0x101F4126 changed to 0x101F4127 to avoid a potential clash in non-unique UIDs!
// So infact we have 19 UIDs allocated from 0x101F4127.
// 
#define KUidSASimTsyValue	(0x101F4127)	// < First Uid allocated for the Sim TSY
#define KUidSASimTsyEndValue	(0x101F413A)	// < Last Uid allocated for the Sim TSY


/**
P&S category for all SimTsy P&S properties.
*/
static const TUid KUidPSSimTsyCategory = {0x101F7989}; // RootServer's KUidCommsProcess


/**
P&S Test number property for SimTsy
*/
const TInt KPSSimTsyTestNumber = KUidSASimTsyValue+0;
const RProperty::TType KPSSimTsyTestNumberKeyType = RProperty::EInt;


/**
P&S Incoming voice call property for SimTsy
*/
const TInt KPSSimTsyIncomingVoiceCall = KUidSASimTsyValue+1;
const RProperty::TType KPSSimTsyIncomingVoiceCallKeyType = RProperty::EInt;


/**
P&S Incoming data call property for SimTsy
*/
const TInt KPSSimTsyIncomingDataCall = KUidSASimTsyValue+2;
const RProperty::TType KPSSimTsyIncomingDataCallKeyType = RProperty::EInt;


/**
P&S Registration status change property for SimTsy
*/
const TInt KPSSimTsyRegStatChange = KUidSASimTsyValue+3;
const RProperty::TType KPSSimTsyRegStatChangeKeyType = RProperty::EInt;


/**
P&S Signal strength status change property for SimTsy
*/
const TInt KPSSimTsySignalStrengthChange = KUidSASimTsyValue+4;
const RProperty::TType KPSSimTsySignalStrengthChangeKeyType = RProperty::EInt;


/**
P&S Reduce Sim.Tsy timers property for SimTsy
*/
const TInt KPSSimTsyTimersReduceTime = KUidSASimTsyValue+5;
const RProperty::TType KPSSimTsyTimersReduceTimeSignalKeyType = RProperty::EInt;


/**
Value used in P&S property KPSSimTsyTimersReduceTime to indicate a
request to reduce the time allocated to the simtsy timers.  The
timers are started when simtsy is initialised, and are set to the
durations specified in the simtsy configuration file.
*/
const TInt KReduceSimTsyTimers = KMaxTInt;


/**
P&S Network change QoS property for SimTsy
*/
const TInt KPSSimTsyNetworkQoSChange = KUidSASimTsyValue+6;
const RProperty::TType KPSSimTsyNetworkQoSChangeKeyType = RProperty::EInt;


/**
P&S Network reg status change property for SimTsy
*/
const TInt KPSSimtsyPacketServiceNtwkRegStatusChange = KUidSASimTsyValue+7;
const RProperty::TType KPSSimtsyPacketServiceNtwkRegStatusChangeKeyType = RProperty::EInt;


/**
P&S Network context status change property for SimTsy
*/
const TInt KPSSimtsyPacketContextStatusChange = KUidSASimTsyValue+8;
const RProperty::TType KPSSimtsyPacketContextStatusChangeKeyType = RProperty::EInt;


/**
P&S Test number in use property for SimTsy
*/
const TInt KPSSimTsyTestNumberInUse = KUidSASimTsyValue+9;
const RProperty::TType KPSSimTsyTestNumberInUseKeyType = RProperty::EInt;

/**
P&S Remote Hangup property for SimTsy
*/
const TInt KPSSimTsyRemoteHangup = KUidSASimTsyValue+10;
const RProperty::TType KPSSimTsyRemoteHangupKeyType = RProperty::EInt;

/**
P&S Remote KPSSimTsyForceIncomingContextActivation property for SimTsy
*/
const TInt KPSSimTsyForceIncomingContextActivation = KUidSASimTsyValue+11;
const RProperty::TType KPSSimTsyForceIncomingContextActivationType = RProperty::EInt;

/**
P&S Remote KPSSimTsyMbmsService property for SimTsy
*/
const TInt KPSSimTsyMbmsService = KUidSASimTsyValue+12;
const RProperty::TType KPSSimTsyMbmsServiceType = RProperty::EInt;

/**
Enumerated type used for identifying different timer events.
*/
enum TTimerEventId
	{
	ETimerIdAllTimers = KMaxTInt,
	ETimerIdDtmfCdmaGetMode =1,
	ETimerIdDtmfCdmaSetMode =2,
	ETimerIdDtmfCdmaNotifyMode =3,
	ETimerIdDtmfCdmaGetDuration =4,
	ETimerIdDtmfCdmaSetDuration =5,
	ETimerIdDtmfCdmaNotifyDuration =6,
	ETimerIdEmergEnd =7,
	ETimerIdFlashCallTxFlashEvent =8,
	ETimerIdFlashCallRxFlashEvent =9,
	ETimerIdInfoRecordRx =10,
	ETimerIdLocPriGet =11,
	ETimerIdLocPriSet =12,
	ETimerIdLocPriNotify =13,
	ETimerIdNtwkStatRegStat =14,
	ETimerIdNtwkStatNitz =15,
	ETimerIdNtwkStatRadioMode =16, 
	ETimerIdNtwkStatRoamStat =17,
	ETimerIdOtaGet =18,
	ETimerIdOtaSet =19,
	ETimerIdOtaStart =20,
	ETimerIdOtaStop =21,
	ETimerIdOtaVerify =22,
	ETimerIdOtaProcess =23,
	ETimerIdOtaSession =24,
	ETimerIdOtaGetBlockLength =25,
	ETimerIdOtaReadBlock =26,
	ETimerIdOtaWriteBlock =27,
	ETimerIdOtaNotifyBlock =28,
	ETimerIdPcktContextNormal =29,
	ETimerIdPcktContextNetWork =30,
	ETimerIdPcktContextStatusChange =31,
	ETimerIdPcktContextSetConfig =32,
	ETimerIdPcktContextInitContext =33,
	ETimerIdPcktContextCreateTft =34,
	ETimerIdPcktContextConfigChange =35,
	ETimerIdPcktQosNormal =36,
	ETimerIdPcktQosNetWork =37,
	ETimerIdPcktQosR5Network =38,
	ETimerIdPcktServNormal =39,
	ETimerIdPcktServPacket =40,
	ETimerIdPcktServReleaseMode =41,
	ETimerIdPcktServNtwkStatusChange =42,
	ETimerIdPcktServStatusSuspend =43,
	ETimerIdPcktServStatusDetach =44,
	ETimerIdPcktServStatusUnattach =45,
	ETimerIdPhBkStorReq =46,
	ETimerIdPhBkStorOOBWrite =47,
	ETimerIdPhBkStorOOBDelete =48,
	ETimerIdPhBkUSimStorReq =49,
	ETimerIdPhBkUSimStorOOBWrite =50,
	ETimerIdPhBkUSimStorOOBDelete	=51,
	ETimerIdSecurityNotifyEvent =52,
	ETimerIdSecurityNotifyLockInfo =53,
	ETimerIdSecurityUnlockNam =54,
	ETimerIdSecurityLockNam =55,
	ETimerIdPrefLangGet =56,
	ETimerIdPrefLangStore =57,
	ETimerIdPrefLangNotify =58,
	ETimerIdRuimGetRevision =59,
	ETimerIdSmsMessRx =60,
	ETimerIdSmsMessTx =61,
	ETimerIdSmsMessSmsp =62,
	ETimerIdTtyGet =63,
	ETimerIdTtySet =64,
	ETimerIdTtyNotify =65,
	ETimerIdSmsMessResumeReception =66,
	ETimerIdPcktQoSSet =67,
	ETimerIdContextActivationRequestedChange = 68,
	ETimerIdNotifyWlanDataEvent=69,
	ETimerIdNotifyUSimRel6Event =70,
	ETimerIdCellInfo = 71,
	ETimerIdMbmsPcktService=72,
	ETimerIdMbmsPcktContextSetConfig=73,
	ETimerIdMbmsUpdateSessionId=74,
	ETimerIdONStorReq =75,
	ETimerIdPcktServDynamicCaps=76
	};


_LIT(KConfigFilename,"c:\\config.txt");  //< Configuration filename.
_LIT(KPhoneName,"SimulatorPhone");       //< The name of the phone supported by the Simulator TSY.
_LIT(KVoiceLineName,"Voice");            //< The name of the voice line supported by the Simulator TSY.
_LIT(KDataLineName,"Data");              //< The name of the data line supported by the Simulator TSY.
_LIT8(KSIMTSYSubSystem, "SIMTSY");       ///< The name of the sub-system used by Comms Debug Utility.
_LIT8(KSubscriberIdDefault,"01234567890123");   /// < Default Subscriber ID for the Simulator TSY (used if no Subscriber ID tag is present)
_LIT8(KIccIdDefault, "01234567890123456789");   /// < Default ICC ID for the Simulator TSY (used if no ICC ID tag is present)

#endif // __SIMTSY_H__
