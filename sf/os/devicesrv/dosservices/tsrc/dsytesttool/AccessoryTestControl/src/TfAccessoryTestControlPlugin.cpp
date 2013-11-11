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


// INCLUDE FILES
#include <tftestmacros.h>
#include <ctfatestenvironment.h>
#include "siftrace.h"
#include "CtfAccessoryTestControlPlugin.h"
#include "CtfAccessoryTestControl.h"
#include "CtfAccessoryTestCaseParamControl.h"
#include "CtfAccessoryTestCaseControl.h"
#include <AccPolProprietaryNameValuePairs.h>



//M10703 KAccAudioOutConnector

// LOCAL FUNCTIONS
static void AccessorySessionsModuleTestSuiteL( CTFATestSuite* aSuite );
static void WiredAccessoryModuleTestSuiteL( CTFATestSuite* aSuite );
static void WirelessAccessoryModuleTestSuiteL( CTFATestSuite* aSuite );
static void AccessoryModeModuleTestSuiteL( CTFATestSuite* aSuite );
static void BtAndAudioLinkModuleTestSuiteL( CTFATestSuite* aSuite );
static void AccessorySettingsModuleTestSuiteL( CTFATestSuite* aSuite );
static void SpecialModuleTestSuiteL( CTFATestSuite* aSuite );
static void MultibleConnectionsModuleTestSuiteL( CTFATestSuite* aSuite );
static void SignatureModuleTestSuiteL( CTFATestSuite* aSuite );
static void PolicyModuleTestSuiteL( CTFATestSuite* aSuite );
static void UsbObjectModuleTestSuiteL( CTFATestSuite* aSuite );
static void HdmiObjectModuleTestSuiteL( CTFATestSuite* aSuite );

//
// [M10101] [[AccServer] Open And Close Accessory Connection]
//
const TTFAccessoryTestCaseStateControl KTFAccessoryConnectionOpenCloseStates[2] =
    {
    { ETFAccessoryConnection_Open,  0, 0, 0, 0, 0, 10101, ETFAsyNone, ReqAccRequestNone, KErrNone, 0},
    { ETFAccessoryConnection_Close, 0, 0, 0, 0, 0, 10101, ETFAsyNone, ReqAccRequestNone, KErrNone, 0}
    };


//
// [M10102] [[AccServer] Open And Close Accessory Single Connection]
//
const TTFAccessoryTestCaseStateControl KTFAccessorySingleConnectionOpenCloseStates[2] =
    {
    { ETFAccessorySingleConnection_Open,  0, 0, 0, 0, 0, 10102, ETFAsyNone, ReqAccRequestNone, KErrNone, 0},
    { ETFAccessorySingleConnection_Close, 0, 0, 0, 0, 0, 10102, ETFAsyNone, ReqAccRequestNone, KErrNone, 0}
    };

//
// [M10103] [[AccServer] Open And Close Accessory Control]
//
const TTFAccessoryTestCaseStateControl KTFAccessoryControlOpenCloseStates[2] =
    {
    { ETFAccessoryControl_Open,  0, 0, 0, 0, 0, 10103, ETFAsyNone, ReqAccRequestNone, KErrNone, 0},
    { ETFAccessoryControl_Close, 0, 0, 0, 0, 0, 10103, ETFAsyNone, ReqAccRequestNone, KErrNone, 0}
    };
//
// [M10104] [[AccServer] Open And Close Accessory Settings]
//
const TTFAccessoryTestCaseStateControl KTFAccessorySettingOpenCloseStates[2] =
    {
    { ETFAccessorySettings_Open,  0, 0, 0, 0, 0, 10104, ETFAsyNone, ReqAccRequestNone, KErrNone, 0},
    { ETFAccessorySettings_Close, 0, 0, 0, 0, 0, 10104, ETFAsyNone, ReqAccRequestNone, KErrNone, 0}
    };

//
// [M10105] [[AccServer] Open And Close Accessory Mode]
//
const TTFAccessoryTestCaseStateControl KTFAccessoryModeOpenCloseStates[2] =
    {
    { ETFAccessoryMode_Open,  0, 0, 0, 0, 0, 10105, ETFAsyNone, ReqAccRequestNone, KErrNone, 0},
    { ETFAccessoryMode_Close, 0, 0, 0, 0, 0, 10105, ETFAsyNone, ReqAccRequestNone, KErrNone, 0}
    };

//
// [M10106] [[AccServer] Open And Close Accessory Audio Control]
//
const TTFAccessoryTestCaseStateControl KTFAccessoryAudioControlOpenCloseStates[2] =
    {
    { ETFAccessoryAudioControl_Open,  0, 0, 0, 0, 0, 10106, ETFAsyNone, ReqAccRequestNone, KErrNone, 0},
    { ETFAccessoryAudioControl_Close, 0, 0, 0, 0, 0, 10106, ETFAsyNone, ReqAccRequestNone, KErrNone, 2000000}
    };

//
// [M10107] [[AccServer] Open And Close Accessory BT Control]
//
const TTFAccessoryTestCaseStateControl KTFAccessoryBTControlOpenCloseStates[2] =
    {
    { ETFAccessoryBtControl_Open,  0, 0, 0, 0, 0, 10107, ETFAsyNone, ReqAccRequestNone, KErrNone, 0},
    { ETFAccessoryBtControl_Close, 0, 0, 0, 0, 0, 10107, ETFAsyNone, ReqAccRequestNone, KErrNone, 2000000}
    };

//
// [M10108] [[AccServer] All Async method called twice]
//
const TTFAccessoryTestCaseStateControl KTFAccessoryAsyncsCalledTwice[]=
    {
    { ETFAccessoryConnection_Open,                        0, 0, 0, 0,           0, 10108, ETFAsyNone, ReqAccRequestNone,                                KErrNone, 1000000},
    { ETFAccessorySingleConnection_Open,                  0, 0, 0, 0,           0, 10108, ETFAsyNone, ReqAccRequestNone,                                KErrNone, 1000000},
    { ETFAccessoryControl_Open,                           0, 0, 0, 0,           0, 10108, ETFAsyNone, ReqAccRequestNone,                                KErrNone, 1000000},
    { ETFAccessorySettings_Open,                          0, 0, 0, 0,           0, 10108, ETFAsyNone, ReqAccRequestNone,                                KErrNone, 1000000},
    { ETFAccessoryMode_Open,                              0, 0, 0, 0,           0, 10108, ETFAsyNone, ReqAccRequestNone,                                KErrNone, 1000000},
    { ETFAccessoryAudioControl_Open,                      0, 0, 0, 0,           0, 10108, ETFAsyNone, ReqAccRequestNone,                                KErrNone, 1000000},
    { ETFAccessoryBtControl_Open,                         0, 0, 0, 0,           0, 10108, ETFAsyNone, ReqAccRequestNone,                                KErrNone, 1000000},

    //***SingleConnection***
    { ETFAccessoryNotifyNewAccessoryConnected,            0, 0, 0, 0,           0, 10108, ETFAsyNone, ReqAccRequestNone,                                KErrNone, 2000000},
    { ETFAccessoryNotifyNewAccessoryConnected,            0, 0, 0, 0,           0, 10108, ETFAsyNone, ReqAccRequestNone,                                KErrNone, 2000000},
    { ETFFindAndCheckRequest,                             0, 0, 0, 0,           0, 10108, ETFAsyNone, ReqNotifyNewAccessoryConnected,                   KErrAlreadyExists, 0},

    { ETFAccessoryNotifyAccessoryDisconnected,            0, 0, 0, 0,           0, 10108, ETFAsyNone, ReqAccRequestNone,                                KErrNone, 2000000},
    { ETFAccessoryNotifyAccessoryDisconnected,            0, 0, 0, 0,           0, 10108, ETFAsyNone, ReqAccRequestNone,                                KErrNone, 2000000},
    { ETFFindAndCheckRequest,                             0, 0, 0, 0,           0, 10108, ETFAsyNone, ReqNotifyAccessoryDisconnected,                   KErrAlreadyExists, 0},

    { ETFNotifyIntAccessoryValueChanged,                  0, 0, 0, 0,           0, 10108, ETFAsyNone, ReqAccRequestNone,                                KErrNone, 2000000},
    { ETFNotifyIntAccessoryValueChanged,                  0, 0, 0, 0,           0, 10108, ETFAsyNone, ReqAccRequestNone,                                KErrNone, 2000000},
    { ETFFindAndCheckRequest,                             0, 0, 0, 0,           0, 10108, ETFAsyNone, ReqNotifyIntegerAccessoryValueChanged,            KErrArgument, 0},

    { ETFNotifyBoolAccessoryValueChanged,                 0, 1, 0, 0,           0, 10108, ETFAsyNone, ReqAccRequestNone,                                KErrNone, 2000000},
    { ETFNotifyBoolAccessoryValueChanged,                 0, 1, 0, 0,           0, 10108, ETFAsyNone, ReqAccRequestNone,                                KErrNone, 2000000},
    { ETFFindAndCheckRequest,                             0, 0, 0, 0,           0, 10108, ETFAsyNone, ReqNotifyBooleanAccessoryValueChanged,            KErrArgument, 0},

    { ETFCancelNotifyNewAccessoryConnected,               0, 0, 0, 0,           0, 10108, ETFAsyNone, ReqAccRequestNone,                                KErrNone, 2000000},
    { ETFCancelNotifyAccessoryDisconnected,               0, 0, 0, 0,           0, 10108, ETFAsyNone, ReqAccRequestNone,                                KErrNone, 2000000},
    { ETFCancelNotifyAccessoryValueChanged,               0, 0, 0, 0,           0, 10108, ETFAsyNone, ReqAccRequestNone,                                KErrNone, 2000000},
    { ETFClearStack,                                      0, 0, 0, 0,           0, 10108, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 2000000},
    
      //cancel with name
    { ETFNotifyIntAccessoryValueChanged,                  0, 0, 0, KAccAudioOutputType, 0, 10108, ETFAsyNone, ReqAccRequestNone,                        KErrNone, 2000000},
    { ETFNotifyIntAccessoryValueChanged,                  0, 0, 0, 0,                   0, 10108, ETFAsyNone, ReqAccRequestNone,                        KErrNone, 2000000},
    { ETFFindAndCheckRequest,                             0, 0, 0, 0,                   0, 10108, ETFAsyNone, ReqNotifyIntegerAccessoryValueChanged,    KErrArgument, 0},
    { ETFNotifyBoolAccessoryValueChanged,                 0, 1, 0, KAccMuteAllAudio,    0, 10108, ETFAsyNone, ReqAccRequestNone,                        KErrNone, 2000000},
    { ETFNotifyBoolAccessoryValueChanged,                 0, 1, 0, 0,                   0, 10108, ETFAsyNone, ReqAccRequestNone,                        KErrNone, 2000000},
    { ETFFindAndCheckRequest,                             0, 0, 0, 0,                   0, 10108, ETFAsyNone, ReqNotifyBooleanAccessoryValueChanged,    KErrArgument, 0},
    { ETFCancelNotifyAccessoryValueChanged,               0, 0, 0, KAccAudioOutputType, 0, 10108, ETFAsyNone, ReqAccRequestNone,                        KErrNone, 2000000},
    { ETFCancelNotifyAccessoryValueChanged,               0, 0, 0, KAccMuteAllAudio,    0, 10108, ETFAsyNone, ReqAccRequestNone,                        KErrNone, 2000000},
    { ETFClearStack,                                      0, 0, 0, 0,           0, 10108, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 2000000},
    
    //***AccessoryMode***
    { ETFAccessoryNotifyAccessoryModeChanged,             0, 0, 0, 0,           0, 10108, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 2000000},
    { ETFAccessoryAccessoryModeASync,                     0, 0, 0, 0,           0, 10108, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 2000000},
    { ETFAccessoryNotifyAccessoryModeChanged,             0, 0, 0, 0,           0, 10108, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 2000000},
    { ETFFindAndCheckRequest,                             0, 0, 0, 0,           0, 10108, ETFAsyNone, ReqNotifyAccessoryModeChanged,             KErrAlreadyExists, 0},    
    { ETFAccessoryAccessoryModeASync,                     0, 0, 0, 0,           0, 10108, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 2000000},
            
    { ETFCancelGetAccessoryMode,                          0, 0, 0, 0,           0, 10108, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 2000000},
    { ETFCancelNotifyAccessoryModeChanged,                0, 0, 0, 0,           0, 10108, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 2000000},
    { ETFClearStack,                                      0, 0, 0, 0,           0, 10108, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 2000000},
    
    //***AccessoryConnection***
    { ETFAccessoryNotifyAccessoryConnectionStatusChanged, 0, 0, 0, 0,           0, 10108, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 2000000},
    { ETFASyncGetAccessoryConnectionStatus,               0, 0, 0, 0,           0, 10108, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 2000000},
    { ETFAccessoryNotifyAccessoryConnectionStatusChanged, 0, 0, 0, 0,           0, 10108, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 2000000},
    { ETFFindAndCheckRequest,                             0, 0, 0, 0,           0, 10108, ETFAsyNone, ReqNotifyAccessoryConnectionStatusChanged, KErrAlreadyExists, 0},    
    { ETFASyncGetAccessoryConnectionStatus,               0, 0, 0, 0,           0, 10108, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 2000000},
            
    { ETFCancelGetAccessoryConnectionStatus,              0, 0, 0, 0,           0, 10108, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 2000000},
    { ETFCancelNotifyAccessoryConnectionStatusChanged,    0, 0, 0, 0,           0, 10108, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 2000000},
    { ETFClearStack,                                      0, 0, 0, 0,           0, 10108, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 2000000},

    //***AccessoryAudioControl***
    
    { ETFAccessoryNotifyNewAccessoryConnected,            0, 0, 0, 0,           0, 10108, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 1000000},
    { ETFConnectWiredAccessory,                           0, 0x010001, 0, 0,    0, 10108, ETFAsyConn, ReqAccRequestNone,                         KErrNone, 3000000},
    { ETFFindAndCheckRequest,                             0, 0, 0, 0,           0, 10108, ETFAsyNone, ReqNotifyNewAccessoryConnected,            KErrNone, 0}, 
    
    { ETFAccessoryAudioLinkOpen,                          0, 0, 0, 0,           0, 10108, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 2000000},
    { ETFAccessoryAccessoryAudioLinkClose,                0, 0, 0, 0,           0, 10108, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 2000000},
    { ETFAccessoryNotifyAccessoryAudioLinkOpened,         0, 0, 0, 0,           0, 10108, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 2000000},
    { ETFAccessoryNotifyAccessoryAudioLinkClosed,         0, 0, 0, 0,           0, 10108, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 2000000},
    { ETFAccessoryAudioLinkOpen,                          0, 0, 0, 0,           0, 10108, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 2000000},
    { ETFFindAndCheckRequest,                             0, 0, 0, 0,           0, 10108, ETFAsyNone, ReqAccessoryAudioLinkOpen,                 KErrAlreadyExists, 0},    
    { ETFAccessoryAccessoryAudioLinkClose,                0, 0, 0, 0,           0, 10108, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 2000000},
    { ETFFindAndCheckRequest,                             0, 0, 0, 0,           0, 10108, ETFAsyNone, ReqAccessoryAudioLinkClose,                KErrAlreadyExists, 0},    
    { ETFAccessoryNotifyAccessoryAudioLinkOpened,         0, 0, 0, 0,           0, 10108, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 2000000},
    { ETFFindAndCheckRequest,                             0, 0, 0, 0,           0, 10108, ETFAsyNone, ReqNotifyAccessoryAudioLinkOpened,         KErrAlreadyExists, 0},    
    { ETFAccessoryNotifyAccessoryAudioLinkClosed,         0, 0, 0, 0,           0, 10108, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 2000000},
    { ETFFindAndCheckRequest,                             0, 0, 0, 0,           0, 10108, ETFAsyNone, ReqNotifyAccessoryAudioLinkClosed,         KErrAlreadyExists, 0},    
    
    { ETFCancelAccessoryAudioLinkOpen,                    0, 0, 0, 0,           0, 10108, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 2000000},
    { ETFCancelAccessoryAudioLinkClose,                   0, 0, 0, 0,           0, 10108, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 2000000},
    { ETFCancelNotifyAccessoryAudioLinkOpened,            0, 0, 0, 0,           0, 10108, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 2000000},
    { ETFCancelNotifyAccessoryAudioLinkClosed,            0, 0, 0, 0,           0, 10108, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 2000000},

    { ETFDisconnectWiredAccessory,                        0, 0, 0, 0,           0, 10108, ETFAsyDisc, ReqAccRequestNone,                         KErrNone, 2000000},
    { ETFClearStack,                                      0, 0, 0, 0,           0, 10108, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 2000000},

    //***AccessoryBTControl
    { ETFAccessoryNotifyBluetoothAudioLinkOpenReq,        0, 0, 0, 0,           0, 10108, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 2000000},
    { ETFAccessoryNotifyBluetoothAudioLinkCloseReq,       0, 0, 0, 0,           0, 10108, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 2000000},    
    { ETFAccessoryNotifyBluetoothAudioLinkOpenReq,        0, 0, 0, 0,           0, 10108, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 2000000},
    { ETFFindAndCheckRequest,                             0, 0, 0, 0,           0, 10108, ETFAsyNone, ReqNotifyBluetoothAudioLinkOpenReq,        KErrAlreadyExists, 0},    
    { ETFAccessoryNotifyBluetoothAudioLinkCloseReq,       0, 0, 0, 0,           0, 10108, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 2000000},
    { ETFFindAndCheckRequest,                             0, 0, 0, 0,           0, 10108, ETFAsyNone, ReqNotifyBluetoothAudioLinkCloseReq,       KErrAlreadyExists, 0},    
    
    { ETFCancelNotifyBluetoothAudioLinkOpenReq,           0, 0, 0, 0,           0, 10108, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 2000000},
    { ETFCancelNotifyBluetoothAudioLinkCloseReq,          0, 0, 0, 0,           0, 10108, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 2000000},
    { ETFClearStack,                                      0, 0, 0, 0,           0, 10108, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 2000000},
    //***
    { ETFAccessoryBtControl_Close,                        0, 0, 0, 0,           0, 10108, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 1000000},
    { ETFAccessoryAudioControl_Close,                     0, 0, 0, 0,           0, 10108, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 1000000},
    { ETFAccessoryMode_Close,                             0, 0, 0, 0,           0, 10108, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 1000000},
    { ETFAccessorySettings_Close,                         0, 0, 0, 0,           0, 10108, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 1000000},
    { ETFAccessoryControl_Close,                          0, 0, 0, 0,           0, 10108, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 1000000},
    { ETFAccessorySingleConnection_Close,                 0, 0, 0, 0,           0, 10108, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 1000000},
    { ETFAccessoryConnection_Close,                       0, 0, 0, 0,           0, 10108, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 1000000}
    };

//
//[M10109] [[AccServer] Cancel NotifyAccessoryValueChanged]
//
const TTFAccessoryTestCaseStateControl KTFCancelValueChanged[]=
    {
    { ETFAccessoryPublishAndSubscribe,         0, 1,        0, KAccAudioInConnector, 0, 10109, ETFAsySetCapability, ReqAccRequestNone, KErrNone, 2000000},
    { ETFAccessorySingleConnection_Open,       0, 0,        0, 0,                    0, 10109, ETFAsyNone,          ReqAccRequestNone, KErrNone, 0},
    { ETFAccessoryControl_Open,                0, 0,        0, 0,                    0, 10109, ETFAsyNone,          ReqAccRequestNone, KErrNone, 0},
    { ETFAccessoryNotifyNewAccessoryConnected, 0, 0,        0, 0,                    0, 10109, ETFAsyNone,          ReqAccRequestNone, KErrNone, 1000000},
    { ETFConnectWiredAccessory,                0, 0x010201, 0, 0,                    0, 10109, ETFAsyConn,          ReqAccRequestNone, KErrNone, 2000000},        
    //Cancel both notifications with one call
    { ETFNotifyIntAccessoryValueChanged,       0, 0,        0, KAccAudioOutputType,  0, 10109, ETFAsyNone,          ReqAccRequestNone, KErrNone, 2000000},
    { ETFNotifyBoolAccessoryValueChanged,      0, 1,        0, KAccMuteAllAudio,     0, 10109, ETFAsyNone,          ReqAccRequestNone, KErrNone, 2000000},
    { ETFCancelNotifyAccessoryValueChanged,    0, 0,        0, 0,                    0, 10109, ETFAsyNone,          ReqAccRequestNone, KErrNone, 0},
    //Cancel notifications with separate cancel-call
    { ETFNotifyIntAccessoryValueChanged,       0, 0,        0, KAccAudioOutputType,  0, 10109, ETFAsyNone,          ReqAccRequestNone, KErrNone, 2000000},
    { ETFNotifyBoolAccessoryValueChanged,      0, 1,        0, KAccMuteAllAudio,     0, 10109, ETFAsyNone,          ReqAccRequestNone, KErrNone, 2000000},
    { ETFCancelNotifyAccessoryValueChanged,    0, 0,        0, KAccAudioOutputType,  0, 10109, ETFAsyNone,          ReqAccRequestNone, KErrNone, 0},
    { ETFCancelNotifyAccessoryValueChanged,    0, 0,        0, KAccMuteAllAudio,     0, 10109, ETFAsyNone,          ReqAccRequestNone, KErrNone, 0},
    { ETFClearStack,                           0, 0,        0, 0,                    0, 10109, ETFAsyNone, ReqAccRequestNone,          KErrNone, 0},        
    { ETFDisconnectWiredAccessory,             0, 0,        0, 0,                    0, 10109, ETFAsyDisc,          ReqAccRequestNone, KErrNone, 2000000},
    { ETFAccessoryControl_Close,               0, 0,        0, 0,                    0, 10109, ETFAsyNone,          ReqAccRequestNone, KErrNone, 0},
    { ETFAccessorySingleConnection_Close,      0, 0,        0, 0,                    0, 10109, ETFAsyNone,          ReqAccRequestNone, KErrNone, 0},
    };

/*****************************                       *********************************/
/***************************** Wired Accessory Tests *********************************/
/*****************************                       *********************************/


//
// [M10201] [[AccServer] Connect Accessory]
//
const TTFAccessoryTestCaseStateControl KTFWiredHeadsetConnectStates[3]=
    {
    { ETFAccessoryControl_Open, 0, 0,        0, 0, 0, 10201, ETFAsyNone, ReqAccRequestNone,      KErrNone, 0},
    { ETFConnectWiredAccessory, 0, 0x010103, 0, 0, 0, 10201, ETFAsyConn, ReqAccRequestNone,      KErrNone, 2000000},
    };

//
// [M10202] [[AccServer] Disconnect Accessory]
//
const TTFAccessoryTestCaseStateControl KTFWiredHeadsetDisconnectStates[3]=
    {
    { ETFDisconnectWiredAccessory, 0, 0, 0, 0, 0, 10202, ETFAsyDisc, ReqAccRequestNone,         KErrNone, 2000000},
    { ETFAccessoryControl_Close,   0, 0, 0, 0, 0, 10202, ETFAsyNone, ReqAccRequestNone,         KErrNone, 0}
    };

//
// [M10203] [[AccServer] Notify New Accessory Connected]
//
const TTFAccessoryTestCaseStateControl KTFNotifyNewAccessoryConnectionState[4]=
    {
    { ETFAccessorySingleConnection_Open,        0, 0,        0, 0, 0, 10203, ETFAsyNone, ReqAccRequestNone,              KErrNone, 0},
    { ETFAccessoryNotifyNewAccessoryConnected,  0, 0,        0, 0, 0, 10203, ETFAsyNone, ReqAccRequestNone,              KErrNone, 1000000},
    { ETFConnectWiredAccessory,                 0, 0x010103, 0, 0, 0, 10203, ETFAsyConn, ReqAccRequestNone,              KErrNone, 2000000},
    { ETFFindAndCheckRequest,                   0, 0,        0, 0, 0, 10203, ETFAsyNone, ReqNotifyNewAccessoryConnected, KErrNone, 0},
    };

//
// [M10204] [[AccServer] Notify Accessory Disconnected]
//

const TTFAccessoryTestCaseStateControl KTFNotifyAccessoryDisconnectedStates[4]=
    {
    { ETFAccessoryNotifyAccessoryDisconnected, 0, 0, 0, 0, 0, 10204, ETFAsyNone, ReqAccRequestNone,              KErrNone, 2000000},
    { ETFDisconnectWiredAccessory,             0, 0, 0, 0, 0, 10204, ETFAsyDisc, ReqAccRequestNone,              KErrNone, 2000000},
    { ETFFindAndCheckRequest,                  0, 0, 0, 0, 0, 10204, ETFAsyNone, ReqNotifyAccessoryDisconnected, KErrNone, 0},
    { ETFAccessorySingleConnection_Close,      0, 0, 0, 0, 0, 10204, ETFAsyNone, ReqAccRequestNone,              KErrNone, 0},
    };

//
// [M10205] [[AccServer] Notify Accessory Connection Status Change (Connection)]
//

const TTFAccessoryTestCaseStateControl KTFNotifyAccessoryConnectionStatusChangeConnectionStates[4]=
    {
    { ETFAccessoryConnection_Open,                        0, 0,        0, 0, 0, 10205, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 0},
    { ETFAccessoryNotifyAccessoryConnectionStatusChanged, 0, 0,        0, 0, 0, 10205, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 2000000},
    { ETFConnectWiredAccessory,                           0, 0x010103, 0, 0, 0, 10205, ETFAsyConn, ReqAccRequestNone,                         KErrNone, 2000000},
    { ETFFindAndCheckRequest,                             0, 0,        0, 0, 0, 10205, ETFAsyNone, ReqNotifyAccessoryConnectionStatusChanged, KErrNone, 0}
    };

//
// [M10206] [[AccServer] Notify Accessory Connection Status Change (Disconnection)]
//

const TTFAccessoryTestCaseStateControl KTFNotifyAccessoryConnectionStatusChangeDisconnectionStates[4]=
    {
    { ETFAccessoryNotifyAccessoryConnectionStatusChanged, 0, 0, 0, 0, 0, 10206, ETFAsyNone, ReqAccRequestNone,                          KErrNone, 2000000},
    { ETFDisconnectWiredAccessory,                        0, 0, 0, 0, 0, 10206, ETFAsyDisc, ReqAccRequestNone,                          KErrNone, 2000000},
    { ETFFindAndCheckRequest,                             0, 0, 0, 0, 0, 10206, ETFAsyNone, ReqNotifyAccessoryConnectionStatusChanged,  KErrNone, 0},
    { ETFAccessoryConnection_Close,                       0, 0, 0, 0, 0, 10206, ETFAsyNone, ReqAccRequestNone,                          KErrNone, 0}
    };

//
// [M10207] [[AccServer] Sync Get Accessory Connection Status]
//
const TTFAccessoryTestCaseStateControl KTFSyncGetAccessoryConnectionStatusStates[5]=
    {
    { ETFAccessoryConnection_Open,        0, 0,        0, 0, 0, 10207, ETFAsyNone, ReqAccRequestNone,  KErrNone, 0},
    { ETFConnectWiredAccessory,           0, 0x010103, 0, 0, 0, 10207, ETFAsyConn, ReqAccRequestNone,  KErrNone, 2000000},
    { ETFSyncGetAccessoryConnectionStatus,0, 1,        0, 0, 0, 10207, ETFAsyNone, ReqAccRequestNone,  KErrNone, 2000000},
    { ETFDisconnectWiredAccessory,        0, 0,        0, 0, 0, 10207, ETFAsyDisc, ReqAccRequestNone,  KErrNone, 2000000},
    { ETFAccessoryConnection_Close,       0, 0,        0, 0, 0, 10207, ETFAsyNone, ReqAccRequestNone,  KErrNone, 0}
    };

//
// [M10208] [[AccServer] Request Get Accessory Connection Status Before Connection]
//
const TTFAccessoryTestCaseStateControl KTFRequestGetAccessoryConnectionStatusBeforeConnectionStates[4]=
    {
    { ETFAccessoryConnection_Open,          0, 0, 0, 0, 0, 10208,  ETFAsyNone, ReqAccRequestNone,               KErrNone, 0},
    { ETFASyncGetAccessoryConnectionStatus, 0, 0, 0, 0, 0, 10208,  ETFAsyNone, ReqAccRequestNone,               KErrNone, 2000000},
    { ETFFindAndCheckRequest,               0, 0, 0, 0, 0, 10208,  ETFAsyNone, ReqGetAccessoryConnectionStatus, KErrNone, 0},
    { ETFAccessoryConnection_Close,         0, 0, 0, 0, 0, 10208,  ETFAsyNone, ReqAccRequestNone,               KErrNone, 0}
    };

//
// [M10209] [[AccServer] Request Get Accessory Connection Status After Connection]
//
const TTFAccessoryTestCaseStateControl KTFRequestGetAccessoryConnectionStatusAfterConnectionStates[6]=
    {
    { ETFAccessoryConnection_Open,           0, 0,        0, 0, 0, 10209, ETFAsyNone, ReqAccRequestNone,                KErrNone, 0},
    { ETFConnectWiredAccessory,              0, 0x010103, 0, 0, 0, 10209, ETFAsyConn, ReqAccRequestNone,                KErrNone, 2000000},
    { ETFASyncGetAccessoryConnectionStatus,  0, 0,        0, 0, 0, 10209, ETFAsyNone, ReqAccRequestNone,                KErrNone, 2000000},
    { ETFFindAndCheckRequest,                0, 1,        0, 0, 0, 10209, ETFAsyNone, ReqGetAccessoryConnectionStatus,  KErrNone, 0},
    { ETFDisconnectWiredAccessory,           0, 0,        0, 0, 0, 10209, ETFAsyDisc, ReqAccRequestNone,                KErrNone, 2000000},
    { ETFAccessoryConnection_Close,          0, 0,        0, 0, 0, 10209, ETFAsyNone, ReqAccRequestNone,                KErrNone, 0}
    };

//
// [M10210] [[AccServer] Notify Accessory Value Changed (TInt)]
//
const TTFAccessoryTestCaseStateControl KTFNotifyAccessoryValueChangedIntegerStates[11]=
    {
    { ETFAccessorySingleConnection_Open,          0, 0,        0, 0,           0,        10210, ETFAsyNone,      ReqAccRequestNone,                     KErrNone, 0},
    { ETFAccessoryNotifyNewAccessoryConnected,    0, 0,        0, 0,           0,        10210, ETFAsyNone,      ReqAccRequestNone,                     KErrNone, 1000000},
    { ETFConnectWiredAccessory,                   0, 0x010103, 0, 0,           0,        10210, ETFAsyConn,      ReqAccRequestNone,                     KErrNone, 3000000},
    { ETFFindAndCheckRequest,                     0, 0,        0, 0,           0,        10210, ETFAsyNone,      ReqNotifyNewAccessoryConnected,        KErrNone, 0},
    { ETFNotifyIntAccessoryValueChanged,          0, 0,        0, KAccAudioOutputType, 0,10210, ETFAsyNone,      ReqAccRequestNone,                     KErrNone, 2000000},
    { ETFAccessoryPublishAndSubscribe,            0, 1,        0, KAccAudioOutputType, 0,10210, ETFAsyIntNotify, ReqAccRequestNone,                     KErrNone, 3000000},
    { ETFFindAndCheckRequest,                     0, 1,        0, KAccAudioOutputType, 0,10210, ETFAsyNone,      ReqNotifyIntegerAccessoryValueChanged, KErrNone, 0},
    { ETFDisconnectWiredAccessory,                0, 0,        0, 0,           0,        10210, ETFAsyDisc,      ReqAccRequestNone,                     KErrNone, 2000000},
    { ETFClearStack,                              0, 0,        0, 0,           0,        10210, ETFAsyNone,     ReqAccRequestNone,                     KErrNone, 0},
    { ETFAccessorySingleConnection_Close,         0, 0,        0, 0,           0,        10210, ETFAsyNone,      ReqAccRequestNone,                     KErrNone, 0},
    };

//
// [M10211] [[AccServer] Notify Accessory Value Changed (TBool)]
//
const TTFAccessoryTestCaseStateControl KTFNotifyAccessoryValueChangedBooleanStates[]=
    {
    { ETFAccessoryPublishAndSubscribe,            0, 1,        0, KAccExtPowerSupply, 0, 10211, ETFAsySetCapability, ReqAccRequestNone,                     KErrNone, 2000000},
    { ETFAccessorySingleConnection_Open,          0, 0,        0, 0,                  0, 10211, ETFAsyNone,          ReqAccRequestNone,                     KErrNone, 0},
    { ETFAccessoryControl_Open,                   0, 0,        0, 0,                  0, 10211, ETFAsyNone,          ReqAccRequestNone,                     KErrNone, 2000000},
    { ETFAccessoryNotifyNewAccessoryConnected,    0, 0,        0, 0,                  0, 10211, ETFAsyNone,          ReqAccRequestNone,                     KErrNone, 1000000},
    { ETFConnectWiredAccessory,                   0, 0x20108,  0, 0,                  0, 10211, ETFAsyConn,          ReqAccRequestNone,                     KErrNone, 3000000},
    { ETFFindAndCheckRequest,                     0, 0,        0, 0,                  0, 10211, ETFAsyNone,          ReqNotifyNewAccessoryConnected,        KErrNone, 0},
    { ETFNotifyBoolAccessoryValueChanged,         0, 1,        0, KAccExtPowerSupply, 0, 10211, ETFAsyNone,          ReqAccRequestNone,                     KErrNone, 3000000},
    { ETFAccessoryPublishAndSubscribe,            0, 1,        0, KAccExtPowerSupply, 0, 10211, ETFAsyBoolNotify,    ReqAccRequestNone,                     KErrNone, 2000000},
    { ETFFindAndCheckRequest,                     0, 1,        0, 0,                  0, 10211, ETFAsyNone,          ReqNotifyBooleanAccessoryValueChanged, KErrNone, 0},
    { ETFDisconnectWiredAccessory,                0, 0,        0, 0,                  0, 10211, ETFAsyDisc,          ReqAccRequestNone,                     KErrNone, 2000000},
    { ETFClearStack,                              0, 0,        0, 0,                  0, 10211, ETFAsyNone,          ReqAccRequestNone,                     KErrNone, 0},
    { ETFAccessorySingleConnection_Close,         0, 0,        0, 0,                  0, 10211, ETFAsyNone,          ReqAccRequestNone,                     KErrNone, 0},
    { ETFAccessoryControl_Close,                  0, 0,        0, 0,                  0, 10211, ETFAsyNone,          ReqAccRequestNone,                     KErrNone, 0}
    };


//
// [M10212] [[AccServer] Accessory Value Changed Notify (TInt)]
//
const TTFAccessoryTestCaseStateControl KTFAccessoryValueChangedNotifyIntegerStates[10]=
    {
    { ETFAccessorySingleConnection_Open,       0, 0,        0, 0, 0,       10212, ETFAsyNone, ReqAccRequestNone,              KErrNone, 0},
    { ETFAccessoryControl_Open,                0, 0,        0, 0, 0,       10212, ETFAsyNone, ReqAccRequestNone,              KErrNone, 0},
    { ETFAccessoryNotifyNewAccessoryConnected, 0, 0,        0, 0, 0,       10212, ETFAsyNone, ReqAccRequestNone,              KErrNone, 1000000},
    { ETFConnectWiredAccessory,                0, 0x010103, 0, 0, 0,       10212, ETFAsyConn, ReqAccRequestNone,              KErrNone, 2000000},
    { ETFFindAndCheckRequest,                  0, 0,        0, 0, 0,       10212, ETFAsyNone, ReqNotifyNewAccessoryConnected, KErrNone, 0},
    { ETFAccessoryValueChangedNotifyInt,       0, 1,        0, 0, 0,       10212, ETFAsyNone, ReqAccRequestNone,              KErrNone, 2000000},
    { ETFClearStack,                           0, 0,        0, 0, 0,       10212, ETFAsyNone, ReqAccRequestNone,      		    KErrNone, 0},
    { ETFDisconnectWiredAccessory,             0, 0,        0, 0, 0,       10212, ETFAsyDisc, ReqAccRequestNone,              KErrNone, 2000000},
    { ETFAccessorySingleConnection_Close,      0, 0,        0, 0, 0,       10212, ETFAsyNone, ReqAccRequestNone,              KErrNone, 0},
    { ETFAccessoryControl_Close,               0, 0,        0, 0, 0,       10212, ETFAsyNone, ReqAccRequestNone,              KErrNone, 0}
    };

//
// [M10213] [[AccServer] Accessory Value Changed Notify (TBool)]
//
const TTFAccessoryTestCaseStateControl KTFAccessoryValueChangedNotifyBooleanStates[]=
    {
    { ETFAccessoryPublishAndSubscribe,         0, 1,                0, KAccAudioOutConnector, 0,          10213, ETFAsySetCapability, ReqAccRequestNone,              KErrNone, 2000000},
    { ETFAccessorySingleConnection_Open,       0, 0,                0,                        0, 0,       10213, ETFAsyNone,          ReqAccRequestNone,              KErrNone, 0},
    { ETFAccessoryControl_Open,                0, 0,                0,                        0, 0,       10213, ETFAsyNone,          ReqAccRequestNone,              KErrNone, 0},
    { ETFAccessoryNotifyNewAccessoryConnected, 0, 0,                0,                        0, 0,       10213, ETFAsyNone,          ReqAccRequestNone,              KErrNone, 1000000},
    { ETFConnectWiredAccessory,                0, 0x010602,         0,                        0, 0,       10213, ETFAsyConn,          ReqAccRequestNone,              KErrNone, 2000000},
    { ETFFindAndCheckRequest,                  0, 0,                0,                        0, 0,       10213, ETFAsyNone,          ReqNotifyNewAccessoryConnected, KErrNone, 0},
    { ETFAccessoryValueChangedNotifyBool,      0, 1,                0, KAccAudioOutConnector,    0,       10213, ETFAsyNone,          ReqAccRequestNone,              KErrNone, 2000000},
    { ETFDisconnectWiredAccessory,             0, 0,                0,                        0, 0,       10213, ETFAsyDisc,          ReqAccRequestNone,              KErrNone, 2000000},
    { ETFClearStack,                           0, 0,                0,                        0, 0,       10213, ETFAsyNone,          ReqAccRequestNone,              KErrNone, 0},
    { ETFAccessorySingleConnection_Close,      0, 0,                0,                        0, 0,       10213, ETFAsyNone,          ReqAccRequestNone,              KErrNone, 0},
    { ETFAccessoryControl_Close,               0, 0,                0,                        0, 0,       10213, ETFAsyNone,          ReqAccRequestNone,              KErrNone, 0}
    };
