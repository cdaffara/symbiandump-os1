/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Common constants.
*
*/


#ifndef PHCLTCLIENTSERVER_H
#define PHCLTCLIENTSERVER_H



// INCLUDES
#include <e32base.h>
#include <cntdef.h>
#include <etelmm.h>
#include <data_caging_path_literals.hrh>

// CONSTANTS


// Server startup related
const TUid KPhCltServerUid = { 0x10000850 }; // from PhoneServer.mmp
_LIT( PHONE_SERVER_NAME, "Phone Server" );

_LIT( KPhServerPathAndNameWINS, "PhoneServer" );
_LIT( KPhServerPathAndNameMARM, "PhoneServer.exe" );

_LIT( KPhServerThreadName, "PhoneServerThread" );
_LIT( KPhServerThreadNameFormat, "0x%8x" );
const TInt KPhMaxServerThreadNameLength = 30;
const TUint KDefaultHeapSize = 0x10000;

const TUid KUssdSecureId = { 0x10005955 };
const TUid KSatSecureId = { 0x1000A833 };
const TUid KPhoneSecureId = { 0x100058b3 };

// Phone client panic code - when the client is panicked by the server
_LIT( KPhClientPanicCategory, "PhoneClient" );

// Resource file for USSD part.
_LIT( KPhClientAndServerResourceFileName, "PhoneServer" );
_LIT( KPhCltServerZDrive, "z:" );

_LIT( KPhClientAndServerResourceFileExtensionWild, ".R??" );
_LIT( KPhClientAndServerResourceFileExtensionNoWild, ".RSC" );
const TInt KPhSrvMagicResourceFileSignature = 0;

// PhoneClient resource file.
_LIT( KPhCltResourceFileNameAndPath, "phoneclient.rsc" );

// The max amount of encoded USSD octects.
const TInt KPhCltUssdMax7BitCharacterOctets = 
    RMobileUssdMessaging::KGsmUssdDataSize; // 160
// The max amount of characters to be encoded as USSD octects. 
const TInt KPhCltUssdMax8BitCharacters = 
    ( KPhCltUssdMax7BitCharacterOctets * 8 ) / 7; // 182


// DATA TYPES

// Server initiated Client Panics - when the client is
// mis-using the server API
enum TPhCltServerInitiatedPanic
    {
    EPhCltServerInitiatedPanicInvalidRequest = 0, // Request is invalid.
    EPhCltServerInitiatedPanicInvalidHandle,      // Handle is invalid.
    EPhCltServerInitiatedPanicBadDescriptor,      // Descriptor is invalid.
    // Session request is inappropriate.
    EPhCltServerInitiatedPanicInappropriateSessionRequest,
    // The active object or request is already active.
    EPhCltServerInitiatedPanicRequestAlreadyActive,
    // The request is not active.
    EPhCltServerInitiatedPanicRequestNotActive,

    // From ussd
    EPhCltServerInitiatedPanicNotAssigned,        // Not assigned.
    EPhCltServerInitiatedPanicNotConnected,       // Connection not established.
    // The handler is missing.
    EPhCltServerInitiatedPanicNotifyHandlerMissing,
    EPhCltServerInitiatedPanicWaitingIsUsed,      // Waiting is used.
    //
    EPhCltServerInitiatedPanicLast   // Keep as last.
    };

