/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Consts and message definitions
*
*/


#ifndef ACCCLIENTSERVERMESSAGES_H
#define ACCCLIENTSERVERMESSAGES_H

// INCLUDES
#include <e32base.h>
#include <AsyCmdTypes.h>

// CONSTANTS
// Server version. A version must be specified when 
// creating a session with the server
const TUint KAccServMajorVersionNumber = 1;
const TUint KAccServMinorVersionNumber = 0;
const TUint KAccServBuildVersionNumber = 0;

// The number of message slots available.
// This determines the maximum number of outstanding requests.
const TUint KDefaultMessageSlots       = 12;

// Message parameter psoitions
const TUint8 KAccServParamPosFirst     = 0;
const TUint8 KAccServParamPosSecond    = 1;
const TUint8 KAccServParamPosThird     = 2;
const TUint8 KAccServHandlePos         = 3;

// The third uid which ASYProxy has
const TInt32 KAsyProxyTInt32           = 0x10205038;

//iFlags bits definitions for sub-sessions
const TInt KAccSrvNotifyNewAccessoryConnectedBit       = 1; //RAccessorySingleConnection::NotifyNewAccessoryConnected
const TInt KAccSrvNotifyAccessoryDisconnectedBit       = 2; //RAccessorySingleConnection::NotifyAccessoryDisconnected
const TInt KAccSrvNotifyAccessoryValueChangedTBoolBit  = 4; //RAccessorySingleConnection::NotifyAccessoryValueChanged TBool
const TInt KAccSrvNotifyAccessoryValueChangedTIntBit   = 8; //RAccessorySingleConnection::NotifyAccessoryValueChanged TInt
const TInt KAccSrvNotifyAccessoryValueChangedObjectBit = 10;//RAccessorySingleConnection::NotifyAccessoryValueChanged Object

const TInt KAccSrvGetAccessoryModeBit           = 1;//RAccessoryMode::GetAccessoryMode
const TInt KAccSrvNotifyAccessoryModeChangedBit = 2;//RAccessoryMode::NotifyAccessoryModeChanged

const TInt KAccSrvGetAccessoryConnectionStatusBit = 1;//RAccessoryConnection::GetAccessoryConnectionStatus
const TInt KAccSrvNotifyAccConnStatusChangedBit   = 2;//RAccessoryConnection::NotifyAccessoryConnectionStatusChanged

const TInt KAccSrvAccessoryAudioLinkOpenBit         = 1;//RAccessoryAudioControl::AccessoryAudioLinkOpen
const TInt KAccSrvAccessoryAudioLinkCloseBit        = 2;//RAccessoryAudioControl::AccessoryAudioLinkClose
const TInt KAccSrvNotifyAccessoryAudioLinkOpenedBit = 4;//RAccessoryAudioControl::NotifyAccessoryAudioLinkOpened
const TInt KAccSrvNotifyAccessoryAudioLinkClosedBit = 8;//RAccessoryAudioControl::NotifyAccessoryAudioLinkClosed

const TInt KAccSrvNotifyBluetoothAudioLinkOpenReqBit  = 1;//RAccessoryBTControl::NotifyBluetoothAudioLinkOpenReq
const TInt KAccSrvNotifyBluetoothAudioLinkCloseReqBit = 2;//RAccessoryBTControl::NotifyBluetoothAudioLinkCloseReq
const TInt KAccSrvConnectBTAccessoryBit               = 4;//RAccessoryBTControl::ConnectAccessory
const TInt KAccSrvDisconnectBTAccessoryBit            = 8;//RAccessoryBTControl::DisconnectAccessory

const TInt KAccSrvConnectAccessoryBit    = 1;//RAccessoryControl::ConnectAccessory
const TInt KAccSrvDisconnectAccessoryBit = 2;//RAccessoryControl::DisconnectAccessory

//AccessoryServers internal codes
const TInt KAccSrvGenerateGID = -2;//A new GID has been constructed by ConfigFileParser and 
                                   //AccessoryServer needs to generate unique ID for that

//Buffer size for accessory objects.
const TInt KAccSrvObjectBaseStreamBufMaxSize = 256;
const TInt KAccSrvObjectBaseStreamBufGranularity = 50;

// MACROS

// Server name
_LIT( KAccServerName,"!AccServer" );

// ASYProxy dll name
_LIT( KAsyProxyName, "AsyProxy.dll" );

//Accessory Server panic category
_LIT( KAccServerPanicCategory, "Accessory Server panic client" );

// DATA TYPES