//
//[M10214] [[AccServer] Set Value (TInt)]
//
const TTFAccessoryTestCaseStateControl KTFSetValueIntegerStates[]=
    {
    { ETFAccessorySingleConnection_Open,       0, 0,                         0,                          0,                   0, 10214, ETFAsyNone, ReqAccRequestNone, KErrNone, 0},
    { ETFAccessoryControl_Open,                0, 0,                         0,                          0,                   0, 10214, ETFAsyNone, ReqAccRequestNone, KErrNone, 0},
    { ETFAccessoryNotifyNewAccessoryConnected, 0, 0,                         0,                          0,                   0, 10214, ETFAsyNone, ReqAccRequestNone, KErrNone, 1000000},
    { ETFConnectWiredAccessory,                0, 0x010103,                  0,                          0,                   0, 10214, ETFAsyConn, ReqAccRequestNone, KErrNone, 2000000},
    { ETFAccessoryIntGetValue,                 0, 0,                         EAccAudioOutPutTypePrivate, KAccAudioOutputType ,0, 10214, ETFAsyNone, ReqAccRequestNone, KErrNone, 0},
    { ETFSetIntValue,                          0, EAccAudioOutPutTypePublic, 0,                          KAccAudioOutputType, 0, 10214, ETFAsyNone, ReqAccRequestNone, KErrNone, 2000000},  //Integer
    { ETFAccessoryIntGetValue,                 0, 0,                         EAccAudioOutPutTypePublic,  KAccAudioOutputType ,0, 10214, ETFAsyNone, ReqAccRequestNone, KErrNone, 0},
    { ETFDisconnectWiredAccessory,             0, 0,                         0,                          0,                   0, 10214, ETFAsyDisc, ReqAccRequestNone, KErrNone, 2000000},
    { ETFClearStack,                           0, 0,                         0,                          0,                   0, 10214, ETFAsyNone, ReqAccRequestNone, KErrNone, 2000000},
    { ETFAccessoryControl_Close,               0, 0,                         0,                          0,                   0, 10214, ETFAsyNone, ReqAccRequestNone, KErrNone, 2000000},
    { ETFAccessorySingleConnection_Close,      0, 0,                         0,                          0,                   0, 10214, ETFAsyNone, ReqAccRequestNone, KErrNone, 2000000},
    };


//
//[M10215] [[AccServer] Set Value (TBool)]
//
const TTFAccessoryTestCaseStateControl KTFSetValueBooleanStates[]=
    {
    { ETFAccessoryPublishAndSubscribe,         0, 1,        0, KAccAudioInConnector, 0, 10215, ETFAsySetCapability, ReqAccRequestNone, KErrNone, 2000000},
    { ETFAccessorySingleConnection_Open,       0, 0,        0, 0,            0, 10215, ETFAsyNone,          ReqAccRequestNone, KErrNone, 0},
    { ETFAccessoryControl_Open,                0, 0,        0, 0,            0, 10215, ETFAsyNone,          ReqAccRequestNone, KErrNone, 0},
    { ETFAccessoryNotifyNewAccessoryConnected, 0, 0,        0, 0,            0, 10215, ETFAsyNone,          ReqAccRequestNone, KErrNone, 1000000},
    { ETFConnectWiredAccessory,                0, 0x010201, 0, 0,            0, 10215, ETFAsyConn,          ReqAccRequestNone, KErrNone, 3000000},
    { ETFSetBoolValue,                         0, 1,        0, KAccAudioInConnector, 0, 10215, ETFAsyNone,  ReqAccRequestNone, KErrNone, 3000000},  //Boolean
    { ETFDisconnectWiredAccessory,             0, 0,        0, 0,            0, 10215, ETFAsyDisc,          ReqAccRequestNone, KErrNone, 2000000},
    { ETFClearStack,                           0, 0,        0, 0,            0, 10215, ETFAsyNone,          ReqAccRequestNone, KErrNone, 2000000},
    { ETFAccessoryControl_Close,               0, 0,        0, 0,            0, 10215, ETFAsyNone,          ReqAccRequestNone, KErrNone, 2000000},
    { ETFAccessorySingleConnection_Close,      0, 0,        0, 0,            0, 10215, ETFAsyNone,          ReqAccRequestNone, KErrNone, 2000000},
    };


//
// [M10216] AccServer [ Get Value (TInt)]
//
const TTFAccessoryTestCaseStateControl KTFGetTIntStates[]=
    {
    { ETFAccessoryPublishAndSubscribe,              0, 1,        0, KAccAudioOutConnector,                        0, 10216, ETFAsySetCapability, ReqAccRequestNone,              KErrNone, 2000000},
    { ETFAccessorySingleConnection_Open,            0, 0,        0, 0,                                            0, 10216, ETFAsyNone,          ReqAccRequestNone,              KErrNone, 0},
    { ETFAccessoryNotifyNewAccessoryConnected,      0, 0,        0, 0,                                            0, 10216, ETFAsyNone,          ReqAccRequestNone,              KErrNone, 1000000},
    { ETFConnectWiredAccessory,                     0, 0x010602, 0, 0,                                            0, 10216, ETFAsyConn,          ReqAccRequestNone,              KErrNone, 2000000},
    { ETFFindAndCheckRequest,                       0, 0,        0, 0,                                            0, 10216, ETFAsyNone,          ReqNotifyNewAccessoryConnected, KErrNone, 0},
    { ETFAccessoryIntGetValue,                      0, 0,        EAccAudioOutPutTypePrivate, KAccAudioOutputType, 0, 10216, ETFAsyNone,          ReqAccRequestNone,              KErrNone, 2000000},
    { ETFAccessoryNotifyAccessoryDisconnected,      0, 0,        0, 0,                                            0, 10216, ETFAsyNone,          ReqAccRequestNone,              KErrNone, 2000000},
    { ETFDisconnectWiredAccessory,                  0, 0,        0, 0,                                            0, 10216, ETFAsyDisc,          ReqAccRequestNone,              KErrNone, 2000000},
    { ETFFindAndCheckRequest,                       0, 0,        0, 0,                                            0, 10216, ETFAsyNone,          ReqNotifyAccessoryDisconnected, KErrNone, 0},
    { ETFAccessorySingleConnection_Close,           0, 0,        0, 0,                                            0, 10216, ETFAsyNone,          ReqAccRequestNone,              KErrNone, 0},
    };

//
// [M10217] AccServer [ Get Value (TBool)]
//
const TTFAccessoryTestCaseStateControl KTFGetTBoolStates[]=
    {
    { ETFAccessoryPublishAndSubscribe,             0, 1,        0, KAccAudioInConnector,  0, 10217, ETFAsySetCapability, ReqAccRequestNone,              KErrNone, 2000000},
    { ETFAccessorySingleConnection_Open,           0, 0,        0, 0,             0, 10217, ETFAsyNone,          ReqAccRequestNone,              KErrNone, 0},
    { ETFAccessoryNotifyNewAccessoryConnected,     0, 0,        0, 0,             0, 10217, ETFAsyNone,          ReqAccRequestNone,              KErrNone, 1000000},
    { ETFConnectWiredAccessory,                    0, 0x010201, 0, 0,             0, 10217, ETFAsyConn,          ReqAccRequestNone,              KErrNone, 2000000},
    { ETFFindAndCheckRequest,                      0, 0,        0, 0,             0, 10217, ETFAsyNone,          ReqNotifyNewAccessoryConnected, KErrNone, 0},
    { ETFAccessoryBoolGetValue,                    0, 1,        0, KAccAudioInConnector,  0, 10217, ETFAsyNone,          ReqAccRequestNone,              KErrNone, 2000000}, //Boolean
    { ETFAccessoryBoolGetValue,                    0, 1,        0, KAccAudioInConnector,  0, 10217, ETFAsyNone,          ReqAccRequestNone,              KErrNone, 2000000}, //Boolean
    { ETFAccessoryNotifyAccessoryDisconnected,     0, 0,        0, 0,             0, 10217, ETFAsyNone,          ReqAccRequestNone,              KErrNone, 2000000},
    { ETFDisconnectWiredAccessory,                 0, 0,        0, 0,             0, 10217, ETFAsyDisc,          ReqAccRequestNone,              KErrNone, 2000000},
    { ETFFindAndCheckRequest,                      0, 0,        0, 0,             0, 10217, ETFAsyNone,          ReqNotifyAccessoryDisconnected, KErrNone, 0},
    { ETFAccessorySingleConnection_Close,          0, 0,        0, 0,             0, 10217, ETFAsyNone,          ReqAccRequestNone,              KErrNone, 0},
    };

//
// [M10218] AccServer [ Get Value fails (TDes8)]
//
const TTFAccessoryTestCaseStateControl KTFGetTDes8States[9]=
    {
    { ETFAccessorySingleConnection_Open,           0, 0,                        0, 0,                 0, 10218, ETFAsyNone, ReqAccRequestNone,              KErrNone, 0},
    { ETFAccessoryNotifyNewAccessoryConnected,     0, 0,                        0, 0,                 0, 10218, ETFAsyNone, ReqAccRequestNone,              KErrNone, 1000000},
    { ETFConnectWiredAccessory,                    0, 0x010103,                 0, 0,                 0, 10218, ETFAsyConn, ReqAccRequestNone,              KErrNone, 2000000},
    { ETFFindAndCheckRequest,                      0, 0,                        0, 0,                 0, 10218, ETFAsyNone, ReqNotifyNewAccessoryConnected, KErrNone, 0},
    { ETFAccessoryDes8GetValue,                    0, KAccECIAudioBlock,        0, KAccECIAudioBlock, 0, 10218, ETFAsyNone, ReqAccRequestNone,              KErrArgument, 2000000}, //Desc
    { ETFAccessoryNotifyAccessoryDisconnected,     0, 0,                        0, 0,                 0, 10218, ETFAsyNone, ReqAccRequestNone,              KErrNone, 2000000},
    { ETFDisconnectWiredAccessory,                 0, 0,                        0, 0,                 0, 10218, ETFAsyDisc, ReqAccRequestNone,              KErrNone, 2000000},
    { ETFFindAndCheckRequest,                      0, 0,                        0, 0,                 0, 10218, ETFAsyNone, ReqNotifyAccessoryDisconnected, KErrNone, 0},
    { ETFAccessorySingleConnection_Close,          0, 0,                        0, 0,                 0, 10218, ETFAsyNone, ReqAccRequestNone,              KErrNone, 0},
    };

//
// [M10219] AccServer [ Connect and Disconnect Carkit (CK-10)]
//
const TTFAccessoryTestCaseStateControl KTFConnDisConnCarkit[]=
    {
    { ETFAccessoryPublishAndSubscribe,             0, 1,        0, KAccExtPowerSupply,  0, 10219, ETFAsySetCapability, ReqAccRequestNone,              KErrNone, 2000000},
    { ETFAccessorySingleConnection_Open,           0, 0,        0, 0,             0, 10219, ETFAsyNone,                ReqAccRequestNone,              KErrNone, 0},
    { ETFAccessoryNotifyNewAccessoryConnected,     0, 0,        0, 0,             0, 10219, ETFAsyNone,                ReqAccRequestNone,              KErrNone, 1000000},
    { ETFConnectWiredAccessory,                    0, 0x20108,  0, 0,             0, 10219, ETFAsyConn,                ReqAccRequestNone,              KErrNone, 2000000},
    { ETFFindAndCheckRequest,                      0, 0,        0, 0,             0, 10219, ETFAsyNone,                ReqNotifyNewAccessoryConnected, KErrNone, 0},
    { ETFAccessoryNotifyAccessoryDisconnected,     0, 0,        0, 0,             0, 10219, ETFAsyNone,                ReqAccRequestNone,              KErrNone, 2000000},
    { ETFDisconnectWiredAccessory,                 0, 0,        0, 0,             0, 10219, ETFAsyDisc,                ReqAccRequestNone,              KErrNone, 2000000},
    { ETFFindAndCheckRequest,                      0, 0,        0, 0,             0, 10219, ETFAsyNone,                ReqNotifyAccessoryDisconnected, KErrNone, 0},
    { ETFAccessorySingleConnection_Close,          0, 0,        0, 0,             0, 10219, ETFAsyNone,                ReqAccRequestNone,              KErrNone, 0},
    };
    
//
// [M10221] AccServer [ Connect Accessory, Error response from ASY]
//
const TTFAccessoryTestCaseStateControl KTFConnErrorFromASY[]=
    {
    { ETFAccessorySingleConnection_Open,           0, 0,                     0, 0,             0, 10221, ETFAsyNone,          ReqAccRequestNone,              KErrNone, 0},
    { ETFAccessoryNotifyNewAccessoryConnected,     0, 0,                     0, 0,             0, 10221, ETFAsyNone,          ReqAccRequestNone,              KErrNone, 1000000},
    { ETFConnectWiredAccessory,                    0, 0x20108,               0, 0,             0, 10221, ETFAsyConn,          ReqAccRequestNone,              KErrNone, 2000000},
    { ETFFindAndCheckRequest,                      0, KTFDontDeleteObserver, 0, 0,             0, 10221, ETFAsyNone,          ReqNotifyNewAccessoryConnected, KErrNone, 0},
    { ETFAccessoryNotifyAccessoryDisconnected,     0, 0,                     0, 0,             0, 10221, ETFAsyNone,          ReqAccRequestNone,              KErrNone, 2000000},
    { ETFDisconnectWiredAccessory,                 0, 0,                     0, 0,             0, 10221, ETFAsyDisc,          ReqAccRequestNone,              KErrNone, 2000000},
    { ETFFindAndCheckRequest,                      0, KTFDontDeleteObserver, 0, 0,             0, 10221, ETFAsyNone,          ReqNotifyAccessoryDisconnected, KErrNone, 0},
    { ETFCancelNotifyNewAccessoryConnected,        0, 0,                     0, 0,             0, 10221, ETFAsyNone,          ReqAccRequestNone,              KErrNone, 0},
    { ETFCancelNotifyAccessoryDisconnected,        0, 0,                     0, 0,             0, 10221, ETFAsyNone,          ReqAccRequestNone,              KErrNone, 0},
    { ETFClearStack,                               0, 0,                     0, 0,             0, 10221, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 0},
    { ETFAccessorySingleConnection_Close,          0, 0,                     0, 0,             0, 10221, ETFAsyNone,          ReqAccRequestNone,              KErrNone, 0},
    };
    
/*****************************                          *********************************/
/***************************** Wireless test cases      *********************************/
/*****************************                          *********************************/


//
// [M10401] [[AccServer] Connect BT with Headset And Handsfree Profile]
//
const TTFAccessoryTestCaseStateControl KTFBTHeadsetAndHandsfreeProfileConnectStates[]=
    {
    { ETFAccessoryBtControl_Open,              0, 0,                       0,      0,                        0, 10401, ETFAsyNone, ReqAccRequestNone,               KErrNone, 0},
    { ETFAccessorySingleConnection_Open,       0, 0,                       0,      0,                        0, 10401, ETFAsyNone, ReqAccRequestNone,               KErrNone, 0},
    { ETFAccessoryNotifyNewAccessoryConnected, 0, 0,                       0,      0,                        0, 10401, ETFAsyNone, ReqAccRequestNone,               KErrNone, 1000000},
    { ETFAccessoryBtConnectAccessory,          0, KBTAddrHeadsetHSPAndHFP, 0,      0,                        0, 10401, ETFAsyNone, ReqAccRequestNone,               KErrNone, 2000000},
    { ETFFindAndCheckRequest,                  0, 0,                       0,      0,                        0, 10401, ETFAsyNone, ReqNotifyNewAccessoryConnected,  KErrNone, 0},
    { ETFFindAndCheckRequest,                  0, 0,                       0,      0,                        0, 10401, ETFAsyNone, ReqConnectBtAccessory,           KErrNone, 0},
    { ETFCheckCapability,                      0, 0,                       ETrue,  KAccAudioOutputType,      0, 10401, ETFAsyNone, ReqAccRequestNone,               KErrNone, 0},
    { ETFCheckCapability,                      0, 0,                       ETrue,  KAccIntegratedAudioInput, 0, 10401, ETFAsyNone, ReqAccRequestNone,               KErrNone, 0},
    { ETFCheckCapability,                      0, 0,                       ETrue,  KAccBTHSP,                0, 10401, ETFAsyNone, ReqAccRequestNone,               KErrNone, 0},
    { ETFCheckCapability,                      0, 0,                       ETrue,  KAccBTHFP,                0, 10401, ETFAsyNone, ReqAccRequestNone,               KErrNone, 0},
    { ETFCheckCapability,                      0, 0,                       EFalse, KAccNoDRMOutput,          0, 10401, ETFAsyNone, ReqAccRequestNone,               KErrNone, 0},
    { ETFCheckCapabilityGroup,                 0, 0,                       ETrue,  KSBAudioSubblock,         0, 10401, ETFAsyNone, ReqAccRequestNone,               KErrNone, 0},
    { ETFCheckCapabilityGroup,                 0, 0,                       ETrue,  KSBBluetoothSubblock,     0, 10401, ETFAsyNone, ReqAccRequestNone,               KErrNone, 0},
    { ETFCheckCapabilityGroup,                 0, 0,                       EFalse, KSBVideoSubblock,         0, 10401, ETFAsyNone, ReqAccRequestNone,               KErrNone, 0},
    };

//
// [M10402] [[AccServer] Disconnect BT with Headset And Handsfree Profile]
//
const TTFAccessoryTestCaseStateControl KTFBTHeadsetAndHandsfreeProfileDisconnectStates[6]=
    {
    { ETFAccessoryNotifyAccessoryDisconnected, 0, 0,                       0, 0, 0, 10402, ETFAsyNone, ReqAccRequestNone,              KErrNone, 2000000},
    { ETFAccessoryBtDisconnectAccessory,       0, KBTAddrHeadsetHSPAndHFP, 0, 0, 0, 10402, ETFAsyNone, ReqAccRequestNone,              KErrNone, 2000000},
    { ETFFindAndCheckRequest,                  0, 0,                       0, 0, 0, 10402, ETFAsyNone, ReqNotifyAccessoryDisconnected, KErrNone, 0},
    { ETFFindAndCheckRequest,                  0, 0,                       0, 0, 0, 10402, ETFAsyNone, ReqDisconnectBtAccessory,       KErrNone, 0},
    { ETFAccessorySingleConnection_Close,      0, 0,                       0, 0, 0, 10402, ETFAsyNone, ReqAccRequestNone,              KErrNone, 0},
    { ETFAccessoryBtControl_Close,             0, 0,                       0, 0, 0, 10402, ETFAsyNone, ReqAccRequestNone,              KErrNone, 0},
    };



//
// [M10403] [[AccServer] Connect BT with Headset Profile]
//
const TTFAccessoryTestCaseStateControl KTFBTHeadsetProfileConnectStates[]=
    {
    { ETFAccessoryBtControl_Open,              0, 0,                 0, 0,                             0, 10403, ETFAsyNone, ReqAccRequestNone,               KErrNone, 0},
    { ETFAccessorySingleConnection_Open,       0, 0,                 0, 0,                             0, 10403, ETFAsyNone, ReqAccRequestNone,               KErrNone, 0},
    { ETFAccessoryNotifyNewAccessoryConnected, 0, 0,                 0, 0,                             0, 10403, ETFAsyNone, ReqAccRequestNone,               KErrNone, 1000000},
    { ETFAccessoryBtConnectAccessory,          0, KBTAddrHeadsetHSP, 0, 0,                             0, 10403, ETFAsyNone, ReqAccRequestNone,               KErrNone, 2000000},
    { ETFFindAndCheckRequest,                  0, 0,                 0, 0,                             0, 10403, ETFAsyNone, ReqNotifyNewAccessoryConnected,  KErrNone, 2000000},
    { ETFFindAndCheckRequest,                  0, 0,                 0, 0,                             0, 10403, ETFAsyNone, ReqConnectBtAccessory,           KErrNone, 2000000},
    { ETFCheckCapability,                      0, 0,                 1, KAccIntegratedAudioInput,      0, 10403, ETFAsyNone, ReqAccRequestNone,               KErrNone, 0},
    { ETFCheckCapability,                      0, 0,                 0,      KAccBTHFP,                0, 10403, ETFAsyNone, ReqAccRequestNone,               KErrNone, 0},
    { ETFCheckCapability,                      0, 0,                 1,      KAccBTHSP,                0, 10403, ETFAsyNone, ReqAccRequestNone,               KErrNone, 0},
    { ETFCheckCapabilityGroup,                 0, 0,                 ETrue,  KSBAudioSubblock,         0, 10403, ETFAsyNone, ReqAccRequestNone,               KErrNone, 0},
    { ETFCheckCapabilityGroup,                 0, 0,                 ETrue,  KSBBluetoothSubblock,     0, 10403, ETFAsyNone, ReqAccRequestNone,               KErrNone, 0},
    { ETFCheckCapabilityGroup,                 0, 0,                 EFalse, KSBVideoSubblock,         0, 10403, ETFAsyNone, ReqAccRequestNone,               KErrNone, 0},
    };

//
// [M10404] [[AccServer] Disconnect BT with Headset Profile]
//
const TTFAccessoryTestCaseStateControl KTFBTHeadsetProfileDisconnectStates[6]=
    {
    { ETFAccessoryNotifyAccessoryDisconnected, 0, 0,                 0, 0, 0, 10404, ETFAsyNone, ReqAccRequestNone,              KErrNone, 2000000},
    { ETFAccessoryBtDisconnectAccessory,       0, KBTAddrHeadsetHSP, 0, 0, 0, 10404, ETFAsyNone, ReqAccRequestNone,              KErrNone, 2000000},
    { ETFFindAndCheckRequest,                  0, 0,                 0, 0, 0, 10404, ETFAsyNone, ReqNotifyAccessoryDisconnected, KErrNone, 2000000},
    { ETFFindAndCheckRequest,                  0, 0,                 0, 0, 0, 10404, ETFAsyNone, ReqDisconnectBtAccessory,       KErrNone, 2000000},
    { ETFAccessorySingleConnection_Close,      0, 0,                 0, 0, 0, 10404, ETFAsyNone, ReqAccRequestNone,              KErrNone, 0},
    { ETFAccessoryBtControl_Close,             0, 0,                 0, 0, 0, 10404, ETFAsyNone, ReqAccRequestNone,              KErrNone, 0},
    };


//
// [M10405] [[AccServer] Connect BT with Handsfree Profile]
//
const TTFAccessoryTestCaseStateControl KTFBTHandsfreeProfileConnectStates[]=
    {
    { ETFAccessoryBtControl_Open,              0, 0,                 0, 0,                             0, 10405, ETFAsyNone, ReqAccRequestNone,              KErrNone, 0},
    { ETFAccessorySingleConnection_Open,       0, 0,                 0, 0,                             0, 10405, ETFAsyNone, ReqAccRequestNone,              KErrNone, 0},
    { ETFAccessoryNotifyNewAccessoryConnected, 0, 0,                 0, 0,                             0, 10405, ETFAsyNone, ReqAccRequestNone,              KErrNone, 1000000},
    { ETFAccessoryBtConnectAccessory,          0, KBTAddrHeadsetHFP, 0, 0,                             0, 10405, ETFAsyNone, ReqAccRequestNone,              KErrNone, 2000000},
    { ETFFindAndCheckRequest,                  0, 0,                 0, 0,                             0, 10405, ETFAsyNone, ReqNotifyNewAccessoryConnected, KErrNone, 2000000},
    { ETFFindAndCheckRequest,                  0, 0,                 0, 0,                             0, 10405, ETFAsyNone, ReqConnectBtAccessory,          KErrNone, 2000000},
    { ETFCheckCapability,                      0, 0,                 1, KAccIntegratedAudioInput,      0, 10405, ETFAsyNone, ReqAccRequestNone,              KErrNone, 0},
    { ETFCheckCapability,                      0, 0,                 1,      KAccBTHFP,                0, 10405, ETFAsyNone, ReqAccRequestNone,              KErrNone, 0},
    { ETFCheckCapability,                      0, 0,                 0,      KAccBTHSP,                0, 10405, ETFAsyNone, ReqAccRequestNone,              KErrNone, 0},
    { ETFCheckCapabilityGroup,                 0, 0,                 ETrue,  KSBAudioSubblock,         0, 10405, ETFAsyNone, ReqAccRequestNone,              KErrNone, 0},
    { ETFCheckCapabilityGroup,                 0, 0,                 ETrue,  KSBBluetoothSubblock,     0, 10405, ETFAsyNone, ReqAccRequestNone,              KErrNone, 0},
    { ETFCheckCapabilityGroup,                 0, 0,                 EFalse, KSBVideoSubblock,         0, 10405, ETFAsyNone, ReqAccRequestNone,              KErrNone, 0},
    };

//
// [M10406] [[AccServer] Disconnect BT with Headset Profile]
//
const TTFAccessoryTestCaseStateControl KTFBTHandsfreeProfileDisconnectStates[]=
    {
    { ETFAccessoryNotifyAccessoryDisconnected, 0, 0,                 0, 0, 0, 10406, ETFAsyNone, ReqAccRequestNone,              KErrNone, 2000000},
    { ETFAccessoryBtDisconnectAccessory,       0, KBTAddrHeadsetHFP, 0, 0, 0, 10406, ETFAsyNone, ReqAccRequestNone,              KErrNone, 2000000},
    { ETFFindAndCheckRequest,                  0, 0,                 0, 0, 0, 10406, ETFAsyNone, ReqNotifyAccessoryDisconnected, KErrNone, 2000000},
    { ETFFindAndCheckRequest,                  0, 0,                 0, 0, 0, 10406, ETFAsyNone, ReqDisconnectBtAccessory,       KErrNone, 2000000},
    { ETFAccessorySingleConnection_Close,      0, 0,                 0, 0, 0, 10406, ETFAsyNone, ReqAccRequestNone,              KErrNone, 0},
    { ETFAccessoryBtControl_Close,             0, 0,                 0, 0, 0, 10406, ETFAsyNone, ReqAccRequestNone,              KErrNone, 0},
    };

//
// [M10407] [[AccServer] Connect BT with Empty Profile]
//
const TTFAccessoryTestCaseStateControl KTFBTEmptyProfileConnectStates[]=
    {
    { ETFAccessoryBtControl_Open,              0, 0,                     0, 0,                             0, 10407, ETFAsyNone, ReqAccRequestNone,              KErrNone, 0},
    { ETFAccessorySingleConnection_Open,       0, 0,                     0, 0,                             0, 10407, ETFAsyNone, ReqAccRequestNone,              KErrNone, 0},
    { ETFAccessoryNotifyNewAccessoryConnected, 0, 0,                     0, 0,                             0, 10407, ETFAsyNone, ReqAccRequestNone,              KErrNone, 1000000},
    { ETFAccessoryBtConnectAccessory,          0, KBTAddrHeadsetEmptyPR, 0, 0,                             0, 10407, ETFAsyNone, ReqAccRequestNone,              KErrNone, 2000000},
    { ETFFindAndCheckRequest,                  0, 0,                     0, 0,                             0, 10407, ETFAsyNone, ReqNotifyNewAccessoryConnected, KErrNone, 2000000},
    { ETFFindAndCheckRequest,                  0, 0,                     0, 0,                             0, 10407, ETFAsyNone, ReqConnectBtAccessory,          KErrNone, 2000000},
    { ETFCheckCapability,                      0, 0,                     0, KAccIntegratedAudioInput,      0, 10407, ETFAsyNone, ReqAccRequestNone,              KErrNone, 0},
    { ETFCheckCapabilityGroup,                 0, 0,                     ETrue,  KSBAudioSubblock,         0, 10401, ETFAsyNone, ReqAccRequestNone,              KErrNone, 0},
    { ETFCheckCapabilityGroup,                 0, 0,                     EFalse, KSBBluetoothSubblock,     0, 10401, ETFAsyNone, ReqAccRequestNone,              KErrNone, 0},
    { ETFCheckCapabilityGroup,                 0, 0,                     EFalse, KSBVideoSubblock,         0, 10401, ETFAsyNone, ReqAccRequestNone,              KErrNone, 0},
    };


//
// [M10408] [[AccServer] Disconnect BT with Empty Profile]
//
const TTFAccessoryTestCaseStateControl KTFBTEmptyProfileDisconnectStates[]=
    {
    { ETFAccessoryNotifyAccessoryDisconnected, 0, 0,                     0, 0, 0, 10408, ETFAsyNone, ReqAccRequestNone,              KErrNone, 2000000},
    { ETFAccessoryBtDisconnectAccessory,       0, KBTAddrHeadsetEmptyPR, 0, 0, 0, 10408, ETFAsyNone, ReqAccRequestNone,              KErrNone, 2000000},
    { ETFFindAndCheckRequest,                  0, 0,                     0, 0, 0, 10408, ETFAsyNone, ReqNotifyAccessoryDisconnected, KErrNone, 2000000},
    { ETFFindAndCheckRequest,                  0, 0,                     0, 0, 0, 10408, ETFAsyNone, ReqDisconnectBtAccessory,       KErrNone, 2000000},
    { ETFAccessorySingleConnection_Close,      0, 0,                     0, 0, 0, 10408, ETFAsyNone, ReqAccRequestNone,              KErrNone, 0},
    { ETFAccessoryBtControl_Close,             0, 0,                     0, 0, 0, 10408, ETFAsyNone, ReqAccRequestNone,              KErrNone, 0},
    };

//
// [M10409] [[AccServer] Connect BT with Nokia Spesific Device Id]
//
const TTFAccessoryTestCaseStateControl KTFBTNokiaSpesificDeviceIdConnectStates[]=
    {
    { ETFAccessoryBtControl_Open,              0, 0,                       0,      0,                    0, 10409, ETFAsyNone, ReqAccRequestNone,              KErrNone, 0},
    { ETFAccessorySingleConnection_Open,       0, 0,                       0,      0,                    0, 10409, ETFAsyNone, ReqAccRequestNone,              KErrNone, 0},
    { ETFAccessoryNotifyNewAccessoryConnected, 0, 0,                       0,      0,                    0, 10409, ETFAsyNone, ReqAccRequestNone,              KErrNone, 1000000},
    { ETFAccessoryBtConnectAccessory,          0, KBTAddrHeadsetNokiaSPId, 0,      0,                    0, 10409, ETFAsyNone, ReqAccRequestNone,              KErrNone, 2000000},
    { ETFFindAndCheckRequest,                  0, 0,                       0,      0,                    0, 10409, ETFAsyNone, ReqNotifyNewAccessoryConnected, KErrNone, 2000000},
    { ETFFindAndCheckRequest,                  0, 0,                       0,      0,                    0, 10409, ETFAsyNone, ReqConnectBtAccessory,          KErrNone, 2000000},
    { ETFCheckCapabilityGroup,                 0, 0,                       ETrue,  KSBAudioSubblock,     0, 10401, ETFAsyNone, ReqAccRequestNone,              KErrNone, 0},
    { ETFCheckCapabilityGroup,                 0, 0,                       ETrue,  KSBBluetoothSubblock, 0, 10401, ETFAsyNone, ReqAccRequestNone,              KErrNone, 0},
    { ETFCheckCapabilityGroup,                 0, 0,                       EFalse, KSBVideoSubblock,     0, 10401, ETFAsyNone, ReqAccRequestNone,              KErrNone, 0},
    };


//
// [M10410] [[AccServer] Disconnect BT with Nokia Spesific Device Id]
//
const TTFAccessoryTestCaseStateControl KTFBTNokiaSpesificDeviceIdDisconnectStates[]=
    {
    { ETFAccessoryNotifyAccessoryDisconnected, 0, 0,                       0, 0, 0, 10410, ETFAsyNone, ReqAccRequestNone,              KErrNone, 2000000},
    { ETFAccessoryBtDisconnectAccessory,       0, KBTAddrHeadsetNokiaSPId, 0, 0, 0, 10410, ETFAsyNone, ReqAccRequestNone,              KErrNone, 2000000},
    { ETFFindAndCheckRequest,                  0, 0,                       0, 0, 0, 10410, ETFAsyNone, ReqNotifyAccessoryDisconnected, KErrNone, 2000000},
    { ETFFindAndCheckRequest,                  0, 0,                       0, 0, 0, 10410, ETFAsyNone, ReqDisconnectBtAccessory,       KErrNone, 2000000},
    { ETFClearStack,                           0, 0,                       0, 0, 0, 10410, ETFAsyNone, ReqAccRequestNone,              KErrNone, 0},
    { ETFAccessorySingleConnection_Close,      0, 0,                       0, 0, 0, 10410, ETFAsyNone, ReqAccRequestNone,              KErrNone, 0},
    { ETFAccessoryBtControl_Close,             0, 0,                       0, 0, 0, 10410, ETFAsyNone, ReqAccRequestNone,              KErrNone, 0},
    };

//***************************************************************************************

//
// [M10411] [[AccServer] BT Connect, No Response From BT ASY ]
//
const TTFAccessoryTestCaseStateControl KTFBTNoResponseFromBtAsyConnectStates[]=
    {
    { ETFAccessoryBtControl_Open,              0, 0,                 0, 0, 0, 10411, ETFAsyNone, ReqAccRequestNone,              KErrNone, 0},
    { ETFAccessorySingleConnection_Open,       0, 0,                 0, 0, 0, 10411, ETFAsyNone, ReqAccRequestNone,              KErrNone, 0},
    { ETFAccessoryBtConnectAccessory,          0, KBTAddrNoResponse, 0, 0, 0, 10411, ETFAsyNone, ReqAccRequestNone,              KErrNone, 4000000},
    { ETFFindAndCheckRequest,                  0, 0,                 0, 0, 0, 10411, ETFAsyNone, ReqConnectBtAccessory,          KErrTimedOut, 0},
    };

//
// [M10412] [[AccServer] BT Disconnect, No Response From BT ASY ]
//
const TTFAccessoryTestCaseStateControl KTFBTNoResponseFromBtAsyDisconnectStates[]=
    {
    { ETFAccessoryBtDisconnectAccessory,       0, KBTAddrNoResponse, 0, 0, 0, 10412, ETFAsyNone, ReqAccRequestNone,              KErrNone,     2000000},
    { ETFFindAndCheckRequest,                  0, 0,                 0, 0, 0, 10412, ETFAsyNone, ReqDisconnectBtAccessory,       KErrArgument, 0},
    { ETFAccessorySingleConnection_Close,      0, 0,                 0, 0, 0, 10412, ETFAsyNone, ReqAccRequestNone,              KErrNone,     0},
    { ETFAccessoryBtControl_Close,             0, 0,                 0, 0, 0, 10412, ETFAsyNone, ReqAccRequestNone,              KErrNone,     0},
    };
    
//***************************************************************************************

//
// [M10413] [[AccServer] BT Connect, Response with Error Code]
//
const TTFAccessoryTestCaseStateControl KTFBTAsyResponseWithErrorCodeConnectStates[]=
    {
    { ETFAccessoryBtControl_Open,              0, 0,              0, 0, 0, 10413, ETFAsyNone, ReqAccRequestNone,              KErrNone,    0},
    { ETFAccessorySingleConnection_Open,       0, 0,              0, 0, 0, 10413, ETFAsyNone, ReqAccRequestNone,              KErrNone,    0},
    { ETFAccessoryBtConnectAccessory,          0, KBTAddrErrCode, 0, 0, 0, 10413, ETFAsyNone, ReqAccRequestNone,              KErrNone,    2000000},
    { ETFFindAndCheckRequest,                  0, 0,              0, 0, 0, 10413, ETFAsyNone, ReqConnectBtAccessory,          KErrGeneral, 2000000},
    };

//
// [M10414] [[AccServer] BT Disconnect, Response with Error Code]
//
const TTFAccessoryTestCaseStateControl KTFBTAsyResponseWithErrorCodeDisconnectStates[]=
    {
    { ETFAccessoryBtDisconnectAccessory,       0, KBTAddrErrCode, 0, 0, 0, 10414, ETFAsyNone, ReqAccRequestNone,              KErrNone,     2000000},
    { ETFFindAndCheckRequest,                  0, 0,              0, 0, 0, 10414, ETFAsyNone, ReqDisconnectBtAccessory,       KErrArgument, 2000000},
    { ETFAccessorySingleConnection_Close,      0, 0,              0, 0, 0, 10414, ETFAsyNone, ReqAccRequestNone,              KErrNone,     0},
    { ETFAccessoryBtControl_Close,             0, 0,              0, 0, 0, 10414, ETFAsyNone, ReqAccRequestNone,              KErrNone,     0},
    };

//
// [M10415] [[AccServer] BT Connect, All Service UUIDs]
//
const TTFAccessoryTestCaseStateControl KTFBTAllServiceUUIDsConnectStates[]=
    {
    { ETFAccessoryBtControl_Open,              0, 0,                       0,     0,                  0, 10415, ETFAsyNone, ReqAccRequestNone,              KErrNone, 0},
    { ETFAccessorySingleConnection_Open,       0, 0,                       0,     0,                  0, 10415, ETFAsyNone, ReqAccRequestNone,              KErrNone, 0},
    { ETFAccessoryNotifyNewAccessoryConnected, 0, 0,                       0,     0,                  0, 10415, ETFAsyNone, ReqAccRequestNone,              KErrNone, 1000000},
    { ETFAccessoryBtConnectAccessory,          0, KBTAllBitsOn,            0,     0,                  0, 10415, ETFAsyNone, ReqAccRequestNone,              KErrNone, 2000000},
    { ETFFindAndCheckRequest,                  0, 0,                       0,     0,                  0, 10415, ETFAsyNone, ReqNotifyNewAccessoryConnected, KErrNone, 2000000},
    { ETFFindAndCheckRequest,                  0, 0,                       0,     0,                  0, 10415, ETFAsyNone, ReqConnectBtAccessory,          KErrNone, 2000000},   
    { ETFCheckCapability,                      0, 0,                       ETrue, KAccSetVolumeLevel, 0, 10415, ETFAsyNone, ReqAccRequestNone,              KErrNone, 0},
    { ETFAccessoryIntGetValue,                 0, 0,                       0x3,   KAccSetVolumeLevel ,0, 10415, ETFAsyNone, ReqAccRequestNone,              KErrNone, 0},
    { ETFSetIntValue,                          0, KAccSetVolumeLevelAVRCP, 0,     KAccSetVolumeLevel, 0, 10415, ETFAsyNone, ReqAccRequestNone,              KErrNone, 0},
    { ETFAccessoryIntGetValue,                 0, 0,                       0x7,   KAccSetVolumeLevel ,0, 10415, ETFAsyNone, ReqAccRequestNone,              KErrNone, 0},
    };

