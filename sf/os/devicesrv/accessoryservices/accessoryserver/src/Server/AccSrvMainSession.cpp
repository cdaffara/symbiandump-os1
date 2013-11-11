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
* Description:  Server's side session class
*
*/


// INCLUDE FILES
#include "AccSrvMainSession.h"
#include "AccClientServerMessages.h"
#include "acc_debug.h"
#include <e32svr.h>

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES
extern void PanicClient( const RMessage2& aMessage, TAccServerError aPanic );

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CAccSrvMainSession::CAccSrvMainSession
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CAccSrvMainSession::CAccSrvMainSession(
      CAccSrvConnectionController* aConnectionController,
      CAccSrvServerModel* aServerModel )
      : iAccServer( NULL ),
        iObjectIx( NULL ),
        iObjectCon( NULL ),
        iProxy( NULL ),
        iConnectionController( aConnectionController ),
        iServerModel( aServerModel )
    {
    COM_TRACE_1( "[AccFW:AccServer] CAccSrvMainSession::CAccSrvMainSession(0x%x)", &aConnectionController );
    COM_TRACE_( "[AccFW:AccServer] CAccSrvMainSession::CAccSrvMainSession()" );
    }

// -----------------------------------------------------------------------------
// CAccSrvMainSession::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CAccSrvMainSession::ConstructL( CAccServer* aServer, CASYProxyAPI* aProxy )
    {
    COM_TRACE_2( "[AccFW:AccServer] CAccSrvMainSession::ConstructL(0x%x, 0x%x)", &aServer, &aProxy );

    iAccServer = aServer;
    iObjectCon = iAccServer->NewContainerL();
    iObjectIx = CObjectIx::NewL();
    iProxy = aProxy;

    COM_TRACE_( "[AccFW:AccServer] CAccSrvMainSession::ConstructL - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvMainSession::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CAccSrvMainSession* CAccSrvMainSession::NewL(
        RThread& aClient,
        CAccServer& aServer,
        CASYProxyAPI* aProxy,
        CAccSrvConnectionController* aConnectionController,
        CAccSrvServerModel* aServerModel )
    {
    COM_TRACE_4( "[AccFW:AccServer] CAccSrvMainSession::NewL(0x%x, 0x%x, 0x%x, 0x%x)", &aClient, &aServer, &aProxy, &aConnectionController );

    CAccSrvMainSession* self = CAccSrvMainSession::NewLC( 
                                            aClient,
                                            aServer,
                                            aProxy,
                                            aConnectionController,
                                            aServerModel );
    CleanupStack::Pop( self );

    COM_TRACE_1( "[AccFW:AccServer] CAccSrvMainSession::NewL - return 0x%x", self );
    return self;
    }

// -----------------------------------------------------------------------------
// CAccSrvMainSession::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CAccSrvMainSession* CAccSrvMainSession::NewLC(
        RThread& /*aClient*/,
        CAccServer& aServer,
        CASYProxyAPI* aProxy,
        CAccSrvConnectionController* aConnectionController,
        CAccSrvServerModel* aServerModel )
    {
    COM_TRACE_3( "[AccFW:AccServer] CAccSrvMainSession::NewLC( 0x%x, 0x%x, 0x%x)", &aServer, &aProxy, &aConnectionController );

    CAccSrvMainSession* self =
        new ( ELeave ) CAccSrvMainSession( aConnectionController, aServerModel );

    CleanupStack::PushL( self ) ;
    self->ConstructL( &aServer, aProxy );

    COM_TRACE_1( "[AccFW:AccServer] CAccSrvMainSession::NewLC - return 0x%x", self );
    return self;
    }

// Destructor
CAccSrvMainSession::~CAccSrvMainSession()
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvMainSession::~CAccSrvMainSession()" );

    if ( iObjectIx )
        {
        delete iObjectIx;
        }
    if ( iAccServer && iObjectCon )
        {
        iAccServer->RemoveContainer( iObjectCon ); // Remove and delete the container
        }

    COM_TRACE_( "[AccFW:AccServer] CAccSrvMainSession::~CAccSrvMainSession - return()" );
    }