enum TAccSrvMsgs
    {
    // Changes to TAccSrvMsgs enumeration affects also to Accessory Server's
    // policy definitions, see accessoryServerRanges

    /**
    * Accessory API
    */    
    // RAccessoryMode
    EAccSrvCreateSubSessionAccessoryMode,
    EAccSrvCloseSubSessionAccessoryMode,
    EAccSrvGetAccessoryMode,
    EAccSrvGetAccessoryModeAsync,
    EAccSrvNotifyAccessoryModeChanged,
    EAccSrvCancelNotifyAccessoryModeChanged,
    EAccSrvCancelGetAccessoryMode,  
    
    // RAccessoryConnection
    EAccSrvGetAccessoryConnectionStatusAsync,
    EAccSrvCancelGetAccessoryConnectionStatus,

    // RAccessorySingleConnection
    EAccSrvCreateSubSessionAccessorySingleConnection,
    EAccSrvCloseSubSessionAccessorySingleConnection,
    EAccSrvNotifyNewAccessoryConnected,
    EAccSrvNotifyAccessoryDisconnected,
    EAccSrvNotifyAccessoryValueChangedTBool,
    EAccSrvNotifyAccessoryValueChangedTInt,
    EAccSrvNotifyAccessoryValueChangedObject,
    EAccSrvCancelNotifyNewAccessoryConnected,
    EAccSrvCancelNotifyAccessoryDisconnected,
    EAccSrvCancelNotifyAccessoryValueChanged,

    EAccSrvGetValueTBool,
    EAccSrvGetValueTDes8,
    EAccSrvGetValueObjectBase,
    EAccSrvGetObjectValue,

    /**
    * Accessory & Accessory Monitoring API
    * These calls do not require capabilities
    */    
    // RAccessoryConnection & RAccessoryConnectionBase
    EAccSrvCreateSubSessionAccessoryConnection,
    EAccSrvCloseSubSessionAccessoryConnection,
    EAccSrvGetAccessoryConnectionStatus,
    EAccSrvNotifyAccessoryConnectionStatusChanged,
    EAccSrvCancelNotifyAccessoryConnectionStatusChanged,
    EAccSrvGetSubblockNameArray,
    EAccSrvGetValueTInt,
    
    /**
    * Accessory Control API
    */
    //RAccessoryControl
    EAccSrvCreateSubSessionAccessoryControl,
    EAccSrvCloseSubSessionAccessoryControl,
    EAccSrvConnectAccessory2,
    EAccSrvDisconnectAccessory,
    EAccSrvAccessoryValueChangedTBoolNotify,
    EAccSrvAccessoryValueChangedTIntNotify,
    EAccSrvAccessoryValueChangedObjectNotify,
    EAccSrvSetValueTBool,
    EAccSrvSetValueTInt,    
    EAccSrvSetValueObject,  
    EAccSrvCancelConnectAccessory,
    EAccSrvCancelDisconnectAccessory,

    // RAccessorySettings
    EAccSrvCreateSubSessionAccessorySettings,
    EAccSrvCloseSubSessionAccessorySettings,
    EAccSrvGetSupportedHWDevices,
    EAccSrvGetHWDeviceSettings,
    EAccSrvSetHWDeviceSettings,
    EAccSrvGetAccessoryModeSetting,
    EAccSrvSetAccessoryModeSetting,
    EAccSrvGetAccessoryModeSettings,
    EAccSrvSetAccessoryModeSettings,
    
    /**
    * Accessory BT Control API
    */
    EAccSrvCreateSubSessionAccessoryBTControl,
    EAccSrvCloseSubSessionAccessoryBTControl,
    EAccSrvBluetoothConnectAccessory,
    EAccSrvBluetoothDisconnectAccessory,
    EAccSrvBluetoothAudioLinkOpenResp,
    EAccSrvBluetoothAudioLinkCloseResp,
    EAccSrvBluetoothAudioLinkOpenedNotify,
    EAccSrvBluetoothAudioLinkClosedNotify,
    EAccSrvNotifyBluetoothAudioLinkOpenReq,
    EAccSrvNotifyBluetoothAudioLinkCloseReq,
    EAccSrvBTAccessoryValueChangedTBoolNotify,
    EAccSrvBTAccessoryValueChangedTIntNotify,
    EAccSrvCancelNotifyBluetoothAudioLinkOpenReq,
    EAccSrvCancelNotifyBluetoothAudioLinkCloseReq,

    /**
    * Accessory Audio Control API
    */
    EAccSrvCreateSubSessionAccessoryAudioControl,
    EAccSrvCloseSubSessionAccessoryAudioControl,
    EAccSrvAccessoryAudioLinkOpen,
    EAccSrvAccessoryAudioLinkClose,
    EAccSrvAudioRoutingStatusNotify,
    EAccSrvNotifyAccessoryAudioLinkOpened,
    EAccSrvNotifyAccessoryAudioLinkClosed,
    EAccSrvCancelAccessoryAudioLinkOpen,
    EAccSrvCancelAccessoryAudioLinkClose,
    EAccSrvCancelNotifyAccessoryAudioLinkOpened,
    EAccSrvCancelNotifyAccessoryAudioLinkClosed,

    /**
    * Accessory Asy Comms API
    */
    EAccSrvCreateSubSessionAccessoryAsyComms,
    EAccSrvCloseSubSessionAccessoryAsyComms,
    EAccSrvNotifyProcessCommand,
    EAccSrvCancelNotifyProcessCommand,
    EAccSrvProcessResponseTBool,
    EAccSrvProcessResponseTInt,
    EAccSrvProcessResponseTDes,
    EAccSrvGetObjectBuffer,
    EAccSrvProcessResponseObject,
        
    // Initialisation value for error tracking
    // Must be last one; used in security checks
    EAccSrvUnknown
    };

// Panic codes for some Server panics cases
enum TAccServerError
    {
    EAccServerErrorCreateServer         = 1,
    EAccServerErrorCreateTrapCleanup    = 2,
    EAccServerErrorBadDescriptor        = 3,
    EAccServerErrorNotSupported         = 4
    };

// For CAccPolAccessoryPolicy::IsValidConnection to tell connection type
enum TAccSrvConnection
    {
    EAccSrvDetect,
    EAccSrvConnect,
    EAccSrvDeny
    };

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
 * Command parameters transferred between ASY Proxy and Accessory Server
 */
class TProxyCommand 
    {

public: // data

    //Transaction id
    TInt iTrId;

    // Timeout transaction id
    TInt iTimeOutTrId;
    
    // Process command id
    TProcessCmdId iCmdId;

    // ASY command parameters
    TASYCmdParams iAsyCmdParams;
    
    };
    
typedef TPckgBuf<TProxyCommand> TProxyCommandPckgBuf;

#endif // ACCCLIENTSERVERMESSAGES_H

// End of File