//
// [M10416] [[AccServer] BT Disconnect, All Services UUIDs]
//
const TTFAccessoryTestCaseStateControl KTFBTAllServiceUUIDsDisconnectStates[6]=
    {
    { ETFAccessoryNotifyAccessoryDisconnected, 0, 0,              0, 0, 0, 10416, ETFAsyNone, ReqAccRequestNone,              KErrNone, 2000000},
    { ETFAccessoryBtDisconnectAccessory,       0, KBTAllBitsOn,   0, 0, 0, 10416, ETFAsyNone, ReqAccRequestNone,              KErrNone, 2000000},
    { ETFFindAndCheckRequest,                  0, 0,              0, 0, 0, 10416, ETFAsyNone, ReqNotifyAccessoryDisconnected, KErrNone, 2000000},
    { ETFFindAndCheckRequest,                  0, 0,              0, 0, 0, 10416, ETFAsyNone, ReqDisconnectBtAccessory,       KErrNone, 2000000},
    { ETFAccessorySingleConnection_Close,      0, 0,              0, 0, 0, 10416, ETFAsyNone, ReqAccRequestNone,              KErrNone, 0},
    { ETFAccessoryBtControl_Close,             0, 0,              0, 0, 0, 10416, ETFAsyNone, ReqAccRequestNone,              KErrNone, 0},
    };

//
// [M10417] [[AccServer] BT Connect, BT with AVRCP profile]
//
const TTFAccessoryTestCaseStateControl KTFBTAVRCPConnectStates[6]=
    {
    { ETFAccessoryBtControl_Open,              0, 0,                   0, 0, 0, 10417, ETFAsyNone, ReqAccRequestNone,              KErrNone, 0},
    { ETFAccessorySingleConnection_Open,       0, 0,                   0, 0, 0, 10417, ETFAsyNone, ReqAccRequestNone,              KErrNone, 0},
    { ETFAccessoryNotifyNewAccessoryConnected, 0, 0,                   0, 0, 0, 10417, ETFAsyNone, ReqAccRequestNone,              KErrNone, 1000000},
    { ETFAccessoryBtConnectAccessory,          0, KBTAddrHeadsetAVRCP, 0, 0, 0, 10417, ETFAsyNone, ReqAccRequestNone,              KErrNone, 2000000},
    { ETFFindAndCheckRequest,                  0, 0,                   0, 0, 0, 10417, ETFAsyNone, ReqNotifyNewAccessoryConnected, KErrNone, 2000000},
    { ETFFindAndCheckRequest,                  0, 0,                   0, 0, 0, 10417, ETFAsyNone, ReqConnectBtAccessory,          KErrNone, 2000000},
    };

//
// [M10418] [[AccServer] BT Disconnect, BT with AVRCP profile]
//
const TTFAccessoryTestCaseStateControl KTFBTAVRCPDisconnectStates[6]=
    {
    { ETFAccessoryNotifyAccessoryDisconnected, 0, 0,                   0, 0, 0, 10418, ETFAsyNone, ReqAccRequestNone,              KErrNone, 2000000},
    { ETFAccessoryBtDisconnectAccessory,       0, KBTAddrHeadsetAVRCP, 0, 0, 0, 10418, ETFAsyNone, ReqAccRequestNone,              KErrNone, 2000000},
    { ETFFindAndCheckRequest,                  0, 0,                   0, 0, 0, 10418, ETFAsyNone, ReqNotifyAccessoryDisconnected, KErrNone, 2000000},
    { ETFFindAndCheckRequest,                  0, 0,                   0, 0, 0, 10418, ETFAsyNone, ReqDisconnectBtAccessory,       KErrNone, 2000000},
    { ETFAccessorySingleConnection_Close,      0, 0,                   0, 0, 0, 10418, ETFAsyNone, ReqAccRequestNone,              KErrNone, 0},
    { ETFAccessoryBtControl_Close,             0, 0,                   0, 0, 0, 10418, ETFAsyNone, ReqAccRequestNone,              KErrNone, 0},
    };


//
// [M10419] [[AccServer] BT Connect, BT with A2DP profile]
//
const TTFAccessoryTestCaseStateControl KTFBTA2DPConnectStates[]=
    {
    { ETFAccessoryBtControl_Open,              0, 0,                  0,      0,               0, 10419, ETFAsyNone, ReqAccRequestNone,              KErrNone, 0},
    { ETFAccessorySingleConnection_Open,       0, 0,                  0,      0,               0, 10419, ETFAsyNone, ReqAccRequestNone,              KErrNone, 0},
    { ETFAccessoryNotifyNewAccessoryConnected, 0, 0,                  0,      0,               0, 10419, ETFAsyNone, ReqAccRequestNone,              KErrNone, 1000000},
    { ETFAccessoryBtConnectAccessory,          0, KBTAddrHeadseA2DP,  0,      0,               0, 10419, ETFAsyNone, ReqAccRequestNone,              KErrNone, 2000000},
    { ETFFindAndCheckRequest,                  0, 0,                  0,      0,               0, 10419, ETFAsyNone, ReqNotifyNewAccessoryConnected, KErrNone, 2000000},
    { ETFFindAndCheckRequest,                  0, 0,                  0,      0,               0, 10419, ETFAsyNone, ReqConnectBtAccessory,          KErrNone, 2000000},
    { ETFCheckCapability,                      0, 0,                  ETrue,  KAccNoDRMOutput, 0, 10419, ETFAsyNone, ReqAccRequestNone,              KErrNone, 0},
    };

//
// [M10420] [[AccServer] BT Disconnect, BT with A2DP profile]
//
const TTFAccessoryTestCaseStateControl KTFBTA2DPDisconnectStates[6]=
    {
    { ETFAccessoryNotifyAccessoryDisconnected, 0, 0,                   0, 0, 0, 10420, ETFAsyNone, ReqAccRequestNone,              KErrNone, 2000000},
    { ETFAccessoryBtDisconnectAccessory,       0, KBTAddrHeadseA2DP,   0, 0, 0, 10420, ETFAsyNone, ReqAccRequestNone,              KErrNone, 2000000},
    { ETFFindAndCheckRequest,                  0, 0,                   0, 0, 0, 10420, ETFAsyNone, ReqNotifyAccessoryDisconnected, KErrNone, 2000000},
    { ETFFindAndCheckRequest,                  0, 0,                   0, 0, 0, 10420, ETFAsyNone, ReqDisconnectBtAccessory,       KErrNone, 2000000},
    { ETFAccessorySingleConnection_Close,      0, 0,                   0, 0, 0, 10420, ETFAsyNone, ReqAccRequestNone,              KErrNone, 0},
    { ETFAccessoryBtControl_Close,             0, 0,                   0, 0, 0, 10420, ETFAsyNone, ReqAccRequestNone,              KErrNone, 0},
    };

//
// [M10421] [[AccServer] BT Accessory Value Changed Notify (TBool)]
//
const TTFAccessoryTestCaseStateControl KTFBTAccessoryValueChangedNotifyBooleanStates[]=
    {
    { ETFAccessoryBtControl_Open,              0, 0,                 0, 0,               0, 10421, ETFAsyNone, ReqAccRequestNone,              KErrNone, 0},
    { ETFAccessorySingleConnection_Open,       0, 0,                 0, 0,               0, 10421, ETFAsyNone, ReqAccRequestNone,              KErrNone, 0},
    { ETFAccessoryNotifyNewAccessoryConnected, 0, 0,                 0, 0,               0, 10421, ETFAsyNone, ReqAccRequestNone,              KErrNone, 1000000},
    { ETFAccessoryBtConnectAccessory,          0, KBTAddrHeadsetHFP, 0, 0,               0, 10421, ETFAsyNone, ReqAccRequestNone,              KErrNone, 2000000},
    { ETFFindAndCheckRequest,                  0, 0,                 0, 0,               0, 10421, ETFAsyNone, ReqNotifyNewAccessoryConnected, KErrNone, 2000000},        
    { ETFAccessoryBoolGetValue,                0, 0,                 0, KAccAcousticEchoControl, 0, 10421, ETFAsyNone, ReqAccRequestNone,      KErrNone, 2000000},
    { ETFAccessoryBoolGetValue,                0, 0,                 0, KAccNoiseReduction,      0, 10421, ETFAsyNone, ReqAccRequestNone,      KErrNone, 2000000},     
    { ETFBTAccessoryValueChangedNotifyBool,    0, KBTAddrHeadsetHFP, 0, KAccAcousticEchoControl, 1, 10421, ETFAsyNone, ReqAccRequestNone,      KErrNone, 2000000},
    { ETFBTAccessoryValueChangedNotifyBool,    0, KBTAddrHeadsetHFP, 0, KAccNoiseReduction,      1, 10421, ETFAsyNone, ReqAccRequestNone,      KErrNone, 2000000},
    { ETFAccessoryBoolGetValue,                0, 1,                 0, KAccAcousticEchoControl, 0, 10421, ETFAsyNone, ReqAccRequestNone,      KErrNone, 2000000},
    { ETFAccessoryBoolGetValue,                0, 1,                 0, KAccNoiseReduction,      0, 10421, ETFAsyNone, ReqAccRequestNone,      KErrNone, 2000000},    
    { ETFAccessoryNotifyAccessoryDisconnected, 0, 0,                 0, 0,               0, 10421, ETFAsyNone, ReqAccRequestNone,              KErrNone, 2000000},
    { ETFAccessoryBtDisconnectAccessory,       0, KBTAddrHeadsetHFP, 0, 0,               0, 10421, ETFAsyNone, ReqAccRequestNone,              KErrNone, 2000000},
    { ETFFindAndCheckRequest,                  0, 0,                 0, 0,               0, 10421, ETFAsyNone, ReqNotifyAccessoryDisconnected, KErrNone, 2000000},
    { ETFAccessorySingleConnection_Close,      0, 0,                 0, 0,               0, 10421, ETFAsyNone, ReqAccRequestNone,              KErrNone, 0},
    { ETFAccessoryBtControl_Close,             0, 0,                 0, 0,               0, 10421, ETFAsyNone, ReqAccRequestNone,              KErrNone, 0},
    };

//
// [M10422] [[AccServer] Audio is routed to wired accessory and BT makes audio transfer]
//
const TTFAccessoryTestCaseStateControl KTFBTFailedAudiotransferFromBT[]=
    {
    { ETFAccessoryMode_Open,                              0, 0,                       0, 0,               0, 10422, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 2000000},
    { ETFAccessorySingleConnection_Open,                  0, 0,                       0, 0,               0, 10422, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 2000000},
    { ETFAccessoryBtControl_Open,                         0, 0,                       0, 0,               0, 10422, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 2000000},
    { ETFAccessoryConnection_Open,                        0, 0,                       0, 0,               0, 10422, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 2000000},
    { ETFAccessoryNotifyAccessoryModeChanged,             0, 0,                       0, 0,               0, 10422, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 3000000},
    { ETFAccessoryNotifyNewAccessoryConnected,            0, 0,                       0, 0,               0, 10422, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 3000000},
    { ETFAccessoryNotifyAccessoryConnectionStatusChanged, 0, 0,                       0, 0,               0, 10422, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 3000000},
    { ETFConnectWiredAccessory,                           0, 0x010103,                0, 0,               0, 10422, ETFAsyConn, ReqAccRequestNone,                         KErrNone, 2000000},
    { ETFFindAndCheckRequest,                             0, 0,                       0, 0,               0, 10422, ETFAsyNone, ReqNotifyNewAccessoryConnected,            KErrNone, 2000000},
    { ETFFindAndCheckRequest,                             0, 1,                       0, 0,               0, 10422, ETFAsyNone, ReqNotifyAccessoryModeChanged,             KErrNone, 2000000},
    { ETFFindAndCheckRequest,                             0, 0,                       0, 0,               0, 10422, ETFAsyNone, ReqNotifyAccessoryConnectionStatusChanged, KErrNone, 2000000},
    { ETFAccessoryNotifyNewAccessoryConnected,            1, 0,                       0, 0,               0, 10422, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 3000000},
    { ETFAccessoryNotifyAccessoryConnectionStatusChanged, 1, 0,                       0, 0,               0, 10422, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 3000000},
    { ETFAccessoryBtConnectAccessory,                     1, KBTAddrHeadsetHSPAndHFP, 0, EAccStereoAudio, 0, 10422, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 3000000},
    { ETFFindAndCheckRequest,                             1, 0,                       0, 0,               0, 10422, ETFAsyNone, ReqNotifyNewAccessoryConnected,            KErrNone, 2000000},
    { ETFFindAndCheckRequest,                             1, 0,                       0, 0,               0, 10422, ETFAsyNone, ReqConnectBtAccessory,                     KErrNone, 2000000},
    { ETFAudioRoutingStatusNotify,                        0, ETrue,                   0, 0,               0, 10422, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 2000000},
    { ETFAccessoryBluetoothAudioLinkOpenedNotify,         0, KBTAddrHeadsetHSPAndHFP, 0, EAccStereoAudio, 0, 10422, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 2000000},
    { ETFAudioRoutingStatusNotify,                        0, EFalse,                  0, 0,               0, 10422, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 2000000},
    { ETFAccessoryNotifyAccessoryDisconnected,            0, 0,                       0, 0,               0, 10422, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 3000000},
    { ETFAccessoryNotifyAccessoryModeChanged,             0, 0,                       0, 0,               0, 10422, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 3000000},
    { ETFAccessoryNotifyAccessoryConnectionStatusChanged, 0, 0,                       0, 0,               0, 10422, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 3000000},
    { ETFDisconnectWiredAccessory,                        0, 0,                       0, 0,               0, 10422, ETFAsyDisc, ReqAccRequestNone,                         KErrNone, 2000000},
    { ETFFindAndCheckRequest,                             0, 0,                       0, 0,               0, 10422, ETFAsyNone, ReqNotifyAccessoryDisconnected,            KErrNone, 2000000},
    { ETFFindAndCheckRequest,                             0, 0,                       0, 0,               0, 10422, ETFAsyNone, ReqNotifyAccessoryConnectionStatusChanged, KErrNone, 2000000},    
    { ETFFindAndCheckRequest,                             0, 2,                       0, 0,               0, 10422, ETFAsyNone, ReqNotifyAccessoryModeChanged,             KErrNone, 2000000},
    { ETFAccessoryNotifyAccessoryDisconnected,            1, 0,                       0, 0,               0, 10422, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 2000000},    
    { ETFAccessoryNotifyAccessoryConnectionStatusChanged, 1, 0,                       0, 0,               0, 10422, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 2000000},
#ifdef __WINS__ // Change order because audio is changing audio routing status!
    { ETFAccessoryNotifyAccessoryModeChanged,             1, 0,                       0, 0,               0, 10422, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 2000000},    
    { ETFAccessoryBtDisconnectAccessory,                  1, KBTAddrHeadsetHSPAndHFP, 0, EAccStereoAudio, 0, 10422, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 2000000},
#else
    { ETFAccessoryBtDisconnectAccessory,                  1, KBTAddrHeadsetHSPAndHFP, 0, EAccStereoAudio, 0, 10422, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 2000000},
    { ETFAccessoryNotifyAccessoryModeChanged,             1, 0,                       0, 0,               0, 10422, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 2000000},        
#endif    
    { ETFFindAndCheckRequest,                             1, 0,                       0, 0,               0, 10422, ETFAsyNone, ReqNotifyAccessoryDisconnected,            KErrNone, 1000000},
    { ETFFindAndCheckRequest,                             1, 0,                       0, 0,               0, 10402, ETFAsyNone, ReqDisconnectBtAccessory,                  KErrNone, 1000000},    
    { ETFFindAndCheckRequest,                             1, 0,                       0, 0,               0, 10422, ETFAsyNone, ReqNotifyAccessoryConnectionStatusChanged, KErrNone, 1000000},
    { ETFFindAndCheckRequest,                             1, 0,                       0, 0,               0, 10422, ETFAsyNone, ReqNotifyAccessoryModeChanged,             KErrNone, 1000000},
    { ETFClearStack,                                      0, 0,                       0, 0,               0, 10422, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 0},    
    { ETFAccessoryMode_Close,                             0, 0,                       0, 0,               0, 10422, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 0},
    { ETFAccessorySingleConnection_Close,                 0, 0,                       0, 0,               0, 10422, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 0},
    { ETFAccessoryBtControl_Close,                         0, 0,                       0, 0,               0, 10422, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 2000000},
    { ETFAccessoryConnection_Close,                       0, 0,                       0, 0,               0, 10422, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 0}
    };
    
//
// [M10423] [[AccServer] BT Accessory Value Changed Notify (TInt/KAccAudioLatency)]
//
const TTFAccessoryTestCaseStateControl KTFBTAccessoryValueChangedNotifyIntStates[]=
    {
    { ETFAccessoryBtControl_Open,              0, 0,                 0, 0,               0, 10423, ETFAsyNone, ReqAccRequestNone,              KErrNone, 0},
    { ETFAccessorySingleConnection_Open,       0, 0,                 0, 0,               0, 10423, ETFAsyNone, ReqAccRequestNone,              KErrNone, 0},
    { ETFAccessoryNotifyNewAccessoryConnected, 0, 0,                 0, 0,               0, 10423, ETFAsyNone, ReqAccRequestNone,              KErrNone, 1000000},
    { ETFAccessoryBtConnectAccessory,          0, KBTAddrHeadsetHFP, 0, 0,               0, 10423, ETFAsyNone, ReqAccRequestNone,              KErrNone, 2000000},
    { ETFFindAndCheckRequest,                  0, 0,                 0, 0,               0, 10423, ETFAsyNone, ReqNotifyNewAccessoryConnected, KErrNone, 2000000},        
    { ETFAccessoryIntGetValue,                 0, 0,                 0, KAccAudioLatency, 0, 10423, ETFAsyNone, ReqAccRequestNone,      KErrNone, 2000000},
    { ETFBTAccessoryValueChangedNotifyInt,     0, KBTAddrHeadsetHFP, 0, KAccAudioLatency, 70000, 10423, ETFAsyNone, ReqAccRequestNone,      KErrNone, 2000000},
    { ETFAccessoryIntGetValue,                 0, 0,                 70000, KAccAudioLatency, 0, 10423, ETFAsyNone, ReqAccRequestNone,      KErrNone, 2000000},
    { ETFBTAccessoryValueChangedNotifyInt,     0, KBTAddrHeadsetHFP, 0, KAccAudioLatency, 1, 10423, ETFAsyNone, ReqAccRequestNone,      KErrNone, 2000000},
    { ETFAccessoryIntGetValue,                 0, 0,                 1, KAccAudioLatency, 0, 10423, ETFAsyNone, ReqAccRequestNone,      KErrNone, 2000000},
    { ETFAccessoryNotifyAccessoryDisconnected, 0, 0,                 0, 0,               0, 10423, ETFAsyNone, ReqAccRequestNone,              KErrNone, 2000000},
    { ETFAccessoryBtDisconnectAccessory,       0, KBTAddrHeadsetHFP, 0, 0,               0, 10423, ETFAsyNone, ReqAccRequestNone,              KErrNone, 2000000},
    { ETFFindAndCheckRequest,                  0, 0,                 0, 0,               0, 10423, ETFAsyNone, ReqNotifyAccessoryDisconnected, KErrNone, 2000000},
    { ETFAccessorySingleConnection_Close,      0, 0,                 0, 0,               0, 10423, ETFAsyNone, ReqAccRequestNone,              KErrNone, 0},
    { ETFAccessoryBtControl_Close,             0, 0,                 0, 0,               0, 10423, ETFAsyNone, ReqAccRequestNone,              KErrNone, 0},
    };

    
/*****************************                            *********************************/
/***************************** Bt and Audio Control Tests *********************************/
/*****************************                            *********************************/


//
// [M10501] [[AccServer] BT And Audio Link Open]
//
const TTFAccessoryTestCaseStateControl KTFBtAndAudioLinkOpenStates[17]=
    {
    { ETFAccessoryBtControl_Open,                 0, 0,                       0, 0,               0, 10501, ETFAsyNone, ReqAccRequestNone,                  KErrNone,   0},
    { ETFAccessoryAudioControl_Open,              0, 0,                       0, 0,               0, 10501, ETFAsyNone, ReqAccRequestNone,                  KErrNone,   0},
    { ETFAccessorySingleConnection_Open,          0, 0,                       0, 0,               0, 10501, ETFAsyNone, ReqAccRequestNone,                  KErrNone,   0},
    { ETFAccessoryNotifyNewAccessoryConnected,    0, 0,                       0, 0,               0, 10501, ETFAsyNone, ReqAccRequestNone,                  KErrNone,   1000000},
    { ETFAccessoryBtConnectAccessory,             0, KBTAddrHeadsetHSPAndHFP, 0, 0,               0, 10501, ETFAsyNone, ReqAccRequestNone,                  KErrNone,   2000000},
    { ETFFindAndCheckRequest,                     0, 0,                       0, 0,               0, 10501, ETFAsyNone, ReqConnectBtAccessory,              KErrNone,   2000000},
    { ETFFindAndCheckRequest,                     0, 0,                       0, 0,               0, 10501, ETFAsyNone, ReqNotifyNewAccessoryConnected,     KErrNone,   2000000},
    { ETFAccessoryNotifyAccessoryAudioLinkOpened, 0, 0,                       0, 0,               0, 10501, ETFAsyNone, ReqAccRequestNone,                  KErrNone,   2000000},
    { ETFAccessoryNotifyBluetoothAudioLinkOpenReq,0, KBTAddrHeadsetHSPAndHFP, 0, 0,               0, 10501, ETFAsyNone, ReqAccRequestNone,                  KErrNone,   2000000},
    { ETFAccessoryAudioLinkOpen,                  0, 0,                       0, EAccStereoAudio, 0, 10501, ETFAsyNone, ReqAccRequestNone,                  KErrNone,   2000000},
    { ETFFindAndCheckRequest,                     0, EAccStereoAudio,         0, 0,               0, 10501, ETFAsyNone, ReqNotifyBluetoothAudioLinkOpenReq, KErrNone,   2000000},
    { ETFAccessoryBluetoothAudioLinkOpenedNotify, 0, KBTAddrHeadsetHSPAndHFP, 0, EAccStereoAudio, 0, 10501, ETFAsyNone, ReqAccRequestNone,                  KErrNone,   2000000},
    { ETFFindAndCheckRequest,                     0, KTFDontDeleteObserver,   0, 0,               0, 10501, ETFAsyNone, ReqNotifyAccessoryAudioLinkOpened,  KErrNone,   2000000},
    { ETFCancelNotifyAccessoryAudioLinkOpened,    0, 0,                       0, 0,               0, 10501, ETFAsyNone, ReqAccRequestNone,                  KErrNone,   0},
    { ETFFindAndCheckRequest,                     0, 0,                       0, 0,               0, 10501, ETFAsyNone, ReqNotifyAccessoryAudioLinkOpened,  KErrCancel, 2000000},
    { ETFAccessoryBluetoothAudioLinkOpenResp,     0, KBTAddrHeadsetHSPAndHFP, 0, 0,               0, 10501, ETFAsyNone, ReqAccRequestNone,                  KErrNone,   2000000},
    { ETFFindAndCheckRequest,                     0, 0,                       0, 0,               0, 10501, ETFAsyNone, ReqAccessoryAudioLinkOpen,          KErrNone,   2000000},
    };

//
// [M10502] [[AccServer] BT And Audio Link Close]
//
const TTFAccessoryTestCaseStateControl KTFBtAndAudioLinkCloseStates[17]=
    {
    { ETFAccessoryNotifyAccessoryAudioLinkClosed,   0, 0,                       0, 0,               0, 10502, ETFAsyNone, ReqAccRequestNone,                   KErrNone,   2000000},
    { ETFAccessoryNotifyBluetoothAudioLinkCloseReq, 0, KBTAddrHeadsetHSPAndHFP, 0, 0,               0, 10502, ETFAsyNone, ReqAccRequestNone,                   KErrNone,   2000000},
    { ETFAccessoryAccessoryAudioLinkClose,          0, 0,                       0, EAccStereoAudio, 0, 10502, ETFAsyNone, ReqAccRequestNone,                   KErrNone,   2000000},
    { ETFFindAndCheckRequest,                       0, EAccStereoAudio,         0, 0,               0, 10502, ETFAsyNone, ReqNotifyBluetoothAudioLinkCloseReq, KErrNone,   2000000},
    { ETFAccessoryBluetoothAudioLinkClosedNotify,   0, KBTAddrHeadsetHSPAndHFP, 0, EAccStereoAudio, 0, 10502, ETFAsyNone, ReqAccRequestNone,                   KErrNone,   2000000},
    { ETFFindAndCheckRequest,                       0, KTFDontDeleteObserver,   0, 0,               0, 10502, ETFAsyNone, ReqNotifyAccessoryAudioLinkClosed,   KErrNone,   2000000},
    { ETFCancelNotifyAccessoryAudioLinkClosed,      0, 0,                       0, 0,               0, 10502, ETFAsyNone, ReqAccRequestNone,                   KErrNone,   0},
    { ETFFindAndCheckRequest,                       0, 0,                       0, 0,               0, 10502, ETFAsyNone, ReqNotifyAccessoryAudioLinkClosed,   KErrCancel, 2000000},
    { ETFAccessoryBluetoothAudioLinkCloseResp,      0, KBTAddrHeadsetHSPAndHFP, 0, 0,               0, 10502, ETFAsyNone, ReqAccRequestNone,                   KErrNone,   2000000},
    { ETFFindAndCheckRequest,                       0, 0,                       0, 0,               0, 10502, ETFAsyNone, ReqAccessoryAudioLinkClose,          KErrNone,   2000000},
    { ETFAccessoryNotifyAccessoryDisconnected,      0, 0,                       0, 0,               0, 10502, ETFAsyNone, ReqAccRequestNone,                   KErrNone,   2000000},
    { ETFAccessoryBtDisconnectAccessory,            0, KBTAddrHeadsetHSPAndHFP, 0, 0,               0, 10502, ETFAsyNone, ReqAccRequestNone,                   KErrNone,   2000000},
    { ETFFindAndCheckRequest,                       0, 0,                       0, 0,               0, 10502, ETFAsyNone, ReqNotifyAccessoryDisconnected,      KErrNone,   2000000},
    { ETFClearStack,                                0, 0,                       0, 0,               0, 10502, ETFAsyNone, ReqAccRequestNone,                   KErrNone,   0},
    { ETFAccessoryAudioControl_Close,               0, 0,                       0, 0,               0, 10502, ETFAsyNone, ReqAccRequestNone,                   KErrNone,   0},
    { ETFAccessoryBtControl_Close,                  0, 0,                       0, 0,               0, 10502, ETFAsyNone, ReqAccRequestNone,                   KErrNone,   0},
    { ETFAccessorySingleConnection_Close,           0, 0,                       0, 0,               0, 10502, ETFAsyNone, ReqAccRequestNone,                   KErrNone,   0},
    };


//
// [M10503] [[AccServer] Accessory initiated BT Audio Transfer to Phone from BT Accessory]
//
const TTFAccessoryTestCaseStateControl KTFBtAudioTransferFromBTtoPhoneStates[26]=
    {
    { ETFAccessoryBtControl_Open,                 0, 0,                       0, 0, 0, 10503, ETFAsyNone,         ReqAccRequestNone,                  KErrNone, 0},
    { ETFAccessoryAudioControl_Open,              0, 0,                       0, 0, 0, 10503, ETFAsyNone,         ReqAccRequestNone,                  KErrNone, 0},
    { ETFAccessorySingleConnection_Open,          0, 0,                       0, 0, 0, 10503, ETFAsyNone,         ReqAccRequestNone,                  KErrNone, 0},
    { ETFAccessoryNotifyNewAccessoryConnected,    0, 0,                       0, 0, 0, 10503, ETFAsyNone,         ReqAccRequestNone,                  KErrNone, 1000000},
    { ETFAccessoryBtConnectAccessory,             0, KBTAddrHeadsetHSPAndHFP, 0, 0, 0, 10503, ETFAsyNone,         ReqAccRequestNone,                  KErrNone, 2000000},
    { ETFFindAndCheckRequest,                     0, 0,                       0, 0, 0, 10503, ETFAsyNone,         ReqConnectBtAccessory,              KErrNone, 2000000},
    { ETFFindAndCheckRequest,                     0, 0,                       0, 0, 0, 10503, ETFAsyNone,         ReqNotifyNewAccessoryConnected,     KErrNone, 2000000},
//  { ETFAccessoryNotifyAccessoryAudioLinkOpened, 0, 0,                       0, 0, 0, 10503, ETFAsyNone,         ReqAccRequestNone,                  KErrNone, 2000000},
    { ETFAccessoryNotifyBluetoothAudioLinkOpenReq,0, KBTAddrHeadsetHSPAndHFP, 0, 0, 0, 10503, ETFAsyNone,         ReqAccRequestNone,                  KErrNone, 2000000},
    { ETFAccessoryAudioLinkOpen,                  0, 0,                       0, 0, 0, 10503, ETFAsyNone,         ReqAccRequestNone,                  KErrNone, 2000000},
    { ETFFindAndCheckRequest,                     0, 0,                       0, 0, 0, 10503, ETFAsyNone,         ReqNotifyBluetoothAudioLinkOpenReq, KErrNone, 2000000},
    { ETFAccessoryBluetoothAudioLinkOpenedNotify, 0, KBTAddrHeadsetHSPAndHFP, 0, 0, 0, 10503, ETFAsyNone,         ReqAccRequestNone,                  KErrNone, 2000000},
//  { ETFFindAndCheckRequest,                     0, 0,                       0, 0, 0, 10503, ETFAsyNone,         ReqNotifyAccessoryAudioLinkOpened,  KErrNone, 2000000},
//  { ETFCancelNotifyAccessoryAudioLinkOpened,    0, 0,                       0, 0, 0, 10503, ETFAsyNone,         ReqAccRequestNone,                  KErrNone, 2000000},
//  { ETFFindAndCheckRequest,                     0, 0,                       0, 0, 0, 10503, ETFAsyNone,         ReqNotifyAccessoryAudioLinkOpened,  KErrNone, 2000000},
    { ETFAccessoryBluetoothAudioLinkOpenResp,     0, KBTAddrHeadsetHSPAndHFP, 0, 0, 0, 10503, ETFAsyNone,         ReqAccRequestNone,                  KErrNone, 2000000},
    { ETFFindAndCheckRequest,                     0, 0,                       0, 0, 0, 10503, ETFAsyNone,         ReqAccessoryAudioLinkOpen,          KErrNone, 2000000},
    { ETFAccessoryNotifyAccessoryAudioLinkClosed, 0, 0,                       0, 0, 0, 10503, ETFAsyNone,         ReqAccRequestNone,                  KErrNone, 2000000},
    { ETFAccessoryPublishAndSubscribe,            0, KBTAddrHeadsetHSPAndHFP, 0, 0, 0, 10503, ETFBTALinkClosedNo, ReqAccRequestNone,                  KErrNone, 2000000},
    //This is made form asy
    { ETFFindAndCheckRequest,                     0, 0,                       0, 0, 0, 10503, ETFAsyNone,         ReqNotifyAccessoryAudioLinkClosed,  KErrNone, 2000000},
    //Just in case
    { ETFAccessoryNotifyAccessoryAudioLinkOpened, 0, 0,                       0, 0, 0, 10503, ETFAsyNone,         ReqAccRequestNone,                  KErrNone, 2000000},
    { ETFAccessoryPublishAndSubscribe,            0, KBTAddrHeadsetHSPAndHFP, 0, 0, 0, 10503, ETFBTALinkOpenedNo, ReqAccRequestNone,                  KErrNone, 2000000},
    //This is made form asy
    { ETFFindAndCheckRequest,                     0, 0,                       0, 0, 0, 10503, ETFAsyNone,         ReqNotifyAccessoryAudioLinkOpened,  KErrNone, 2000000},
    //Just in case end
    { ETFAccessoryNotifyAccessoryDisconnected,    0, 0,                       0, 0, 0, 10503, ETFAsyNone,         ReqAccRequestNone,                  KErrNone, 2000000},
    { ETFAccessoryBtDisconnectAccessory,          0, KBTAddrHeadsetHSPAndHFP, 0, 0, 0, 10503, ETFAsyNone,         ReqAccRequestNone,                  KErrNone, 2000000},
    { ETFFindAndCheckRequest,                     0, 0,                       0, 0, 0, 10503, ETFAsyNone,         ReqNotifyAccessoryDisconnected,     KErrNone, 2000000},
    { ETFClearStack,                              0, 0,                       0, 0, 0, 10503, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 0},
    { ETFAccessoryAudioControl_Close,             0, 0,                       0, 0, 0, 10503, ETFAsyNone,         ReqAccRequestNone,                  KErrNone, 0},
    { ETFAccessoryBtControl_Close,                0, 0,                       0, 0, 0, 10503, ETFAsyNone,         ReqAccRequestNone,                  KErrNone, 0},
    { ETFAccessorySingleConnection_Close,         0, 0,                       0, 0, 0, 10503, ETFAsyNone,         ReqAccRequestNone,                  KErrNone, 0},
    };

//
// [M10504] [[AccServer] AudioLink Open and Close cancels]
//
const TTFAccessoryTestCaseStateControl KTFBtAudioLinkCancels[]=
    {
    { ETFAccessoryBtControl_Open,                 0, 0,                       0,          0,               0, 10504, ETFAsyNone, ReqAccRequestNone,                  KErrNone,   0},
    { ETFAccessoryAudioControl_Open,              0, 0,                       0,          0,               0, 10504, ETFAsyNone, ReqAccRequestNone,                  KErrNone,   0},
    { ETFAccessorySingleConnection_Open,          0, 0,                       0,          0,               0, 10504, ETFAsyNone, ReqAccRequestNone,                  KErrNone,   0},
    { ETFAccessoryNotifyNewAccessoryConnected,    0, 0,                       0,          0,               0, 10504, ETFAsyNone, ReqAccRequestNone,                  KErrNone,   1000000},
    { ETFAccessoryBtConnectAccessory,             0, KBTAddrHeadsetHSPAndHFP, 0,          0,               0, 10504, ETFAsyNone, ReqAccRequestNone,                  KErrNone,   2000000},
    { ETFFindAndCheckRequest,                     0, 0,                       0,          0,               0, 10504, ETFAsyNone, ReqConnectBtAccessory,              KErrNone,   2000000},
    { ETFFindAndCheckRequest,                     0, 0,                       0,          0,               0, 10504, ETFAsyNone, ReqNotifyNewAccessoryConnected,     KErrNone,   2000000},
    { ETFFindAndCheckRequest,                     0, KTFDontDeleteObserver,   0,          0,               0, 10504, ETFAsyNone, ReqNotifyNewAccessoryConnected,     KErrNone,   0},          
    //*********************************************
    // Open/CancalOpen/Close/CancelClose
    //********************************************* 
                { ETFAccessoryNotifyBluetoothAudioLinkOpenReq,  0, KBTAddrHeadsetHSPAndHFP, 0,          0,               0, 10504, ETFAsyNone, ReqAccRequestNone,                   KErrNone,   2000000},
                { ETFAccessoryNotifyAccessoryAudioLinkOpened,   0, 0,                       0,          0,               0, 10504, ETFAsyNone, ReqAccRequestNone,                   KErrNone,   2000000},
                { ETFAccessoryNotifyAccessoryAudioLinkClosed,   0, 0,                       0,          0,               0, 10504, ETFAsyNone, ReqAccRequestNone,                   KErrNone,   2000000},
                { ETFAccessoryNotifyBluetoothAudioLinkCloseReq, 0, KBTAddrHeadsetHSPAndHFP, 0,          0,               0, 10504, ETFAsyNone, ReqAccRequestNone,                   KErrNone,   2000000},
/*Open*/        { ETFAccessoryAudioLinkOpen,                    0, 0,                       0,          EAccStereoAudio, 0, 10504, ETFAsyNone, ReqAccRequestNone,                   KErrNone,   2000000},
                { ETFFindAndCheckRequest,                       0, EAccStereoAudio,         0,          0,               0, 10504, ETFAsyNone, ReqNotifyBluetoothAudioLinkOpenReq,  KErrNone,   2000000},
/*CancalOpen*/  { ETFCancelAccessoryAudioLinkOpen,              0, 0,                       0,          0,               0, 10504, ETFAsyNone, ReqAccRequestNone,                   KErrNone,   0},
                { ETFAccessoryBluetoothAudioLinkOpenResp,       0, KBTAddrHeadsetHSPAndHFP, 0,          0,               0, 10504, ETFAsyNone, ReqAccRequestNone,                   KErrCancel, 2000000},
                { ETFFindAndCheckRequest,                       0, 0,                       0,          0,               0, 10504, ETFAsyNone, ReqAccessoryAudioLinkOpen,           KErrCancel, 2000000},
                { ETFAccessoryBluetoothAudioLinkOpenedNotify,   0, KBTAddrHeadsetHSPAndHFP, 0,          EAccStereoAudio, 0, 10504, ETFAsyNone, ReqAccRequestNone,                   KErrCancel, 2000000},  
/*Close*/       { ETFAccessoryAccessoryAudioLinkClose,          0, 0,                       0,          EAccStereoAudio, 0, 10504, ETFAsyNone, ReqAccRequestNone,                   KErrNone,   2000000},
                { ETFFindAndCheckRequest,                       0, EAccStereoAudio,         0,          0,               0, 10504, ETFAsyNone, ReqNotifyBluetoothAudioLinkCloseReq, KErrNone,   2000000},
/*CancelClose*/ { ETFCancelAccessoryAudioLinkClose,             0, 0,                       0,          0,               0, 10504, ETFAsyNone, ReqAccRequestNone,                   KErrNone,   0},
                { ETFAccessoryBluetoothAudioLinkClosedNotify,   0, KBTAddrHeadsetHSPAndHFP, 0,          EAccStereoAudio, 0, 10504, ETFAsyNone, ReqAccRequestNone,                   KErrCancel, 2000000},
                { ETFAccessoryBluetoothAudioLinkCloseResp,      0, KBTAddrHeadsetHSPAndHFP, KErrGeneral,0,               0, 10504, ETFAsyNone, ReqAccRequestNone,                   KErrCancel, 2000000},
                { ETFFindAndCheckRequest,                       0, 0,                       0,          0,               0, 10504, ETFAsyNone, ReqAccessoryAudioLinkClose,          KErrCancel, 2000000},
    //*********************************************
    // Open/Close
    //*********************************************     
                { ETFAccessoryNotifyBluetoothAudioLinkOpenReq,  0, KBTAddrHeadsetHSPAndHFP, 0,          0,               0, 10504, ETFAsyNone, ReqAccRequestNone,                   KErrNone,   2000000},
                { ETFAccessoryNotifyBluetoothAudioLinkCloseReq, 0, KBTAddrHeadsetHSPAndHFP, 0,          0,               0, 10504, ETFAsyNone, ReqAccRequestNone,                   KErrNone,   2000000},
/*Open*/        { ETFAccessoryAudioLinkOpen,                    0, 0,                       0,          EAccStereoAudio, 0, 10504, ETFAsyNone, ReqAccRequestNone,                   KErrNone,   2000000},
                { ETFFindAndCheckRequest,                       0, EAccStereoAudio,         0,          0,               0, 10504, ETFAsyNone, ReqNotifyBluetoothAudioLinkOpenReq,  KErrNone,   2000000},
                { ETFAccessoryBluetoothAudioLinkOpenedNotify,   0, KBTAddrHeadsetHSPAndHFP, 0,          EAccStereoAudio, 0, 10504, ETFAsyNone, ReqAccRequestNone,                   KErrNone,   2000000},
                { ETFAccessoryBluetoothAudioLinkOpenResp,       0, KBTAddrHeadsetHSPAndHFP, 0,          0,               0, 10504, ETFAsyNone, ReqAccRequestNone,                   KErrNone,   2000000},
                { ETFFindAndCheckRequest,                       0, 0,                       0,          0,               0, 10504, ETFAsyNone, ReqAccessoryAudioLinkOpen,           KErrNone,   2000000},
/*Close*/       { ETFAccessoryAccessoryAudioLinkClose,          0, 0,                       0,          EAccStereoAudio, 0, 10504, ETFAsyNone, ReqAccRequestNone,                   KErrNone,   2000000},
                { ETFFindAndCheckRequest,                       0, EAccStereoAudio,         0,          0,               0, 10504, ETFAsyNone, ReqNotifyBluetoothAudioLinkCloseReq, KErrNone,   2000000},
                { ETFAccessoryBluetoothAudioLinkClosedNotify,   0, KBTAddrHeadsetHSPAndHFP, 0,          EAccStereoAudio, 0, 10504, ETFAsyNone, ReqAccRequestNone,                   KErrNone,   2000000},
                { ETFFindAndCheckRequest,                       0, KTFDontDeleteObserver,   0,          0,               0, 10504, ETFAsyNone, ReqNotifyAccessoryAudioLinkClosed,   KErrNone,   2000000},
                { ETFAccessoryBluetoothAudioLinkCloseResp,      0, KBTAddrHeadsetHSPAndHFP, 0,          0,               0, 10504, ETFAsyNone, ReqAccRequestNone,                   KErrNone,   2000000},
                { ETFFindAndCheckRequest,                       0, 0,                       0,          0,               0, 10504, ETFAsyNone, ReqAccessoryAudioLinkClose,          KErrNone,   2000000},
    //
    { ETFAccessoryBtDisconnectAccessory,          0, KBTAddrHeadsetHSPAndHFP, 0,          0,               0, 10504, ETFAsyNone, ReqAccRequestNone,                  KErrNone,   2000000},
    { ETFClearStack,                              0, 0,                       0,          0,            0,    10504, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 0},
    { ETFAccessoryAudioControl_Close,             0, 0,                       0,          0,               0, 10504, ETFAsyNone, ReqAccRequestNone,                  KErrNone,   0},
    { ETFAccessoryBtControl_Close,                0, 0,                       0,          0,               0, 10504, ETFAsyNone, ReqAccRequestNone,                  KErrNone,   0},
    { ETFAccessorySingleConnection_Close,         0, 0,                       0,          0,               0, 10504, ETFAsyNone, ReqAccRequestNone,                  KErrNone,   0},
    };

