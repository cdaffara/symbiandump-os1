/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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


#ifndef __TFACCESSORYTESTCONTROLTYPES_H__
#define __TFACCESSORYTESTCONTROLTYPES_H__

#include <e32base.h>
#include <AccPolCommonNameValuePairs.h>
#include <AccPolGenericid.h>

const TUid    KTFAccessoryTestProperty=     {0x10200c70};
const TUint32 KTFAccessoryMethod          =  0x00001001;
const TUint32 KTFAccessoryNotifierMethod      =  0x00001002;

enum TTFAccessoryFunction
    {
    ETFAccessory_None                                           = 0,
    ETFAccessoryConnection_Open                                 = 1,
    ETFAccessoryConnection_Close                                = 2,
    ETFAccessorySingleConnection_Open                           = 3,
    ETFAccessorySingleConnection_Close                          = 4,
    ETFAccessoryControl_Open                                    = 5,
    ETFAccessoryControl_Close                                   = 6,
    ETFAccessorySettings_Open                                   = 7,
    ETFAccessorySettings_Close                                  = 8,
    ETFAccessoryMode_Open                                       = 9,
    ETFAccessoryMode_Close                                      = 10,
    ETFAccessoryAudioControl_Open                               = 11,
    ETFAccessoryAudioControl_Close                              = 12,
    ETFAccessoryBtControl_Open                                  = 13,
    ETFAccessoryBtControl_Close                                 = 14,
    ETFAccessoryBtConnectAccessory                              = 15,
    ETFAccessoryBtDisconnectAccessory                           = 16,
    ETFAccessoryNotifyAccessoryConnectionStatusChanged          = 17,
    ETFAccessoryNotifyNewAccessoryConnected                     = 18,
    ETFAccessoryNotifyAccessoryDisconnected                     = 19,
    ETFAccessoryNotifyAccessoryModeChanged                      = 20,
    ETFAccessoryAccessoryModeSync                               = 21,
    ETFAccessoryAccessoryModeASync                              = 22,
    ETFAccessoryNotifyBluetoothAudioLinkOpenReq                 = 23,
    ETFAccessoryAudioLinkOpen                                   = 24,
    ETFAccessoryNotifyAccessoryAudioLinkOpened                  = 25,
    ETFAccessoryBluetoothAudioLinkOpenedNotify                  = 26,
    ETFAccessoryBluetoothAudioLinkOpenResp                      = 27,
    ETFAccessoryNotifyAccessoryAudioLinkClosed                  = 28,
    ETFAccessoryNotifyBluetoothAudioLinkCloseReq                = 29,
    ETFAccessoryAccessoryAudioLinkClose                         = 30,
    ETFAccessoryBluetoothAudioLinkClosedNotify                  = 31,
    ETFAccessoryBluetoothAudioLinkCloseResp                     = 32,
    ETFAccessoryPublishAndSubscribe                             = 33,
    ETFAccessoryBtCancelConnectAccessory                        = 34,
    ETFAccessoryBtCancelDisconnectAccessory                     = 35,
    ETFNotifyIntAccessoryValueChanged                           = 36,
    ETFNotifyBoolAccessoryValueChanged                          = 37,
    ETFAccessoryIntGetValue                                     = 38,
    ETFAccessoryBoolGetValue                                    = 39,
    ETFAccessoryDes8GetValue                                    = 40,
    ETFConnectWiredAccessory                                    = 41,
    ETFDisconnectWiredAccessory                                 = 42,
    ETFSyncGetAccessoryConnectionStatus                         = 43,
    ETFASyncGetAccessoryConnectionStatus                        = 44,
    ETFSetIntValue                                              = 45,
    ETFSetBoolValue                                             = 46,
    ETFAccessoryValueChangedNotifyInt                           = 47,
    ETFAccessoryValueChangedNotifyBool                          = 48,
    ETFBTAccessoryValueChangedNotifyBool                        = 49,
    ETFSetHWDeviceSettings                                      = 50,
    ETFGetHWDeviceSettings                                      = 51,
    ETFGetSupportedHWDeviceSettings                             = 52,
    ETFSetIntAccessoryModeSetting                               = 53,
    ETFSetBoolAccessoryModeSetting                              = 54,
    ETFSetDesAccessoryModeSetting                               = 55,
    ETFGetIntAccessoryModeSetting                               = 56,
    ETFGetBoolAccessoryModeSetting                              = 57,
    ETFGetDesAccessoryModeSetting                               = 58,
    ETFSetSeveralAccessoryModeSetting                           = 59,
    ETFGetSeveralAccessoryModeSetting                           = 60,
    ETFAccessory_Cancel                                         = 61,
    ETFAccessorySynchronized                                    = 62,
    ETFFindAndCheckRequest                                      = 63,
    ETFCheckAllRequest                                          = 64,
    ETFCancelAccessoryAudioLinkOpen                             = 65,
    ETFCancelAccessoryAudioLinkClose                            = 66,
    ETFCancelNotifyAccessoryAudioLinkOpened                     = 67,
    ETFCancelNotifyAccessoryAudioLinkClosed                     = 68,
    ETFCancelNotifyProcessCommand                               = 69,
    ETFCancelConnectAccessory                                   = 70,
    ETFCancelDisconnectAccessory                                = 71,
    ETFCancelNotifyBluetoothAudioLinkOpenReq                    = 72,
    ETFCancelNotifyBluetoothAudioLinkCloseReq                   = 73,
    ETFCancelNotifyAccessoryConnectionStatusChanged             = 74,
    ETFCancelGetAccessoryConnectionStatus                       = 75,
    ETFCancelConnectAccessoryBT                                 = 76,
    ETFCancelDisconnectAccessoryBT                              = 77,
    ETFCancelNotifyAccessoryModeChanged                         = 78,
    ETFCancelGetAccessoryMode                                   = 79,
    ETFCancelNotifyNewAccessoryConnected                        = 80,
    ETFCancelNotifyAccessoryDisconnected                        = 81,
    ETFCancelNotifyAccessoryValueChanged                        = 82,
    ETFAccSrvSubBase                                            = 83,
    ETFCheckCapability                                          = 84,
    ETFAudioRoutingStatusNotify                                 = 85,
    ETFSelectionDialogText                                      = 86,
    ETFSelectionDialogCancel                                    = 87,
    ETFNotSupportedNote                                         = 88,
    ETFUISelectionIndex                                         = 89,
    ETFCheckCapabilityGroup                                     = 90,
    ETFValueRecordTests                                         = 91,
    ETFCheckConnectedAccessory									= 92,
    ETFAccessorySet_PubSub										= 93,
    };