// Opcodes used in message passing between Phone Client and Phone Server.
enum TPhCltServerRequest
    {
    /**
    * ----> FROM RPhCltExtCall
    */
    EPhoneServerExtCallSubSessionOpen = 0,       // Open ExtCall subsession.
    EPhoneServerExtCallSubSessionClose,          // Close ExtCall subsession.
    EPhoneServerExtCallSubSessionMakeCall,       // Make a call.
    EPhoneServerExtCallSubSessionMakeCallCancel, // Cancel call creation.

    /**
    * ----> FROM RPhCltCallNotify
    */
    EPhoneServerNotifySubSessionOpen = 100,      // Open notify subsession.
    EPhoneServerNotifySubSessionClose,           // Close notify subsession.
    // Notify call request attempt.
    EPhoneServerNotifySubSessionNotifyCallRequest,
    // Notify call request canceling.
    EPhoneServerNotifySubSessionNotifyCallRequestCancel,
    // Notify call status request.
    EPhoneServerNotifySubSessionNotifyStatusRequest,
    // Notify call status request cancel.
    EPhoneServerNotifySubSessionNotifyStatusRequestCancel,
    // Report phone status.
    EPhoneServerNotifySubSessionReportPhoneStatus,
    // Report phone status cancel.
    EPhoneServerNotifySubSessionReportPhoneStatusCancel,
    // Report the result of the call.
    EPhoneServerNotifySubSessionReportCallResult,
    // Notify dial data
    EPhoneServerNotifySubSessionDialData,
        

    /**
    * ----> FROM RPhCltUssd
    */
    EPhoneServerUSSDSubSessionOpen = 200,       // Open USSD subsession.
    EPhoneServerUSSDSubSessionClose,            // Close USSD subsession.
    EPhoneServerUSSDSubSessionSendMessage,      // Send USSD message.
    EPhoneServerUSSDSubSessionSendMessageCancel,// Cancel USSD message sending.
    EPhoneServerUSSDSubSessionStartEditing,     // Start Service command editor.
    EPhoneServerUSSDSubSessionAppStarting,      // SCE is starting.
    EPhoneServerUSSDSubSessionAppTerminating,   // SCE is terminating.
    EPhoneServerUSSDSubSessionAppToForeground,  // SCE comes to foreground.
    EPhoneServerUSSDSubSessionAppToBackground,  // SCE goes to background.
    EPhoneServerUSSDSubSessionStartSAT,         // SAT session is started.
    EPhoneServerUSSDSubSessionStopSAT,          // Stop SAT session.

    /**
    * ----> FROM RPhCltEmergencyNumber
    */
    EPhoneServerEmergencyNumberSubSessionOpen = 300, // Open EN subsession.
    EPhoneServerEmergencyNumberSubSessionClose,      // Close EN subsession.
    // Check is the number emergency number or not.
    EPhoneServerEmergencyNumberSubSessionIsEmergencyNumber,
    EPhoneServerEmergencyNumberSubsessionEmergencyDial,
    EPhoneServerEmergencyNumberSubsessionEmergencyDialCancel,
    /**
    * ---> FROM RPhCltIhf
    */
    EPhoneServerIhfSubSessionOpen = 400,        // Open IHF subsession.
    EPhoneServerIhfSubSessionClose,             // Close IHF subsession.
    EPhoneServerIhfSubSessionSetMode,           // Set IHF mode.
    EPhoneServerIhfSubSessionSetModeFromPhone,  // Set IHF mode from Phone app.

    /**
    * ----> Other
    */
    EPhoneCreateAll = 500,  // Create all subsessions

    //
    EPhoneServerLast,        // Keep as last for PhoneClient.

    /**
    * ----> Base for Phone Client extensions, that may be defined in some other
    *       header file.    
    */
    EPhoneServerExtensionBase = 600,
    
    /**
    * ----> FROM RPhCltEmergencyCallNotify
    */
    //EPhoneServerNotifyEmergencySubSessionOpen,
    EPhoneServerNotifyEmergencySubSessionOpen = 1500,      // Open notify subsession.
    EPhoneServerNotifyEmergencySubSessionClose,           // Close notify subsession.
    // Notify call request attempt.
    EPhoneServerNotifyEmergencySubSessionNotifyCallRequest,
    // Notify call request canceling.
    EPhoneServerNotifyEmergencySubSessionNotifyCallRequestCancel,
    // Report the result of the call.
    EPhoneServerNotifyEmergencySubSessionReportCallResult 
   


    };
    
    /**
* Opcodes used in message passing between Phone Client and Phone Server.
*/
enum TPhCltComHandServerRequest
    {
    /**
    * ---> FROM RPhCltCommandHandler
    */
    // Open command handler subsession.
    EPhoneServerComHandSubSessionOpen = EPhoneServerExtensionBase, 
    EPhoneServerComHandSubSessionClose,      // Close comhand subsession.
    EPhoneServerComHandSubSessionAtd,        // Atd.
    EPhoneServerComHandSubSessionAta,        // Ata.
    EPhoneServerComHandSubSessionChld,       // Chld.
    EPhoneServerComHandSubSessionChup,       // Chup.
    EPhoneServerComHandSubSessionVts,        // Vts.
    EPhoneServerComHandSubSessionBvra,       // Bvra.
    EPhoneServerComHandSubSessionCancel,     // Cancel ongoing request.
    EPhoneServerComHandSubSessionMuteMic,    // MuteMic
    EPhoneServerComHandSubSessionMuteRingingTone,   // MuteRingingTone

    /**
    * ----> FROM RPhCltCommandHandlerNotify
    */
    // Open command handler notify subsession.
    EPhoneServerComHandNotifySubSessionOpen = EPhoneServerExtensionBase + 100,
    // Close command handler notify subsession.
    EPhoneServerComHandNotifySubSessionClose,
    // Notify command handler request attempt.
    EPhoneServerComHandNotifySubSessionComHandRequest,
    // Notify command handler request canceling.
    EPhoneServerComHandNotifySubSessionComHandRequestCancel,
    // Report the result of the command handler request.
    EPhoneServerComHandNotifySubSessionReportComHandResult,   
    //
    EPhoneServerComHandLast        // Keep as last.
    };