/*****************************                       *********************************/
/***************************** Accessory Modes Tests *********************************/
/*****************************                       *********************************/


//
// [M10601] [[AccServer] Notify Accessory Mode Changed to HandPortable]
//
const TTFAccessoryTestCaseStateControl KTFHandPortableAccessoryModeStates[7]=
    {
    { ETFAccessoryMode_Open,                 0, 0,              0, 0, 0, 10601, ETFAsyNone, ReqAccRequestNone,             KErrNone, 0},
    { ETFConnectWiredAccessory,              0, 0x010103,       0, 0, 0, 10601, ETFAsyConn, ReqAccRequestNone,             KErrNone, 2000000},
    { ETFAccessoryNotifyAccessoryModeChanged,0, 0,              0, 0, 0, 10601, ETFAsyNone, ReqAccRequestNone,             KErrNone, 1000000},
    { ETFDisconnectWiredAccessory,           0, 0,              0, 0, 0, 10601, ETFAsyDisc, ReqAccRequestNone,             KErrNone, 3000000},
    { ETFFindAndCheckRequest,                0, 0,              0, 0, 0, 10601, ETFAsyNone, ReqNotifyAccessoryModeChanged, KErrNone, 1000000},
    { ETFClearStack,                         0, 0,              0, 0, 0, 10601, ETFAsyNone, ReqAccRequestNone,             KErrNone, 0},
    { ETFAccessoryMode_Close,                0, 0,              0, 0, 0, 10601, ETFAsyNone, ReqAccRequestNone,             KErrNone, 0},
    };
    
//
// [M10602] [[AccServer] Notify Accessory Mode Changed to Wired Headset]
//
const TTFAccessoryTestCaseStateControl KTFWiredHeadsetAccessoryModeStates[]=
    {
    { ETFAccessorySingleConnection_Open,       0, 0,                    0,      0, 0, 10602, ETFAsyNone, ReqAccRequestNone,              KErrNone, 1000000},
    { ETFAccessoryMode_Open,                   0, 0,                    0,      0, 0, 10602, ETFAsyNone, ReqAccRequestNone,              KErrNone, 1000000},
    { ETFAccessoryNotifyNewAccessoryConnected, 0, 0,                    0,      0, 0, 10602, ETFAsyNone, ReqAccRequestNone,              KErrNone, 1000000},
    { ETFAccessoryNotifyAccessoryModeChanged,  0, 0,                    0,      0, 0, 10602, ETFAsyNone, ReqAccRequestNone,              KErrNone, 1000000},
    { ETFConnectWiredAccessory,                0, 0x010103,             0,      0, 0, 10602, ETFAsyConn, ReqAccRequestNone,              KErrNone, 3000000},
    { ETFFindAndCheckRequest,                  0, 0,                    0,      0, 0, 10602, ETFAsyNone, ReqNotifyNewAccessoryConnected, KErrNone, 1000000},
    { ETFFindAndCheckRequest,                  0, 1,                    0,      0, 0, 10602, ETFAsyNone, ReqNotifyAccessoryModeChanged,  KErrNone, 1000000},
    { ETFAccessoryAccessoryModeSync,           0, EAccModeWiredHeadset, EFalse, 0, 0, 10602, ETFAsyNone, ReqAccRequestNone,              KErrNone, 1000000},
#ifdef __WINS__ // Just ignore because audio policy is set the routing status!
    { ETFAccessoryNotifyAccessoryModeChanged,  0, 0,                    0,      0, 0, 10602, ETFAsyNone, ReqAccRequestNone,              KErrNone, 1000000},
    { ETFAudioRoutingStatusNotify,             0, ETrue,                0,      0, 0, 10602, ETFAsyNone, ReqAccRequestNone,              KErrNone, 2000000},
    { ETFFindAndCheckRequest,                  0, 1,                    0,      0, 0, 10602, ETFAsyNone, ReqNotifyAccessoryModeChanged,  KErrNone, 1000000},
    { ETFAccessoryAccessoryModeSync,           0, EAccModeWiredHeadset, ETrue,  0, 0, 10602, ETFAsyNone, ReqAccRequestNone,              KErrNone, 1000000},
#endif        
    { ETFDisconnectWiredAccessory,             0, 0,                    0,      0, 0, 10602, ETFAsyDisc, ReqAccRequestNone,              KErrNone, 1000000},
    { ETFAudioRoutingStatusNotify,             0, EFalse,               0,      0, 0, 10602, ETFAsyNone, ReqAccRequestNone,              KErrNone, 1000000},
    { ETFClearStack,                           0, 0,                    0,      0, 0, 10602, ETFAsyNone, ReqAccRequestNone,              KErrNone, 1000000},
    { ETFAccessoryMode_Close,                  0, 0,                    0,      0, 0, 10602, ETFAsyNone, ReqAccRequestNone,              KErrNone, 1000000},
    { ETFAccessorySingleConnection_Close,      0, 0,                    0,      0, 0, 10602, ETFAsyNone, ReqAccRequestNone,              KErrNone, 1000000},
    };

//
// [M10603] [[AccServer] Notify Accessory Mode Changed to WirelessHeadset]
//
const TTFAccessoryTestCaseStateControl KTFWirelessHeadesetAccessoryModeStates[9]=
    {
    { ETFAccessoryMode_Open,                  0, 0,                 0, 0, 0, 10603, ETFAsyNone, ReqAccRequestNone,             KErrNone, 1000000},
    { ETFAccessoryBtControl_Open,             0, 0,                 0, 0, 0, 10603, ETFAsyNone, ReqAccRequestNone,             KErrNone, 1000000},
    { ETFAccessoryNotifyAccessoryModeChanged, 0, 0,                 0, 0, 0, 10603, ETFAsyNone, ReqAccRequestNone,             KErrNone, 1000000},
    { ETFAccessoryBtConnectAccessory,         0, KBTAddrHeadsetHSP, 0, 0, 0, 10603, ETFAsyNone, ReqAccRequestNone,             KErrNone, 3000000},
    { ETFFindAndCheckRequest,                 0, 2,                 0, 0, 0, 10603, ETFAsyNone, ReqNotifyAccessoryModeChanged, KErrNone, 1000000},
    { ETFAccessoryBtDisconnectAccessory,      0, KBTAddrHeadsetHSP, 0, 0, 0, 10603, ETFAsyNone, ReqAccRequestNone,             KErrNone, 1000000},
    { ETFClearStack,                          0, 0,                 0, 0, 0, 10603, ETFAsyNone, ReqAccRequestNone,             KErrNone, 1000000},
    { ETFAccessoryBtControl_Close,            0, 0,                 0, 0, 0, 10603, ETFAsyNone, ReqAccRequestNone,             KErrNone, 1000000},
    { ETFAccessoryMode_Close,                 0, 0,                 0, 0, 0, 10603, ETFAsyNone, ReqAccRequestNone,             KErrNone, 1000000}
    };

//
// [M10604] [[AccServer] Notify Accessory Mode Changed to Wired CarKit]
//
const TTFAccessoryTestCaseStateControl KTFWiredCarkitAccessoryModeStates[]=
    {
    { ETFAccessoryPublishAndSubscribe,       0, 1,         0, KAccExtPowerSupply, 0, 10604, ETFAsySetCapability, ReqAccRequestNone,     KErrNone, 1000000},
    { ETFAccessoryMode_Open,                 0, 0,         0, 0,          0, 10604, ETFAsyNone,          ReqAccRequestNone,             KErrNone, 1000000},
    { ETFAccessoryNotifyAccessoryModeChanged,0, 0,         0, 0,          0, 10604, ETFAsyNone,          ReqAccRequestNone,             KErrNone, 1000000},
    { ETFConnectWiredAccessory,              0, 0x020104,  0, 0,          0, 10604, ETFAsyConn,          ReqAccRequestNone,             KErrNone, 3000000},
    { ETFFindAndCheckRequest,                0, 3,         0, 0,          0, 10604, ETFAsyNone,          ReqNotifyAccessoryModeChanged, KErrNone, 1000000},
    { ETFClearStack,                         0, 0,         0, 0,          0, 10604, ETFAsyNone,          ReqAccRequestNone,             KErrNone, 1000000},
    { ETFDisconnectWiredAccessory,           0, 0,         0, 0,          0, 10604, ETFAsyDisc,          ReqAccRequestNone,             KErrNone, 1000000},
    { ETFAccessoryMode_Close,                0, 0,         0, 0,          0, 10604, ETFAsyNone,          ReqAccRequestNone,             KErrNone, 1000000},
    };

//
// [M10605] [[AccServer] Notify Accessory Mode Changed to Wireless CarKit]
//
const TTFAccessoryTestCaseStateControl KTFWirelessCarKitAccessoryModeStatus[9]=
    {
    { ETFAccessoryMode_Open,                  0, 0,              0, 0, 0, 10605, ETFAsyNone, ReqAccRequestNone,             KErrNone, 1000000},
    { ETFAccessoryBtControl_Open,             0, 0,              0, 0, 0, 10605, ETFAsyNone, ReqAccRequestNone,             KErrNone, 1000000},
    { ETFAccessoryNotifyAccessoryModeChanged, 0, 0,              0, 0, 0, 10605, ETFAsyNone, ReqAccRequestNone,             KErrNone, 1000000},
    { ETFAccessoryBtConnectAccessory,         0, KBTAddrCarkit,  0, 0, 0, 10605, ETFAsyNone, ReqAccRequestNone,             KErrNone, 3000000},
    { ETFFindAndCheckRequest,                 0, 4,              0, 0, 0, 10605, ETFAsyNone, ReqNotifyAccessoryModeChanged, KErrNone, 1000000},
    { ETFClearStack,                          0, 0,              0, 0, 0, 10605, ETFAsyNone, ReqAccRequestNone,             KErrNone, 1000000},
    { ETFAccessoryBtDisconnectAccessory,      0, KBTAddrCarkit,  0, 0, 0, 10605, ETFAsyNone, ReqAccRequestNone,             KErrNone, 1000000},
    { ETFAccessoryBtControl_Close,            0, 0,              0, 0, 0, 10605, ETFAsyNone, ReqAccRequestNone,             KErrNone, 1000000},
    { ETFAccessoryMode_Close,                 0, 0,              0, 0, 0, 10605, ETFAsyNone, ReqAccRequestNone,             KErrNone, 1000000}
    };

//
// [M10606] [[AccServer] Notify Accessory Mode Changed to Text Device]
//
const TTFAccessoryTestCaseStateControl KTFTextDeviceAccessoryModeStatus[11]=
    {
    { ETFAccessoryMode_Open,                 0, 0,              0, 0, 0, 10606, ETFAsyNone, ReqAccRequestNone,             KErrNone, 1000000},
    { ETFAccessorySettings_Open,             0, 0,              0, 0, 0, 10606, ETFAsyNone, ReqAccRequestNone,             KErrNone, 1000000},
    { ETFSetHWDeviceSettings,                0, KASTTY,         1, 0, 0, 10606, ETFAsyNone, ReqAccRequestNone,             KErrNone, 1000000}, // Set TTY On!
    { ETFAccessoryNotifyAccessoryModeChanged,0, 0,              0, 0, 0, 10606, ETFAsyNone, ReqAccRequestNone,             KErrNone, 1000000},
    { ETFConnectWiredAccessory,              0, 0x010001,       0, 0, 0, 10606, ETFAsyConn, ReqAccRequestNone,             KErrNone, 3000000},
    { ETFFindAndCheckRequest,                0, 5,              0, 0, 0, 10606, ETFAsyNone, ReqNotifyAccessoryModeChanged, KErrNone, 1000000}, // Mode should be TTY
    { ETFDisconnectWiredAccessory,           0, 0,              0, 0, 0, 10606, ETFAsyDisc, ReqAccRequestNone,             KErrNone, 1000000},
    { ETFSetHWDeviceSettings,                0, KASTTY,         0, 0, 0, 10606, ETFAsyNone, ReqAccRequestNone,             KErrNone, 1000000}, // Set TTY On!
    { ETFClearStack,                         0, 0,              0, 0, 0, 10606, ETFAsyNone, ReqAccRequestNone,             KErrNone, 1000000},
    { ETFAccessoryMode_Close,                0, 0,              0, 0, 0, 10606, ETFAsyNone, ReqAccRequestNone,             KErrNone, 1000000},
    { ETFAccessorySettings_Close,            0, 0,              0, 0, 0, 10606, ETFAsyNone, ReqAccRequestNone,             KErrNone, 1000000},
    };



//
// [M10607] [[AccServer] Notify Accessory Mode Changed to Loopset]
//
const TTFAccessoryTestCaseStateControl KTFLoopsetAccessoryModeStates[11]=
    {
    { ETFAccessoryMode_Open,                 0, 0,              0, 0, 0, 10607, ETFAsyNone, ReqAccRequestNone,             KErrNone, 1000000},
    { ETFAccessorySettings_Open,             0, 0,              0, 0, 0, 10607, ETFAsyNone, ReqAccRequestNone,             KErrNone, 1000000},
    { ETFSetHWDeviceSettings,                0, KASLoopset,     1, 0, 0, 10607, ETFAsyNone, ReqAccRequestNone,             KErrNone, 1000000}, // Set Loopset On!
    { ETFAccessoryNotifyAccessoryModeChanged,0, 0,              0, 0, 0, 10607, ETFAsyNone, ReqAccRequestNone,             KErrNone, 1000000},
    { ETFConnectWiredAccessory,              0, 0x010201,       0, 0, 0, 10607, ETFAsyConn, ReqAccRequestNone,             KErrNone, 3000000},
    { ETFFindAndCheckRequest,                0, 6,              0, 0, 0, 10607, ETFAsyNone, ReqNotifyAccessoryModeChanged, KErrNone, 1000000}, // Mode should be Loopset
    { ETFDisconnectWiredAccessory,           0, 0,              0, 0, 0, 10607, ETFAsyDisc, ReqAccRequestNone,             KErrNone, 1000000},
    { ETFSetHWDeviceSettings,                0, 0,              0, 0, 0, 10607, ETFAsyNone, ReqAccRequestNone,             KErrNone, 1000000}, // Set Loopset On!
    { ETFClearStack,                         0, 0,              0, 0, 0, 10607, ETFAsyNone, ReqAccRequestNone,             KErrNone, 1000000},
    { ETFAccessoryMode_Close,                0, 0,              0, 0, 0, 10607, ETFAsyNone, ReqAccRequestNone,             KErrNone, 1000000},
    { ETFAccessorySettings_Close,            0, 0,              0, 0, 0, 10607, ETFAsyNone, ReqAccRequestNone,             KErrNone, 1000000},
    };

//
// [M10608] [[AccServer] Notify Accessory Mode Changed to Music Stand]
//
const TTFAccessoryTestCaseStateControl KTFMusicStandAccessoryModeStates[]=
    {
    { ETFAccessoryPublishAndSubscribe,       0, 1,        0, KAccExtPowerSupply, 0, 10608, ETFAsySetCapability, ReqAccRequestNone, KErrNone, 1000000},
    { ETFAccessoryMode_Open,                 0, 0,        0, 0,          0, 10608, ETFAsyNone, ReqAccRequestNone,                  KErrNone, 1000000},
    { ETFAccessoryNotifyAccessoryModeChanged,0, 0,        0, 0,          0, 10608, ETFAsyNone, ReqAccRequestNone,                  KErrNone, 1000000},
    { ETFConnectWiredAccessory,              0, 0x020202, 0, 0,          0, 10608, ETFAsyConn, ReqAccRequestNone,                  KErrNone, 3000000},
    { ETFFindAndCheckRequest,                0, 7,        0, 0,          0, 10608, ETFAsyNone, ReqNotifyAccessoryModeChanged,      KErrNone, 1000000},
    { ETFDisconnectWiredAccessory,           0, 0,        0, 0,          0, 10608, ETFAsyDisc, ReqAccRequestNone,                  KErrNone, 1000000},
    { ETFClearStack,                         0, 0,        0, 0,          0, 10608, ETFAsyNone, ReqAccRequestNone,                  KErrNone, 1000000},
    { ETFAccessoryMode_Close,                0, 0,        0, 0,          0, 10608, ETFAsyNone, ReqAccRequestNone,                  KErrNone, 1000000},
    };

//
// [M10609] [[AccServer] Sync Get Accessory Mode]
//
const TTFAccessoryTestCaseStateControl KTFSyncGetAccessoryModeStates[5]=
    {
    { ETFAccessoryMode_Open,           0, 0,              0, 0, 0, 10609, ETFAsyNone, ReqAccRequestNone, KErrNone, 1000000},
    { ETFConnectWiredAccessory,        0, 0x010103,       0, 0, 0, 10609, ETFAsyConn, ReqAccRequestNone, KErrNone, 3000000},
    { ETFAccessoryAccessoryModeSync,   0, 1,              0, 0, 0, 10609, ETFAsyNone, ReqAccRequestNone, KErrNone, 1000000},
    { ETFDisconnectWiredAccessory,     0, 0,              0, 0, 0, 10609, ETFAsyDisc, ReqAccRequestNone, KErrNone, 1000000},
    { ETFAccessoryMode_Close,          0, 0,              0, 0, 0, 10609, ETFAsyNone, ReqAccRequestNone, KErrNone, 1000000},
    };

//
// [M10610] [[AccServer] ASync Get Accessory Mode]
//
const TTFAccessoryTestCaseStateControl KTFASyncGetAccessoryModeStates[7]=
    {
    { ETFAccessoryMode_Open,           0, 0,              0, 0, 0, 10610, ETFAsyNone, ReqAccRequestNone,   KErrNone, 1000000},
    { ETFConnectWiredAccessory,        0, 0x010103,       0, 0, 0, 10610, ETFAsyConn, ReqAccRequestNone,   KErrNone, 3000000},
    { ETFAccessoryAccessoryModeASync,  0, 0,              0, 0, 0, 10610, ETFAsyNone, ReqAccRequestNone,   KErrNone, 3000000},
    { ETFFindAndCheckRequest,          0, 1,              0, 0, 0, 10610, ETFAsyNone, ReqGetAccessoryMode, KErrNone, 1000000},
    { ETFDisconnectWiredAccessory,     0, 0,              0, 0, 0, 10610, ETFAsyDisc, ReqAccRequestNone,   KErrNone, 1000000},
    { ETFClearStack,                   0, 0,              0, 0, 0, 10610, ETFAsyNone, ReqAccRequestNone,   KErrNone, 1000000},
    { ETFAccessoryMode_Close,          0, 0,              0, 0, 0, 10610, ETFAsyNone, ReqAccRequestNone,   KErrNone, 1000000},
    };


/*****************************                 *********************************/
/***************************** Special cases   *********************************/
/*****************************                 *********************************/

//
// [M10701] [[AccServer] MD-1 Power Supply On After Connection] ////KAccExtPowerSupply connect(off)->on->disconnect
//
const TTFAccessoryTestCaseStateControl KTFMD1PowerSupplyOnAfterConnectionState[15]=
    {
    { ETFAccessoryPublishAndSubscribe,          0, 0,         0, KAccExtPowerSupply, 0, 10701, ETFAsySetCapability, ReqAccRequestNone,                 KErrNone, 2000000},
    { ETFAccessoryMode_Open,                    0, 0,         0, 0,          0, 10701, ETFAsyNone,                  ReqAccRequestNone,                 KErrNone, 0},
    { ETFAccessorySingleConnection_Open,        0, 0,         0, 0,          0, 10701, ETFAsyNone,                  ReqAccRequestNone,                 KErrNone, 0},
    { ETFAccessoryNotifyNewAccessoryConnected,  0, 0,         0, 0,          0, 10701, ETFAsyNone,                  ReqAccRequestNone,                 KErrNone, 2000000},
    { ETFAccessoryNotifyAccessoryModeChanged,   0, 0,         0, 0,          0, 10701, ETFAsyNone,                  ReqAccRequestNone,                 KErrNone, 3000000},
    { ETFConnectWiredAccessory,                 0, 0x020202,  0, 0,          0, 10701, ETFAsyConn,                  ReqAccRequestNone,                 KErrNone, 3000000},
    { ETFAccessoryPublishAndSubscribe,          0, 1,         0, KAccExtPowerSupply, 0, 10701, ETFAsyBoolNotify,    ReqAccRequestNone,                 KErrNone, 2000000},
    { ETFFindAndCheckRequest,                   0, 0,         0, 0,          0, 10701, ETFAsyNone,          ReqNotifyNewAccessoryConnected,            KErrNone, 2000000},
    { ETFFindAndCheckRequest,                   0, 7,         0, 0,          0, 10701, ETFAsyNone,          ReqNotifyAccessoryModeChanged,             KErrNone, 2000000},
    { ETFAccessoryNotifyAccessoryDisconnected,  0, 0,         0, 0,          0, 10701, ETFAsyNone,          ReqAccRequestNone,                         KErrNone, 3000000},
    { ETFDisconnectWiredAccessory,              0, 0,         0, 0,          0, 10701, ETFAsyDisc,          ReqAccRequestNone,                         KErrNone, 2000000},
    { ETFFindAndCheckRequest,                   0, 0,         0, 0,          0, 10701, ETFAsyNone,          ReqNotifyAccessoryDisconnected,            KErrNone, 2000000},
    { ETFClearStack,                            0, 0,         0, 0,          0, 10701, ETFAsyNone,          ReqAccRequestNone,                         KErrNone, 0},
    { ETFAccessorySingleConnection_Close,       0, 0,         0, 0,          0, 10701, ETFAsyNone,          ReqAccRequestNone,                         KErrNone, 0},
    { ETFAccessoryMode_Close,                   0, 0,         0, 0,          0, 10701, ETFAsyNone,          ReqAccRequestNone,                         KErrNone, 0},
    };

//
// [M10702] [[AccServer] MD-1 Power Supply On/Off] //KAccExtPowerSupply connect(on)->off->on->disconnect
//
const TTFAccessoryTestCaseStateControl KTFMD1PowerSupplyOnOffState[]=
    {
    { ETFAccessoryPublishAndSubscribe,                    0, 1,         0, KAccExtPowerSupply, 0, 10702, ETFAsySetCapability, ReqAccRequestNone,                         KErrNone, 2000000},
    { ETFAccessoryConnection_Open,                        0, 0,         0, 0,                  0, 10702, ETFAsyNone,          ReqAccRequestNone,                         KErrNone, 0},
    { ETFAccessoryMode_Open,                              0, 0,         0, 0,                  0, 10702, ETFAsyNone,          ReqAccRequestNone,                         KErrNone, 0},
    { ETFAccessorySingleConnection_Open,                  0, 0,         0, 0,                  0, 10702, ETFAsyNone,          ReqAccRequestNone,                         KErrNone, 0},
    { ETFAccessoryNotifyNewAccessoryConnected,            0, 0,         0, 0,                  0, 10702, ETFAsyNone,          ReqAccRequestNone,                         KErrNone, 1000000},
    { ETFAccessoryNotifyAccessoryModeChanged,             0, 0,         0, 0,                  0, 10702, ETFAsyNone,          ReqAccRequestNone,                         KErrNone, 2000000},
    { ETFAccessoryNotifyAccessoryConnectionStatusChanged, 0, 0,         0, 0,                  0, 10702, ETFAsyNone,          ReqAccRequestNone,                         KErrNone, 2000000},
    { ETFConnectWiredAccessory,                           0, 0x020202,  0, 0,                  0, 10702, ETFAsyConn,          ReqAccRequestNone,                         KErrNone, 2000000},
    { ETFFindAndCheckRequest,                             0, 0,         0, 0,                  0, 10702, ETFAsyNone,          ReqNotifyNewAccessoryConnected,            KErrNone, 2000000},
    { ETFFindAndCheckRequest,                             0, 0,         0, 0,                  0, 10702, ETFAsyNone,          ReqNotifyAccessoryConnectionStatusChanged, KErrNone, 2000000},
    { ETFFindAndCheckRequest,                             0, 7,         0, 0,                  0, 10702, ETFAsyNone,          ReqNotifyAccessoryModeChanged,             KErrNone, 2000000},
    //->Power Supply Off
    { ETFAccessoryNotifyAccessoryDisconnected,            0, 0,         0, 0,                  0, 10702, ETFAsyNone,          ReqAccRequestNone,                         KErrNone, 2000000},    
    { ETFAccessoryNotifyAccessoryConnectionStatusChanged, 0, 0,         0, 0,                  0, 10702, ETFAsyNone,          ReqAccRequestNone,                         KErrNone, 2000000},
    { ETFNotifyBoolAccessoryValueChanged,                 0, 1,         0, KAccExtPowerSupply, 0, 10702, ETFAsyNone,          ReqAccRequestNone,                         KErrNone, 2000000},    
#ifdef __WINS__
    { ETFAccessoryNotifyAccessoryModeChanged,             0, 0,         0, 0,                  0, 10702, ETFAsyNone,          ReqAccRequestNone,                         KErrNone, 2000000},
    { ETFAccessoryPublishAndSubscribe,                    0, 0,         0, KAccExtPowerSupply, 0, 10702, ETFAsyBoolNotify,    ReqAccRequestNone,                         KErrNone, 2000000}, //False   
#else
    { ETFAccessoryPublishAndSubscribe,                    0, 0,         0, KAccExtPowerSupply, 0, 10702, ETFAsyBoolNotify,    ReqAccRequestNone,                         KErrNone, 2000000}, //False   
    { ETFAccessoryNotifyAccessoryModeChanged,             0, 0,         0, 0,                  0, 10702, ETFAsyNone,          ReqAccRequestNone,                         KErrNone, 2000000},    
#endif    
    { ETFFindAndCheckRequest,                             0, 0,         0, 0,                  0, 10702, ETFAsyNone,          ReqNotifyAccessoryDisconnected,            KErrNone, 2000000},
    { ETFFindAndCheckRequest,                             0, 0,         0, KAccExtPowerSupply, 0, 10702, ETFAsyNone,          ReqNotifyBooleanAccessoryValueChanged,     KErrNone, 2000000},
    { ETFFindAndCheckRequest,                             0, 0,         0, 0,                  0, 10702, ETFAsyNone,          ReqNotifyAccessoryConnectionStatusChanged, KErrNone, 2000000},
    { ETFFindAndCheckRequest,                             0, 0,         0, 0,                  0, 10702, ETFAsyNone,          ReqNotifyAccessoryModeChanged,             KErrNone, 2000000},
    //->Power Supply On
    { ETFNotifyBoolAccessoryValueChanged,                 0, 1,         0, KAccExtPowerSupply, 0, 10702, ETFAsyNone,          ReqAccRequestNone,                         KErrNone, 2000000},    
    { ETFAccessoryNotifyAccessoryConnectionStatusChanged, 0, 0,         0, 0,                  0, 10702, ETFAsyNone,          ReqAccRequestNone,                         KErrNone, 2000000},
#ifdef __WINS__    
    { ETFAccessoryNotifyAccessoryModeChanged,             0, 0,         0, 0,                  0, 10702, ETFAsyNone,          ReqAccRequestNone,                         KErrNone, 2000000},
    { ETFAccessoryPublishAndSubscribe,                    0, 1,         0, KAccExtPowerSupply, 0, 10702, ETFAsyBoolNotify,    ReqAccRequestNone,                         KErrNone, 2000000}, //False
#else
    { ETFAccessoryPublishAndSubscribe,                    0, 1,         0, KAccExtPowerSupply, 0, 10702, ETFAsyBoolNotify,    ReqAccRequestNone,                         KErrNone, 2000000}, //False
    { ETFAccessoryNotifyAccessoryModeChanged,             0, 0,         0, 0,                  0, 10702, ETFAsyNone,          ReqAccRequestNone,                         KErrNone, 2000000},    
#endif    
    { ETFFindAndCheckRequest,                             0, 1,         0, KAccExtPowerSupply, 0, 10702, ETFAsyNone,          ReqNotifyBooleanAccessoryValueChanged,     KErrNone, 2000000},
    { ETFFindAndCheckRequest,                             0, 0,         0, 0,                  0, 10702, ETFAsyNone,          ReqNotifyAccessoryConnectionStatusChanged, KErrNone, 2000000},
    { ETFFindAndCheckRequest,                             0, 7,         0, 0,                  0, 10702, ETFAsyNone,          ReqNotifyAccessoryModeChanged,             KErrNone, 2000000},
    //Disconnect
    { ETFAccessoryNotifyAccessoryDisconnected,            0, 0,         0, 0,                  0, 10702, ETFAsyNone,          ReqAccRequestNone,                         KErrNone, 2000000},
    { ETFDisconnectWiredAccessory,                        0, 0,         0, 0,                  0, 10702, ETFAsyDisc,          ReqAccRequestNone,                         KErrNone, 2000000},
    { ETFFindAndCheckRequest,                             0, 0,         0, 0,                  0, 10702, ETFAsyNone,          ReqNotifyAccessoryDisconnected,            KErrNone, 2000000},
    { ETFClearStack,                                      0, 0,         0, 0,                  0, 10702, ETFAsyNone,          ReqAccRequestNone,                         KErrNone, 0},
    { ETFAccessorySingleConnection_Close,                 0, 0,         0, 0,                  0, 10702, ETFAsyNone,          ReqAccRequestNone,                         KErrNone, 0},
    { ETFAccessoryMode_Close,                             0, 0,         0, 0,                  0, 10702, ETFAsyNone,          ReqAccRequestNone,                         KErrNone, 0},
    { ETFAccessoryConnection_Close,                       0, 0,         0, 0,                  0, 10702, ETFAsyNone,          ReqAccRequestNone,                         KErrNone, 0},
    };

//
// KAccAudioOutConnector is "On" in connection state and "Off" in disconnection state.
//
// [M10703] [[AccServer] HS-20 Audio Out Connector On/Off] //KAccAudioOutConnector
//
const TTFAccessoryTestCaseStateControl KTFHS20AudioOutConnectorOnOffState[23]=
    {
    { ETFAccessoryPublishAndSubscribe,                    0, 1,         0, KAccAudioOutConnector, 0, 10703, ETFAsySetCapability, ReqAccRequestNone,                 KErrNone, 2000000},
    { ETFAccessoryConnection_Open,                        0, 0,         0, 0,             0, 10703, ETFAsyNone,          ReqAccRequestNone,                         KErrNone, 0},
    { ETFAccessoryMode_Open,                              0, 0,         0, 0,             0, 10703, ETFAsyNone,          ReqAccRequestNone,                         KErrNone, 0},
    { ETFAccessorySingleConnection_Open,                  0, 0,         0, 0,             0, 10703, ETFAsyNone,          ReqAccRequestNone,                         KErrNone, 0},
    { ETFAccessoryNotifyNewAccessoryConnected,            0, 0,         0, 0,             0, 10703, ETFAsyNone,          ReqAccRequestNone,                         KErrNone, 1000000},
    { ETFAccessoryNotifyAccessoryModeChanged,             0, 0,         0, 0,             0, 10703, ETFAsyNone,          ReqAccRequestNone,                         KErrNone, 2000000},
    { ETFConnectWiredAccessory,                           0, 0x010602,  0, 0,             0, 10703, ETFAsyConn,          ReqAccRequestNone,                         KErrNone, 2000000},
    { ETFFindAndCheckRequest,                             0, 0,         0, 0,             0, 10703, ETFAsyNone,          ReqNotifyNewAccessoryConnected,            KErrNone, 2000000},
    { ETFFindAndCheckRequest,                             0, 1,         0, 0,             0, 10703, ETFAsyNone,               ReqNotifyAccessoryModeChanged,        KErrNone, 2000000},
    { ETFNotifyBoolAccessoryValueChanged,                 0, 0,         0, KAccAudioOutConnector, 0, 10703, ETFAsyNone,       ReqAccRequestNone,                    KErrNone, 2000000},
    { ETFAccessoryNotifyAccessoryConnectionStatusChanged, 0, 0,         0, 0,             0, 10703, ETFAsyNone,               ReqAccRequestNone,                    KErrNone, 2000000},
#ifdef __WINS__    
    { ETFAccessoryNotifyAccessoryModeChanged,             0, 0,         0, 0,             0, 10703, ETFAsyNone,               ReqAccRequestNone,                    KErrNone, 2000000},    
    { ETFAccessoryPublishAndSubscribe,                    0, 0,         0, KAccAudioOutConnector, 0, 10703, ETFAsyBoolNotify, ReqAccRequestNone,                    KErrNone, 2000000},
#else
    { ETFAccessoryPublishAndSubscribe,                    0, 0,         0, KAccAudioOutConnector, 0, 10703, ETFAsyBoolNotify, ReqAccRequestNone,                    KErrNone, 2000000},
    { ETFAccessoryNotifyAccessoryModeChanged,             0, 0,         0, 0,             0, 10703, ETFAsyNone,               ReqAccRequestNone,                    KErrNone, 2000000},        
#endif    
    { ETFFindAndCheckRequest,                             0, 0,         0, KAccAudioOutConnector, 0, 10703, ETFAsyNone,  ReqNotifyBooleanAccessoryValueChanged,     KErrNone, 2000000},
    { ETFFindAndCheckRequest,                             0, 0,         0, 0,             0, 10703, ETFAsyNone,          ReqNotifyAccessoryModeChanged,             KErrNone, 2000000},
    { ETFFindAndCheckRequest,                             0, 0,         0, 0,             0, 10703, ETFAsyNone,          ReqNotifyAccessoryConnectionStatusChanged, KErrNone, 2000000},
    { ETFAccessoryNotifyAccessoryDisconnected,            0, 0,         0, 0,             0, 10703, ETFAsyNone,          ReqAccRequestNone,                         KErrNone, 2000000},
    { ETFDisconnectWiredAccessory,                        0, 0,         0, 0,             0, 10703, ETFAsyDisc,          ReqAccRequestNone,                         KErrNone, 2000000},
    { ETFFindAndCheckRequest,                             0, 0,         0, 0,             0, 10703, ETFAsyNone,          ReqNotifyAccessoryDisconnected,            KErrNone, 2000000},
    { ETFClearStack,                                      0, 0,         0, 0,             0, 10703, ETFAsyNone,          ReqAccRequestNone,                         KErrNone, 0},
    { ETFAccessorySingleConnection_Close,                 0, 0,         0, 0,             0, 10703, ETFAsyNone,          ReqAccRequestNone,                         KErrNone, 0},
    { ETFAccessoryMode_Close,                             0, 0,         0, 0,             0, 10703, ETFAsyNone,          ReqAccRequestNone,                         KErrNone, 0},
    { ETFAccessoryConnection_Close,                       0, 0,         0, 0,             0, 10703, ETFAsyNone,          ReqAccRequestNone,                         KErrNone, 0},
    };