// -----------------------------------------------------------------------------
// CAccSrvMainSession::ServiceL
// Calls request handling functions.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CAccSrvMainSession::ServiceL( const RMessage2& aMessage )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvMainSession::ServiceL()" );

    TInt function = aMessage.Function();

    COM_TRACE_1( "[AccFW:AccServer] CAccSrvMainSession::ServiceL - function == 0x%x", function );

    TInt err( KErrNone );

    TRAP( err, DispatchMessageL( aMessage ) );

    switch ( function )
        {
        case EAccSrvCreateSubSessionAccessoryMode:
        case EAccSrvCreateSubSessionAccessorySettings:
        case EAccSrvCreateSubSessionAccessoryConnection:
        case EAccSrvCreateSubSessionAccessoryControl:
        case EAccSrvCreateSubSessionAccessorySingleConnection:
        case EAccSrvCreateSubSessionAccessoryBTControl:
        case EAccSrvCreateSubSessionAccessoryAudioControl:
        case EAccSrvCreateSubSessionAccessoryAsyComms:
        case EAccSrvCloseSubSessionAccessoryMode:
        case EAccSrvCloseSubSessionAccessorySettings:
        case EAccSrvCloseSubSessionAccessoryConnection:
        case EAccSrvCloseSubSessionAccessoryControl:
        case EAccSrvCloseSubSessionAccessorySingleConnection:
        case EAccSrvCloseSubSessionAccessoryBTControl:
        case EAccSrvCloseSubSessionAccessoryAudioControl:
        case EAccSrvCloseSubSessionAccessoryAsyComms:
        case EAccSrvGetAccessoryConnectionStatus:
        case EAccSrvCancelNotifyAccessoryConnectionStatusChanged:
        case EAccSrvCancelNotifyNewAccessoryConnected:
        case EAccSrvCancelNotifyAccessoryDisconnected:
        case EAccSrvCancelNotifyAccessoryValueChanged:
        case EAccSrvGetSubblockNameArray:        
        case EAccSrvCancelNotifyAccessoryModeChanged:
        case EAccSrvGetSupportedHWDevices:
        case EAccSrvGetHWDeviceSettings:
        case EAccSrvSetHWDeviceSettings:
        case EAccSrvGetAccessoryModeSetting:
        case EAccSrvSetAccessoryModeSetting:
        case EAccSrvGetAccessoryModeSettings:
        case EAccSrvSetAccessoryModeSettings:
        case EAccSrvConnectAccessory2:
        case EAccSrvDisconnectAccessory:
        case EAccSrvAccessoryValueChangedTBoolNotify:
        case EAccSrvBTAccessoryValueChangedTBoolNotify:
        case EAccSrvBTAccessoryValueChangedTIntNotify:
        case EAccSrvAccessoryValueChangedTIntNotify:
        case EAccSrvAccessoryValueChangedObjectNotify:
        case EAccSrvCancelConnectAccessory:
        case EAccSrvCancelDisconnectAccessory:
        case EAccSrvBluetoothDisconnectAccessory:
        case EAccSrvBluetoothAudioLinkOpenResp:
        case EAccSrvBluetoothAudioLinkCloseResp:
        case EAccSrvBluetoothAudioLinkOpenedNotify:
        case EAccSrvBluetoothAudioLinkClosedNotify:
        case EAccSrvCancelNotifyBluetoothAudioLinkOpenReq:
        case EAccSrvCancelNotifyBluetoothAudioLinkCloseReq:
        case EAccSrvAudioRoutingStatusNotify:
        case EAccSrvCancelAccessoryAudioLinkOpen:
        case EAccSrvCancelAccessoryAudioLinkClose:
        case EAccSrvCancelNotifyAccessoryAudioLinkOpened:
        case EAccSrvCancelNotifyAccessoryAudioLinkClosed:
        case EAccSrvCancelNotifyProcessCommand:
        case EAccSrvProcessResponseTBool:
        case EAccSrvProcessResponseTInt:
        case EAccSrvProcessResponseTDes:        
        case EAccSrvProcessResponseObject:
        case EAccSrvCancelGetAccessoryMode:
        case EAccSrvCancelGetAccessoryConnectionStatus:
        case EAccSrvGetObjectBuffer:
        case EAccSrvGetObjectValue:
            {
            
            if( aMessage.Handle() )	
                {
                COM_TRACE_1( "[AccFW:AccServer] CAccSrvMainSession::ServiceL - message completed, reason=%d", err );
                WriteL( aMessage, function, err );//Update iFlags parameter to client side
                aMessage.Complete( err );
                }
            }
            break;

        case EAccSrvBluetoothConnectAccessory:
        case EAccSrvNotifyAccessoryAudioLinkOpened:
        case EAccSrvNotifyAccessoryAudioLinkClosed:
        case EAccSrvNotifyBluetoothAudioLinkOpenReq:
        case EAccSrvNotifyBluetoothAudioLinkCloseReq:
        case EAccSrvNotifyNewAccessoryConnected:
        case EAccSrvNotifyAccessoryDisconnected:
        case EAccSrvNotifyAccessoryValueChangedTBool:
        case EAccSrvNotifyAccessoryValueChangedTInt:
        case EAccSrvNotifyAccessoryValueChangedObject:
        case EAccSrvNotifyAccessoryConnectionStatusChanged:
        case EAccSrvNotifyAccessoryModeChanged:
        case EAccSrvAccessoryAudioLinkOpen:
        case EAccSrvAccessoryAudioLinkClose:
        case EAccSrvNotifyProcessCommand:
        case EAccSrvGetAccessoryMode:
        case EAccSrvGetAccessoryModeAsync:
        case EAccSrvGetAccessoryConnectionStatusAsync:
        case EAccSrvSetValueTBool:
        case EAccSrvSetValueTInt:        
        case EAccSrvSetValueObject:
        case EAccSrvGetValueTBool:
        case EAccSrvGetValueTInt:
        case EAccSrvGetValueTDes8:        
        case EAccSrvGetValueObjectBase:
            {
            if (err != KErrNone)
                {
                COM_TRACE_1( "[AccFW:AccServer] CAccSrvMainSession::ServiceL - message completed with error %d", err );
                // These messages are completed in subsessions (on time).
                // In case of error, completion is done here with
                // appropriate error code.
                WriteL( aMessage, function, err );//Update iFlags parameter to client side
                aMessage.Complete( err );
                }
            }
            break;

        default:
            {
            TRACE_ASSERT_ALWAYS;
            COM_TRACE_1( "[AccFW:AccServer] CAccSrvMainSession::ServiceL - unhandled function: 0x%x", function );
            PanicClient( aMessage, EAccServerErrorNotSupported );
            }
            break;

        } //switch

    COM_TRACE_( "[AccFW:AccServer] CAccSrvMainSession::ServiceL - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvMainSession::DispatchMessageL
// Handles messages
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CAccSrvMainSession::DispatchMessageL( const RMessage2& aMessage )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvMainSession::DispatchMessageL()" );

    TInt function = aMessage.Function();

    COM_TRACE_1( "[AccFW:AccServer] CAccSrvMainSession::DispatchMessageL - function = 0x%x", function );

    CAccSrvSubBase* subSession = NULL;

    switch ( function )
        {
        case EAccSrvCreateSubSessionAccessoryMode:
        case EAccSrvCreateSubSessionAccessorySettings:
        case EAccSrvCreateSubSessionAccessoryConnection:
        case EAccSrvCreateSubSessionAccessoryControl:
        case EAccSrvCreateSubSessionAccessorySingleConnection:
        case EAccSrvCreateSubSessionAccessoryBTControl:
        case EAccSrvCreateSubSessionAccessoryAudioControl:
        case EAccSrvCreateSubSessionAccessoryAsyComms:

            NewSubSessionL( aMessage);
            return;

        case EAccSrvCloseSubSessionAccessoryMode:
        case EAccSrvCloseSubSessionAccessorySettings:
        case EAccSrvCloseSubSessionAccessoryConnection:
        case EAccSrvCloseSubSessionAccessoryControl:
        case EAccSrvCloseSubSessionAccessorySingleConnection:
        case EAccSrvCloseSubSessionAccessoryBTControl:
        case EAccSrvCloseSubSessionAccessoryAudioControl:
        case EAccSrvCloseSubSessionAccessoryAsyComms:

            CloseSubSessionL( aMessage );
            return;

        default:
            {
            COM_TRACE_( "[AccFW:AccServer] CAccSrvMainSession::DispatchMessageL - subsession specific request" );
            }
            break;
        }

    // Request is subsession specific
    subSession = REINTERPRET_CAST( CAccSrvSubBase*, iObjectIx->At( aMessage.Int3() ) );

    if ( subSession )
        {
        subSession->DispatchMessageL( aMessage );
        }
    else
        {
        COM_TRACE_( "[AccFW:AccServer] CAccSrvMainSession::DispatchMessageL - subsession specific create/connect" );
        aMessage.Panic( KAccServerPanicCategory, EAccServerErrorBadDescriptor );//Panics the client
        }

    COM_TRACE_( "[AccFW:AccServer] CAccSrvMainSession::DispatchMessageL - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvMainSession::NewSubSessionL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CAccSrvMainSession::NewSubSessionL( const RMessage2& aMessage )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvMainSession::NewSubSessionL()" );

    TInt handle( 0 );
    TInt type ( aMessage.Function() );
    CObject* session = NULL;

    // No need to restrict number of Sub-Sessions
    // Number of Sessions should be restricted instead (to 1/client thread)

    switch ( type )
        {
        case EAccSrvCreateSubSessionAccessoryMode:
            {
            session = CAccSrvSubMode::NewL( iProxy, iConnectionController, iServerModel );
            break;
            }

        case EAccSrvCreateSubSessionAccessorySettings:
            {
            session = CAccSrvSubSettings::NewL( iProxy, iConnectionController, iServerModel );
            break;
            }

        case EAccSrvCreateSubSessionAccessorySingleConnection:
            {
            session = CAccSrvSubSingleConnection::NewL( 
                                                    iProxy, 
                                                    iConnectionController, 
                                                    iServerModel );
            break;
            }

        case EAccSrvCreateSubSessionAccessoryConnection:
            {
            session = CAccSrvSubConnection::NewL( iProxy, iConnectionController, iServerModel );
            break;
            }

        case EAccSrvCreateSubSessionAccessoryControl:
            {
            session = CAccSrvSubControl::NewL( iProxy, iConnectionController, iServerModel );
            break;
            }

        case EAccSrvCreateSubSessionAccessoryBTControl:
            {
            session = CAccSrvSubBTControl::NewL( iProxy, iConnectionController, iServerModel );
            break;
            }

        case EAccSrvCreateSubSessionAccessoryAudioControl:
            {
            //Get the thread ID of the caller
            TUint callerThreadID( 0 );
            TPckg<TUint> callerThreadIDPckg( callerThreadID );
            aMessage.Read( 0, callerThreadIDPckg );

            session = CAccSrvSubAudioControl::NewL( iProxy,
                                                    iConnectionController,
                                                    iServerModel,
                                                    callerThreadID );
            break;
            }

        case EAccSrvCreateSubSessionAccessoryAsyComms:
            {
            //Get the thread ID of the ASY
            TUint ASYThreadID( 0 );
            TPckg<TUint> ASYThreadIDPckg( ASYThreadID );
            aMessage.Read( 0, ASYThreadIDPckg );

            // Main service UID of the ASY
            TUid mainServiceUid( TUid::Null() );
            TPckg<TUid> mainServiceUidPckg( mainServiceUid );
            aMessage.Read( 1, mainServiceUidPckg );

            session = CAccSrvSubAsyComms::NewL( iProxy,
                                                iConnectionController,
                                                iServerModel,
                                                ASYThreadID,
                                                mainServiceUid );
            break;
            }

        default:
            TRACE_ASSERT_ALWAYS;
            User::Leave( KErrGeneral );
            break;
        }

    CleanupStack::PushL( session );
    iObjectCon->AddL( session );
    CleanupStack::Pop( session );

    TRAPD( error, handle = iObjectIx->AddL( session ) );
    if ( error )
        {
        // AddL didn't complete successfully.
        // Remove object from container and delete object.
        iObjectCon->Remove( session );
        User::Leave( error );
        }

    // Package to pass information to the client
    TPckgC<TInt> handlePckg( handle );

    // Send handle to the client
    TRAP( error, aMessage.WriteL( KAccServHandlePos, handlePckg ) );
    if ( error )
        {
        // Destroy newly created subsession, nobody needs it now
        iObjectIx->Remove( handle );
        }

    COM_TRACE_( "[AccFW:AccServer] CAccSrvMainSession::NewSubSessionL - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvMainSession::CloseSubSessionL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CAccSrvMainSession::CloseSubSessionL( const RMessage2& aMessage )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvMainSession::CloseSubSessionL()" );

    COM_TRACE_1( "[AccFW:AccServer] CAccSrvMainSession::CloseSubSessionL - aMessage.Int3() = 0x%x", aMessage.Int3() );
    iObjectIx->AtL( aMessage.Int3() );
    iObjectIx->Remove( aMessage.Int3() );

    COM_TRACE_( "[AccFW:AccServer] CAccSrvMainSession::CloseSubSessionL - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvMainSession::WriteL
// -----------------------------------------------------------------------------
//
void CAccSrvMainSession::WriteL( const RMessage2& aMessage, TInt aFunction, TInt aErr )
    {
    COM_TRACE_2( "[AccFW:AccServer] CAccSrvMainSession::WriteL(func=%d err=%d)", aFunction, aErr );
    
    TInt flag( 0 );
    TInt flags( 0 );
    TPckg<TInt> flagsPckg( flags );
    
    if ( KErrNone == aErr )
        {
        COM_TRACE_( "[AccFW:AccServer] CAccSrvMainSession::WriteL - no need to update flags - return void" );
        return;//In succesfull case, iFlags parameter is updated in subsessions
        }
    
    switch ( aFunction )
        {
        case EAccSrvAccessoryAudioLinkOpen:
            {
            flag = KAccSrvAccessoryAudioLinkOpenBit;
            }
            break;
        case EAccSrvAccessoryAudioLinkClose:
            {
            flag = KAccSrvAccessoryAudioLinkCloseBit;
            }
            break;  
        case EAccSrvNotifyAccessoryAudioLinkOpened:
            {
            flag = KAccSrvNotifyAccessoryAudioLinkOpenedBit;
            }
            break;
        case EAccSrvNotifyAccessoryAudioLinkClosed:
            {
            flag = KAccSrvNotifyAccessoryAudioLinkClosedBit;
            }
            break;   
        case EAccSrvBluetoothConnectAccessory:
            {
            flag = KAccSrvConnectBTAccessoryBit;
            }
            break;  
        case EAccSrvBluetoothDisconnectAccessory:
            {
            flag = KAccSrvDisconnectBTAccessoryBit;
            }
            break;  
        case EAccSrvNotifyBluetoothAudioLinkOpenReq:
            {
            flag = KAccSrvNotifyBluetoothAudioLinkOpenReqBit;
            }
            break; 
        case EAccSrvNotifyBluetoothAudioLinkCloseReq:
            {
            flag = KAccSrvNotifyBluetoothAudioLinkCloseReqBit;
            }
            break;  
        case EAccSrvGetAccessoryConnectionStatusAsync:
            {
            flag = KAccSrvGetAccessoryConnectionStatusBit;
            }
            break;   
        case EAccSrvNotifyAccessoryConnectionStatusChanged:
            {
            flag = KAccSrvNotifyAccConnStatusChangedBit;
            }
            break;
        case EAccSrvGetAccessoryModeAsync:
            {
            flag = KAccSrvGetAccessoryModeBit;
            }
            break;
        case EAccSrvNotifyAccessoryModeChanged:
            {
            flag = KAccSrvNotifyAccessoryModeChangedBit;
            }
            break;
        case EAccSrvNotifyNewAccessoryConnected:
            {
            flag = KAccSrvNotifyNewAccessoryConnectedBit;
            }
            break;
        case EAccSrvNotifyAccessoryDisconnected:
            {
            flag = KAccSrvNotifyAccessoryDisconnectedBit;
            }
            break;
        case EAccSrvNotifyAccessoryValueChangedTBool:
            {
            flag = KAccSrvNotifyAccessoryValueChangedTBoolBit;
            }
            break;
        case EAccSrvNotifyAccessoryValueChangedTInt:
            {
            flag = KAccSrvNotifyAccessoryValueChangedTIntBit;
            }
            break;
        case EAccSrvNotifyAccessoryValueChangedObject:
            {
            flag = KAccSrvNotifyAccessoryValueChangedObjectBit;
            }
            break;
        case EAccSrvDisconnectAccessory:
            {
            flag = KAccSrvDisconnectAccessoryBit;
            }
            break;
        case EAccSrvConnectAccessory2:
            {
            flag = KAccSrvDisconnectAccessoryBit;
            }
            break;
        default:
            {
            COM_TRACE_( "[AccFW:AccServer] CAccSrvMainSession::WriteL - no need to update flags for this msg - return void" );
            return;
            }
        }

    aMessage.ReadL( KAccServParamPosThird, flagsPckg );//It's allways argument number 2 which is used in client side
    flags = flags & (~flag);//Turn bit number flag to zero
    aMessage.Write( KAccServParamPosThird, flagsPckg );
    
    COM_TRACE_( "[AccFW:AccServer] CAccSrvMainSession::WriteL - flags updated - return void" );
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

// End of File
