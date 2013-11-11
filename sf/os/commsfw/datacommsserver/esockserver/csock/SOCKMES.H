// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file
 @internalComponent
*/

#if !defined(__SOCKMES_H__)
#define __SOCKMES_H__

#include <e32def.h>

/**
Socket server messages
@internalComponent
*/
enum TSockMess
	{
	EIPCUninitialised = -1,
    //When editing this list make sure you add enums to their policy range so that
	//you don't need to extend the policy file (ss_secpol.cpp) unless a new capability
	//is really required. And be sure to add them to the logging file (ss_log.cpp)


	// policyNetworkControl
	ESSInstallExtension = 0,


	// policyCustom
	ECNControl = 16,


	// policyDeferred
	ESoCreate = 32,
	ESoSend = 33,
	ESoSendNoLength = 34,
	ESoRecv = 35,
	ESoRecvNoLength = 36,
	ESoRecvOneOrMore = 37,
	ESoRecvOneOrMoreNoLength = 38,
	ESoRead = 39,
	ESoWrite = 40,
	ESoSendTo = 41,
	ESoSendToNoLength = 42,
	ESoRecvFrom = 43,
	ESoRecvFromNoLength = 44,
	ESoConnect = 45,
	ESoBind = 46,
	ESoAccept = 47,
	ESoListen = 48,
	ESoSetOpt = 49,
	ESoGetOpt = 50,
	ESoIoctl = 51,
	ESoGetDiscData = 52,
	ESoShutdown = 53,
	ESoTransfer = 54,
	EHRCreate = 55,
	EHRGetByName = 56,
	EHRNext = 57,
	EHRGetByAddress = 58,
	EHRGetHostName = 59,
	EHRSetHostName = 60,
	ESRCreate = 61,
	ESRGetByName = 62,
	ESRGetByNumber = 63,
	ESRRegisterService = 64,
	ESRRemoveService = 65,
	ENDCreate = 66,
	ENDQuery = 67,
	ENDAdd = 68,
	ENDRemove = 69,
	ESoCreateWithConnection = 70,
	EHRCreateWithConnection = 71,
	ECNCreate = 72,
	ECNCreateWithName = 73,
	ECNStart = 74,
	ECNSetStartPrefs = 75,
	ECNStop = 76,
	ECNGetIntSetting = 77,
	ECNGetBoolSetting = 78,
	ECNGetDes8Setting = 79,
	ECNGetDes16Setting = 80,
	ECNGetLongDesSetting = 81,
	ECNAttach = 82,
	ESCPSStop = 83,
    EHrQuery = 84,
    EHrQueryNext = 85,
    ECNIoctl = 86,
	ECommsApiExtBindIface = 87,
	ECommsApiExtIfaceSendReceive = 88,
	ECommsApiExtIfaceSend = 89,
	ECommsApiExtIfaceClose = 90,
	ECNWaitForIncoming = 91,
	ECNCancelWaitForIncoming = 92,
	ESCStart = 93,
	ESCStop = 94,
	ESCAccept = 95,
	ESCEventAllNotifications = 96,
	ESCEventNotificationSetup = 97,
	ESCProgressNotification = 98,
	ESCCancelProgressNotification = 99,
	ESCProgress = 100,
	ESCIsSubConnectionActiveRequest = 101,
	ESCIsSubConnectionActiveCancel = 102,
	ECNGetOrSetParameters = 103,
	EHRSetOpt = 104,

	// policyPass
	ESSNumProtocols = 128,
	ESSProtocolInfo = 129,
	ESSProtocolInfoByName = 130,
	ESSProtocolStart = 131,
	ESSProtocolStop = 132,
	ESoCreateNull = 133,
	ESoGetLocalName = 134,
	ESoGetRemoteName = 135,
	ESoClose = 136,
	ESoCancelIoctl = 137,
	ESoCancelRecv = 138,
	ESoCancelSend = 139,
	ESoCancelConnect = 140,
	ESoCancelAccept = 141,
	ESoCancelAll = 142,
	ESoSocketInfo = 143,
	ESoReference = 144,
	EHRCancel = 145,
	EHRClose = 146,
	ESRCancel = 147,
	ESRClose = 148,
	ENDCancel = 149,
	ENDClose = 150,
	ECNClose = 151,
	ECNReference = 152,
	ECNProgress = 153,
	ECNProgressNotification = 154,
	ECNCancelProgressNotification = 155,
	ECNLastProgressError = 156,
	ECNServiceChangeNotification = 157,
	ECNCancelServiceChangeNotification = 158,
	ECNEnumerateConnections = 159,
	ECNGetConnectionInfo = 160,
	ECNAllInterfaceNotification = 161,
	ECNCancelAllInterfaceNotification = 162,
	ESCPSProgress = 163,
	ESCPSProgressNotification = 164,
	ESCPSCancelProgressNotification = 165,
	ECNEnumerateSubConnections = 166,
	ESCPSGetSubConnectionInfo = 167,
	ECNAllSubConnectionNotification = 168,
	ECNCancelAllSubConnectionNotification = 169,
	ESCPSDataTransferred = 170,
	ESCPSDataTransferredCancel = 171,
	ESCPSDataSentNotificationRequest = 172,
	ESCPSDataSentNotificationCancel = 173,
	ESCPSDataReceivedNotificationRequest = 174,
	ESCPSDataReceivedNotificationCancel = 175,
	ESCPSIsSubConnectionActiveRequest = 176,
	ESCPSIsSubConnectionActiveCancel = 177,
	ESCCreate = 178,
	ESCClose = 179,
	ESCAddSocket = 180,
	ESCRemoveSocket = 181,
	ESCSetParameters = 182,
	ESCGetParameters = 183,
	ESCGetParametersLength = 184,
	ESCEventNotification = 185,
	ESCEventNotificationCancel = 186,
	ESCControl = 187,
	ESoCreateWithSubConnection = 188,
	ECNCancelIoctl = 189,
	ESSRequestOptimalDealer = 190,
	ECNGetParametersResponseLength = 191,
	ECNGetParametersResponse = 192,


	// policyVIDandNetworkControl
	ESSDbgMarkHeap = 208,
	ESSDbgCheckHeap = 209,
	ESSDbgMarkEnd = 210,
	ESSDbgFailNext = 211,
	ESSDbgFailNextMbuf = 212,
	ESSDbgSetMbufPoolLimit = 213,
	ESSDbgCheckMbuf = 214,
	ESSDbgMbufFreeSpace = 215,
	ESSDbgMbufTotalSpace = 216,
 	ESSDbgCheckFailNext = 217,
	ESSDbgControl = 218,
    ESSExclusiveMode = 219,
	ESSClearExclusiveMode = 220,


    //When editing this list, please do not forget to update
    //the associated policy file  (ss_secpol.cpp)
    // .. and the logging file (ss_log.cpp)

	// Leave this at the end
	ESSInvalidFunction
    };

class TSockOpt
/**
@internalComponent
*/
	{
public:
	TUint optionName;
	const TAny* optionVal;
	TInt optionLen;
	};

/** Maximum length of a SocketServer name (the server name of a dealer).
@internalComponent
*/
const TInt KCFMaxSocketServerName = 0x20;

/** Name of a SocketServer (the server name of a dealer).
@internalComponent
*/
typedef TBuf<KCFMaxSocketServerName> TServerName;

#endif