//
// [M10704] [[AccServer] Connect Unsupported Accessory]
//
const TTFAccessoryTestCaseStateControl KTFUnsupportedWiredAccessoryStates[4]=
    {
    { ETFAccessoryControl_Open,    0,0,        0, 0, 0, 10704, ETFAsyNone, ReqAccRequestNone,         KErrNone, 0},
    { ETFConnectWiredAccessory,    0,0x000000, 0, 0, 0, 10704, ETFAsyConn, ReqAccRequestNone,         KErrNone, 2000000},
    { ETFDisconnectWiredAccessory, 0,0,        0, 0, 0, 10704, ETFAsyDisc, ReqAccRequestNone,         KErrNone, 2000000},  
    { ETFAccessoryControl_Close,   0,0,        0, 0, 0, 10704, ETFAsyNone, ReqAccRequestNone,         KErrNone, 2000000}
    };

//
// [M10705] [[AccServer] Transaction matching for ASY command]
//
const TTFAccessoryTestCaseStateControl KTFTransactionMatchStates[]=
    {
    { ETFAccessoryPublishAndSubscribe,         0, 1,        0, KAccAudioInConnector, 0, 10705, ETFAsySetCapability, ReqAccRequestNone, KErrNone, 2000000},
    { ETFAccessorySingleConnection_Open,       0, 0,        0, 0,            0, 10705, ETFAsyNone,          ReqAccRequestNone, KErrNone, 0},
    { ETFAccessoryControl_Open,                0, 0,        0, 0,            0, 10705, ETFAsyNone,          ReqAccRequestNone, KErrNone, 0},
    { ETFAccessoryNotifyNewAccessoryConnected, 0, 0,        0, 0,            0, 10705, ETFAsyNone,          ReqAccRequestNone, KErrNone, 1000000},
    { ETFConnectWiredAccessory,                0, 0x010201, 0, 0,            0, 10705, ETFAsyConn,          ReqAccRequestNone, KErrNone, 3000000},
    { ETFSetBoolValue,                         0, 1,        0, KAccAudioInConnector, 0, 10705, ETFAsyNone,  ReqAccRequestNone, KErrNone, 3000000},  //Boolean
    { ETFDisconnectWiredAccessory,             0, 0,        0, 0,            0, 10705, ETFAsyDisc,          ReqAccRequestNone, KErrNone, 2000000},
    { ETFClearStack,                           0, 0,        0, 0,            0, 10705, ETFAsyNone,          ReqAccRequestNone, KErrNone, 2000000},
    { ETFAccessoryControl_Close,               0, 0,        0, 0,            0, 10705, ETFAsyNone,          ReqAccRequestNone, KErrNone, 2000000},
    { ETFAccessorySingleConnection_Close,      0, 0,        0, 0,            0, 10705, ETFAsyNone,          ReqAccRequestNone, KErrNone, 2000000},
    };

//
// [M10706] [[AccServer] Fast Mode Notification Change]
//
// NotifyAccessoryModeChanged() is not reordered fast enough 
// (e.g. due to different process sceduling by kernel)
//
const TTFAccessoryTestCaseStateControl KTFFastModeNotificationChange[]=
    {    
    { ETFAccessoryMode_Open,                   0, 0,                    0, 0, 0, 10706, ETFAsyNone, ReqAccRequestNone,              KErrNone, 0},
    { ETFAccessorySingleConnection_Open,       0, 0,                    0, 0, 0, 10706, ETFAsyNone, ReqAccRequestNone,              KErrNone, 0},
    { ETFAccessoryNotifyAccessoryModeChanged,  0, 0,                    0, 0, 0, 10706, ETFAsyNone, ReqAccRequestNone,              KErrNone, 2000000},
    { ETFAccessoryNotifyNewAccessoryConnected, 0, 0,                    0, 0, 0, 10706, ETFAsyNone, ReqAccRequestNone,              KErrNone, 1000000},
    { ETFConnectWiredAccessory,                0, 0x010103,             0, 0, 0, 10706, ETFAsyConn, ReqAccRequestNone,              KErrNone, 2000000},
    { ETFFindAndCheckRequest,                  0, 0,                    0, 0, 0, 10706, ETFAsyNone, ReqNotifyNewAccessoryConnected, KErrNone, 2000000},
    { ETFFindAndCheckRequest,                  0, EAccModeWiredHeadset, 0, 0, 0, 10706, ETFAsyNone, ReqNotifyAccessoryModeChanged,  KErrNone, 2000000},
    //Ordering NotifyAccessoryModeChanged() after calling AudioRoutingStatusNotify() fakes the use-case under test
    { ETFAudioRoutingStatusNotify,             0, ETrue,                0, 0, 0, 10706, ETFAsyNone, ReqAccRequestNone,              KErrNone, 2000000},
    { ETFAccessoryNotifyAccessoryModeChanged,  0, 0,                    0, 0, 0, 10706, ETFAsyNone, ReqAccRequestNone,              KErrNone, 2000000},
    { ETFFindAndCheckRequest,                  0, EAccModeWiredHeadset, 0, 0, 0, 10706, ETFAsyNone, ReqNotifyAccessoryModeChanged,  KErrNone, 2000000},
    { ETFDisconnectWiredAccessory,             0, 0,                    0, 0, 0, 10706, ETFAsyDisc, ReqAccRequestNone,              KErrNone, 2000000},
    { ETFClearStack,                           0, 0,                    0, 0, 0, 10706, ETFAsyNone, ReqAccRequestNone,              KErrNone, 0},
    { ETFAccessorySingleConnection_Close,      0, 0,                    0, 0, 0, 10706, ETFAsyNone, ReqAccRequestNone,              KErrNone, 0},
    { ETFAccessoryMode_Close,                  0, 0,                    0, 0, 0, 10706, ETFAsyNone, ReqAccRequestNone,              KErrNone, 0},
    };
    
//
// [M10707] [[AccServer] Not supported note]
//
const TTFAccessoryTestCaseStateControl KTFNotSupportedNote[]=
    {    
    { ETFNotSupportedNote,             0, 0,                       0, 0, 0, 10707, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 20000000},
    };
        
    
    

/*****************************                    *********************************/
/***************************** Accessory Settings *********************************/
/*****************************                    *********************************/


//
// [M10801] [[AccServer] Set Loopset Mode]
//
const TTFAccessoryTestCaseStateControl KTFSetLoopsetModeStates[3]=
    {
    { ETFAccessorySettings_Open,  0, 0,         0, 0, 0, 10801, ETFAsyNone, ReqAccRequestNone, KErrNone, 0},
    { ETFSetHWDeviceSettings,     0, KASLoopset,1, 0, 0, 10801, ETFAsyNone, ReqAccRequestNone, KErrNone, 100000},
    { ETFAccessorySettings_Close, 0, 0,         0, 0, 0, 10801, ETFAsyNone, ReqAccRequestNone, KErrNone, 0},
    };

//
// [M10802] [[AccServer] Get Loopset Mode]
//
const TTFAccessoryTestCaseStateControl KTFGetLoopsetModeStates[4]=
    {
    { ETFAccessorySettings_Open,  0, 0,          0, 0, 0, 10802, ETFAsyNone, ReqAccRequestNone, KErrNone, 0},
    { ETFGetHWDeviceSettings,     0, 2,          0, 0, 0, 10802, ETFAsyNone, ReqAccRequestNone, KErrNone, 100000},
    { ETFSetHWDeviceSettings,     0, KASLoopset, 0, 0, 0, 10801, ETFAsyNone, ReqAccRequestNone, KErrNone, 100000},
    { ETFAccessorySettings_Close, 0, 0,          0, 0, 0, 10802, ETFAsyNone, ReqAccRequestNone, KErrNone, 0},
    };

//
// [M10803] [[AccServer] Set And Get Loopset Mode]
//
const TTFAccessoryTestCaseStateControl KTFSetAndGetLoopsetModeStates[6]=
    {
    { ETFAccessorySettings_Open,  0, 0,          0, 0, 0, 10803, ETFAsyNone, ReqAccRequestNone, KErrNone, 0},
    { ETFSetHWDeviceSettings,     0, KASLoopset, 1, 0, 0, 10803, ETFAsyNone, ReqAccRequestNone, KErrNone, 100000},
    { ETFGetHWDeviceSettings,     0, 2,          0, 0, 0, 10803, ETFAsyNone, ReqAccRequestNone, KErrNone, 100000},
    { ETFSetHWDeviceSettings,     0, KASLoopset, 0, 0, 0, 10803, ETFAsyNone, ReqAccRequestNone, KErrNone, 100000},
    { ETFGetHWDeviceSettings,     0, 0,          0, 0, 0, 10803, ETFAsyNone, ReqAccRequestNone, KErrNone, 100000},
    { ETFAccessorySettings_Close, 0, 0,          0, 0, 0, 10803, ETFAsyNone, ReqAccRequestNone, KErrNone, 0},
    };

//
// [M10804] [[AccServer] Set TTY Mode]
//
const TTFAccessoryTestCaseStateControl KTFSetTTYModeStates[3]=
    {
    { ETFAccessorySettings_Open,  0,0,      0, 0, 0, 10804, ETFAsyNone, ReqAccRequestNone, KErrNone, 0},
    { ETFSetHWDeviceSettings,     0,KASTTY, 1, 0, 0, 10804, ETFAsyNone, ReqAccRequestNone, KErrNone, 100000},
    { ETFAccessorySettings_Close, 0,0,      0, 0, 0, 10804, ETFAsyNone, ReqAccRequestNone, KErrNone, 0},
    };

//
// [M10805] [[AccServer] Get TTY Mode]
//
const TTFAccessoryTestCaseStateControl KTFGetTTYModeStates[3]=
    {
    { ETFAccessorySettings_Open,  0,0, 0, 0, 0, 10805, ETFAsyNone, ReqAccRequestNone, KErrNone, 0},
    { ETFGetHWDeviceSettings,     0,1, 0, 0, 0, 10805, ETFAsyNone, ReqAccRequestNone, KErrNone, 100000},
    { ETFAccessorySettings_Close, 0,0, 0, 0, 0, 10805, ETFAsyNone, ReqAccRequestNone, KErrNone, 0},
    };

//
// [M10806] [[AccServer] Set And Get TTY Mode]
//
const TTFAccessoryTestCaseStateControl KTFSetAndGetTTYModeStates[6]=
    {
    { ETFAccessorySettings_Open,  0, 0,      0, 0, 0, 10806, ETFAsyNone, ReqAccRequestNone, KErrNone, 0},
    { ETFSetHWDeviceSettings,     0, KASTTY, 1, 0, 0, 10806, ETFAsyNone, ReqAccRequestNone, KErrNone, 100000},
    { ETFGetHWDeviceSettings,     0, 1,      0, 0, 0, 10806, ETFAsyNone, ReqAccRequestNone, KErrNone, 100000},
    { ETFSetHWDeviceSettings,     0, KASTTY, 0, 0, 0, 10806, ETFAsyNone, ReqAccRequestNone, KErrNone, 100000},
    { ETFGetHWDeviceSettings,     0, 0,      0, 0, 0, 10806, ETFAsyNone, ReqAccRequestNone, KErrNone, 100000},
    { ETFAccessorySettings_Close, 0, 0,      0, 0, 0, 10806, ETFAsyNone, ReqAccRequestNone, KErrNone, 0},
    };

//
// [M10807] [[AccServer] Set Loopset Mode After Connection]
//
const TTFAccessoryTestCaseStateControl KTFSetLoopsetModeAfterConnectionStates[17]=
    {
    { ETFAccessoryConnection_Open,                        0, 0,              0, 0, 0, 10807, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 0},
    { ETFAccessoryMode_Open,                              0, 0,              0, 0, 0, 10807, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 0},
    { ETFAccessorySettings_Open,                          0, 0,              0, 0, 0, 10807, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 0},
    { ETFAccessoryNotifyAccessoryModeChanged,             0, 2,              0, 0, 0, 10807, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 2000000},
    { ETFConnectWiredAccessory,                           0, 0x010001,       0, 0, 0, 10807, ETFAsyConn, ReqAccRequestNone,                         KErrNone, 2000000},
    { ETFFindAndCheckRequest,                             0, 1,              0, 0, 0, 10807, ETFAsyNone, ReqNotifyAccessoryModeChanged,             KErrNone, 2000000},
    { ETFAccessoryNotifyAccessoryConnectionStatusChanged, 0, 0,              0, 0, 0, 10807, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 2000000},
    { ETFAccessoryNotifyAccessoryModeChanged,             0, 0,              0, 0, 0, 10807, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 2000000},
    { ETFSetHWDeviceSettings,                             1, KASLoopset,     1, 0, 0, 10807, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 100000},
    { ETFFindAndCheckRequest,                             0, 6,              0, 0, 0, 10807, ETFAsyNone, ReqNotifyAccessoryModeChanged,             KErrNone, 2000000},
    { ETFFindAndCheckRequest,                             0, 0,              0, 0, 0, 10807, ETFAsyNone, ReqNotifyAccessoryConnectionStatusChanged, KErrNone, 2000000},
    { ETFSetHWDeviceSettings,                             0, KASLoopset,     0, 0, 0, 10807, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 0},
    { ETFDisconnectWiredAccessory,                        0, 0,              0, 0, 0, 10807, ETFAsyDisc, ReqAccRequestNone,                         KErrNone, 2000000},
    { ETFClearStack,                                      0, 0,              0, 0, 0, 10807, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 0},
    { ETFAccessoryMode_Close,                             0, 0,              0, 0, 0, 10807, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 0},
    { ETFAccessorySettings_Close,                         0, 0,              0, 0, 0, 10807, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 0},
    { ETFAccessoryConnection_Close,                       0, 0,              0, 0, 0, 10807, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 0},
    };

//
// [M10808] [[AccServer] Set TTY Mode After Connection]
//
const TTFAccessoryTestCaseStateControl KTFSetTTYModeAfterConnectionStates[17]=
    {
    { ETFAccessoryConnection_Open,                        0, 0,              0, 0, 0, 10808, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 0},
    { ETFAccessoryMode_Open,                              0, 0,              0, 0, 0, 10808, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 0},
    { ETFAccessorySettings_Open,                          0, 0,              0, 0, 0, 10808, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 0},
    { ETFAccessoryNotifyAccessoryModeChanged,             0, 2,              0, 0, 0, 10808, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 2000000},
    { ETFConnectWiredAccessory,                           0, 0x010001,       0, 0, 0, 10808, ETFAsyConn, ReqAccRequestNone,                         KErrNone, 2000000},
    { ETFFindAndCheckRequest,                             0, 1,              0, 0, 0, 10808, ETFAsyNone, ReqNotifyAccessoryModeChanged,             KErrNone, 2000000},
    { ETFAccessoryNotifyAccessoryConnectionStatusChanged, 0, 0,              0, 0, 0, 10808, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 2000000},
    { ETFAccessoryNotifyAccessoryModeChanged,             0, 0,              0, 0, 0, 10808, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 2000000},
    { ETFSetHWDeviceSettings,                             1, KASTTY,         1, 0, 0, 10808, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 100000},
    { ETFFindAndCheckRequest,                             0, 5,              0, 0, 0, 10808, ETFAsyNone, ReqNotifyAccessoryModeChanged,             KErrNone, 2000000},
    { ETFFindAndCheckRequest,                             0, 0,              0, 0, 0, 10808, ETFAsyNone, ReqNotifyAccessoryConnectionStatusChanged, KErrNone, 2000000},
    { ETFSetHWDeviceSettings,                             0, KASTTY,         0, 0, 0, 10808, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 0},
    { ETFDisconnectWiredAccessory,                        0, 0,              0, 0, 0, 10808, ETFAsyDisc, ReqAccRequestNone,                         KErrNone, 2000000},
    { ETFClearStack,                                      0, 0,              0, 0, 0, 10808, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 0},
    { ETFAccessoryMode_Close,                             0, 0,              0, 0, 0, 10808, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 0},
    { ETFAccessorySettings_Close,                         0, 0,              0, 0, 0, 10808, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 0},
    { ETFAccessoryConnection_Close,                       0, 0,              0, 0, 0, 10808, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 0},
    };

//
// [M10809] [[AccServer] Set Loopset Mode Before Connection]
//
const TTFAccessoryTestCaseStateControl KTFSetLoopsetModeBeforeConnectionStates[15]=
    {
    { ETFAccessoryConnection_Open,                        0, 0,              0, 0, 0, 10809, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 0},
    { ETFAccessoryMode_Open,                              0, 0,              0, 0, 0, 10809, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 0},
    { ETFAccessorySettings_Open,                          0, 0,              0, 0, 0, 10809, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 0},
    { ETFSetHWDeviceSettings,                             0, KASLoopset,     1, 0, 0, 10809, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 0},
    { ETFAccessoryNotifyAccessoryModeChanged,             0, 2,              0, 0, 0, 10809, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 2000000},
    { ETFAccessoryNotifyAccessoryConnectionStatusChanged, 0, 0,              0, 0, 0, 10809, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 2000000},
    { ETFConnectWiredAccessory,                           0, 0x010001,       0, 0, 0, 10809, ETFAsyConn, ReqAccRequestNone,                         KErrNone, 2000000},
    { ETFFindAndCheckRequest,                             0, 6,              0, 0, 0, 10809, ETFAsyNone, ReqNotifyAccessoryModeChanged,             KErrNone, 2000000},
    { ETFFindAndCheckRequest,                             0, 0,              0, 0, 0, 10809, ETFAsyNone, ReqNotifyAccessoryConnectionStatusChanged, KErrNone, 2000000},
    { ETFDisconnectWiredAccessory,                        0, 0,              0, 0, 0, 10809, ETFAsyDisc, ReqAccRequestNone,                         KErrNone, 2000000},
    { ETFSetHWDeviceSettings,                             0, KASLoopset,     0, 0, 0, 10809, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 100000},
    { ETFClearStack,                                      0, 0,              0, 0, 0, 10809, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 0},
    { ETFAccessoryMode_Close,                             0, 0,              0, 0, 0, 10809, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 0},
    { ETFAccessorySettings_Close,                         0, 0,              0, 0, 0, 10809, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 0},
    { ETFAccessoryConnection_Close,                       0, 0,              0, 0, 0, 10809, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 0},
    };

//
// [M10810] [[AccServer] Set TTY Mode Before Connection]
//
const TTFAccessoryTestCaseStateControl KTFSetTTYModeBeforeConnectionStates[15]=
    {
    { ETFAccessoryConnection_Open,                        0, 0,              0, 0, 0, 10810, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 0},
    { ETFAccessoryMode_Open,                              0, 0,              0, 0, 0, 10810, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 0},
    { ETFAccessorySettings_Open,                          0, 0,              0, 0, 0, 10810, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 0},
    { ETFSetHWDeviceSettings,                             0, KASTTY,         1, 0, 0, 10810, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 100000},
    { ETFAccessoryNotifyAccessoryModeChanged,             0, 2,              0, 0, 0, 10810, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 2000000},
    { ETFAccessoryNotifyAccessoryConnectionStatusChanged, 0, 0,              0, 0, 0, 10810, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 2000000},
    { ETFConnectWiredAccessory,                           0, 0x010001,       0, 0, 0, 10810, ETFAsyConn, ReqAccRequestNone,                         KErrNone, 2000000},
    { ETFFindAndCheckRequest,                             0, 5,              0, 0, 0, 10810, ETFAsyNone, ReqNotifyAccessoryModeChanged,             KErrNone, 2000000},
    { ETFFindAndCheckRequest,                             0, 0,              0, 0, 0, 10810, ETFAsyNone, ReqNotifyAccessoryConnectionStatusChanged, KErrNone, 2000000},
    { ETFDisconnectWiredAccessory,                        0, 0,              0, 0, 0, 10810, ETFAsyDisc, ReqAccRequestNone,                         KErrNone, 2000000},
    { ETFSetHWDeviceSettings,                             0, KASTTY,         0, 0, 0, 10810, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 100000},
    { ETFClearStack,                                      0, 0,              0, 0, 0, 10810, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 0},
    { ETFAccessoryMode_Close,                             0, 0,              0, 0, 0, 10810, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 0},
    { ETFAccessorySettings_Close,                         0, 0,              0, 0, 0, 10810, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 0},
    { ETFAccessoryConnection_Close,                       0, 0,              0, 0, 0, 10810, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 0},
    };

//
// [M10811] [[AccServer] Get Supported HW Devices]
//
const TTFAccessoryTestCaseStateControl KTFGetSupportedHWDevicesStates[3]=
    {
    { ETFAccessorySettings_Open,       0, 0,      0,          0, 0, 10811, ETFAsyNone, ReqAccRequestNone, KErrNone, 0},
    { ETFGetSupportedHWDeviceSettings, 0, KASTTY, KASLoopset, 0, 0, 10811, ETFAsyNone, ReqAccRequestNone, KErrNone, 100000},
    { ETFAccessorySettings_Close,      0, 0,      0,          0, 0, 10811, ETFAsyNone, ReqAccRequestNone, KErrNone, 0},
    };

//
// [M10812] [[AccServer] Accessory Mode Settings (TInt)]
//
const TTFAccessoryTestCaseStateControl KTFAccessoryModeSettingsTIntStates[5]=
    {
    { ETFAccessorySettings_Open,       0, 0,                    0,                       0, 0, 10812, ETFAsyNone, ReqAccRequestNone, KErrNone, 0},
    { ETFSetIntAccessoryModeSetting,   0, EAccModeWiredHeadset, KAccServerWiredHSLights, 0, 1, 10812, ETFAsyNone, ReqAccRequestNone, KErrNone, 100000},
    { ETFGetIntAccessoryModeSetting,   0, EAccModeWiredHeadset, KAccServerWiredHSLights, 0, 1, 10812, ETFAsyNone, ReqAccRequestNone, KErrNone, 100000},
    { ETFSetIntAccessoryModeSetting,   0, EAccModeWiredHeadset, KAccServerWiredHSLights, 0, 0, 10812, ETFAsyNone, ReqAccRequestNone, KErrNone, 100000},
    { ETFAccessorySettings_Close,      0, 0,                    0,                       0, 0, 10812, ETFAsyNone, ReqAccRequestNone, KErrNone, 0},
    };

//
// [M10813] [[AccServer] Accessory Mode Settings (TBool)]
//
const TTFAccessoryTestCaseStateControl KTFAccessoryModeSettingsTBoolStates[5]=
    {
    { ETFAccessorySettings_Open,       0, 0,                     0, 0, 0, 10813, ETFAsyNone, ReqAccRequestNone, KErrNone, 0},
    { ETFSetBoolAccessoryModeSetting,  0, EAccModeWiredHeadset, 11, 0, 1, 10813, ETFAsyNone, ReqAccRequestNone, KErrNone, 100000},
    { ETFGetBoolAccessoryModeSetting,  0, EAccModeWiredHeadset, 11, 0, 1, 10813, ETFAsyNone, ReqAccRequestNone, KErrNone, 100000},
    { ETFSetBoolAccessoryModeSetting,  0, EAccModeWiredHeadset, 11, 0, 0, 10813, ETFAsyNone, ReqAccRequestNone, KErrNone, 100000},
    { ETFAccessorySettings_Close,      0, 0,                     0, 0, 0, 10813, ETFAsyNone, ReqAccRequestNone, KErrNone, 0},
    };


//
// [M10814] [[AccServer] Accessory Mode Settings (TDes)]
//
const TTFAccessoryTestCaseStateControl KTFAccessoryModeSettingsTDesStates[4]=
    {
    { ETFAccessorySettings_Open,     0, 0,                    0,  0,         0, 10814, ETFAsyNone, ReqAccRequestNone, KErrNone, 0},
    { ETFSetDesAccessoryModeSetting, 0, EAccModeWiredHeadset, 12, 123456789, 0, 10814, ETFAsyNone, ReqAccRequestNone, KErrNone, 100000},
    { ETFGetDesAccessoryModeSetting, 0, EAccModeWiredHeadset, 12, 123456789, 0, 10814, ETFAsyNone, ReqAccRequestNone, KErrNone, 100000},
    { ETFAccessorySettings_Close,    0, 0,                    0,  0,         0, 10814, ETFAsyNone, ReqAccRequestNone, KErrNone, 0},
    };

#ifdef __WINS__
//
// [M10815] [[AccServer] Accessory Mode Settings (Array)]
//
const TTFAccessoryTestCaseStateControl KTFAccessoryModeSettingsArrayStates[4]=
    {
    { ETFAccessorySettings_Open,         0, 0,    0, 0, 0, 10815, ETFAsyNone, ReqAccRequestNone, KErrNone, 0},
    { ETFSetSeveralAccessoryModeSetting, 0, 0,    0, 0, 0, 10815, ETFAsyNone, ReqAccRequestNone, KErrNone, 100000},
    { ETFGetSeveralAccessoryModeSetting, 0, 0,    0, 0, 0, 10815, ETFAsyNone, ReqAccRequestNone, KErrNone, 100000},
    { ETFAccessorySettings_Close,        0, 0,    0, 0, 0, 10815, ETFAsyNone, ReqAccRequestNone, KErrNone, 0},
    };
#endif

//
// [M10816] [[AccServer] Set And Get Wireless Accessory Mode Setting ]
//
const TTFAccessoryTestCaseStateControl KTFSetAndGetWirelessAccessoryModeSettingsStates[5]=
    {
    { ETFAccessorySettings_Open,       0, 0,                       0,                          0, 0, 10816, ETFAsyNone, ReqAccRequestNone, KErrNone, 0},
    { ETFSetIntAccessoryModeSetting,   0, EAccModeWirelessHeadset, KAccServerWirelessHSLights, 0, 1, 10816, ETFAsyNone, ReqAccRequestNone, KErrNone, 100000},
    { ETFGetIntAccessoryModeSetting,   0, EAccModeWirelessHeadset, KAccServerWirelessHSLights, 0, 1, 10816, ETFAsyNone, ReqAccRequestNone, KErrNone, 100000},
    { ETFSetIntAccessoryModeSetting,   0, EAccModeWirelessHeadset, KAccServerWirelessHSLights, 0, 0, 10816, ETFAsyNone, ReqAccRequestNone, KErrNone, 100000},
    { ETFAccessorySettings_Close,      0, 0,                       0,                          0, 0, 10816, ETFAsyNone, ReqAccRequestNone, KErrNone, 0},
    };

//
// [M10817] [[AccServer] Set And Get Wired Carkit Accessory Mode Setting ]
//
const TTFAccessoryTestCaseStateControl KTFSetAndGetWiredCarkitAccessoryModeSettingsStates[5]=
    {
    { ETFAccessorySettings_Open,       0, 0,                       0,                          0, 0, 10817, ETFAsyNone, ReqAccRequestNone, KErrNone, 0},
    { ETFSetIntAccessoryModeSetting,   0, EAccModeWiredCarKit, KAccServerWiredCarKitLights,    0, 1, 10817, ETFAsyNone, ReqAccRequestNone, KErrNone, 100000},
    { ETFGetIntAccessoryModeSetting,   0, EAccModeWiredCarKit, KAccServerWiredCarKitLights,    0, 1, 10817, ETFAsyNone, ReqAccRequestNone, KErrNone, 100000},
    { ETFSetIntAccessoryModeSetting,   0, EAccModeWiredCarKit, KAccServerWiredCarKitLights,    0, 0, 10817, ETFAsyNone, ReqAccRequestNone, KErrNone, 100000},
    { ETFAccessorySettings_Close,      0, 0,                       0,                          0, 0, 10817, ETFAsyNone, ReqAccRequestNone, KErrNone, 0},
    };

//
// [M10818] [[AccServer] Set And Get Wireless Carkit Accessory Mode Setting ]
//
const TTFAccessoryTestCaseStateControl KTFSetAndGetWirelessCarkitAccessoryModeSettingsStates[5]=
    {
    { ETFAccessorySettings_Open,       0, 0,                      0,                              0, 0, 10818, ETFAsyNone, ReqAccRequestNone, KErrNone, 0},
    { ETFSetIntAccessoryModeSetting,   0, EAccModeWirelessCarKit, KAccServerWirelessCarKitLights, 0, 1, 10818, ETFAsyNone, ReqAccRequestNone, KErrNone, 100000},
    { ETFGetIntAccessoryModeSetting,   0, EAccModeWirelessCarKit, KAccServerWirelessCarKitLights, 0, 1, 10818, ETFAsyNone, ReqAccRequestNone, KErrNone, 100000},
    { ETFSetIntAccessoryModeSetting,   0, EAccModeWirelessCarKit, KAccServerWirelessCarKitLights, 0, 0, 10818, ETFAsyNone, ReqAccRequestNone, KErrNone, 100000},
    { ETFAccessorySettings_Close,      0, 0,                      0,                              0, 0, 10818, ETFAsyNone, ReqAccRequestNone, KErrNone, 0},
    };

//
// [M10819] [[AccServer] Set And Get Text Device Accessory Mode Setting ]
//
const TTFAccessoryTestCaseStateControl KTFSetAndGetTextDeviceAccessoryModeSettingsStates[5]=
    {
    { ETFAccessorySettings_Open,       0, 0,                  0,                          0, 0, 10819, ETFAsyNone, ReqAccRequestNone, KErrNone, 0},
    { ETFSetIntAccessoryModeSetting,   0, EAccModeTextDevice, KAccServerTextDeviceLights, 0, 1, 10819, ETFAsyNone, ReqAccRequestNone, KErrNone, 100000},
    { ETFGetIntAccessoryModeSetting,   0, EAccModeTextDevice, KAccServerTextDeviceLights, 0, 1, 10819, ETFAsyNone, ReqAccRequestNone, KErrNone, 100000},
    { ETFSetIntAccessoryModeSetting,   0, EAccModeTextDevice, KAccServerTextDeviceLights, 0, 0, 10819, ETFAsyNone, ReqAccRequestNone, KErrNone, 100000},
    { ETFAccessorySettings_Close,      0, 0,                  0,                          0, 0, 10819, ETFAsyNone, ReqAccRequestNone, KErrNone, 0},
    };

//
// [M10820] [[AccServer] Set And Get Loopset Accessory Mode Setting ]
//
const TTFAccessoryTestCaseStateControl KTFSetAndGetLoopsetAccessoryModeSettingsStates[5]=
    {
    { ETFAccessorySettings_Open,       0, 0,               0,                       0, 0, 10820, ETFAsyNone, ReqAccRequestNone, KErrNone, 0},
    { ETFSetIntAccessoryModeSetting,   0, EAccModeLoopset, KAccServerLoopsetLights, 0, 1, 10820, ETFAsyNone, ReqAccRequestNone, KErrNone, 100000},
    { ETFGetIntAccessoryModeSetting,   0, EAccModeLoopset, KAccServerLoopsetLights, 0, 1, 10820, ETFAsyNone, ReqAccRequestNone, KErrNone, 100000},
    { ETFSetIntAccessoryModeSetting,   0, EAccModeLoopset, KAccServerLoopsetLights, 0, 0, 10820, ETFAsyNone, ReqAccRequestNone, KErrNone, 100000},
    { ETFAccessorySettings_Close,      0, 0,               0,                       0, 0, 10820, ETFAsyNone, ReqAccRequestNone, KErrNone, 0},
    };

//
// [M10821] [[AccServer] Set And Get Music Stand Accessory Mode Setting ]
//
const TTFAccessoryTestCaseStateControl KTFSetAndGetMusicStandAccessoryModeSettingsStates[5]=
    {
    { ETFAccessorySettings_Open,       0, 0,                  0,                          0, 0, 10821, ETFAsyNone, ReqAccRequestNone, KErrNone, 0},
    { ETFSetIntAccessoryModeSetting,   0, EAccModeMusicStand, KAccServerMusicStandLights, 0, 1, 10821, ETFAsyNone, ReqAccRequestNone, KErrNone, 100000},
    { ETFGetIntAccessoryModeSetting,   0, EAccModeMusicStand, KAccServerMusicStandLights, 0, 1, 10821, ETFAsyNone, ReqAccRequestNone, KErrNone, 100000},
    { ETFSetIntAccessoryModeSetting,   0, EAccModeMusicStand, KAccServerMusicStandLights, 0, 0, 10821, ETFAsyNone, ReqAccRequestNone, KErrNone, 100000},
    { ETFAccessorySettings_Close,      0, 0,                  0,                          0, 0, 10821, ETFAsyNone, ReqAccRequestNone, KErrNone, 0},
    };

//
// [M10822] [[AccServer] Accessory Mode Settings (EAccLightsAutomatic / EAccLightsOn)]
//
const TTFAccessoryTestCaseStateControl KTFAccessoryModeSettingsGsSettingsState[5]=
    {
    { ETFAccessorySettings_Open,       0, 0,                    0,                           0, 0,                   10822, ETFAsyNone, ReqAccRequestNone, KErrNone, 0},
    { ETFSetIntAccessoryModeSetting,   0, EAccModeWiredCarKit,  KAccServerWiredCarKitLights, 0, EAccLightsOn,        10822, ETFAsyNone, ReqAccRequestNone, KErrNone, 100000},
    { ETFGetIntAccessoryModeSetting,   0, EAccModeWiredCarKit,  KAccServerWiredCarKitLights, 0, EAccLightsOn,        10822, ETFAsyNone, ReqAccRequestNone, KErrNone, 100000},
    { ETFSetIntAccessoryModeSetting,   0, EAccModeWiredCarKit,  KAccServerWiredCarKitLights, 0, EAccLightsAutomatic, 10822, ETFAsyNone, ReqAccRequestNone, KErrNone, 100000},
    { ETFAccessorySettings_Close,      0, 0,                    0,                           0, 0,                   10822, ETFAsyNone, ReqAccRequestNone, KErrNone, 200000},
    };

/*****************************                       *********************************/
/*****************************  Multible Connections *********************************/
/*****************************                       *********************************/

//
// [M10901] [[AccServer] Connect Wired And Wireless]
//
const TTFAccessoryTestCaseStateControl KTFConnectWiredAndBTArrayStates[]=
    {
    { ETFAccessoryMode_Open,                              0, 0,                       0, 0, 0, 10901, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 0},
    { ETFAccessorySingleConnection_Open,                  0, 0,                       0, 0, 0, 10901, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 0},
    { ETFAccessoryBtControl_Open,                         0, 0,                       0, 0, 0, 10901, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 0},
    { ETFAccessoryConnection_Open,                        0, 0,                       0, 0, 0, 10901, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 0},
    { ETFAccessoryNotifyAccessoryModeChanged,             0, 0,                       0, 0, 0, 10901, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 2000000},
    { ETFAccessoryNotifyNewAccessoryConnected,            0, 0,                       0, 0, 0, 10901, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 1000000},
    { ETFAccessoryNotifyAccessoryConnectionStatusChanged, 0, 0,                       0, 0, 0, 10901, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 2000000},
    { ETFConnectWiredAccessory,                           0, 0x010103,                0, 0, 0, 10901, ETFAsyConn, ReqAccRequestNone,                         KErrNone, 2000000},
    { ETFFindAndCheckRequest,                             0, 0,                       0, 0, 0, 10901, ETFAsyNone, ReqNotifyNewAccessoryConnected,            KErrNone, 2000000},
    { ETFFindAndCheckRequest,                             0, 0,                       0, 0, 0, 10901, ETFAsyNone, ReqNotifyAccessoryConnectionStatusChanged, KErrNone, 2000000},
    { ETFFindAndCheckRequest,                             0, 1,                       0, 0, 0, 10901, ETFAsyNone, ReqNotifyAccessoryModeChanged,             KErrNone, 2000000},    
    { ETFAccessoryNotifyNewAccessoryConnected,            1, 0,                       0, 0, 0, 10901, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 1000000},
    { ETFAccessoryNotifyAccessoryConnectionStatusChanged, 0, 0,                       0, 0, 0, 10901, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 2000000},
    { ETFAccessoryBtConnectAccessory,                     1, KBTAddrHeadsetHSPAndHFP, 0, 0, 0, 10901, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 4000000},
    { ETFFindAndCheckRequest,                             1, 0,                       0, 0, 0, 10901, ETFAsyNone, ReqNotifyNewAccessoryConnected,            KErrNone, 2000000},
    { ETFFindAndCheckRequest,                             1, 0,                       0, 0, 0, 10901, ETFAsyNone, ReqConnectBtAccessory,                     KErrNone, 2000000},
    { ETFFindAndCheckRequest,                             0, 0,                       0, 0, 0, 10901, ETFAsyNone, ReqNotifyAccessoryConnectionStatusChanged, KErrNone, 2000000},
};

//
// [M10902] [[AccServer] Disconnect Wired And Wireleass]
//
const TTFAccessoryTestCaseStateControl KTFDisconnectWiredAndBTArrayStates[]=
{
    { ETFAccessoryNotifyAccessoryDisconnected,            0, 0,                       0, 0, 0, 10902, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 2000000},
    { ETFAccessoryNotifyAccessoryModeChanged,             0, 0,                       0, 0, 0, 10902, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 2000000},
    { ETFAccessoryNotifyAccessoryConnectionStatusChanged, 0, 0,                       0, 0, 0, 10902, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 2000000},
    { ETFDisconnectWiredAccessory,                        0, 0,                       0, 0, 0, 10902, ETFAsyDisc, ReqAccRequestNone,                         KErrNone, 2000000},
    { ETFFindAndCheckRequest,                             0, 0,                       0, 0, 0, 10902, ETFAsyNone, ReqNotifyAccessoryDisconnected,            KErrNone, 2000000},
    { ETFFindAndCheckRequest,                             0, 0,                       0, 0, 0, 10902, ETFAsyNone, ReqNotifyAccessoryConnectionStatusChanged, KErrNone, 3000000},
    { ETFFindAndCheckRequest,                             0, 2,                       0, 0, 0, 10902, ETFAsyNone, ReqNotifyAccessoryModeChanged,             KErrNone, 2000000},
    { ETFAccessoryNotifyAccessoryDisconnected,            1, 0,                       0, 0, 0, 10902, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 2000000},
    { ETFAccessoryNotifyAccessoryModeChanged,             0, 0,                       0, 0, 0, 10902, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 2000000},
    { ETFAccessoryNotifyAccessoryConnectionStatusChanged, 0, 0,                       0, 0, 0, 10902, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 3000000},
    { ETFAccessoryBtDisconnectAccessory,                  1, KBTAddrHeadsetHSPAndHFP, 0, 0, 0, 10902, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 2000000},
    { ETFFindAndCheckRequest,                             1, 0,                       0, 0, 0, 10902, ETFAsyNone, ReqNotifyAccessoryDisconnected,            KErrNone, 2000000},
    { ETFFindAndCheckRequest,                             0, 0,                       0, 0, 0, 10902, ETFAsyNone, ReqNotifyAccessoryConnectionStatusChanged, KErrNone, 2000000},
    { ETFFindAndCheckRequest,                             0, 0,                       0, 0, 0, 10902, ETFAsyNone, ReqNotifyAccessoryModeChanged,             KErrNone, 2000000},
    { ETFClearStack,                                      0, 0,                       0, 0, 0, 10902, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 1000000},
    { ETFAccessoryMode_Close,                             0, 0,                       0, 0, 0, 10902, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 2000000},
    { ETFAccessorySingleConnection_Close,                 0, 0,                       0, 0, 0, 10902, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 2000000},
    { ETFAccessoryBtControl_Close,                        0, 0,                       0, 0, 0, 10902, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 2000000},
    { ETFAccessoryConnection_Close,                       0, 0,                       0, 0, 0, 10902, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 2000000}
};

