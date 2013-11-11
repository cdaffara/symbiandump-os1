/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Server-side sub-session of Accessory Mode
*
*/



// INCLUDE FILES
#include "AccSrvSubMode.h"
#include "AccSrvMainSession.h"
#include "AccClientServerMessages.h"
#include "AccPolGenericID.h"
#include "acc_debug.h"
#include "AccSrvServerModel.h"
#include <AsyCmdTypes.h>

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CAccSrvSubMode::CAccSrvSubMode
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CAccSrvSubMode::CAccSrvSubMode(
    CASYProxyAPI* aProxy,
    CAccSrvConnectionController* aConnectionController,
    CAccSrvServerModel* aServerModel )
    : CAccSrvSubBase( aProxy, aConnectionController, aServerModel ) ,
    iFirstOrder ( ETrue )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubMode::CAccSrvSubMode()" );

    iRecentAccessoryMode = iServerModel->AccessoryMode();

    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubMode::CAccSrvSubMode - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvSubMode::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CAccSrvSubMode::ConstructL()
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubMode::ConstructL()" );

    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubMode::ConstructL - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvSubMode::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CAccSrvSubMode* CAccSrvSubMode::NewL(
    CASYProxyAPI* aProxy,
    CAccSrvConnectionController* aConnectionController,
    CAccSrvServerModel* aServerModel )
    {
    if ( !aProxy )
        {
        User::Leave( KErrArgument );
        }

    CAccSrvSubMode* self = new ( ELeave ) CAccSrvSubMode( aProxy,
                                                          aConnectionController,
                                                          aServerModel );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// Destructor
CAccSrvSubMode::~CAccSrvSubMode()
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubMode::~CAccSrvSubMode()" );
    RMessage2 dummy;
    
    CAccSrvSubMode::CancelGetAccessoryMode( dummy );//For iGetAccessoryModeAsync
    CAccSrvSubMode::CancelNotifyAccessoryModeChanged( dummy );//For iNotifyAccessoryModeChanged

    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubMode::~CAccSrvSubMode - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvSubMode::GetAccessoryModeL
// -----------------------------------------------------------------------------
//
void CAccSrvSubMode::GetAccessoryModeL( const RMessage2& aMessage )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubMode::GetAccessoryModeL()" );

    if ( iServerModel->ASYsLoadCompleted() )
        {
        TAccPolAccessoryMode accMode = { EAccModeTextDevice, EFalse };
        accMode = iServerModel->AccessoryMode();
        TAccSrvAccessoryModePckg accModePckg( accMode );
        aMessage.WriteL( KAccServParamPosFirst, accModePckg );
        CAccSrvSubMode::Complete( aMessage, KErrNone, accMode );
        }
    else
        {
        COM_TRACE_( "[AccFW:AccServer] CAccSrvSubMode::GetAccessoryModeL - Mode not initialized yet" );
        User::Leave( KErrNotReady );
        }

    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubMode::GetAccessoryModeL - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvSubMode::GetAccessoryModeAsyncL
// -----------------------------------------------------------------------------
//
void CAccSrvSubMode::GetAccessoryModeAsyncL( const RMessage2& aMessage )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubMode::GetAccessoryModeAsyncL()" );

    if ( iServerModel->ASYsLoadCompleted() )
        {
        COM_TRACE_( "[AccFW:AccServer] CAccSrvSubMode::GetAccessoryModeAsyncL - completing synchronously" );
        TAccPolAccessoryMode accMode = { EAccModeTextDevice, EFalse };
        accMode = iServerModel->AccessoryMode();
        TAccSrvAccessoryModePckg accModePckg( accMode );
        aMessage.WriteL( KAccServParamPosFirst, accModePckg );
        Write( aMessage, KAccSrvGetAccessoryModeBit );
        CAccSrvSubMode::Complete( aMessage, KErrNone, accMode );
        }
    else
        {
        iConnectionController->NotificationQueue().RegisterControlMessage( 
                                                        EAccessoryModeAsyncChanged,
                                                        this );
        iGetAccessoryModeAsync = aMessage;//Store RMessage2 for future completion
        }

    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubMode::GetAccessoryModeAsyncL - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvSubMode::CancelGetAccessoryModeL
// -----------------------------------------------------------------------------
//
void CAccSrvSubMode::CancelGetAccessoryMode( const RMessage2& /*aMessage*/ )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubMode::CancelGetAccessoryModeL()" );

    // If request is null there is nothing to cancel
    if ( !iGetAccessoryModeAsync.IsNull() )
        {
        iConnectionController->NotificationQueue().CancelControlMessage( 
                                                        EAccessoryModeAsyncChanged,
                                                        this );

        Write( iGetAccessoryModeAsync, KAccSrvGetAccessoryModeBit );
        TAccPolAccessoryMode accessoryMode( iServerModel->AccessoryMode() );
        CAccSrvSubMode::Complete( iGetAccessoryModeAsync, KErrCancel, accessoryMode );
        }
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubMode::CancelGetAccessoryModeL - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvSubMode::NotifyAccessoryModeChangedL
// -----------------------------------------------------------------------------
//
void CAccSrvSubMode::NotifyAccessoryModeChangedL( const RMessage2& aMessage )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubMode::NotifyAccessoryModeChangedL()" );

    TAccPolAccessoryMode currentMode( iConnectionController->ServerModel().AccessoryMode() );

    iNotifyAccessoryModeChanged = aMessage;//Store RMessage2 for future completion, don't leave after this

    if ( iRecentAccessoryMode.iAccessoryMode == currentMode.iAccessoryMode && 
         iRecentAccessoryMode.iAudioOutputStatus == currentMode.iAudioOutputStatus || 
         iFirstOrder )
        {
        // Add session to queue.
        iConnectionController->NotificationQueue().RegisterControlMessage( 
                                                        EAccessoryModeChanged,
                                                        this );
        }
    else 
        {
        //Accessory mode has been changes before before notification is re-ordered
        CAccSrvSubMode::CompleteMessageL( EAccessoryModeChanged, KErrNone, KErrUnknown );
        }

    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubMode::NotifyAccessoryModeChangedL - return void" );
    }
  
