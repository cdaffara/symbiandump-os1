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

const TUid    KTFAccessoryTestProperty    =  {0x10200c70};
const TUint32 KTFAccessoryMethod          =  0x00001001;
const TUint32 KTFAccessoryNotifierMethod  =  0x00001002;
const TUint32 KTFAccessoryObjectMethod    =  0x00001003;

enum TTFAccessoryFunction
    {
    ETFAccessory_None = 0,
    ETFAccessoryConnection_Open,
    ETFAccessoryConnection_Close,
    ETFAccessorySingleConnection_Open,
    ETFAccessorySingleConnection_Close,
    ETFAccessoryControl_Open,
    ETFAccessoryControl_Close,
    ETFAccessorySettings_Open,
    ETFAccessorySettings_Close,
    ETFAccessoryMode_Open,
    ETFAccessoryMode_Close,
    ETFAccessoryAudioControl_Open,
    ETFAccessoryAudioControl_Close,
    ETFAccessoryBtControl_Open,
    ETFAccessoryBtControl_Close,
    ETFAccessoryBtConnectAccessory,
    ETFAccessoryBtDisconnectAccessory,
    ETFAccessoryNotifyAccessoryConnectionStatusChanged,
    ETFAccessoryNotifyNewAccessoryConnected,
    ETFAccessoryNotifyAccessoryDisconnected,
    ETFAccessoryNotifyAccessoryModeChanged,
    ETFAccessoryAccessoryModeSync,
    ETFAccessoryAccessoryModeASync,
    ETFAccessoryNotifyBluetoothAudioLinkOpenReq,
    ETFAccessoryAudioLinkOpen,
    ETFAccessoryNotifyAccessoryAudioLinkOpened,
    ETFAccessoryBluetoothAudioLinkOpenedNotify,
    ETFAccessoryBluetoothAudioLinkOpenResp,
    ETFAccessoryNotifyAccessoryAudioLinkClosed,
    ETFAccessoryNotifyBluetoothAudioLinkCloseReq,
    ETFAccessoryAccessoryAudioLinkClose,
    ETFAccessoryBluetoothAudioLinkClosedNotify,
    ETFAccessoryBluetoothAudioLinkCloseResp,
    ETFAccessoryPublishAndSubscribe,
    ETFAccessoryBtCancelConnectAccessory,
    ETFAccessoryBtCancelDisconnectAccessory,
    ETFNotifyIntAccessoryValueChanged,
    ETFNotifyBoolAccessoryValueChanged,
    ETFAccessoryIntGetValue,
    ETFAccessoryBoolGetValue,
    ETFAccessoryDes8GetValue,
    ETFConnectWiredAccessory,
    ETFDisconnectWiredAccessory,
    ETFSyncGetAccessoryConnectionStatus,
    ETFASyncGetAccessoryConnectionStatus,
    ETFSetIntValue,
    ETFSetBoolValue,
    ETFAccessoryValueChangedNotifyInt,
    ETFAccessoryValueChangedNotifyBool,
    ETFBTAccessoryValueChangedNotifyBool,
    ETFSetHWDeviceSettings,
    ETFGetHWDeviceSettings,
    ETFGetSupportedHWDeviceSettings,
    ETFSetIntAccessoryModeSetting,
    ETFSetBoolAccessoryModeSetting,
    ETFSetDesAccessoryModeSetting,
    ETFGetIntAccessoryModeSetting,
    ETFGetBoolAccessoryModeSetting,
    ETFGetDesAccessoryModeSetting,
    ETFSetSeveralAccessoryModeSetting,
    ETFGetSeveralAccessoryModeSetting,
    ETFAccessory_Cancel,
    ETFAccessorySynchronized,
    ETFFindAndCheckRequest,
    ETFCheckAllRequest,
    ETFCancelAccessoryAudioLinkOpen,
    ETFCancelAccessoryAudioLinkClose,
    ETFCancelNotifyAccessoryAudioLinkOpened,
    ETFCancelNotifyAccessoryAudioLinkClosed,
    ETFCancelNotifyProcessCommand,
    ETFCancelConnectAccessory,
    ETFCancelDisconnectAccessory,
    ETFCancelNotifyBluetoothAudioLinkOpenReq,
    ETFCancelNotifyBluetoothAudioLinkCloseReq,
    ETFCancelNotifyAccessoryConnectionStatusChanged,
    ETFCancelGetAccessoryConnectionStatus,
    ETFCancelConnectAccessoryBT,
    ETFCancelDisconnectAccessoryBT,
    ETFCancelNotifyAccessoryModeChanged,
    ETFCancelGetAccessoryMode,
    ETFCancelNotifyNewAccessoryConnected,
    ETFCancelNotifyAccessoryDisconnected,
    ETFCancelNotifyAccessoryValueChanged,
    ETFAccSrvSubBase,
    ETFCheckCapability,
    ETFAudioRoutingStatusNotify,
    ETFSelectionDialogText,
    ETFSelectionDialogCancel,
    ETFNotSupportedNote,
    ETFUISelectionIndex,
    ETFCheckCapabilityGroup,
    ETFValueRecordTests,
    ETFCheckConnectedAccessory,
    ETFAccessorySet_PubSub,
    ETFClearStack,
    ETFCancelConnects,
    ETFCallTClassMethods,
    ETFBTAccessoryValueChangedNotifyInt,
    ETFAccessorySet_ObjectPubSub,    
    ETFAccessorySet_TopologyObjectsPubSub,
    ETFAccessoryGetTopologyObjects,
    ETFAccessoryGetVolumeControlObjects,
    ETFAccessorySetVolumeControlObjects,
    ETFAccessoryGetMuteControlObjects,
    ETFAccessorySetMuteControlObjects,
    ETFAccessoryObjectParseTopology,
    ETFAccessoryGetStreamObjects,
    ETFAccessorySetStreamObjects,	
    ETFObjectBaseSerializationTests,   
    ETFAccessoryGetStreamObjectsFromPolicy,
    ETFNotifyObjectAccessoryValueChanged,
    ETFFindAndCheckObjectRequest,
    ETFSetHdmiObject_PubSub,
    ETFGetHdmiObject,
    ETFCheckHdmiObject                                     
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
    ETFAccModeMusicStand                                        = 8
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
    ReqGetAccessoryMode                                                 =19,
    ReqNotifyObjectAccessoryValueChanged                                =20
    };

enum TTFASYReference
    {
    ETFAsyNone  =0,
    ETFAsyConn,
    ETFAsyDisc,
    ETFAsyIntNotify,
    ETFAsyBoolNotify,
    ETFAsyObjectNotify,
    ETFBTALinkClosedNo,
    ETFBTALinkOpenedNo,
    ETFAsySetCapability,
    ETFAsySetBoolValue,
    ETFAsyConnOpenCable,
    ETFAsyUpdateTVOut,
    ETFAsyUpdateHeadset,
    ETFAsyConnectECIAccessory,
    ETFAsyConnectHeadset,
    ETFAsyConnectOpenCable3Poles,
    ETFAsyConnUSB,
    ETFAsyConnUSB2,
    ETFAsyConnHDMI
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
    TUint32                        iParam2;
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