//
// [M10903] [[AccServer] Connect Wireless And Wired]
//
const TTFAccessoryTestCaseStateControl KTFConnectBTAndWiredArrayStates[]=
    {
    { ETFAccessoryMode_Open,                              0, 0,                       0, 0, 0, 10903, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 0},
    { ETFAccessoryBtControl_Open,                         0, 0,                       0, 0, 0, 10903, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 0},
    { ETFAccessorySingleConnection_Open,                  0, 0,                       0, 0, 0, 10903, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 0},
    { ETFAccessoryConnection_Open,                        0, 0,                       0, 0, 0, 10903, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 0},
    { ETFAccessoryNotifyNewAccessoryConnected,            0, 0,                       0, 0, 0, 10903, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 1000000},
    { ETFAccessoryNotifyAccessoryModeChanged,             0, 0,                       0, 0, 0, 10903, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 2000000},
    { ETFAccessoryNotifyAccessoryConnectionStatusChanged, 0, 0,                       0, 0, 0, 10903, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 2000000},
    { ETFAccessoryBtConnectAccessory,                     0, KBTAddrHeadsetHSPAndHFP, 0, 0, 0, 10903, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 4000000},    
    { ETFFindAndCheckRequest,                             0, 0,                       0, 0, 0, 10903, ETFAsyNone, ReqNotifyNewAccessoryConnected,            KErrNone, 2000000},    
    { ETFFindAndCheckRequest,                             0, 2,                       0, 0, 0, 10903, ETFAsyNone, ReqNotifyAccessoryModeChanged,             KErrNone, 2000000},
    { ETFFindAndCheckRequest,                             0, 0,                       0, 0, 0, 10903, ETFAsyNone, ReqNotifyAccessoryConnectionStatusChanged, KErrNone, 2000000},
    { ETFAccessoryNotifyAccessoryModeChanged,             0, 0,                       0, 0, 0, 10903, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 2000000},
    { ETFAccessoryNotifyNewAccessoryConnected,            1, 0,                       0, 0, 0, 10903, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 1000000},
    { ETFAccessoryNotifyAccessoryConnectionStatusChanged, 0, 0,                       0, 0, 0, 10903, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 2000000},
    { ETFConnectWiredAccessory,                           1, 0x010103,                0, 0, 0, 10903, ETFAsyConn, ReqAccRequestNone,                         KErrNone, 2000000},
    { ETFFindAndCheckRequest,                             1, 0,                       0, 0, 0, 10903, ETFAsyNone, ReqNotifyNewAccessoryConnected,            KErrNone, 2000000},
    { ETFFindAndCheckRequest,                             0, 1,                       0, 0, 0, 10903, ETFAsyNone, ReqNotifyAccessoryModeChanged,             KErrNone, 2000000},
    { ETFFindAndCheckRequest,                             0, 0,                       0, 0, 0, 10903, ETFAsyNone, ReqNotifyAccessoryConnectionStatusChanged, KErrNone, 2000000},
    };

//
// [M10904] [[AccServer] Disconnect Wireless And Wired]
//
const TTFAccessoryTestCaseStateControl KTFDisconnectBTAndWiredArrayStates[]=
{
    { ETFAccessoryNotifyAccessoryDisconnected,            0, 0,                       0, 0, 0, 10904, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 2000000},    
    { ETFAccessoryNotifyAccessoryConnectionStatusChanged, 0, 0,                       0, 0, 0, 10904, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 2000000},
    { ETFAccessoryBtDisconnectAccessory,                  0, KBTAddrHeadsetHSPAndHFP, 0, 0, 0, 10904, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 2000000},
    { ETFFindAndCheckRequest,                             0, 0,                       0, 0, 0, 10904, ETFAsyNone, ReqNotifyAccessoryDisconnected,            KErrNone, 2000000},    
    { ETFFindAndCheckRequest,                             0, 0,                       0, 0, 0, 10904, ETFAsyNone, ReqNotifyAccessoryConnectionStatusChanged, KErrNone, 2000000},
    { ETFAccessoryNotifyAccessoryDisconnected,            1, 0,                       0, 0, 0, 10904, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 2000000},
    { ETFAccessoryNotifyAccessoryModeChanged,             0, 0,                       0, 0, 0, 10904, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 2000000},
    { ETFAccessoryNotifyAccessoryConnectionStatusChanged, 0, 0,                       0, 0, 0, 10904, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 2000000},
    { ETFDisconnectWiredAccessory,                        1, 0,                       0, 0, 0, 10904, ETFAsyDisc, ReqAccRequestNone,                         KErrNone, 2000000},
    { ETFFindAndCheckRequest,                             1, 0,                       0, 0, 0, 10904, ETFAsyNone, ReqNotifyAccessoryDisconnected,            KErrNone, 2000000},
    { ETFFindAndCheckRequest,                             0, 0,                       0, 0, 0, 10904, ETFAsyNone, ReqNotifyAccessoryConnectionStatusChanged, KErrNone, 2000000},
    { ETFFindAndCheckRequest,                             0, 0,                       0, 0, 0, 10904, ETFAsyNone, ReqNotifyAccessoryModeChanged,             KErrNone, 2000000},
    { ETFClearStack,                                      0, 0,                       0, 0, 0, 10904, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 1000000},
    { ETFAccessoryMode_Close,                             0, 0,                       0, 0, 0, 10904, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 2000000},
    { ETFAccessorySingleConnection_Close,                 0, 0,                       0, 0, 0, 10904, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 2000000},
    { ETFAccessoryBtControl_Close,                        0, 0,                       0, 0, 0, 10904, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 2000000},
    { ETFAccessoryConnection_Close,                       0, 0,                       0, 0, 0, 10904, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 2000000}
};


//
// [M10905] [[AccServer] Connect Wireless And Wireless]
//
const TTFAccessoryTestCaseStateControl KTFConnectBTAndBTArrayStates[]=
    {
    { ETFAccessoryMode_Open,                              0, 0,                       0, 0, 0, 10905, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 0},
    { ETFAccessoryBtControl_Open,                         0, 0,                       0, 0, 0, 10905, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 0},
    { ETFAccessorySingleConnection_Open,                  0, 0,                       0, 0, 0, 10905, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 0},
    { ETFAccessoryConnection_Open,                        0, 0,                       0, 0, 0, 10905, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 0},
    { ETFAccessoryNotifyAccessoryModeChanged,             0, 0,                       0, 0, 0, 10905, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 2000000},
    { ETFAccessoryNotifyNewAccessoryConnected,            0, 0,                       0, 0, 0, 10905, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 1000000},
    { ETFAccessoryNotifyAccessoryConnectionStatusChanged, 0, 0,                       0, 0, 0, 10905, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 2000000},
    { ETFAccessoryBtConnectAccessory,                     0, KBTAddrHeadsetHSP,       0, 0, 0, 10905, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 2000000},
    { ETFFindAndCheckRequest,                             0, 0,                       0, 0, 0, 10905, ETFAsyNone, ReqNotifyNewAccessoryConnected,            KErrNone, 2000000},    
    { ETFFindAndCheckRequest,                             0, 0,                       0, 0, 0, 10905, ETFAsyNone, ReqNotifyAccessoryConnectionStatusChanged, KErrNone, 2000000},
    { ETFFindAndCheckRequest,                             0, 2,                       0, 0, 0, 10905, ETFAsyNone, ReqNotifyAccessoryModeChanged,             KErrNone, 2000000},    
    { ETFAccessoryNotifyNewAccessoryConnected,            1, 0,                       0, 0, 0, 10905, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 1000000},
    { ETFAccessoryNotifyAccessoryConnectionStatusChanged, 0, 0,                       0, 0, 0, 10905, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 2000000},
    { ETFAccessoryBtConnectAccessory,                     1, KBTAddrHeadsetHFP,       0, 0, 0, 10905, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 2000000},
    { ETFFindAndCheckRequest,                             1, 0,                       0, 0, 0, 10905, ETFAsyNone, ReqNotifyNewAccessoryConnected,            KErrNone, 4000000},        
    { ETFFindAndCheckRequest,                             0, 0,                       0, 0, 0, 10905, ETFAsyNone, ReqNotifyAccessoryConnectionStatusChanged, KErrNone, 2000000},    
    };

//
// [M10906] [[AccServer] Disconnect Wireless And Wireless]
//
const TTFAccessoryTestCaseStateControl KTFDisconnectBTAndBTArrayStates[]=
    {    
    { ETFAccessoryNotifyAccessoryDisconnected,            0, 0,                       0, 0, 0, 10906, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 2000000},
    { ETFAccessoryNotifyAccessoryConnectionStatusChanged, 0, 0,                       0, 0, 0, 10906, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 2000000},
    { ETFAccessoryBtDisconnectAccessory,                  0, KBTAddrHeadsetHSP,       0, 0, 0, 10906, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 2000000},    
    { ETFFindAndCheckRequest,                             0, 0,                       0, 0, 0, 10906, ETFAsyNone, ReqNotifyAccessoryDisconnected,            KErrNone, 2000000},    
    { ETFFindAndCheckRequest,                             0, 0,                       0, 0, 0, 10906, ETFAsyNone, ReqNotifyAccessoryConnectionStatusChanged, KErrNone, 2000000},
    { ETFAccessoryNotifyAccessoryModeChanged,             0, 0,                       0, 0, 0, 10906, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 2000000},
    { ETFAccessoryNotifyAccessoryDisconnected,            1, 0,                       0, 0, 0, 10906, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 2000000},
    { ETFAccessoryNotifyAccessoryConnectionStatusChanged, 0, 0,                       0, 0, 0, 10906, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 2000000},
    { ETFAccessoryBtDisconnectAccessory,                  1, KBTAddrHeadsetHFP,       0, 0, 0, 10906, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 2000000},    
    { ETFFindAndCheckRequest,                             1, 0,                       0, 0, 0, 10906, ETFAsyNone, ReqNotifyAccessoryDisconnected,            KErrNone, 2000000},    
    { ETFFindAndCheckRequest,                             0, 0,                       0, 0, 0, 10906, ETFAsyNone, ReqNotifyAccessoryModeChanged,             KErrNone, 2000000},
    { ETFFindAndCheckRequest,                             0, 0,                       0, 0, 0, 10906, ETFAsyNone, ReqNotifyAccessoryConnectionStatusChanged, KErrNone, 2000000},
    { ETFClearStack,                                      0, 0,                       0, 0, 0, 10906, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 1000000},
    { ETFAccessoryMode_Close,                             0, 0,                       0, 0, 0, 10906, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 2000000},
    { ETFAccessorySingleConnection_Close,                 0, 0,                       0, 0, 0, 10906, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 2000000},
    { ETFAccessoryBtControl_Close,                        0, 0,                       0, 0, 0, 10906, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 2000000},
    { ETFAccessoryConnection_Close,                       0, 0,                       0, 0, 0, 10906, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 2000000}
    };

/*****************************                    *********************************/
/*****************************  Selection Dialog  *********************************/
/*****************************                    *********************************/

//
// [11001] [[AccServer] Selection dialog test
//
const TTFAccessoryTestCaseStateControl KTFTestSelectionDialog[]=
    {
    { ETFAccessorySet_PubSub,             0, KASTVOut,                       0, 0, 0, 11001, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 2000000},
    { ETFSelectionDialogText,             0, 0,                       0, 0, 0, 11001, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 2000000},
    };

//
// [11002] [[AccServer] Selection dialog test
//
const TTFAccessoryTestCaseStateControl KTFTestSelectionDialogOpenCancel[]=
    {
    { ETFAccessorySet_PubSub,             0, KASNoDevice,                       0, 0, 0, 11001, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 2000000},
   // { ETFSelectionDialogText,             0, 0,                       0, 0, 0, 11001, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 2000000},
    { ETFSelectionDialogCancel,             0, 0,                       0, 0, 0, 11002, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 2000000},
    };

//
// [M11003] [[AccServer] Connect open cable, user select tv-out]]
//
const TTFAccessoryTestCaseStateControl KTFTestConnectOpenCableUserSelectTVOUT[]=
    {
        { ETFAccessorySettings_Open,                0, 0,           0, 0, 0, 11003, ETFAsyNone, ReqAccRequestNone,             KErrNone, 0},
	    { ETFSetHWDeviceSettings,                   0, KASNoDevice, 1, 0, 0, 11003, ETFAsyNone, ReqAccRequestNone,             KErrNone, 100000},
        { ETFAccessorySettings_Close,               0, 0,           0, 0, 0, 11003, ETFAsyNone, ReqAccRequestNone,             KErrNone, 0},
    	{ ETFAccessorySet_PubSub,  		            0, KASTVOut,    0, 0, 0, 11003, ETFAsyNone, ReqAccRequestNone,             KErrNone, 0},
    	{ ETFAccessoryControl_Open, 				0, 0,           0, 0, 0, 11003, ETFAsyNone, ReqAccRequestNone,             KErrNone, 0},
	    { ETFAccessorySingleConnection_Open,  		0, 0, 		    0, 0, 0, 11003, ETFAsyNone, ReqAccRequestNone,             KErrNone, 0},
	    { ETFAccessoryNotifyNewAccessoryConnected,  0, 0,           0, 0, 0, 11003, ETFAsyNone, ReqAccRequestNone,		       KErrNone, 1000000},
	    { ETFConnectWiredAccessory, 				0, 0,		    1, 0, 0, 11003, ETFAsyConnOpenCable, ReqAccRequestNone,    KErrNone, 20000000},
   		{ ETFFindAndCheckRequest,              		0, 0,		    0, 0, 0, 11003, ETFAsyNone, ReqNotifyNewAccessoryConnected,KErrNone,  2000000},
    	{ ETFDisconnectWiredAccessory, 				0, 0, 		    0, 0, 0, 11003, ETFAsyDisc, ReqAccRequestNone,             KErrNone, 2000000},
	    { ETFAccessorySingleConnection_Close,		0, 0,		    0, 0, 0, 11003, ETFAsyNone, ReqAccRequestNone,			   KErrNone, 0},
	    { ETFAccessoryControl_Close,   				0, 0, 		    0, 0, 0, 11003, ETFAsyNone, ReqAccRequestNone,             KErrNone, 0}
    };


//
// [M11004] [[AccServer] Connect open cable, ASY selects tv-out]
//
const TTFAccessoryTestCaseStateControl KTFTestConnectOpenCableAsySelectTVOUT[]=
    {
        { ETFAccessorySettings_Open,                0, 0,       0, 0, 0, 11004, ETFAsyNone, ReqAccRequestNone,             KErrNone, 0},
	    { ETFSetHWDeviceSettings,                   0, KASNoDevice, 1, 0, 0, 11004, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 100000},
        { ETFAccessorySettings_Close,               0, 0,              0, 0, 0, 11004, ETFAsyNone, ReqAccRequestNone,             KErrNone, 0},
    	{ ETFAccessoryControl_Open, 				0, 0,        0, 0, 0, 11004, ETFAsyNone, ReqAccRequestNone,      KErrNone, 0},
	    { ETFAccessorySingleConnection_Open,  		0, 0, 		 0, 0, 0, 11004, ETFAsyNone, ReqAccRequestNone, KErrNone, 0},
	    { ETFAccessoryNotifyNewAccessoryConnected,  0, 0,        0, 0, 0, 11004, ETFAsyNone, ReqAccRequestNone,		 KErrNone, 1000000},
	    { ETFConnectWiredAccessory, 				0, 0,		 1, 0, 0, 11004, ETFAsyConnOpenCable, ReqAccRequestNone,      KErrNone, 2000000},
	    { ETFConnectWiredAccessory, 				0, 0,		 1, 0, 0, 11004, ETFAsyUpdateTVOut, ReqAccRequestNone,      KErrNone,   2000000},
   		{ ETFFindAndCheckRequest,              		0, 0,		 0, 0, 0, 11004, ETFAsyNone, ReqNotifyNewAccessoryConnected, KErrNone,  2000000},
    	{ ETFDisconnectWiredAccessory, 				0, 0, 		 0, 0, 0, 11004, ETFAsyDisc, ReqAccRequestNone,         KErrNone, 2000000},
	    { ETFAccessorySingleConnection_Close,		0, 0,		 0, 0, 0, 11004, ETFAsyNone, ReqAccRequestNone,			KErrNone, 0},
	    { ETFAccessoryControl_Close,   				0, 0, 		 0, 0, 0, 11004, ETFAsyNone, ReqAccRequestNone,         KErrNone, 0}
    };

//
// [M11005] [[AccServer] Connect open cable, User selection device, after that ASY selects tv-out]
//
const TTFAccessoryTestCaseStateControl KTFTestConnectOpenCableUserSelectionAsySelectsTVOUT[]=
    {
        { ETFAccessorySet_PubSub,  		            0, KASTVOut,        0, 0, 0, 11005, ETFAsyNone, ReqAccRequestNone,             KErrNone, 0},
        { ETFAccessorySettings_Open,             0, 0,              0, 0, 0, 11005, ETFAsyNone, ReqAccRequestNone,             KErrNone, 0},
	    { ETFSetHWDeviceSettings,                   0, KASNoDevice, 1, 0, 0, 11005, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 100000},
        { ETFAccessorySettings_Close,             0, 0,              0, 0, 0, 11005, ETFAsyNone, ReqAccRequestNone,             KErrNone, 0},
    	{ ETFAccessoryControl_Open, 				0, 0,        0, 0, 0, 11005, ETFAsyNone, ReqAccRequestNone,      KErrNone, 0},
	    { ETFAccessorySingleConnection_Open,  		0, 0, 		 0, 0, 0, 11005, ETFAsyNone, ReqAccRequestNone, KErrNone, 0},
	    { ETFAccessoryNotifyNewAccessoryConnected,  0, 0,        0, 0, 0, 11005, ETFAsyNone, ReqAccRequestNone,		 KErrNone, 1000000},
	    { ETFConnectWiredAccessory, 				0, 0,		 1, 0, 0, 11005, ETFAsyConnOpenCable, ReqAccRequestNone,      KErrNone, 10000000},
   		{ ETFAccessoryNotifyAccessoryDisconnected,	0, 0, 		 0, 0, 0, 11005, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 2000000},
	    { ETFConnectWiredAccessory, 				0, 0,		 1, 0, 0, 11005, ETFAsyUpdateTVOut, ReqAccRequestNone,      KErrNone,   3000000},
   		{ ETFFindAndCheckRequest,              		0, 0,		 0, 0, 0, 11005, ETFAsyNone, ReqNotifyNewAccessoryConnected, KErrNone,  2000000},
   		{ ETFDisconnectWiredAccessory,                 0, 0,        0, 0, 0, 11005, ETFAsyDisc, ReqAccRequestNone,         KErrNone, 3000000},
   		{ ETFFindAndCheckRequest,              		0, 0,		 0, 0, 0, 11005, ETFAsyNone, ReqNotifyAccessoryDisconnected, KErrNone,  2000000},    	
	    { ETFAccessorySingleConnection_Close,		0, 0,		 0, 0, 0, 11005, ETFAsyNone, ReqAccRequestNone,			KErrNone, 0},
	    { ETFAccessoryControl_Close,   				0, 0, 		 0, 0, 0, 11005, ETFAsyNone, ReqAccRequestNone,         KErrNone, 0}
    };

//
// [M11006] [[AccServer] Connect open cable, open selection dialog, disconnect]
//
const TTFAccessoryTestCaseStateControl KTFTestConnectOpenCableOpenSelectionDialogDisconnect[]=
    {
        { ETFAccessorySettings_Open,             0, 0,              0, 0, 0, 11006, ETFAsyNone, ReqAccRequestNone,             KErrNone, 0},
	    { ETFSetHWDeviceSettings,                   0, KASNoDevice, 1, 0, 0, 11006, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 100000},
        { ETFAccessorySettings_Close,             0, 0,              0, 0, 0, 11006, ETFAsyNone, ReqAccRequestNone,             KErrNone, 0},
    	{ ETFAccessorySet_PubSub,  		            0, KASTVOut,        0, 0, 0, 11006, ETFAsyNone, ReqAccRequestNone,             KErrNone, 0},
    	{ ETFAccessoryControl_Open, 				0, 0,        0, 0, 0, 11006, ETFAsyNone, ReqAccRequestNone,      KErrNone, 0},
	    { ETFAccessorySingleConnection_Open,  		0, 0, 		 0, 0, 0, 11006, ETFAsyNone, ReqAccRequestNone, KErrNone, 0},
	    { ETFConnectWiredAccessory, 				0, 0,		 1, 0, 0, 11006, ETFAsyConnOpenCable, ReqAccRequestNone,      KErrNone, 2000000},
    	{ ETFDisconnectWiredAccessory, 				0, 0, 		 0, 0, 0, 11006, ETFAsyDisc, ReqAccRequestNone,         KErrNone, 2000000},
	    { ETFAccessorySingleConnection_Close,		0, 0,		 0, 0, 0, 11006, ETFAsyNone, ReqAccRequestNone,			KErrNone, 0},
	    { ETFAccessoryControl_Close,   				0, 0, 		 0, 0, 0, 11006, ETFAsyNone, ReqAccRequestNone,         KErrNone, 0}
    };


//
// [M11007] [[AccServer] Connect open cable, connect headset, user select headset]
//
const TTFAccessoryTestCaseStateControl KTFTestConnectOpenCableASYConnectHeadsetUserSelectHeadset[]=
    {
        { ETFAccessorySettings_Open,             0, 0,              0, 0, 0, 11007, ETFAsyNone, ReqAccRequestNone,             KErrNone, 0},
	    { ETFSetHWDeviceSettings,                   0, KASNoDevice, 1, 0, 0, 11007, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 100000},
        { ETFAccessorySettings_Close,             0, 0,              0, 0, 0, 11007, ETFAsyNone, ReqAccRequestNone,             KErrNone, 0},
    	{ ETFAccessorySet_PubSub,  		            0, KASHeadset,        0, 0, 0, 11007, ETFAsyNone, ReqAccRequestNone,             KErrNone, 0},
    	{ ETFAccessoryControl_Open, 				0, 0,        0, 0, 0, 11007, ETFAsyNone, ReqAccRequestNone,      KErrNone, 0},
	    { ETFAccessorySingleConnection_Open,  		0, 0, 		 0, 0, 0, 11007, ETFAsyNone, ReqAccRequestNone, KErrNone, 0},
	    { ETFAccessoryNotifyNewAccessoryConnected,  0, 0,        0, 0, 0, 11007, ETFAsyNone, ReqAccRequestNone,		 KErrNone, 1000000},
	    { ETFConnectWiredAccessory, 				0, 0,		 1, 0, 0, 11007, ETFAsyConnOpenCable, ReqAccRequestNone,      KErrNone, 5000000},
	    { ETFConnectWiredAccessory,                	0, 0,		 0, 0, 0, 11007, ETFAsyUpdateHeadset,          ReqAccRequestNone, KErrNone, 10000000},      
    	{ ETFDisconnectWiredAccessory, 				0, 0, 		 0, 0, 0, 11007, ETFAsyDisc, ReqAccRequestNone,         KErrNone, 2000000},
        { ETFFindAndCheckRequest,              		0, 0,		       0, 0, 0, 11007, ETFAsyNone,           ReqNotifyAccessoryDisconnected, KErrNone,  2000000},	    
	    { ETFAccessorySingleConnection_Close,		0, 0,		 0, 0, 0, 11007, ETFAsyNone, ReqAccRequestNone,			KErrNone, 0},
	    { ETFAccessoryControl_Close,   				0, 0, 		 0, 0, 0, 11007, ETFAsyNone, ReqAccRequestNone,         KErrNone, 0}
    };

//
// [11008] [[AccServer] Connect ECI accessory]
//
const TTFAccessoryTestCaseStateControl KTFTestConnectECIAccessory[]=
    {
    	{ ETFAccessoryControl_Open, 				0, 0,                        0, 0,                 0, 11008, ETFAsyNone,                ReqAccRequestNone,              KErrNone, 0},
	    { ETFAccessorySingleConnection_Open,  		0, 0, 	                	 0, 0,                 0, 11008, ETFAsyNone,                ReqAccRequestNone,              KErrNone, 0},
	    { ETFAccessoryNotifyNewAccessoryConnected,  0, 0,                        0, 0,                 0, 11008, ETFAsyNone,                ReqAccRequestNone,              KErrNone, 1000000},
	    { ETFConnectWiredAccessory, 				0, 0,	                   	 1, 0,                 0, 11008, ETFAsyConnectECIAccessory, ReqAccRequestNone,              KErrNone, 5000000},
	    { ETFFindAndCheckRequest,                   0, 0,                        0, 0,                 0, 11008, ETFAsyNone,                ReqNotifyNewAccessoryConnected, KErrNone, 0},
	    //Check ECI capability
	    { ETFAccessoryDes8GetValue,                 0, KAccECIAudioBlock,        0, 0,                 0, 11008, ETFAsyNone,                ReqAccRequestNone,              KErrNone, 5000000},	       		
    	{ ETFDisconnectWiredAccessory, 				0, 0,                 		 0, 0,                 0, 11008, ETFAsyDisc,                ReqAccRequestNone,              KErrNone, 2000000},
   		{ ETFFindAndCheckRequest,              		0, 0,	                     0, 0,                 0, 11008, ETFAsyNone,                ReqNotifyAccessoryDisconnected, KErrNone, 2000000},
	    { ETFAccessorySingleConnection_Close,		0, 0,	                  	 0, 0,                 0, 11008, ETFAsyNone,                ReqAccRequestNone,		        KErrNone, 0},
	    { ETFAccessoryControl_Close,   				0, 0, 	                  	 0, 0,                 0, 11008, ETFAsyNone,                ReqAccRequestNone,              KErrNone, 0}
    };
    
//
// [M11009] [[AccServer] Connect headset, default accessory headset, default is selected]
//
const TTFAccessoryTestCaseStateControl KTFTestConnectOpenCableDefaultSelection[]=
    {
    	{ ETFAccessoryControl_Open, 				0, 0,        0, 0, 0, 11009, ETFAsyNone, ReqAccRequestNone,      KErrNone, 0},
	    { ETFAccessorySingleConnection_Open,  		0, 0, 		 0, 0, 0, 11009, ETFAsyNone, ReqAccRequestNone, KErrNone, 0},
        { ETFAccessorySettings_Open,             0, 0,              0, 0, 0, 11009, ETFAsyNone, ReqAccRequestNone,             KErrNone, 0},
	    { ETFSetHWDeviceSettings,                   0, KASHeadset, 1, 0, 0, 11009, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 100000},
	    { ETFAccessoryNotifyNewAccessoryConnected,  0, 0,        0, 0, 0, 11009, ETFAsyNone, ReqAccRequestNone,		 KErrNone, 1000000},
	    { ETFConnectWiredAccessory, 				0, 0,		 1, 0, 0, 11009, ETFAsyConnectHeadset, ReqAccRequestNone,      KErrNone, 5000000},
    	{ ETFDisconnectWiredAccessory, 				0, 0, 		 0, 0, 0, 11009, ETFAsyDisc, ReqAccRequestNone,         KErrNone, 2000000},
	    { ETFFindAndCheckRequest,              		0, 0,		 0, 0, 0, 11009, ETFAsyNone, ReqNotifyAccessoryDisconnected, KErrNone,  2000000},
	    { ETFSetHWDeviceSettings,                   0, KASNoDevice, 1, 0, 0, 11009, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 100000},
        { ETFAccessorySettings_Close,             0, 0,              0, 0, 0, 11009, ETFAsyNone, ReqAccRequestNone,             KErrNone, 0},
	    { ETFAccessorySingleConnection_Close,		0, 0,		 0, 0, 0, 11009, ETFAsyNone, ReqAccRequestNone,			KErrNone, 0},
	    { ETFAccessoryControl_Close,   				0, 0, 		 0, 0, 0, 11009, ETFAsyNone, ReqAccRequestNone,         KErrNone, 0},
    };   
    
//
// [M11010] [[AccServer] Connect Headset]]
//
const TTFAccessoryTestCaseStateControl KTFTestConnectHeadset[]=
    {
    	{ ETFAccessorySet_PubSub,  		            0, KASHeadset,        0, 0, 0, 11010, ETFAsyNone, ReqAccRequestNone,             KErrNone, 0},
    	{ ETFAccessoryControl_Open, 				0, 0,        0, 0, 0, 11010, ETFAsyNone, ReqAccRequestNone,      KErrNone, 0},
	    { ETFAccessorySingleConnection_Open,  		0, 0, 		 0, 0, 0, 11010, ETFAsyNone, ReqAccRequestNone, KErrNone, 0},    	
        { ETFAccessorySettings_Open,             0, 0,              0, 0, 0, 11010, ETFAsyNone, ReqAccRequestNone,             KErrNone, 0},
	    { ETFSetHWDeviceSettings,                   0, KASHeadset, 1, 0, 0, 11010, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 100000},
	    { ETFConnectWiredAccessory, 				0, 0,		 1, 0, 0, 11010, ETFAsyConnectHeadset, ReqAccRequestNone,      KErrNone, 2000000},
	    { ETFDisconnectWiredAccessory, 				0, 0, 		 0, 0, 0, 11010, ETFAsyDisc, ReqAccRequestNone,         KErrNone, 2000000},
	    { ETFSetHWDeviceSettings,                   0, KASNoDevice, 1, 0, 0, 11010, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 100000},
        { ETFAccessorySettings_Close,             0, 0,              0, 0, 0, 11010, ETFAsyNone, ReqAccRequestNone,             KErrNone, 0},
	    { ETFAccessorySingleConnection_Close,		0, 0,		 0, 0, 0, 11010, ETFAsyNone, ReqAccRequestNone,			KErrNone, 0},        
        { ETFAccessoryControl_Close,   				0, 0, 		 0, 0, 0, 11010, ETFAsyNone, ReqAccRequestNone,         KErrNone, 0}
    };

//
// [M11011] [[AccServer] Connect Headphones]]
//
const TTFAccessoryTestCaseStateControl KTFTestConnectHeadphones[]=
    {
    	{ ETFAccessorySet_PubSub,  		            0, KASHeadphones,        0, 0, 0, 11011, ETFAsyNone, ReqAccRequestNone,             KErrNone, 0},
    	{ ETFAccessoryControl_Open, 				0, 0,        0, 0, 0, 11011, ETFAsyNone, ReqAccRequestNone,      KErrNone, 0},
        { ETFAccessorySingleConnection_Open,  		0, 0, 		 0, 0, 0, 11011, ETFAsyNone, ReqAccRequestNone, KErrNone, 0},    	
        { ETFConnectWiredAccessory, 				0, 0,		 1, 0, 0, 11011, ETFAsyConnectOpenCable3Poles, ReqAccRequestNone,      KErrNone, 10000000},
        { ETFDisconnectWiredAccessory, 				0, 0, 		 0, 0, 0, 11011, ETFAsyDisc, ReqAccRequestNone,         KErrNone, 2000000},
        { ETFAccessorySingleConnection_Close,		0, 0,		 0, 0, 0, 11011, ETFAsyNone, ReqAccRequestNone,			KErrNone, 0},        
        { ETFAccessoryControl_Close,   				0, 0, 		 0, 0, 0, 11011, ETFAsyNone, ReqAccRequestNone,         KErrNone, 0}        
    };

//
// [M11012] [[AccServer] Connect Open Cable]]
//
const TTFAccessoryTestCaseStateControl KTFTestConnectOpenCable[]=
    {
    	{ ETFAccessorySet_PubSub,  		            0, KASHeadphones,        0, 0, 0, 11012, ETFAsyNone, ReqAccRequestNone,             KErrNone, 0},
    	{ ETFAccessoryControl_Open, 				0, 0,        0, 0, 0, 11012, ETFAsyNone, ReqAccRequestNone,      KErrNone, 0},
    	{ ETFAccessorySingleConnection_Open,  		0, 0, 		 0, 0, 0, 11012, ETFAsyNone, ReqAccRequestNone, KErrNone, 0},    	
        { ETFConnectWiredAccessory, 				0, 0,		 1, 0, 0, 11012, ETFAsyConnOpenCable, ReqAccRequestNone,      KErrNone, 10000000},
        { ETFDisconnectWiredAccessory, 				0, 0, 		 0, 0, 0, 11012, ETFAsyDisc, ReqAccRequestNone,         KErrNone, 2000000},
        { ETFAccessorySingleConnection_Close,		0, 0,		 0, 0, 0, 11012, ETFAsyNone, ReqAccRequestNone,			KErrNone, 0},        
        { ETFAccessoryControl_Close,   				0, 0, 		 0, 0, 0, 11012, ETFAsyNone, ReqAccRequestNone,         KErrNone, 0}        
    };

//
// [M11013] [[AccServer] Connect TV Out]]
//
const TTFAccessoryTestCaseStateControl KTFTestConnectTVOUT[]=
    {
    	{ ETFAccessorySet_PubSub,  		            0, KASTVOut,        0, 0, 0, 11013, ETFAsyNone, ReqAccRequestNone,             KErrNone, 0},
    	{ ETFAccessoryControl_Open, 				0, 0,        0, 0, 0, 11013, ETFAsyNone, ReqAccRequestNone,      KErrNone, 0},
    	{ ETFAccessorySingleConnection_Open,  		0, 0, 		 0, 0, 0, 11013, ETFAsyNone, ReqAccRequestNone, KErrNone, 0},    	
        { ETFConnectWiredAccessory, 				0, 0,		 1, 0, 2, 11013, ETFAsyUpdateTVOut, ReqAccRequestNone,      KErrNone, 2000000},
        { ETFDisconnectWiredAccessory, 				0, 0, 		 0, 0, 0, 11013, ETFAsyDisc, ReqAccRequestNone,         KErrNone, 2000000},
        { ETFAccessorySingleConnection_Close,		0, 0,		 0, 0, 0, 11013, ETFAsyNone, ReqAccRequestNone,			KErrNone, 0},        
        { ETFAccessoryControl_Close,   				0, 0, 		 0, 0, 0, 11013, ETFAsyNone, ReqAccRequestNone,         KErrNone, 0}        
    };
    
//
// [M11014 [[AccServer] Disconnect wired accessory]]
//
const TTFAccessoryTestCaseStateControl KTFTestDisconnectWired[]=
    {
    	{ ETFAccessoryControl_Open, 				0, 0,        0, 0, 0, 11014, ETFAsyNone, ReqAccRequestNone,      KErrNone, 0},
    	{ ETFAccessorySingleConnection_Open,  		0, 0, 		 0, 0, 0, 11014, ETFAsyNone, ReqAccRequestNone, KErrNone, 0},    	
    	{ ETFDisconnectWiredAccessory, 				0, 0, 		 0, 0, 0, 11014, ETFAsyDisc, ReqAccRequestNone,         KErrNone, 2000000},
    	{ ETFAccessorySingleConnection_Close,		0, 0,		 0, 0, 0, 11014, ETFAsyNone, ReqAccRequestNone,			KErrNone, 0},        
        { ETFAccessoryControl_Close,   				0, 0, 		 0, 0, 0, 11014, ETFAsyNone, ReqAccRequestNone,         KErrNone, 0}        
    };


//
// [M11015] [[AccServer] Connect Headphones, no selection, disconnect accessory]]
//
const TTFAccessoryTestCaseStateControl KTFTestConnectAndDisconnectHeadphones[]=
    {
    	{ ETFAccessorySet_PubSub,  		            0, KASNoDevice,        0, 0, 0, 11015, ETFAsyNone, ReqAccRequestNone,             KErrNone, 0},
    	{ ETFAccessoryControl_Open, 				0, 0,        0, 0, 0, 11015, ETFAsyNone, ReqAccRequestNone, KErrNone, 0},
        { ETFAccessorySingleConnection_Open,  		0, 0, 		 0, 0, 0, 11015, ETFAsyNone, ReqAccRequestNone, KErrNone, 0},    	
        { ETFConnectWiredAccessory, 				0, 0,		 1, 0, 0, 11015, ETFAsyConnectOpenCable3Poles, ReqAccRequestNone,      KErrNone, 5000000},
        { ETFDisconnectWiredAccessory, 				0, 0, 		 1, 0, 0, 11009, ETFAsyDisc, ReqAccRequestNone,         KErrNone, 1000000},    
        { ETFAccessoryAudioControl_Close,           0, 0,        0, 0, 0, 11015, ETFAsyNone, ReqAccRequestNone, KErrNone, 0},
        { ETFAccessorySingleConnection_Close,		0, 0,		 0, 0, 0, 11015, ETFAsyNone, ReqAccRequestNone,	KErrNone, 0},        
        { ETFAccessoryControl_Close,   				0, 0, 		 0, 0, 0, 11015, ETFAsyNone, ReqAccRequestNone, KErrNone, 0}        
    };
    