/**
* Opcodes used in message passing between 
* Phone Client Extension Messenger and Phone Server.
*/
enum TPhCltMessengerServerRequest
    {
    /**
    * ---> FROM RPhCltMessenger
    * @since 2.6
    */
    // Open Messenger subsession.
    EPhoneServerMessengerSubSessionOpen = EPhoneServerExtensionBase + 200,
    // Construct (reserve memory for) opened subsession.
    EPhoneServerMessengerSubSessionConstruct,
    EPhoneServerMessengerSubSessionClose,    // Close Messenger subsession.
    EPhoneServerMessengerSubSessionReceive,  // Receive.
    EPhoneServerMessengerSubSessionSend,     // Send.
    EPhoneServerMessengerSubSessionCancel,   // CancelRequest.
    EPhoneServerMessengerSubSessionSkip,     // Skip.

    EPhoneServerMessengerLast                // Keep as last.
    };

/**
* Opcodes used in message passing between 
* Phone Client Extension and Phone Server.
* Image handling specific.
*
* @since Series 60 3.0
*/

enum TPhCltImageHandlerRequest 
    {
    EPhoneServerImageHandlerSubSessionOpen = EPhoneServerExtensionBase + 300,
    EPhoneServerImageHandlerSubSessionClose, 	 // Closes image handler subsession.
    EPhoneServerImageHandlerSubSessionSave,      // SaveImages
    EPhoneServerImageHandlerSubSessionLoad,      // LoadImages
    EPhoneServerImageHandlerSubSessionCommitLoad // Finalize load operation
    
    };



    /**
* Enumerates fixed operator logo indexes.
*
* @since Series60 3.2
*
* EPhCltOperatorLogoIndex - only one logo at a time.
* EPhCltCountryCode - logo specific country code.
* EPhCltNetworkCode - logo specific country code.
* EPhCltLogoTypeIndex - logo type
*/
enum TPhCltExtOperatorLogoIndexes
    {
    EPhCltExtIndexNotSet = -1,
    EPhCltExtOperatorLogoIndex = 0,
    EPhCltExtCountryCodeIndex,
    EPhCltExtNetworkCodeIndex,
    EPhCltExtLogoTypeIndex
    };


#endif      // PHCLTCLIENTSERVER_H

// End of File
