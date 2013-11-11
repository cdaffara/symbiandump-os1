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
* Description:  Server-side sub-session of Accessory Connection
*
*/



// INCLUDE FILES
#include "AccSrvSubConnection.h"
#include "AccSrvMainSession.h"
#include "AccClientServerMessages.h"
#include "AccSrvServerModel.h"
#include "acc_debug.h"

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
// CAccSrvSubConnection::CAccSrvSubConnection
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CAccSrvSubConnection::CAccSrvSubConnection(
    CASYProxyAPI* aProxy,
    CAccSrvConnectionController* aConnectionController,
    CAccSrvServerModel* aServerModel )
    : CAccSrvSubConnectionBase ( aProxy, aConnectionController, aServerModel )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubConnection::CAccSrvSubConnection()" );

    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubConnection::CAccSrvSubConnection - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvSubConnection::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CAccSrvSubConnection::ConstructL()
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubConnection::ConstructL" );

    CAccSrvSubConnectionBase::ConstructL();

    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubConnection::ConstructL - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvSubConnection::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CAccSrvSubConnection* CAccSrvSubConnection::NewL( 
    CASYProxyAPI* aProxy,
    CAccSrvConnectionController* aConnectionController,
    CAccSrvServerModel* aServerModel )
    {
    if ( !aProxy )
        {
        User::Leave( KErrArgument );
        }

    CAccSrvSubConnection* self = new (ELeave) CAccSrvSubConnection( aProxy,
                                                                    aConnectionController,
                                                                    aServerModel );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// Destructor
CAccSrvSubConnection::~CAccSrvSubConnection()
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubConnection::~CAccSrvSubConnection()" );
    
    RMessage2 dummy;
    
    CAccSrvSubConnection::CancelNotifyAccessoryConnectionStatusChanged( dummy );//For iNotifyAccessoryConnectionStatusChanged
    CAccSrvSubConnection::CancelGetAccessoryConnectionStatus( dummy );//For iGetAccessoryConnectionStatus        

    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubConnection::~CAccSrvSubConnection - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvSubConnection::GetAccessoryConnectionStatusL
//
// Accessory connection status is valid when
// ASY(s) initialization is done (ECmdProcessCommandInit)
// i.e. ASY(s) informs AccessorySerever about possible connected accessory(/ies).
// Initialization is done during boot.
// -----------------------------------------------------------------------------
//
void CAccSrvSubConnection::GetAccessoryConnectionStatusL( const RMessage2& aMessage )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubConnection::GetAccessoryConnectionStatusL()" );

    if ( iServerModel->ASYsLoadCompleted() )
        {
        TAccPolGenericIDArray sourceGIDArray;
        iServerModel->CurrentConnectionStatusL( sourceGIDArray );
        TAccSrvGenericIDArrayPckg genericIdArrayPckg( sourceGIDArray );
        aMessage.WriteL( KAccServParamPosFirst, genericIdArrayPckg );
        }
    else
        {
        User::Leave( KErrNotReady );
        }

    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubConnection::GetAccessoryConnectionStatusL - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvSubConnection::GetAccessoryConnectionStatusAsyncL
//
// Asynchronous getter for Accessory connection status. If ASY(s) initialization
// is done, this request can be completed immediatelly.
// See CAccSrvSubConnection::GetAccessoryConnectionStatusL (synchronous).
// -----------------------------------------------------------------------------
//
void CAccSrvSubConnection::GetAccessoryConnectionStatusAsyncL( const RMessage2& aMessage )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubConnection::GetAccessoryConnectionStatusAsyncL()" );

    if ( iServerModel->ASYsLoadCompleted() )
        {
        TAccPolGenericIDArray sourceGIDArray;
        iServerModel->CurrentConnectionStatusL( sourceGIDArray );
        TAccSrvGenericIDArrayPckg genericIdArrayPckg( sourceGIDArray );
        aMessage.WriteL( KAccServParamPosFirst, genericIdArrayPckg );
        Write( aMessage, KAccSrvGetAccessoryConnectionStatusBit );
        aMessage.Complete( KErrNone );
        }
    else
        {
        iGetAccessoryConnectionStatus = aMessage;//Store RMessage2 for future completion
        iConnectionController->NotificationQueue().RegisterControlMessage( 
                                                        EAccessoryConnectionStatusAsyncChanged,
                                                        this );
        }

    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubConnection::GetAccessoryConnectionStatusAsyncL - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvSubConnection::NotifyAccessoryConnectionStatusChanged
// -----------------------------------------------------------------------------
//
void CAccSrvSubConnection::NotifyAccessoryConnectionStatusChanged( const RMessage2& aMessage )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubConnection::NotifyAccessoryConnectionStatusChanged()" );

    iConnectionController->NotificationQueue().RegisterControlMessage( 
                                                        EAccessoryConnectionStatusChanged,
                                                        this );
    iNotifyAccessoryConnectionStatusChanged = aMessage;//Store RMessage2 for future completion

    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubConnection::NotifyAccessoryConnectionStatusChanged - return void" );
    }