//
// [M10611 [[AccServer] KTFCheckConnectedAccessory]]
//
const TTFAccessoryTestCaseStateControl KTFCheckConnectedAccessory[]=
    {
    	{ ETFAccessoryControl_Open, 				0, 0,        0, 0, 0, 10611, ETFAsyNone, ReqAccRequestNone, KErrNone, 0},
    	{ ETFAccessorySingleConnection_Open,  		0, 0, 		 0, 0, 0, 10611, ETFAsyNone, ReqAccRequestNone, KErrNone, 0},    	
		{ ETFAccessoryNotifyNewAccessoryConnected,  0, 0,        0, 0, 0, 10611, ETFAsyNone, ReqAccRequestNone, KErrNone, 1000000},
    	{ ETFConnectWiredAccessory, 				0, 0,   	 1, 0, 0, 10611, ETFAsyConnectECIAccessory, ReqAccRequestNone, KErrNone, 5000000},
	    { ETFFindAndCheckRequest,                   0, 0,	     0, 0, 0, 10611, ETFAsyNone, ReqNotifyNewAccessoryConnected, KErrNone, 0},
    	{ ETFCheckConnectedAccessory,				0, 0,		 0, 0, 0, 10611, ETFAsyNone, ReqAccRequestNone,			KErrNone, 2000000},        
    	{ ETFDisconnectWiredAccessory, 				0, 0, 		 0, 0, 0, 10611, ETFAsyDisc, ReqAccRequestNone,         KErrNone, 2000000},
    	{ ETFClearStack,                            0, 0,        0, 0, 0, 10611, ETFAsyNone, ReqAccRequestNone,                         KErrNone, 0},
    	{ ETFAccessorySingleConnection_Close,		0, 0,		 0, 0, 0, 10611, ETFAsyNone, ReqAccRequestNone,			KErrNone, 0},        
        { ETFAccessoryControl_Close,   				0, 0, 		 0, 0, 0, 10611, ETFAsyNone, ReqAccRequestNone,         KErrNone, 0}        
    };

/*****************************                *********************************/
/*****************************  Policy Tests  *********************************/
/*****************************                *********************************/

//
// [11101] [[AccPolicy] ValueRecord test
//

const TTFAccessoryTestCaseStateControl KTFTestValueRecord[]=
    {
    { ETFValueRecordTests, 0, 0, 0, 0, 0, 11101, ETFAsyNone, ReqAccRequestNone, KErrNone, 0},
    };

//
// [M11103] [[AccPolicy] Object Base Serialization test
//
const TTFAccessoryTestCaseStateControl KTFTestObjectBaseSerialization[]=
    {
    { ETFObjectBaseSerializationTests, 0, 0, 0, 0, 0, 11103, ETFAsyNone, ReqAccRequestNone, KErrNone, 0},
    };

/*****************************                *********************************/
/*****************************  New tests to  *********************************/
/*****************************  increase code *********************************/
/*****************************  coverage      *********************************/
/*****************************                *********************************/

//
// [11101] [[AccServer] Cancel connects
//

//
// [M11102] [[AccServer] Cancel connect Accessory]
//
/*const TTFAccessoryTestCaseStateControl KTFWiredHeadsetConnectCancelStates[7]=
    {
    { ETFAccessoryBtControl_Open, 0, 0, 0, 0, 0, 11102, ETFAsyNone, ReqAccRequestNone, KErrNone, 0},
    { ETFAccessoryBtCancelConnectAccessory, 0, 0, 0, 0, 0, 11102, ETFAsyNone, ReqAccRequestNone, KErrNone, 2000000},
    { ETFAccessoryBtConnectAccessory, 0, KBTAddrHeadsetHSPAndHFP, 0, 0, 0, 11102, ETFAsyNone, ReqAccRequestNone, KErrNone, 2000000},
    { ETFAccessoryBtCancelDisconnectAccessory, 0, KBTAddrHeadsetHSPAndHFP, 0, 0, 0, 11102, ETFAsyNone, ReqAccRequestNone, KErrNone, 2000000},
    { ETFAccessoryBtDisconnectAccessory, 0, KBTAddrHeadsetHSPAndHFP, 0, 0, 0, 11102, ETFAsyNone, ReqAccRequestNone, KErrNone, 2000000},
    };
*/

//
// [M11102] [[AccServer] Cancel connect Accessory]
//
const TTFAccessoryTestCaseStateControl KTFCallTClassMethods[1]=
    {
    { ETFCallTClassMethods, 0, 0, 0, 0, 0, 11102, ETFAsyNone, ReqAccRequestNone, KErrNone, 0},
    };

/************************                                            ****************************/
/************************  CAccPolObjectBase related testcases (USB) ****************************/
/************************                                            ****************************/

//
// [M11201] AccServer [ Connect USB headset ]
//
const TTFAccessoryTestCaseStateControl KTFConnectUSBheadsetStates[]=
    {
    { ETFAccessorySingleConnection_Open,        0, 0,        0,     0,                                            0, 11201, ETFAsyNone,             ReqAccRequestNone,              KErrNone, 0},
    { ETFAccessoryNotifyNewAccessoryConnected,  0, 0,        0,     0,                                            0, 11201, ETFAsyNone,             ReqAccRequestNone,              KErrNone, 1000000},
    { ETFConnectWiredAccessory,                 0, 0x010602, 0,     0,                                            0, 11201, ETFAsyConnUSB,          ReqAccRequestNone,              KErrNone, 2000000},
    { ETFFindAndCheckRequest,                   0, 0,        0,     0,                                            0, 11201, ETFAsyNone,             ReqNotifyNewAccessoryConnected, KErrNone, 0},
    { ETFCheckCapability,                       0, 0,        ETrue, KAccAudioStreamsFormat,                       0, 11201, ETFAsyNone,             ReqAccRequestNone,              KErrNone, 0},
    { ETFAccessoryNotifyAccessoryDisconnected,  0, 0,        0,     0,                                            0, 11201, ETFAsyNone,             ReqAccRequestNone,              KErrNone, 2000000},
    { ETFDisconnectWiredAccessory,              0, 0,        0,     0,                                            0, 11201, ETFAsyDisc,             ReqAccRequestNone,              KErrNone, 2000000},
    { ETFFindAndCheckRequest,                   0, 0,        0,     0,                                            0, 11201, ETFAsyNone,             ReqNotifyAccessoryDisconnected, KErrNone, 0},
    { ETFAccessorySingleConnection_Close,       0, 0,        0,     0,                                            0, 11201, ETFAsyNone,             ReqAccRequestNone,              KErrNone, 0},
    };

//
// [M11202] AccServer [ Get and Set USB headset audio streams format parameter]
//
const TTFAccessoryTestCaseStateControl KTFGetSetUSBheadsetValue[]=
    {
    { ETFAccessorySingleConnection_Open,        0, 0,        0,     0,                                               0, 11202, ETFAsyNone,             ReqAccRequestNone,              KErrNone, 0},
    { ETFAccessoryControl_Open,                 0, 0,        0,     0,                                               0, 11202, ETFAsyNone,             ReqAccRequestNone,              KErrNone, 0},
    { ETFAccessoryNotifyNewAccessoryConnected,  0, 0,        0,     0,                                               0, 11202, ETFAsyNone,             ReqAccRequestNone,              KErrNone, 1000000},
    { ETFConnectWiredAccessory,                 0, 0x010602, 0,     0,                                               0, 11202, ETFAsyConnUSB,          ReqAccRequestNone,              KErrNone, 2000000},
    { ETFFindAndCheckRequest,                   0, 0,        0,     0,                                               0, 11202, ETFAsyNone,             ReqNotifyNewAccessoryConnected, KErrNone, 0},    
    { ETFAccessorySet_ObjectPubSub,	            0, EAccPolAudioInputStreamObject, 2, EAccPolAudioOutputStreamObject, 2, 11202, ETFAsyNone,             ReqAccRequestNone,              KErrNone, 0}, 
    { ETFAccessoryGetStreamObjects,             0, 0,        0,     KAccAudioStreamsFormat,                          0, 11202, ETFAsyNone,             ReqAccRequestNone,              KErrNone, 0},
    { ETFAccessorySetStreamObjects,             0, 0,        0,     KAccAudioStreamsFormat,                          0, 11202, ETFAsyNone,             ReqAccRequestNone,              KErrNone, 0},    
    { ETFAccessoryNotifyAccessoryDisconnected,  0, 0,        0,     0,                                               0, 11202, ETFAsyNone,             ReqAccRequestNone,              KErrNone, 2000000},
    { ETFDisconnectWiredAccessory,              0, 0,        0,     0,                                               0, 11202, ETFAsyDisc,             ReqAccRequestNone,              KErrNone, 2000000},
    { ETFFindAndCheckRequest,                   0, 0,        0,     0,                                               0, 11202, ETFAsyNone,             ReqNotifyAccessoryDisconnected, KErrNone, 0},
    { ETFAccessoryControl_Close,                0, 0,        0,     0,                                               0, 11202, ETFAsyNone,             ReqAccRequestNone,              KErrNone, 0},
    { ETFAccessorySingleConnection_Close,       0, 0,        0,     0,                                               0, 11202, ETFAsyNone,             ReqAccRequestNone,              KErrNone, 0},
    };

//
// [M11203] [[AccServer] Get USB audio units (Audio Topology) ]
//
const TTFAccessoryTestCaseStateControl KTFUSBheadsetAudioControls[]=
    {
    { ETFAccessorySingleConnection_Open,        0, 0,        0,     0,                                               0, 11202, ETFAsyNone,             ReqAccRequestNone,              KErrNone, 0},
    { ETFAccessoryControl_Open,                 0, 0,        0,     0,                                               0, 11202, ETFAsyNone,             ReqAccRequestNone,              KErrNone, 0},
    { ETFAccessoryNotifyNewAccessoryConnected,  0, 0,        0,     0,                                               0, 11202, ETFAsyNone,             ReqAccRequestNone,              KErrNone, 1000000},
    { ETFConnectWiredAccessory,                 0, 0x010602, 0,     0,                                               0, 11202, ETFAsyConnUSB,          ReqAccRequestNone,              KErrNone, 2000000},
    { ETFFindAndCheckRequest,                   0, 0,        0,     0,                                               0, 11202, ETFAsyNone,             ReqNotifyNewAccessoryConnected, KErrNone, 0},    
    { ETFAccessorySet_TopologyObjectsPubSub,    0, 0,        0,     0,                                               0, 11203, ETFAsyNone,             ReqAccRequestNone,              KErrNone, 0},         
    { ETFAccessoryGetTopologyObjects,           0, 0,        0,     KAccAudioUnits,                                  0, 11203, ETFAsyNone,             ReqAccRequestNone,              KErrNone, 0},
    { ETFAccessoryObjectParseTopology,          0, 0,        0,     0,                                               0, 11203, ETFAsyNone,             ReqAccRequestNone,              KErrNone, 0},
    { ETFAccessoryNotifyAccessoryDisconnected,  0, 0,        0,     0,                                               0, 11203, ETFAsyNone,             ReqAccRequestNone,              KErrNone, 2000000},
    { ETFDisconnectWiredAccessory,              0, 0,        0,     0,                                               0, 11203, ETFAsyDisc,             ReqAccRequestNone,              KErrNone, 2000000},
    { ETFFindAndCheckRequest,                   0, 0,        0,     0,                                               0, 11203, ETFAsyNone,             ReqNotifyAccessoryDisconnected, KErrNone, 0},
    { ETFAccessoryControl_Close,                0, 0,        0,     0,                                               0, 11203, ETFAsyNone,             ReqAccRequestNone,              KErrNone, 0},
    { ETFAccessorySingleConnection_Close,       0, 0,        0,     0,                                               0, 11203, ETFAsyNone,             ReqAccRequestNone,              KErrNone, 0},
    };
    
//
// [M11204] [[AccServer] Set/Get volume control]
//
const TTFAccessoryTestCaseStateControl KTFSetGetUSBvolumeControl[]=
    {
    { ETFAccessorySingleConnection_Open,        0, 0,        0,     0,                                               0, 11204, ETFAsyNone,             ReqAccRequestNone,              KErrNone, 0},
    { ETFAccessoryControl_Open,                 0, 0,        0,     0,                                               0, 11204, ETFAsyNone,             ReqAccRequestNone,              KErrNone, 0},
    { ETFAccessoryNotifyNewAccessoryConnected,  0, 0,        0,     0,                                               0, 11204, ETFAsyNone,             ReqAccRequestNone,              KErrNone, 1000000},
    { ETFConnectWiredAccessory,                 0, 0x010602, 0,     0,                                               0, 11204, ETFAsyConnUSB,          ReqAccRequestNone,              KErrNone, 2000000},
    { ETFFindAndCheckRequest,                   0, 0,        0,     0,                                               0, 11204, ETFAsyNone,             ReqNotifyNewAccessoryConnected, KErrNone, 0},    
    { ETFAccessorySet_TopologyObjectsPubSub,    0, 0,        0,     0,                                               0, 11204, ETFAsyNone,             ReqAccRequestNone,              KErrNone, 0},         
    { ETFAccessoryGetTopologyObjects,           0, 0,        0,     KAccAudioUnits,                                  0, 11204, ETFAsyNone,             ReqAccRequestNone,              KErrNone, 0},
    { ETFAccessoryObjectParseTopology,          0, 0,        0,     0,                                               0, 11204, ETFAsyNone,             ReqAccRequestNone,              KErrNone, 0},
    { ETFAccessorySetVolumeControlObjects,           0, 10,   0x8000,    KAccAudioControl,                             0x7FFF, 11204, ETFAsyNone,        ReqAccRequestNone,              KErrNone, 1000},        
    { ETFAccessoryGetVolumeControlObjects,           0, 0,        0,     KAccAudioControl,                             0, 11202, ETFAsyNone,             ReqAccRequestNone,              KErrNone, 1000},    
    { ETFAccessorySetVolumeControlObjects,           0, 20,   0x8000,    KAccAudioControl,                             0x7FFF, 11204, ETFAsyNone,        ReqAccRequestNone,              KErrNone, 1000},        
    { ETFAccessoryGetVolumeControlObjects,           0, 0,        0,     KAccAudioControl,                             0, 11202, ETFAsyNone,             ReqAccRequestNone,              KErrNone, 1000},    
    { ETFAccessorySetVolumeControlObjects,           0, 30,   0x8000,    KAccAudioControl,                             0x7FFF, 11204, ETFAsyNone,        ReqAccRequestNone,              KErrNone, 1000},        
    { ETFAccessoryGetVolumeControlObjects,           0, 0,        0,     KAccAudioControl,                             0, 11202, ETFAsyNone,             ReqAccRequestNone,              KErrNone, 1000},    
    { ETFAccessorySetVolumeControlObjects,           0, 40,   0x8000,    KAccAudioControl,                             0x7FFF, 11204, ETFAsyNone,        ReqAccRequestNone,              KErrNone, 1000},        
    { ETFAccessoryGetVolumeControlObjects,           0, 0,        0,     KAccAudioControl,                             0, 11202, ETFAsyNone,             ReqAccRequestNone,              KErrNone, 1000},    
    { ETFAccessorySetVolumeControlObjects,           0, 50,   0x8000,    KAccAudioControl,                             0x7FFF, 11204, ETFAsyNone,        ReqAccRequestNone,              KErrNone, 1000},        
    { ETFAccessoryGetVolumeControlObjects,           0, 0,        0,     KAccAudioControl,                             0, 11202, ETFAsyNone,             ReqAccRequestNone,              KErrNone, 1000},    
    { ETFAccessorySetVolumeControlObjects,           0, 60,   0x8000,    KAccAudioControl,                             0x7FFF, 11204, ETFAsyNone,        ReqAccRequestNone,              KErrNone, 1000},        
    { ETFAccessoryGetVolumeControlObjects,           0, 0,        0,     KAccAudioControl,                             0, 11202, ETFAsyNone,             ReqAccRequestNone,              KErrNone, 1000},        
    { ETFAccessoryNotifyAccessoryDisconnected,  0, 0,        0,     0,                                               0, 11204, ETFAsyNone,             ReqAccRequestNone,              KErrNone, 2000000},
    { ETFDisconnectWiredAccessory,              0, 0,        0,     0,                                               0, 11204, ETFAsyDisc,             ReqAccRequestNone,              KErrNone, 2000000},
    { ETFFindAndCheckRequest,                   0, 0,        0,     0,                                               0, 11204, ETFAsyNone,             ReqNotifyAccessoryDisconnected, KErrNone, 0},
    { ETFAccessoryControl_Close,                0, 0,        0,     0,                                               0, 11204, ETFAsyNone,             ReqAccRequestNone,              KErrNone, 0},
    { ETFAccessorySingleConnection_Close,       0, 0,        0,     0,                                               0, 11204, ETFAsyNone,             ReqAccRequestNone,              KErrNone, 0},
    };
    

//
// [M11205] [[AccServer] Set/Get mute control]
//
const TTFAccessoryTestCaseStateControl KTFSetGetUSBmuteControl[]=
    {
    { ETFAccessorySingleConnection_Open,        0, 0,        0,     0,                                               0, 11205, ETFAsyNone,             ReqAccRequestNone,              KErrNone, 0},
    { ETFAccessoryControl_Open,                 0, 0,        0,     0,                                               0, 11205, ETFAsyNone,             ReqAccRequestNone,              KErrNone, 0},
    { ETFAccessoryNotifyNewAccessoryConnected,  0, 0,        0,     0,                                               0, 11205, ETFAsyNone,             ReqAccRequestNone,              KErrNone, 1000000},
    { ETFConnectWiredAccessory,                 0, 0x010602, 0,     0,                                               0, 11205, ETFAsyConnUSB,          ReqAccRequestNone,              KErrNone, 2000000},
    { ETFFindAndCheckRequest,                   0, 0,        0,     0,                                               0, 11205, ETFAsyNone,             ReqNotifyNewAccessoryConnected, KErrNone, 0},    
    { ETFAccessorySet_TopologyObjectsPubSub,    0, 0,        0,     0,                                               0, 11205, ETFAsyNone,             ReqAccRequestNone,              KErrNone, 0},         
    { ETFAccessoryGetTopologyObjects,           0, 0,        0,     KAccAudioUnits,                                  0, 11205, ETFAsyNone,             ReqAccRequestNone,              KErrNone, 0},
    { ETFAccessoryObjectParseTopology,          0, 0,        0,     0,                                               0, 11205, ETFAsyNone,             ReqAccRequestNone,              KErrNone, 0},
    { ETFAccessorySetMuteControlObjects,           0, 1,        0,     KAccAudioControl,                             0, 11205, ETFAsyNone,        ReqAccRequestNone,              KErrNone, 1000},        
    { ETFAccessoryGetMuteControlObjects,           0, 0,        0,     KAccAudioControl,                             0, 11205, ETFAsyNone,             ReqAccRequestNone,              KErrNone, 1000},    
    { ETFAccessorySetMuteControlObjects,           0, 0,        0,     KAccAudioControl,                             0, 11205, ETFAsyNone,        ReqAccRequestNone,              KErrNone, 1000},        
    { ETFAccessoryGetMuteControlObjects,           0, 0,        0,     KAccAudioControl,                             0, 11205, ETFAsyNone,             ReqAccRequestNone,              KErrNone, 1000},    
    { ETFAccessorySetMuteControlObjects,           0, 1,        0,     KAccAudioControl,                             0, 11205, ETFAsyNone,        ReqAccRequestNone,              KErrNone, 1000},        
    { ETFAccessoryGetMuteControlObjects,           0, 0,        0,     KAccAudioControl,                             0, 11205, ETFAsyNone,             ReqAccRequestNone,              KErrNone, 1000},    
    { ETFAccessorySetMuteControlObjects,           0, 0,        0,     KAccAudioControl,                             0, 11205, ETFAsyNone,        ReqAccRequestNone,              KErrNone, 1000},        
    { ETFAccessoryGetMuteControlObjects,           0, 0,        0,     KAccAudioControl,                             0, 11205, ETFAsyNone,             ReqAccRequestNone,              KErrNone, 1000},    
    { ETFAccessorySetMuteControlObjects,           0, 1,        0,     KAccAudioControl,                             0, 11205, ETFAsyNone,        ReqAccRequestNone,              KErrNone, 1000},        
    { ETFAccessoryGetMuteControlObjects,           0, 0,        0,     KAccAudioControl,                             0, 11205, ETFAsyNone,             ReqAccRequestNone,              KErrNone, 1000},    
    { ETFAccessorySetMuteControlObjects,           0, 0,        0,     KAccAudioControl,                             0, 11205, ETFAsyNone,        ReqAccRequestNone,              KErrNone, 1000},        
    { ETFAccessoryGetMuteControlObjects,           0, 0,        0,     KAccAudioControl,                             0, 11205, ETFAsyNone,             ReqAccRequestNone,              KErrNone, 1000},        
    { ETFAccessoryNotifyAccessoryDisconnected,  0, 0,        0,     0,                                               0, 11205, ETFAsyNone,             ReqAccRequestNone,              KErrNone, 2000000},
    { ETFDisconnectWiredAccessory,              0, 0,        0,     0,                                               0, 11205, ETFAsyDisc,             ReqAccRequestNone,              KErrNone, 2000000},
    { ETFFindAndCheckRequest,                   0, 0,        0,     0,                                               0, 11205, ETFAsyNone,             ReqNotifyAccessoryDisconnected, KErrNone, 0},
    { ETFAccessoryControl_Close,                0, 0,        0,     0,                                               0, 11205, ETFAsyNone,             ReqAccRequestNone,              KErrNone, 0},
    { ETFAccessorySingleConnection_Close,       0, 0,        0,     0,                                               0, 11205, ETFAsyNone,             ReqAccRequestNone,              KErrNone, 0},
    };
    
    
    
//
// [M11206] AccServer [ Connect USB 2 headset / Policy test ]
//
const TTFAccessoryTestCaseStateControl KTFConnectUSB2headsetStates[]=
    {
    { ETFAccessorySingleConnection_Open,        0, 0,        0,     0,                      0, 11201, ETFAsyNone,     ReqAccRequestNone,              KErrNone, 0},
    { ETFAccessoryNotifyNewAccessoryConnected,  0, 0,        0,     0,                      0, 11201, ETFAsyNone,     ReqAccRequestNone,              KErrNone, 1000000},
    { ETFConnectWiredAccessory,                 0, 0x010602, 0,     0,                      0, 11201, ETFAsyConnUSB2, ReqAccRequestNone,              KErrNone, 2000000},
    { ETFFindAndCheckRequest,                   0, 0,        0,     0,                      0, 11201, ETFAsyNone,     ReqNotifyNewAccessoryConnected, KErrNone, 0},
    { ETFCheckCapability,                       0, 0,        ETrue, KAccAudioStreamsFormat, 0, 11201, ETFAsyNone,     ReqAccRequestNone,              KErrNone, 0},
    { ETFAccessoryGetStreamObjectsFromPolicy,   0, 0,        0,     KAccAudioStreamsFormat, 0, 11202, ETFAsyNone,     ReqAccRequestNone,              KErrNone, 0},
    { ETFAccessoryNotifyAccessoryDisconnected,  0, 0,        0,     0,                      0, 11201, ETFAsyNone,     ReqAccRequestNone,              KErrNone, 2000000},
    { ETFDisconnectWiredAccessory,              0, 0,        0,     0,                      0, 11201, ETFAsyDisc,     ReqAccRequestNone,              KErrNone, 2000000},
    { ETFFindAndCheckRequest,                   0, 0,        0,     0,                      0, 11201, ETFAsyNone,     ReqNotifyAccessoryDisconnected, KErrNone, 0},
    { ETFAccessorySingleConnection_Close,       0, 0,        0,     0,                      0, 11201, ETFAsyNone,     ReqAccRequestNone,              KErrNone, 0},
    };

//
// [M11207] AccServer [ Cancel accessory object value changed notification request ]
//
const TTFAccessoryTestCaseStateControl KTFCancelNotifyAccessoryValueChangedObjectStates[]=
    {
    { ETFAccessorySingleConnection_Open,          0, 0,        0, 0,              0, 11207, ETFAsyNone,    ReqAccRequestNone,              KErrNone, 0},
    { ETFAccessoryControl_Open,                   0, 0,        0, 0,              0, 11207, ETFAsyNone,    ReqAccRequestNone,              KErrNone, 0},
    { ETFAccessoryNotifyNewAccessoryConnected,    0, 0,        0, 0,              0, 11207, ETFAsyNone,    ReqAccRequestNone,              KErrNone, 1000000},
    { ETFConnectWiredAccessory,                   0, 0x010602, 0, 0,              0, 11207, ETFAsyConnUSB, ReqAccRequestNone,              KErrNone, 2000000},
    { ETFFindAndCheckRequest,                     0, 0,        0, 0,              0, 11207, ETFAsyNone,    ReqNotifyNewAccessoryConnected, KErrNone, 0},
    { ETFNotifyObjectAccessoryValueChanged,       0, 0,        0, KAccAudioUnits, 0, 11207, ETFAsyNone,    ReqAccRequestNone,              KErrNone, 2000000},
    { ETFCancelNotifyAccessoryValueChanged,       0, 0,        0, KAccAudioUnits, 0, 11207, ETFAsyNone,    ReqAccRequestNone,              KErrNone, 1000000},         
    { ETFDisconnectWiredAccessory,                0, 0,        0, 0,              0, 11207, ETFAsyDisc,    ReqAccRequestNone,              KErrNone, 2000000},
    { ETFClearStack,                              0, 0,        0, 0,              0, 11207, ETFAsyNone,    ReqAccRequestNone,              KErrNone, 0},
    { ETFAccessorySingleConnection_Close,         0, 0,        0, 0,              0, 11207, ETFAsyNone,    ReqAccRequestNone,              KErrNone, 0},
    { ETFAccessoryControl_Close,                  0, 0,        0, 0,              0, 11207, ETFAsyNone,    ReqAccRequestNone,              KErrNone, 0}
    };

//
// [M11208] AccServer [ Accessory object value changed notification request ]
//
const TTFAccessoryTestCaseStateControl KTFNotifyAccessoryValueChangedObjectStates[]=
    {
    { ETFAccessorySingleConnection_Open,          0, 0,        0,     0,              0, 11208, ETFAsyNone,         ReqAccRequestNone,                    KErrNone, 0},
    { ETFAccessoryControl_Open,                   0, 0,        0,     0,              0, 11208, ETFAsyNone,         ReqAccRequestNone,                    KErrNone, 0},
    { ETFAccessoryNotifyNewAccessoryConnected,    0, 0,        0,     0,              0, 11208, ETFAsyNone,         ReqAccRequestNone,                    KErrNone, 1000000},
    { ETFConnectWiredAccessory,                   0, 0x010602, 0,     0,              0, 11208, ETFAsyConnUSB,      ReqAccRequestNone,                    KErrNone, 2000000},
    { ETFFindAndCheckRequest,                     0, 0,        0,     0,              0, 11208, ETFAsyNone,         ReqNotifyNewAccessoryConnected,       KErrNone, 0},
    { ETFNotifyObjectAccessoryValueChanged,       0, 0,        0,     KAccAudioUnits, 0, 11208, ETFAsyNone,         ReqAccRequestNone,                    KErrNone, 2000000},
    { ETFAccessorySet_TopologyObjectsPubSub,      0, 0,        ETrue, KAccAudioUnits, 0, 11208, ETFAsyObjectNotify, ReqAccRequestNone,                    KErrNone, 0},         
    { ETFFindAndCheckObjectRequest,               0, 0,        0,     KAccAudioUnits, 0, 11208, ETFAsyNone,         ReqNotifyObjectAccessoryValueChanged, KErrNone, 0},
    { ETFDisconnectWiredAccessory,                0, 0,        0,     0,              0, 11208, ETFAsyDisc,         ReqAccRequestNone,                    KErrNone, 2000000},
    { ETFClearStack,                              0, 0,        0,     0,              0, 11208, ETFAsyNone,         ReqAccRequestNone,                    KErrNone, 0},
    { ETFAccessorySingleConnection_Close,         0, 0,        0,     0,              0, 11208, ETFAsyNone,         ReqAccRequestNone,                    KErrNone, 0},
    { ETFAccessoryControl_Close,                  0, 0,        0,     0,              0, 11208, ETFAsyNone,         ReqAccRequestNone,                    KErrNone, 0}
    };
   

//
// [M11209] AccServer [ Accessory object value changed notification request and cancel ]
//
const TTFAccessoryTestCaseStateControl KTFNotifyAccessoryValueChangedObjectAndCancelStates[]=
    {
    { ETFAccessorySingleConnection_Open,          0, 0,        0,     0,              0, 11209, ETFAsyNone,         ReqAccRequestNone,                    KErrNone, 0},
    { ETFAccessoryControl_Open,                   0, 0,        0,     0,              0, 11209, ETFAsyNone,         ReqAccRequestNone,                    KErrNone, 0},
    { ETFAccessoryNotifyNewAccessoryConnected,    0, 0,        0,     0,              0, 11209, ETFAsyNone,         ReqAccRequestNone,                    KErrNone, 1000000},
    { ETFConnectWiredAccessory,                   0, 0x010602, 0,     0,              0, 11209, ETFAsyConnUSB,      ReqAccRequestNone,                    KErrNone, 2000000},
    { ETFFindAndCheckRequest,                     0, 0,        0,     0,              0, 11209, ETFAsyNone,         ReqNotifyNewAccessoryConnected,       KErrNone, 0},
    { ETFNotifyObjectAccessoryValueChanged,       0, 0,        0,     KAccAudioUnits, 0, 11209, ETFAsyNone,         ReqAccRequestNone,                    KErrNone, 2000000},
    { ETFAccessorySet_TopologyObjectsPubSub,      0, 0,        ETrue, KAccAudioUnits, 0, 11209, ETFAsyObjectNotify, ReqAccRequestNone,                    KErrNone, 0},         
    { ETFFindAndCheckObjectRequest,               0, 0,        0,     KAccAudioUnits, 0, 11209, ETFAsyNone,         ReqNotifyObjectAccessoryValueChanged, KErrNone, 0},
    { ETFNotifyObjectAccessoryValueChanged,       0, 0,        0,     KAccAudioUnits, 0, 11209, ETFAsyNone,         ReqAccRequestNone,              KErrNone, 2000000},
    { ETFCancelNotifyAccessoryValueChanged,       0, 0,        0,     KAccAudioUnits, 0, 11209, ETFAsyNone,         ReqAccRequestNone,              KErrNone, 1000000},         
    { ETFDisconnectWiredAccessory,                0, 0,        0,     0,              0, 11209, ETFAsyDisc,         ReqAccRequestNone,                    KErrNone, 2000000},
    { ETFClearStack,                              0, 0,        0,     0,              0, 11209, ETFAsyNone,         ReqAccRequestNone,                    KErrNone, 0},
    { ETFAccessorySingleConnection_Close,         0, 0,        0,     0,              0, 11209, ETFAsyNone,         ReqAccRequestNone,                    KErrNone, 0},
    { ETFAccessoryControl_Close,                  0, 0,        0,     0,              0, 11209, ETFAsyNone,         ReqAccRequestNone,                    KErrNone, 0}
    };
   

/************************                                             ****************************/
/************************  CAccPolObjectBase related testcases (HDMI) ****************************/
/************************                                             ****************************/

//
// [M11301] AccServer [ Connect HDMI device ]
//
const TTFAccessoryTestCaseStateControl KTFConnectHDMIDeviceStates[]=
    {
    { ETFAccessorySingleConnection_Open,        0, 0, 0,     0,                       0, 11301, ETFAsyNone,     ReqAccRequestNone,              KErrNone, 0},
    { ETFAccessoryNotifyNewAccessoryConnected,  0, 0, 0,     0,                       0, 11301, ETFAsyNone,     ReqAccRequestNone,              KErrNone, 1000000},
    { ETFConnectWiredAccessory,                 0, 0, 0,     0,                       0, 11301, ETFAsyConnHDMI, ReqAccRequestNone,              KErrNone, 2000000},
    { ETFFindAndCheckRequest,                   0, 0, 0,     0,                       0, 11301, ETFAsyNone,     ReqNotifyNewAccessoryConnected, KErrNone, 0},
    { ETFCheckCapability,                       0, 0, ETrue, KAccVideoOut,            0, 11301, ETFAsyNone,     ReqAccRequestNone,              KErrNone, 0},
    { ETFCheckCapability,                       0, 0, ETrue, KAccVideoHdmiAttributes, 0, 11301, ETFAsyNone,     ReqAccRequestNone,              KErrNone, 0},
    { ETFCheckCapability,                       0, 0, ETrue, KAccVideoFormat,         0, 11301, ETFAsyNone,     ReqAccRequestNone,              KErrNone, 0},
    { ETFCheckCapability,                       0, 0, ETrue, KAccVideoHdcpSupport,    0, 11301, ETFAsyNone,     ReqAccRequestNone,              KErrNone, 0},
    { ETFAccessoryNotifyAccessoryDisconnected,  0, 0, 0,     0,                       0, 11301, ETFAsyNone,     ReqAccRequestNone,              KErrNone, 2000000},
    { ETFDisconnectWiredAccessory,              0, 0, 0,     0,                       0, 11301, ETFAsyDisc,     ReqAccRequestNone,              KErrNone, 2000000},
    { ETFFindAndCheckRequest,                   0, 0, 0,     0,                       0, 11301, ETFAsyNone,     ReqNotifyAccessoryDisconnected, KErrNone, 0},
    { ETFAccessorySingleConnection_Close,       0, 0, 0,     0,                       0, 11301, ETFAsyNone,     ReqAccRequestNone,              KErrNone, 0},
    };


//
// [M11302] AccServer [ Get HDMI device attributes ]
//
const TTFAccessoryTestCaseStateControl KTFGetHDMIDeviceAttributesStates[]=
    {
    { ETFAccessorySingleConnection_Open,        0, 0, 0,     0,                       0, 11302, ETFAsyNone,     ReqAccRequestNone,              KErrNone, 0},
    { ETFAccessoryNotifyNewAccessoryConnected,  0, 0, 0,     0,                       0, 11302, ETFAsyNone,     ReqAccRequestNone,              KErrNone, 1000000},
    { ETFConnectWiredAccessory,                 0, 0, 0,     0,                       0, 11302, ETFAsyConnHDMI, ReqAccRequestNone,              KErrNone, 2000000},
    { ETFFindAndCheckRequest,                   0, 0, 0,     0,                       0, 11302, ETFAsyNone,     ReqNotifyNewAccessoryConnected, KErrNone, 0},
    { ETFSetHdmiObject_PubSub,                  0, 0, 0,     0,                       0, 11302, ETFAsyNone,     ReqAccRequestNone,              KErrNone, 0},
    { ETFGetHdmiObject,                         0, 0, 0,     KAccVideoHdmiAttributes, 0, 11302, ETFAsyNone,     ReqAccRequestNone,              KErrNone, 0},
    { ETFAccessoryNotifyAccessoryDisconnected,  0, 0, 0,     0,                       0, 11302, ETFAsyNone,     ReqAccRequestNone,              KErrNone, 2000000},
    { ETFDisconnectWiredAccessory,              0, 0, 0,     0,                       0, 11302, ETFAsyDisc,     ReqAccRequestNone,              KErrNone, 2000000},
    { ETFFindAndCheckRequest,                   0, 0, 0,     0,                       0, 11302, ETFAsyNone,     ReqNotifyAccessoryDisconnected, KErrNone, 0},
    { ETFAccessorySingleConnection_Close,       0, 0, 0,     0,                       0, 11302, ETFAsyNone,     ReqAccRequestNone,              KErrNone, 0},
    };


//
// [M11303] AccServer [ Notify HDMI device attributes changed ]
//
const TTFAccessoryTestCaseStateControl KTFNotifyHDMIDeviceAttributesChangedStates[]=
    {
    { ETFAccessorySingleConnection_Open,        0, 0, 0,     0,                       0, 11303, ETFAsyNone,         ReqAccRequestNone,                    KErrNone, 0},
    { ETFAccessoryNotifyNewAccessoryConnected,  0, 0, 0,     0,                       0, 11303, ETFAsyNone,         ReqAccRequestNone,                    KErrNone, 1000000},
    { ETFConnectWiredAccessory,                 0, 0, 0,     0,                       0, 11303, ETFAsyConnHDMI,     ReqAccRequestNone,                    KErrNone, 2000000},
    { ETFFindAndCheckRequest,                   0, 0, 0,     0,                       0, 11303, ETFAsyNone,         ReqNotifyNewAccessoryConnected,       KErrNone, 0},
    { ETFNotifyObjectAccessoryValueChanged,     0, 0, 0,     KAccVideoHdmiAttributes, 0, 11303, ETFAsyNone,         ReqAccRequestNone,                    KErrNone, 2000000},
    { ETFSetHdmiObject_PubSub,                  0, 0, ETrue, KAccVideoHdmiAttributes, 0, 11303, ETFAsyObjectNotify, ReqAccRequestNone,                    KErrNone, 0},
    { ETFFindAndCheckObjectRequest,             0, 0, 0,     KAccVideoHdmiAttributes, 0, 11303, ETFAsyNone,         ReqNotifyObjectAccessoryValueChanged, KErrNone, 0},
    { ETFAccessoryNotifyAccessoryDisconnected,  0, 0, 0,     0,                       0, 11303, ETFAsyNone,         ReqAccRequestNone,                    KErrNone, 2000000},
    { ETFDisconnectWiredAccessory,              0, 0, 0,     0,                       0, 11303, ETFAsyDisc,         ReqAccRequestNone,                    KErrNone, 2000000},
    { ETFFindAndCheckRequest,                   0, 0, 0,     0,                       0, 11303, ETFAsyNone,         ReqNotifyAccessoryDisconnected,       KErrNone, 0},
    { ETFAccessorySingleConnection_Close,       0, 0, 0,     0,                       0, 11303, ETFAsyNone,         ReqAccRequestNone,                    KErrNone, 0},
    };


//
// [M11304] AccServer [ Get HDMI video format ]
//
const TTFAccessoryTestCaseStateControl KTFGetHDMIVideoFormatStates[]=
    {
    { ETFAccessorySingleConnection_Open,        0, 0, 0,     0,               0, 11304, ETFAsyNone,     ReqAccRequestNone,              KErrNone, 0},
    { ETFAccessoryNotifyNewAccessoryConnected,  0, 0, 0,     0,               0, 11304, ETFAsyNone,     ReqAccRequestNone,              KErrNone, 1000000},
    { ETFConnectWiredAccessory,                 0, 0, 0,     0,               0, 11304, ETFAsyConnHDMI, ReqAccRequestNone,              KErrNone, 2000000},
    { ETFFindAndCheckRequest,                   0, 0, 0,     0,               0, 11304, ETFAsyNone,     ReqNotifyNewAccessoryConnected, KErrNone, 0},
    { ETFSetHdmiObject_PubSub,                  0, 0, 0,     KAccVideoFormat, 0, 11304, ETFAsyNone,     ReqAccRequestNone,              KErrNone, 0},
    { ETFGetHdmiObject,                         0, 0, 0,     KAccVideoFormat, 0, 11304, ETFAsyNone,     ReqAccRequestNone,              KErrNone, 0},
    { ETFAccessoryNotifyAccessoryDisconnected,  0, 0, 0,     0,               0, 11304, ETFAsyNone,     ReqAccRequestNone,              KErrNone, 2000000},
    { ETFDisconnectWiredAccessory,              0, 0, 0,     0,               0, 11304, ETFAsyDisc,     ReqAccRequestNone,              KErrNone, 2000000},
    { ETFFindAndCheckRequest,                   0, 0, 0,     0,               0, 11304, ETFAsyNone,     ReqNotifyAccessoryDisconnected, KErrNone, 0},
    { ETFAccessorySingleConnection_Close,       0, 0, 0,     0,               0, 11304, ETFAsyNone,     ReqAccRequestNone,              KErrNone, 0},
    };