// -----------------------------------------------------------------------------
// CAccSrvSubMode::CancelNotifyAccessoryModeChangedL
// -----------------------------------------------------------------------------
//
void CAccSrvSubMode::CancelNotifyAccessoryModeChanged( const RMessage2& /*aMessage*/ )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubMode::CancelNotifyAccessoryModeChanged()" );

    // If request is null there is nothing to cancel
    if ( !iNotifyAccessoryModeChanged.IsNull() )
        {
        iConnectionController->NotificationQueue().CancelControlMessage( 
                                                        EAccessoryModeChanged,
                                                        this );
        Write( iNotifyAccessoryModeChanged, KAccSrvNotifyAccessoryModeChangedBit );
        TAccPolAccessoryMode accessoryMode( iServerModel->AccessoryMode() );
        CAccSrvSubMode::Complete( iNotifyAccessoryModeChanged, KErrCancel, accessoryMode );
        }
    
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubMode::CancelNotifyAccessoryModeChanged - return void" );
    }
    
// -----------------------------------------------------------------------------
// CAccSrvSubMode::DispatchMessageL
// Handles client request
// -----------------------------------------------------------------------------
//
void CAccSrvSubMode::DispatchMessageL( const RMessage2& aMessage )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubMode::DispatchMessageL()" );

    TInt function = aMessage.Function();

    switch ( function )
        {
        case EAccSrvGetAccessoryMode:
            {
            GetAccessoryModeL( aMessage );
            break;
            }
        case EAccSrvCancelGetAccessoryMode:
            {
            CancelGetAccessoryMode( aMessage );
            break;
            }
        case EAccSrvGetAccessoryModeAsync:
            {
            GetAccessoryModeAsyncL( aMessage );
            break;
            }
        case EAccSrvNotifyAccessoryModeChanged:
            {
            NotifyAccessoryModeChangedL( aMessage );
            break;
            }
        case EAccSrvCancelNotifyAccessoryModeChanged:
            {
            CancelNotifyAccessoryModeChanged( aMessage );
            break;
            }
        default:
            {
            TRACE_ASSERT_ALWAYS;
            COM_TRACE_1( "[AccFW:AccServer] CAccSrvSubMode::DispatchMessageL - unhandled function %d", function );
            User::Leave( KErrGeneral );
            break;
            }
        } // switch( function )

    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubMode::DispatchMessageL - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvSubMode::CompleteMessageL
// -----------------------------------------------------------------------------
//
TBool CAccSrvSubMode::CompleteMessageL( TMsgID aMsgID,
                                        TInt aErrorCode,
                                        TInt /*aUniqueID*/ )
    {
    COM_TRACE_2( "[AccFW:AccServer] CAccSrvSubMode::CompleteMessageL(msg=%d err=%d)", aMsgID, aErrorCode );

    if ( EAccessoryModeChanged == aMsgID )
        {
        //Write current accessory mode and
        //complete outstanding AccessoryAudioLinkOpen request
        TAccPolAccessoryMode accessoryMode;
        accessoryMode = iConnectionController->ServerModel().AccessoryMode();
        TPckg<TAccPolAccessoryMode> accessoryModePckg( accessoryMode );

        iNotifyAccessoryModeChanged.WriteL( KAccServParamPosFirst, accessoryModePckg );
        Write( iNotifyAccessoryModeChanged, KAccSrvNotifyAccessoryModeChangedBit );
        CAccSrvSubMode::Complete( iNotifyAccessoryModeChanged, aErrorCode, accessoryMode );
        }
    else if ( EAccessoryModeAsyncChanged == aMsgID )
        {
        //Write current accessory mode and
        //complete outstanding GetAccessoryMode ( Async )
        TAccPolAccessoryMode accessoryMode;
        accessoryMode = iServerModel->AccessoryMode();
        TAccSrvAccessoryModePckg accessoryModePckg( accessoryMode );

        iGetAccessoryModeAsync.WriteL( KAccServParamPosFirst, accessoryModePckg );
        Write( iGetAccessoryModeAsync, KAccSrvGetAccessoryModeBit );
        CAccSrvSubMode::Complete( iGetAccessoryModeAsync, aErrorCode, accessoryMode );
        }
    else
        {
        TRACE_ASSERT_ALWAYS;
        User::Leave( KErrGeneral );
        }

    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubMode::CompleteMessageL - return" );

    return ETrue; // if it gets here, this is always ETrue
    }

// -----------------------------------------------------------------------------
// CAccSrvSubMode::Complete
//
// Mode request is completed with updated Accessory Mode.
// This method stores also most recent accessory mode what client have.
// -----------------------------------------------------------------------------
//
void CAccSrvSubMode::Complete( 
    const RMessage2& aMessage,
    TInt aErrorCode,
    TAccPolAccessoryMode& aAccessoryMode )
    {
    COM_TRACE_1( "[AccFW:AccServer] CAccSrvSubMode::Complete(Err=%d)", aErrorCode );
    
    if ( KErrCancel == aErrorCode )
        {
        //If Mode request is cancelled, Next Accessory NotifyAccessoryModeChanged() 
        //is not completed directly.
        iFirstOrder = ETrue;
        }
    else
        {
        iFirstOrder = EFalse; 
        }
    
    iRecentAccessoryMode = aAccessoryMode;    
    aMessage.Complete( aErrorCode );
    
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubMode::Complete - return void" );
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File