// -----------------------------------------------------------------------------
// CancelNotifyAccessoryConnectionStatusChanged
// -----------------------------------------------------------------------------
//
void CAccSrvSubConnection::CancelNotifyAccessoryConnectionStatusChanged(
    const RMessage2& /*aMessage*/ )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubConnection::CancelNotifyAccessoryConnectionStatusChanged()" );

    // If request is Null, there is nothing to cancel
    if ( !iNotifyAccessoryConnectionStatusChanged.IsNull() )
        {
        iConnectionController->NotificationQueue().CancelControlMessage( 
                                                        EAccessoryConnectionStatusChanged,
                                                        this );
        Write( iNotifyAccessoryConnectionStatusChanged, KAccSrvNotifyAccConnStatusChangedBit );
        iNotifyAccessoryConnectionStatusChanged.Complete( KErrCancel );
        }

    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubConnection::CancelNotifyAccessoryConnectionStatusChanged - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvSubConnection::CancelGetAccessoryConnectionStatus
// -----------------------------------------------------------------------------
//
void CAccSrvSubConnection::CancelGetAccessoryConnectionStatus(
    const RMessage2& /*aMessage*/ )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubConnection::CancelGetAccessoryConnectionStatus()" );

    // If request is Null, there is nothing to cancel
    if ( !iGetAccessoryConnectionStatus.IsNull() )
        {
        iConnectionController->NotificationQueue().CancelControlMessage( 
                                                        EAccessoryConnectionStatusAsyncChanged,
                                                        this );
        Write( iGetAccessoryConnectionStatus, KAccSrvGetAccessoryConnectionStatusBit );
        iGetAccessoryConnectionStatus.Complete( KErrCancel );
        }

    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubConnection::CancelGetAccessoryConnectionStatus - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvSubConnection::DispatchMessageL
// Handles client request. If request is not handled here, it is passed to 
// parent class for handling
// -----------------------------------------------------------------------------
//
void CAccSrvSubConnection::DispatchMessageL( const RMessage2& aMessage )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubConnection::DispatchMessageL()" );

    TInt function = aMessage.Function();

    switch ( function )
        {
        case EAccSrvGetAccessoryConnectionStatus:
            {
            GetAccessoryConnectionStatusL( aMessage );
            break;
            }
        case EAccSrvGetAccessoryConnectionStatusAsync:
            {
            GetAccessoryConnectionStatusAsyncL( aMessage );
            break;
            }
        case EAccSrvNotifyAccessoryConnectionStatusChanged:
            {
            NotifyAccessoryConnectionStatusChanged( aMessage );
            break;
            }
        case EAccSrvCancelNotifyAccessoryConnectionStatusChanged:
            {
            CancelNotifyAccessoryConnectionStatusChanged( aMessage );
            break;
            }
        case EAccSrvCancelGetAccessoryConnectionStatus:
            {
            CancelGetAccessoryConnectionStatus( aMessage );
            break;
            }
        default:
            {
            // Not handled here, pass to base class
            CAccSrvSubConnectionBase::DispatchMessageL( aMessage );
            break;
            }
        } // switch( function )

    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubConnection::DispatchMessageL - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvSubConnection::CompleteMessageL
// -----------------------------------------------------------------------------
//
TBool CAccSrvSubConnection::CompleteMessageL( TMsgID aMsgID,
                                              TInt aErrorCode,
                                              TInt /*aUniqueID*/ )
    {
    COM_TRACE_1( "[AccFW:AccServer] CAccSrvSubConnection::CompleteMessageL(%d)", aMsgID );

    if ( EAccessoryConnectionStatusChanged == aMsgID ) 
        {
        //Write new generic ID array and
        //complete outstanding AccessoryConnectionStatusChanged notification
        //CAccSrvSubBaseConnection::WriteL( iMessageArray[0], aGenericIDArray )
        TAccPolGenericIDArray genericIDArray;
        iServerModel->CurrentConnectionStatusL( genericIDArray );
        TPckg<TAccPolGenericIDArray> genericIDArrayPckg( genericIDArray );

        iNotifyAccessoryConnectionStatusChanged.WriteL( KAccServParamPosFirst, genericIDArrayPckg );
        Write( iNotifyAccessoryConnectionStatusChanged, KAccSrvNotifyAccConnStatusChangedBit );
        iNotifyAccessoryConnectionStatusChanged.Complete( aErrorCode );
        }
    else if ( EAccessoryConnectionStatusAsyncChanged == aMsgID )
        {
        //Write new generic ID array and
        //complete outstanding GetAccessoryConnectionStatus ( Async )
        TAccPolGenericIDArray genericIDArray;
        iServerModel->CurrentConnectionStatusL( genericIDArray );
        TAccSrvGenericIDArrayPckg genericIDArrayPckg( genericIDArray );

        iGetAccessoryConnectionStatus.WriteL( KAccServParamPosFirst, genericIDArrayPckg );
        Write( iGetAccessoryConnectionStatus, KAccSrvGetAccessoryConnectionStatusBit );
        iGetAccessoryConnectionStatus.Complete( aErrorCode );
        }
    else
        {
        TRACE_ASSERT_ALWAYS;
        User::Leave( KErrGeneral );
        }

    COM_TRACE_( "[AccFW:AccServer] CAccSrvSubConnection::CompleteMessageL - return" );

    return ETrue; // if it gets here, return ETrue
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File