enum TTFAccessoryParameterResult
    {
    ETFEAccUnsupportedConnected                                 = 0,
    ETFAccModeHandPortable                                      = 1,
    ETFAccModeWiredHeadset                                      = 2,
    ETFAccModeWirelessHeadset                                   = 3,
    ETFAccModeWiredCarKit                                       = 4,
    ETFAccModeWirelessCarKit                                    = 5,
    ETFAccModeTextDevice                                        = 6,
    ETFAccModeLoopset                                           = 7,
    ETFAccModeMusicStand                                        = 8,
    };


enum TTFRequestType
    {
    ReqAccRequestNone                                                   =0,
    ReqNotifyNewAccessoryConnected                                      =1,
    ReqNotifyAccessoryDisconnected                                      =2,
    ReqNotifyAccessoryModeChanged                                       =3,
    ReqNotifyAccessoryConnectionStatusChanged                           =4,
    ReqAccessoryAudioLinkOpen                                           =5,
    ReqNotifyAccessoryAudioLinkOpened                                   =6,
    ReqNotifyAccessoryAudioLinkClosed                                   =7,
    ReqAccessoryAudioLinkClose                                          =8,
    ReqNotifyBluetoothAudioLinkOpenReq                                  =9,
    ReqNotifyBluetoothAudioLinkCloseReq                                 =10,
    ReqConnectBtAccessory                                               =11,
    ReqDisconnectBtAccessory                                            =12,
    ReqNotifyBooleanAccessoryValueChanged                               =13,
    ReqNotifyIntegerAccessoryValueChanged                               =14,
    ReqGetAccessoryConnectionStatus                                     =15,
    ReqAccessoryConnection                                              =16,
    ReqAccessoryDisconnection                                           =17,
    ReqNotifyAccessoryValueChanged                                      =18,
    ReqGetAccessoryMode                                                 =19
    };

enum TTFASYReference
    {
    ETFAsyNone  =0,
    ETFAsyConn,
    ETFAsyDisc,
    ETFAsyIntNotify,
    ETFAsyBoolNotify,
    ETFBTALinkClosedNo,
    ETFBTALinkOpenedNo,
    ETFAsySetCapability,
    ETFAsySetBoolValue,
    ETFAsyConnOpenCable,
    ETFAsyUpdateTVOut,
    ETFAsyUpdateHeadset,
    ETFAsyConnectECIAccessory,
    ETFAsyConnectHeadset,
    ETFAsyConnectOpenCable3Poles
    };


struct TTFAccessoryTestCaseStateControl
    {
    TTFAccessoryFunction    iAccessoryFunction;
    TInt                    iGid; //defines the index of the gereric ID to be used in TestControl
    TInt64                  iArg1;
    TInt64                  iArg2;
    TUint32         	    iArg3;
    TInt64                  iArg4; //Reserved
    TInt                    iTestCaseID;
    TTFASYReference         iMethod;
    TTFRequestType          iRequestType;
    TInt                    iExpectedResult;
    TInt                    iTimer;//Specifies the time which is waited before next state is executed
    };

struct TTFAccessoryPublishAndSubscribe
    {
    TInt                           iTestCaseID;
    TTFASYReference                iMethod;
    TInt64                         iParam1;
    TUint32         iParam2;
    TInt                           iTimeMs;//0 means that command is server synchronously
    TAccPolGenericID               iGenericID;
    };

const TInt KTFErrAccessoryNotificationMissing               = -3000;
const TInt KTFErrAccessoryServiceMissing                    = -4000;
const TInt KTFErrAccessoryUnexpectedStatus                  = -5000;
const TInt KTFErrAccessoryUnexpectedStatusParameter         = -6000;
const TInt KTFErrAccessoryUnexpectedArg1                    = -7000;
const TInt KTFErrAccessoryUnexpectedArg2                    = -8000;
const TInt KTFErrAccessoryUnexpectedArg3                    = -8500;
const TInt KTFErrAccessoryUnexpectedResult                  = -9000;


const TInt64      KBTAddrCarkit            = 0x001122334455;
const TInt64      KBTAddrHeadsetHSPAndHFP  = 0x112233445566;
const TInt64      KBTAddrHeadsetHSP        = 0x223344556677;
const TInt64      KBTAddrHeadsetHFP        = 0x334455667788;
const TInt64      KBTAddrHeadsetEmptyPR    = 0x445566778899;
const TInt64      KBTAddrHeadsetNokiaSPId  = 0x556677889900;
const TInt64      KBTAddrNoResponse        = 0x667788990011;
const TInt64      KBTAddrErrCode           = 0x778899001122;
const TInt64      KBTAllBitsOn             = 0x889900112233;
const TInt64      KBTAddrHeadsetAVRCP      = 0x990011223344;
const TInt64      KBTAddrHeadseA2DP        = 0x111111111111;


const TInt KTFDontDeleteObserver    = -999;

#endif