//
// [M11305] AccServer [ Notify HDMI video format changed ]
//
const TTFAccessoryTestCaseStateControl KTFNotifyHDMIVideoFormatChangedStates[]=
    {
    { ETFAccessorySingleConnection_Open,        0, 0, 0,     0,                       0, 11305, ETFAsyNone,         ReqAccRequestNone,                    KErrNone, 0},
    { ETFAccessoryNotifyNewAccessoryConnected,  0, 0, 0,     0,                       0, 11305, ETFAsyNone,         ReqAccRequestNone,                    KErrNone, 1000000},
    { ETFConnectWiredAccessory,                 0, 0, 0,     0,                       0, 11305, ETFAsyConnHDMI,     ReqAccRequestNone,                    KErrNone, 2000000},
    { ETFFindAndCheckRequest,                   0, 0, 0,     0,                       0, 11305, ETFAsyNone,         ReqNotifyNewAccessoryConnected,       KErrNone, 0},
    { ETFNotifyObjectAccessoryValueChanged,     0, 0, 0,     KAccVideoFormat,         0, 11305, ETFAsyNone,         ReqAccRequestNone,                    KErrNone, 2000000},
    { ETFSetHdmiObject_PubSub,                  0, 0, ETrue, KAccVideoFormat,         0, 11305, ETFAsyObjectNotify, ReqAccRequestNone,                    KErrNone, 0},
    { ETFFindAndCheckObjectRequest,             0, 0, 0,     KAccVideoFormat,         0, 11305, ETFAsyNone,         ReqNotifyObjectAccessoryValueChanged, KErrNone, 0},
    { ETFAccessoryNotifyAccessoryDisconnected,  0, 0, 0,     0,                       0, 11305, ETFAsyNone,         ReqAccRequestNone,                    KErrNone, 2000000},
    { ETFDisconnectWiredAccessory,              0, 0, 0,     0,                       0, 11305, ETFAsyDisc,         ReqAccRequestNone,                    KErrNone, 2000000},
    { ETFFindAndCheckRequest,                   0, 0, 0,     0,                       0, 11305, ETFAsyNone,         ReqNotifyAccessoryDisconnected,       KErrNone, 0},
    { ETFAccessorySingleConnection_Close,       0, 0, 0,     0,                       0, 11305, ETFAsyNone,         ReqAccRequestNone,                    KErrNone, 0},
    };


//
// [M11306] AccServer [ Test HDMI objects ]
//
const TTFAccessoryTestCaseStateControl KTCheckHdmiObjectsStates[]=
    {
    { ETFCheckHdmiObject, 0, EAccPolHdmiObjectCon,               0, 0, 0, 11306, ETFAsyNone, ReqAccRequestNone, KErrNone, 0},
    { ETFCheckHdmiObject, 0, EAccPolHdmiSinkObject,              0, 0, 0, 11306, ETFAsyNone, ReqAccRequestNone, KErrNone, 0},
    { ETFCheckHdmiObject, 0, EAccPolHdmiAudioFormatObject,       0, 0, 0, 11306, ETFAsyNone, ReqAccRequestNone, KErrNone, 0},
    { ETFCheckHdmiObject, 0, EAccPolHdmiLatencyObject,           0, 0, 0, 11306, ETFAsyNone, ReqAccRequestNone, KErrNone, 0},
    { ETFCheckHdmiObject, 0, EAccPolHdmiSpeakerAllocationObject, 0, 0, 0, 11306, ETFAsyNone, ReqAccRequestNone, KErrNone, 0},
    { ETFCheckHdmiObject, 0, EAccPolHdmiVideoFormatObject,       0, 0, 0, 11306, ETFAsyNone, ReqAccRequestNone, KErrNone, 0},
    };


/*****************************             *********************************/
/*****************************  End Tests  *********************************/
/*****************************             *********************************/


CTFAccessoryTestControlPlugin::CTFAccessoryTestControlPlugin( TAny* aInitParams )
: iInitParams( ( CTFStubModuleInterface::TInterfaceInitParams* ) aInitParams )
    {
    }

// Two-phased constructor.
CTFAccessoryTestControlPlugin* CTFAccessoryTestControlPlugin::NewL( TAny* aInitParams )
    {
    return new ( ELeave ) CTFAccessoryTestControlPlugin( aInitParams );
    }

// Destructor.
CTFAccessoryTestControlPlugin::~CTFAccessoryTestControlPlugin()
    {
    iInitParams = NULL;
    }

CTFStub* CTFAccessoryTestControlPlugin::GetStubL( void )
    {

    return new ( ELeave ) CTFAccessoryTestControl();
    }

void CTFAccessoryTestControlPlugin::BuildTestSuiteL( CTFATestSuite* aRootSuite )
    {
    // Memory checks are disabled since Accessory Server runs in separate thread and heap.
    aRootSuite->Environment().SetTestCaseFlags( ETFMayLeakMemory | ETFMayLeakRequests | ETFMayLeakHandles );

    TF_START_TEST_DECLARATION_L( aRootSuite );
        // Use case test cases
        TF_ADD_TEST_SUITE_L( _L( "Module Tests" ) );
            TF_ADD_TEST_SUITE_FUNCTION_L( _L( "Session Tests" ),                    AccessorySessionsModuleTestSuiteL );
            TF_ADD_TEST_SUITE_FUNCTION_L( _L( "Wired Accessory Module Tests" ),     WiredAccessoryModuleTestSuiteL );
            TF_ADD_TEST_SUITE_FUNCTION_L( _L( "Wireless Accessory Module Tests" ),  WirelessAccessoryModuleTestSuiteL );
            TF_ADD_TEST_SUITE_FUNCTION_L( _L( "Bt And Audio Link Module Tests" ),   BtAndAudioLinkModuleTestSuiteL );
            TF_ADD_TEST_SUITE_FUNCTION_L( _L( "Accessory Mode Module Tests" ),      AccessoryModeModuleTestSuiteL );
            TF_ADD_TEST_SUITE_FUNCTION_L( _L( "Special Module Tests" ),             SpecialModuleTestSuiteL );
            TF_ADD_TEST_SUITE_FUNCTION_L( _L( "Accessory Settings Tests" ),         AccessorySettingsModuleTestSuiteL );
            TF_ADD_TEST_SUITE_FUNCTION_L( _L( "Multible Connections" ),             MultibleConnectionsModuleTestSuiteL );
            TF_ADD_TEST_SUITE_FUNCTION_L( _L( "Signature Connection Tests" ),       SignatureModuleTestSuiteL );
            TF_ADD_TEST_SUITE_FUNCTION_L( _L( "Policy Tests" ),                     PolicyModuleTestSuiteL );
            TF_ADD_TEST_SUITE_FUNCTION_L( _L( "USB Object Tests" ),                 UsbObjectModuleTestSuiteL );
            TF_ADD_TEST_SUITE_FUNCTION_L( _L( "HDMI Object Tests" ),                HdmiObjectModuleTestSuiteL );
        TF_END_TEST_SUITE();
    TF_END_TEST_DECLARATION();
    }

MTFStubTestCase* CTFAccessoryTestControlPlugin::GetStubTestCaseL( TInt aTestCaseId )
    {
    TF_START_STUB_TEST_CASES( aTestCaseId );

/***************************** Open And Close Services *********************************/

    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 10101, KTFAccessoryConnectionOpenCloseStates ) ;
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 10102, KTFAccessorySingleConnectionOpenCloseStates ) ;
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 10103, KTFAccessoryControlOpenCloseStates ) ;
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 10104, KTFAccessorySettingOpenCloseStates ) ;
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 10105, KTFAccessoryModeOpenCloseStates ) ;
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 10106, KTFAccessoryAudioControlOpenCloseStates ) ;
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 10107, KTFAccessoryBTControlOpenCloseStates ) ;
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 10108, KTFAccessoryAsyncsCalledTwice );
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 10109, KTFCancelValueChanged );

/***************************** Wired Accessory Tests *********************************/

    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 10201, KTFWiredHeadsetConnectStates ) ;
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 10202, KTFWiredHeadsetDisconnectStates ) ;
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 10203, KTFNotifyNewAccessoryConnectionState ) ;
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 10204, KTFNotifyAccessoryDisconnectedStates ) ;
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 10205, KTFNotifyAccessoryConnectionStatusChangeConnectionStates ) ;
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 10206, KTFNotifyAccessoryConnectionStatusChangeDisconnectionStates ) ;
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 10207, KTFSyncGetAccessoryConnectionStatusStates ) ;
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 10208, KTFRequestGetAccessoryConnectionStatusBeforeConnectionStates ) ;
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 10209, KTFRequestGetAccessoryConnectionStatusAfterConnectionStates ) ;
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 10210, KTFNotifyAccessoryValueChangedIntegerStates ) ;
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 10211, KTFNotifyAccessoryValueChangedBooleanStates ) ;
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 10212, KTFAccessoryValueChangedNotifyIntegerStates ) ;
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 10213, KTFAccessoryValueChangedNotifyBooleanStates ) ;
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 10214, KTFSetValueIntegerStates ) ;
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 10215, KTFSetValueBooleanStates ) ;
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 10216, KTFGetTIntStates ) ;
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 10217, KTFGetTBoolStates ) ;
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 10218, KTFGetTDes8States ) ;
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 10219, KTFConnDisConnCarkit ) ;
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 10221, KTFConnErrorFromASY ) ;

/***************************** Wireless Accessory Tests *********************************/

    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 10401, KTFBTHeadsetAndHandsfreeProfileConnectStates) ;
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 10402, KTFBTHeadsetAndHandsfreeProfileDisconnectStates) ;
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 10403, KTFBTHeadsetProfileConnectStates) ;
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 10404, KTFBTHeadsetProfileDisconnectStates) ;
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 10405, KTFBTHandsfreeProfileConnectStates) ;
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 10406, KTFBTHandsfreeProfileDisconnectStates) ;
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 10407, KTFBTEmptyProfileConnectStates) ;
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 10408, KTFBTEmptyProfileDisconnectStates) ;
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 10409, KTFBTNokiaSpesificDeviceIdConnectStates) ;
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 10410, KTFBTNokiaSpesificDeviceIdDisconnectStates) ;
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 10411, KTFBTNoResponseFromBtAsyConnectStates) ;
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 10412, KTFBTNoResponseFromBtAsyDisconnectStates) ;
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 10413, KTFBTAsyResponseWithErrorCodeConnectStates) ;
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 10414, KTFBTAsyResponseWithErrorCodeDisconnectStates) ;
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 10415, KTFBTAllServiceUUIDsConnectStates) ;
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 10416, KTFBTAllServiceUUIDsDisconnectStates) ;
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 10417, KTFBTAVRCPConnectStates) ;
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 10418, KTFBTAVRCPDisconnectStates) ;
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 10419, KTFBTA2DPConnectStates) ;
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 10420, KTFBTA2DPDisconnectStates) ;
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 10421, KTFBTAccessoryValueChangedNotifyBooleanStates ) ;
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 10422, KTFBTFailedAudiotransferFromBT ) ;
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 10423, KTFBTAccessoryValueChangedNotifyIntStates ) ;

/***************************** BT/Audio Link Tests *********************************/

    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 10501, KTFBtAndAudioLinkOpenStates) ;
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 10502, KTFBtAndAudioLinkCloseStates) ;
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 10503, KTFBtAudioTransferFromBTtoPhoneStates) ;
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 10504, KTFBtAudioLinkCancels) ;

/***************************** Accessory Mode Tests *********************************/

    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 10601, KTFHandPortableAccessoryModeStates) ;
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 10602, KTFWiredHeadsetAccessoryModeStates) ;
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 10603, KTFWirelessHeadesetAccessoryModeStates) ;
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 10604, KTFWiredCarkitAccessoryModeStates) ;
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 10605, KTFWirelessCarKitAccessoryModeStatus) ;
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 10606, KTFTextDeviceAccessoryModeStatus) ;
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 10607, KTFLoopsetAccessoryModeStates) ;
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 10608, KTFMusicStandAccessoryModeStates) ;
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 10609, KTFSyncGetAccessoryModeStates) ;
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 10610, KTFASyncGetAccessoryModeStates) ;

/***************************** Special Tests *********************************/

    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 10701, KTFMD1PowerSupplyOnAfterConnectionState) ;
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 10702, KTFMD1PowerSupplyOnOffState) ;
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 10703, KTFHS20AudioOutConnectorOnOffState) ;
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 10704, KTFUnsupportedWiredAccessoryStates) ;
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 10705, KTFTransactionMatchStates );
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 10706, KTFFastModeNotificationChange );
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 10707, KTFNotSupportedNote );
     
/***************************** Accessory Settings *********************************/
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 10801, KTFSetLoopsetModeStates) ;
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 10802, KTFGetLoopsetModeStates) ;
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 10803, KTFSetAndGetLoopsetModeStates) ;
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 10804, KTFSetTTYModeStates) ;
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 10805, KTFGetTTYModeStates) ;
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 10806, KTFSetAndGetTTYModeStates) ;
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 10807, KTFSetLoopsetModeAfterConnectionStates) ;
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 10808, KTFSetTTYModeAfterConnectionStates) ;
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 10809, KTFSetLoopsetModeBeforeConnectionStates) ;
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 10810, KTFSetTTYModeBeforeConnectionStates) ;
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 10811, KTFGetSupportedHWDevicesStates) ;
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 10812, KTFAccessoryModeSettingsTIntStates) ;
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 10813, KTFAccessoryModeSettingsTBoolStates) ;
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 10814, KTFAccessoryModeSettingsTDesStates) ;

#ifdef __WINS__
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 10815, KTFAccessoryModeSettingsArrayStates) ;
#endif

    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 10816, KTFSetAndGetWirelessAccessoryModeSettingsStates) ;
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 10817, KTFSetAndGetWiredCarkitAccessoryModeSettingsStates) ;
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 10818, KTFSetAndGetWirelessCarkitAccessoryModeSettingsStates) ;
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 10819, KTFSetAndGetTextDeviceAccessoryModeSettingsStates) ;
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 10820, KTFSetAndGetLoopsetAccessoryModeSettingsStates) ;
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 10821, KTFSetAndGetMusicStandAccessoryModeSettingsStates) ;
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 10822, KTFAccessoryModeSettingsGsSettingsState ) ;
    
/***************************** Multible connections *********************************/

    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 10901, KTFConnectWiredAndBTArrayStates );
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 10902, KTFDisconnectWiredAndBTArrayStates );
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 10903, KTFConnectBTAndWiredArrayStates );
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 10904, KTFDisconnectBTAndWiredArrayStates );
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 10905, KTFConnectBTAndBTArrayStates );
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 10906, KTFDisconnectBTAndBTArrayStates );

/***************************** Signature testcases *********************************/

    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 11001, KTFTestSelectionDialog );
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 11002, KTFTestSelectionDialogOpenCancel );
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 11003, KTFTestConnectOpenCableUserSelectTVOUT );
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 11004, KTFTestConnectOpenCableAsySelectTVOUT );
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 11005, KTFTestConnectOpenCableUserSelectionAsySelectsTVOUT );
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 11006, KTFTestConnectOpenCableOpenSelectionDialogDisconnect );
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 11007, KTFTestConnectOpenCableASYConnectHeadsetUserSelectHeadset );
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 11008, KTFTestConnectECIAccessory );    
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 11009, KTFTestConnectOpenCableDefaultSelection );
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 11010, KTFTestConnectHeadset );
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 11011, KTFTestConnectHeadphones );
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 11012, KTFTestConnectOpenCable );
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 11013, KTFTestConnectTVOUT );
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 11014, KTFTestDisconnectWired );
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 11015, KTFTestConnectAndDisconnectHeadphones );
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 10611, KTFCheckConnectedAccessory );

/***************************** Policy testcases *********************************/

    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 11101, KTFTestValueRecord ); 
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 11103, KTFTestObjectBaseSerialization );
        
/************************  CAccPolObjectBase related testcases (USB) ****************************/

    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 11201, KTFConnectUSBheadsetStates );
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 11202, KTFGetSetUSBheadsetValue );
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 11203, KTFUSBheadsetAudioControls );
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 11204, KTFSetGetUSBvolumeControl );
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 11205, KTFSetGetUSBmuteControl );
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 11206, KTFConnectUSB2headsetStates );    
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 11207, KTFCancelNotifyAccessoryValueChangedObjectStates );    
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 11208, KTFNotifyAccessoryValueChangedObjectStates );
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 11209, KTFNotifyAccessoryValueChangedObjectAndCancelStates );
    
/************************  CAccPolObjectBase related testcases (HDMI) ****************************/

    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 11301, KTFConnectHDMIDeviceStates );
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 11302, KTFGetHDMIDeviceAttributesStates );
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 11303, KTFNotifyHDMIDeviceAttributesChangedStates );
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 11304, KTFGetHDMIVideoFormatStates );
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 11305, KTFNotifyHDMIVideoFormatChangedStates );
    TF_ACCESSORY_SERVER_CONTROL_TEST_CASE_L( 11306, KTCheckHdmiObjectsStates );
    
    TF_END_STUB_TEST_CASES();
    }

static void AccessorySessionsModuleTestSuiteL( CTFATestSuite* aSuite )
{
    TF_INIT_TEST_SUITE_FUNCTION( aSuite );

    TF_ADD_TEST_CASE_L( 10101, _L( "[M10101] [[AccServer] Open And Close Accessory Connection]") );
    TF_ADD_TEST_CASE_L( 10102, _L( "[M10102] [[AccServer] Open And Close Accessory Single Connection]" ) );
    TF_ADD_TEST_CASE_L( 10103, _L( "[M10103] [[AccServer] Open And Close Accessory Control]") );
    TF_ADD_TEST_CASE_L( 10104, _L( "[M10104] [[AccServer] Open And Close Accessory Settings]") );
    TF_ADD_TEST_CASE_L( 10105, _L( "[M10105] [[AccServer] Open And Close Accessory Mode]") );
    TF_ADD_TEST_CASE_L( 10106, _L( "[M10106] [[AccServer] Open And Close Accessory Audio Control]") );
    TF_ADD_TEST_CASE_L( 10107, _L( "[M10107] [[AccServer] Open And Close Accessory BT Control]") );
    TF_ADD_TEST_CASE_L( 10108, _L( "[M10108] [[AccServer] All Async method called twice]") ); TF_SET_TEST_TIMEOUT( 200 );
    TF_ADD_TEST_CASE_L( 10109, _L( "[M10109] [[AccServer] Cancel NotifyAccessoryValueChanged]") );
}

static void WiredAccessoryModuleTestSuiteL( CTFATestSuite* aSuite )
{
    TF_INIT_TEST_SUITE_FUNCTION( aSuite );

    TF_ADD_TEST_CASE_L( 10201, _L( "[M10201] [[AccServer] Connect Accessory]") );
    TF_ADD_TEST_CASE_L( 10202, _L( "[M10202] [[AccServer] Disconnect Accessory]") );
    TF_ADD_TEST_CASE_L( 10203, _L( "[M10203] [[AccServer] Notify New Accessory Connected]") );
    TF_ADD_TEST_CASE_L( 10204, _L( "[M10204] [[AccServer] Notify New Accessory Disconnected]") );
    TF_ADD_TEST_CASE_L( 10205, _L( "[M10205] [[AccServer] Notify Accessory Connection Status Change (Connection)]") );
    TF_ADD_TEST_CASE_L( 10206, _L( "[M10206] [[AccServer] Notify Accessory Connection Status Change (Disconnection)]") );
    TF_ADD_TEST_CASE_L( 10207, _L( "[M10207] [[AccServer] Sync Get Accessory Connection Status]" ) );
    TF_ADD_TEST_CASE_L( 10208, _L( "[M10208] [[AccServer] Request Get Accessory Connection Status Before Connection]" ) );
    TF_ADD_TEST_CASE_L( 10209, _L( "[M10209] [[AccServer] Request Get Accessory Connection Status After Connection]" ) );
    TF_ADD_TEST_CASE_L( 10210, _L( "[M10210] [[AccServer] Notify Accessory Value Changed (TInt)]" ) );
    TF_ADD_TEST_CASE_L( 10211, _L( "[M10211] [[AccServer] Notify Accessory Value Changed (TBool)]" ) );
    TF_ADD_TEST_CASE_L( 10212, _L( "[M10212] [[AccServer] Accessory Value Changed Notify (TInt)]" ) );
    TF_ADD_TEST_CASE_L( 10213, _L( "[M10213] [[AccServer] Accessory Value Changed Notify (TBool)]" ) );
    TF_ADD_TEST_CASE_L( 10214, _L( "[M10214] [[AccServer] Set Value (TInt)]" ) );
    TF_ADD_TEST_CASE_L( 10215, _L( "[M10215] [[AccServer] Set Value (TBool)]" ) );
    TF_ADD_TEST_CASE_L( 10216, _L( "[M10216] [[AccServer] Get Value (TInt)]" ) );
    TF_ADD_TEST_CASE_L( 10217, _L( "[M10217] [[AccServer] Get Value (TBool)]" ) );
    TF_ADD_TEST_CASE_L( 10218, _L( "[M10218] [[AccServer] Get Value fails (TDes8)]" ) );
    TF_ADD_TEST_CASE_L( 10219, _L( "[M10219] [[AccServer] Connect and Disconnect Carkit (CK-10)]" ) ); 
    TF_ADD_TEST_CASE_L( 10221, _L( "[M10221] [[AccServer] Connect Accessory, Error response from ASY" ) );
}

static void WirelessAccessoryModuleTestSuiteL( CTFATestSuite* aSuite )
{
    TF_INIT_TEST_SUITE_FUNCTION( aSuite );

    TF_ADD_TEST_CASE_L( 10401, _L( "[M10401] [[AccServer] Connect BT with Headset And Handsfree Profile]"));
    TF_ADD_TEST_CASE_L( 10402, _L( "[M10402] [[AccServer] Disconnect BT with Headset And Handsfree Profile]"));
    TF_ADD_TEST_CASE_L( 10403, _L( "[M10403] [[AccServer] Connect BT with Headset Profile]"));
    TF_ADD_TEST_CASE_L( 10404, _L( "[M10404] [[AccServer] Disconnect BT with Headset Profile]"));
    TF_ADD_TEST_CASE_L( 10405, _L( "[M10405] [[AccServer] Connect BT with Handsfree Profile]"));
    TF_ADD_TEST_CASE_L( 10406, _L( "[M10406] [[AccServer] Disconnect BT with Handsfree Profile]"));
    TF_ADD_TEST_CASE_L( 10407, _L( "[M10407] [[AccServer] Connect BT with Empty Profile]"));
    TF_ADD_TEST_CASE_L( 10408, _L( "[M10408] [[AccServer] Disconnect BT with Empty Profile]"));
    TF_ADD_TEST_CASE_L( 10409, _L( "[M10409] [[AccServer] Connect BT with Nokia Spesific Device Id]"));
    TF_ADD_TEST_CASE_L( 10410, _L( "[M10410] [[AccServer] Disconnect BT with Nokia Spesific Device Id]"));
    TF_ADD_TEST_CASE_L( 10411, _L( "[M10411] [[AccServer] Connect BT, No Response From BT ASY]"));
    TF_ADD_TEST_CASE_L( 10412, _L( "[M10412] [[AccServer] Disconnectect BT, No Response From BT ASY]"));
    TF_ADD_TEST_CASE_L( 10413, _L( "[M10413] [[AccServer] Connect BT, Response with Error Code]"));
    TF_ADD_TEST_CASE_L( 10414, _L( "[M10414] [[AccServer] Disconnect BT, Response with Error Code]"));
    TF_ADD_TEST_CASE_L( 10415, _L( "[M10415] [[AccServer] Connect BT, All Service UUIDs]"));
    TF_ADD_TEST_CASE_L( 10416, _L( "[M10416] [[AccServer] Disconnect BT, All Service UUIDs]"));
    TF_ADD_TEST_CASE_L( 10417, _L( "[M10417] [[AccServer] BT Connect, BT with AVRCP profile]"));
    TF_ADD_TEST_CASE_L( 10418, _L( "[M10418] [[AccServer] BT Disconnect, BT with AVRCP profile]"));
    TF_ADD_TEST_CASE_L( 10419, _L( "[M10419] [[AccServer] BT Connect, BT with A2DP profile]"));
    TF_ADD_TEST_CASE_L( 10420, _L( "[M10420] [[AccServer] BT Disconnect, BT with A2DP profile]"));
    TF_ADD_TEST_CASE_L( 10421, _L( "[M10421] [[AccServer] BT Accessory Value Changed Notify (TBool)]" ) ); TF_SET_TEST_TIMEOUT( 200 );
    TF_ADD_TEST_CASE_L( 10422, _L( "[M10422] [[AccServer] Audio is routed to wired accessory and BT makes audio transfer]" ) ); TF_SET_TEST_TIMEOUT( 200 );    
    TF_ADD_TEST_CASE_L( 10423, _L( "[M10423] [[AccServer] BT Accessory Value Changed Notify (TInt)]" ) ); TF_SET_TEST_TIMEOUT( 200 );
}

static void BtAndAudioLinkModuleTestSuiteL(CTFATestSuite* aSuite )
{
    TF_INIT_TEST_SUITE_FUNCTION( aSuite );

    TF_ADD_TEST_CASE_L( 10501, _L( "[M10501] [[AccServer] BT And Audio Link Open] ")); TF_SET_TEST_TIMEOUT( 200 );
    TF_ADD_TEST_CASE_L( 10502, _L( "[M10502] [[AccServer] BT And Audio Link Close]")); TF_SET_TEST_TIMEOUT( 200 );
    TF_ADD_TEST_CASE_L( 10503, _L( "[M10503] [[AccServer] Accessory initiated BT Audio Transfer to Phone from BT Accessory]")); TF_SET_TEST_TIMEOUT( 200 );
    TF_ADD_TEST_CASE_L( 10504, _L( "[M10504] [[AccServer] AudioLink Open and Close cancels]")); TF_SET_TEST_TIMEOUT( 200 );    
}

static void AccessoryModeModuleTestSuiteL( CTFATestSuite* aSuite )
{

    TF_INIT_TEST_SUITE_FUNCTION( aSuite );

    TF_ADD_TEST_CASE_L( 10601, _L( "[M10601] [[AccServer] Notify Accessory Mode Changed to HandPortable]") );
    TF_ADD_TEST_CASE_L( 10602, _L( "[M10602] [[AccServer] Notify Accessory Mode Changed to Wired Headset]") ); TF_SET_TEST_TIMEOUT( 200 );
    TF_ADD_TEST_CASE_L( 10603, _L( "[M10603] [[AccServer] Notify Accessory Mode Changed to WirelessHeadset]") );
    TF_ADD_TEST_CASE_L( 10604, _L( "[M10604] [[AccServer] Notify Accessory Mode Changed to Wired CarKit]") );
    TF_ADD_TEST_CASE_L( 10605, _L( "[M10605] [[AccServer] Notify Accessory Mode Changed to Wireless CarKit]") );
    TF_ADD_TEST_CASE_L( 10607, _L( "[M10607] [[AccServer] Notify Accessory Mode Changed to Loopset]") );
    TF_ADD_TEST_CASE_L( 10608, _L( "[M10608] [[AccServer] Notify Accessory Mode Changed to Music Stand]") );
    TF_ADD_TEST_CASE_L( 10609, _L( "[M10609] [[AccServer] Sync Get Accessory Mode]") );
    TF_ADD_TEST_CASE_L( 10610, _L( "[M10610] [[AccServer] ASync Get Accessory Mode]") );
    TF_ADD_TEST_CASE_L( 10611, _L( "[M10611] [[AccServer] Check Connected Accessory]]"));
}

static void SpecialModuleTestSuiteL(CTFATestSuite* aSuite )
{
    TF_INIT_TEST_SUITE_FUNCTION( aSuite );

    TF_ADD_TEST_CASE_L( 10701, _L( "[M10701] [[AccServer] MD-1, PowerSupply On After Connection] "));
    TF_ADD_TEST_CASE_L( 10702, _L( "[M10702] [[AccServer] MD-1 Power Supply On/Off] ")); TF_SET_TEST_TIMEOUT( 200 );
    TF_ADD_TEST_CASE_L( 10703, _L( "[M10703] [[AccServer] HS-20 Audio Out Connector On/Off] ")); TF_SET_TEST_TIMEOUT( 200 );
    TF_ADD_TEST_CASE_L( 10704, _L( "[M10704] [[AccServer] Connect Unsupported Accessory] "));
    TF_ADD_TEST_CASE_L( 10705, _L( "[M10705] [[AccServer] Transaction matching for ASY command] "));
    TF_ADD_TEST_CASE_L( 10706, _L( "[M10706] [[AccServer] Fast Mode Notification Change] "));
    TF_ADD_TEST_CASE_L( 10707, _L( "[M10707] [[AccServer] Not supported note "));
}

static void AccessorySettingsModuleTestSuiteL( CTFATestSuite* aSuite )
{
    TF_INIT_TEST_SUITE_FUNCTION( aSuite );

    TF_ADD_TEST_CASE_L( 10801, _L( "[M10801] [[AccServer] Set Loopset Mode] ") );
    TF_ADD_TEST_CASE_L( 10802, _L( "[M10802] [[AccServer] Get Loopset Mode] ") );
    TF_ADD_TEST_CASE_L( 10803, _L( "[M10803] [[AccServer] Set And Get Loopset Mode] ") );
    TF_ADD_TEST_CASE_L( 10804, _L( "[M10804] [[AccServer] Set TTY Mode] ") );
    TF_ADD_TEST_CASE_L( 10805, _L( "[M10805] [[AccServer] Get TTY Mode] ") );
    TF_ADD_TEST_CASE_L( 10806, _L( "[M10806] [[AccServer] Set And Get TTY Mode] ") );
    TF_ADD_TEST_CASE_L( 10812, _L( "[M10812] [[AccServer] Accessory Mode Settings (TInt)] ") );
    TF_ADD_TEST_CASE_L( 10813, _L( "[M10813] [[AccServer] Accessory Mode Settings (TBool)] ") );
    TF_ADD_TEST_CASE_L( 10814, _L( "[M10814] [[AccServer] Accessory Mode Settings (TDes)] ") );
#ifdef __WINS__    
    TF_ADD_TEST_CASE_L( 10815, _L( "[M10815] [[AccServer] Accessory Mode Settings (Array)] ") );
#endif    
    TF_ADD_TEST_CASE_L( 10816, _L( "[M10816] [[AccServer] Set And Get Wireless Accessory Mode Setting]") );
    TF_ADD_TEST_CASE_L( 10817, _L( "[M10817] [[AccServer] Set And Get Wired Carkit Accessory Mode Setting]") );
    TF_ADD_TEST_CASE_L( 10818, _L( "[M10818] [[AccServer] Set And Get Wireless Carkit Accessory Mode Setting]") );
    TF_ADD_TEST_CASE_L( 10819, _L( "[M10819] [[AccServer] Set And Get Text Device Accessory Mode Setting]") );
    TF_ADD_TEST_CASE_L( 10820, _L( "[M10820] [[AccServer] Set And Get Loopset Accessory Mode Setting]") );
    TF_ADD_TEST_CASE_L( 10821, _L( "[M10821] [[AccServer] Set And Get Music Stand Accessory Mode Setting]") );    
    TF_ADD_TEST_CASE_L( 10822, _L( "[M10822] [[AccServer] Accessory Mode Settings (EAccLightsAutomatic / EAccLightsOn)]") );    
}

static void MultibleConnectionsModuleTestSuiteL( CTFATestSuite* aSuite )
{
    TF_INIT_TEST_SUITE_FUNCTION( aSuite );

    TF_ADD_TEST_CASE_L( 10901, _L( "[M10901] [[AccServer] Connect Wired And Wireless] ")); TF_SET_TEST_TIMEOUT( 200 );
    TF_ADD_TEST_CASE_L( 10902, _L( "[M10902] [[AccServer] Disconnect Wired And Wireleass] ")); TF_SET_TEST_TIMEOUT( 200 );
    TF_ADD_TEST_CASE_L( 10903, _L( "[M10903] [[AccServer] Connect Wireless And Wired] ")); TF_SET_TEST_TIMEOUT( 200 );
    TF_ADD_TEST_CASE_L( 10904, _L( "[M10904] [[AccServer] Disconnect Wireless And Wired] ")); TF_SET_TEST_TIMEOUT( 200 );
    TF_ADD_TEST_CASE_L( 10905, _L( "[M10905] [[AccServer] Connect Wireless And Wireless] ")); TF_SET_TEST_TIMEOUT( 200 );
    TF_ADD_TEST_CASE_L( 10906, _L( "[M10906] [[AccServer] Disconnect Wireless And Wireless] ")); TF_SET_TEST_TIMEOUT( 200 );
}

static void SignatureModuleTestSuiteL( CTFATestSuite* aSuite )
{
    TF_INIT_TEST_SUITE_FUNCTION( aSuite );

    TF_ADD_TEST_CASE_L( 11001, _L( "[M11001] [[AccServer] Selection dialog test] "));
    TF_ADD_TEST_CASE_L( 11002, _L( "[M11002] [[AccServer] Selection dialog open/cancel] "));
    TF_ADD_TEST_CASE_L( 11003, _L( "[M11003] [[AccServer] Connect open cable, user select tv-out]"));
    TF_ADD_TEST_CASE_L( 11004, _L( "[M11004] [[AccServer] Connect open cable, ASY selects tv-out]"));
    TF_ADD_TEST_CASE_L( 11005, _L( "[M11005] [[AccServer] Connect open cable, User selection device, after that ASY selects tv-out]"));
    TF_ADD_TEST_CASE_L( 11006, _L( "[M11006] [[AccServer] Connect open cable, open selection dialog, disconnect]"));
    TF_ADD_TEST_CASE_L( 11007, _L( "[M11007] [[AccServer] Connect open cable, connect headset, user select headset]"));
    TF_ADD_TEST_CASE_L( 11008, _L( "[M11008] [[AccServer] Connect ECI accessory]"));
    TF_ADD_TEST_CASE_L( 11009, _L( "[M11009] [[AccServer] Connect open cable, default accessory headset, default is selected]"));
    TF_ADD_TEST_CASE_L( 11010, _L( "[M11010] [[AccServer] Connect Headset]]"));
    TF_ADD_TEST_CASE_L( 11011, _L( "[M11011] [[AccServer] Connect Headphones]]"));
    TF_ADD_TEST_CASE_L( 11012, _L( "[M11012] [[AccServer] Connect Open Cable]]"));
    TF_ADD_TEST_CASE_L( 11013, _L( "[M11013] [[AccServer] Connect TV Out]]"));
    TF_ADD_TEST_CASE_L( 11014, _L( "[M11014] [[AccServer] Disconnect Wired Accessory]]"));
    TF_ADD_TEST_CASE_L( 11015, _L( "[M11015] [[AccServer] Connect and disconnect Headphones]]"));
}


static void PolicyModuleTestSuiteL( CTFATestSuite* aSuite )
{
    TF_INIT_TEST_SUITE_FUNCTION( aSuite );

    TF_ADD_TEST_CASE_L( 11101, _L( "[M11101] [[AccPolicy] ValueRecord test] "));    
    //TF_ADD_TEST_CASE_L( 11102, _L( "[M11102] [[Code coverage] Cancel connects] "));
    TF_ADD_TEST_CASE_L( 11103, _L( "[M11103] [[AccPolicy] Object Base Serialization test] ")); 
}

static void UsbObjectModuleTestSuiteL( CTFATestSuite* aSuite )
{
    TF_INIT_TEST_SUITE_FUNCTION( aSuite );

    TF_ADD_TEST_CASE_L( 11201, _L( "[M11201] [[AccServer] Connect USB headset] "));
    TF_ADD_TEST_CASE_L( 11202, _L( "[M11202] [[AccServer] Get and Set USB headset audio streams format parameter] "));   
    TF_ADD_TEST_CASE_L( 11203, _L( "[M11203] [[AccServer] Get USB audio units (Audio Topology) ] "));   
    TF_ADD_TEST_CASE_L( 11204, _L( "[M11204] [[AccServer] Set/Get volume control] "));   
    TF_ADD_TEST_CASE_L( 11205, _L( "[M11205] [[AccServer] Set/Get mute control] "));   
    TF_ADD_TEST_CASE_L( 11206, _L( "[M11206] [[AccServer] Connect USB 2 headset / Policy test]"));
    TF_ADD_TEST_CASE_L( 11207, _L( "[M11207] [[AccServer] Cancel accessory object value changed notify]"));
    TF_ADD_TEST_CASE_L( 11208, _L( "[M11208] [[AccServer] Accessory object value changed notify]"));
    TF_ADD_TEST_CASE_L( 11209, _L( "[M11209] [[AccServer] Accessory object value changed notify and cancel]"));
}

static void HdmiObjectModuleTestSuiteL( CTFATestSuite* aSuite )
{
    TF_INIT_TEST_SUITE_FUNCTION( aSuite );

    TF_ADD_TEST_CASE_L( 11301, _L( "[M11301] [[AccServer] Connect HDMI device] "));
    TF_ADD_TEST_CASE_L( 11302, _L( "[M11302] [[AccServer] Get HDMI device attributes] "));
    TF_ADD_TEST_CASE_L( 11303, _L( "[M11303] [[AccServer] Notify HDMI device attributes changed] "));
    TF_ADD_TEST_CASE_L( 11304, _L( "[M11304] [[AccServer] Get HDMI video format] "));
    TF_ADD_TEST_CASE_L( 11305, _L( "[M11305] [[AccServer] Notify HDMI video format changed] "));
    TF_ADD_TEST_CASE_L( 11306, _L( "[M11306] [[AccServer] Check HDMI objects] "));
}

// End of File

