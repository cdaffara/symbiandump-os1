/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Client-side handle to a sub-session of Accessory Connection
*
*/


// INCLUDE FILES
#include "AccessoryConnection.h"
#include "AccClientServerMessages.h"
#include "AccessoryServer.h"
#include "AccPolGenericIDArray.h"
#include "acc_debug.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS
const TUint8 KDummyPointer = 0xff;

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// RAccessoryConnection::RAccessoryConnection
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C RAccessoryConnection::RAccessoryConnection() :
    iNotifyConnectionStatusChangedGIDArrayPckg(
                *reinterpret_cast<TAccPolGenericIDArray*>( KDummyPointer ) ),
    iGetConnectionStatusGIDArrayPckg(
                *reinterpret_cast<TAccPolGenericIDArray*>( KDummyPointer ) ),
    iFlags ( 0 ),
    iFlagsPckg( *reinterpret_cast<TInt*>( KDummyPointer ) ),
    iReserved1( *reinterpret_cast<TInt*>( KDummyPointer ) ),
    iReserved2( *reinterpret_cast<TInt*>( KDummyPointer ) )
    {
    API_TRACE_( "[AccFW:Client] RAccessoryConnection::RAccessoryConnection" );
    }

// -----------------------------------------------------------------------------
// RAccessoryConnection::CreateSubSession
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RAccessoryConnection::CreateSubSession(RAccessoryServer& aSession)
    {
    API_TRACE_( "[AccFW:Client] RAccessoryConnection::CreateSubSession()" );

    TInt ret = RSubSessionBase::CreateSubSession(
                                   aSession,
                                   EAccSrvCreateSubSessionAccessoryConnection,
                                   TIpcArgs( TIpcArgs::ENothing ) );

    API_TRACE_1( "[AccFW:Client] RAccessoryConnection::CreateSubSession return 0x%x", ret );

    return ret;
    }

// -----------------------------------------------------------------------------
// RAccessoryConnection::CloseSubSession
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RAccessoryConnection::CloseSubSession()
    {
    API_TRACE_( "[AccFW:Client] RAccessoryConnection::CloseSubSession()" );

    RSubSessionBase::CloseSubSession( EAccSrvCloseSubSessionAccessoryConnection );

    API_TRACE_( "[AccFW:Client] RAccessoryConnection::CloseSubSession - return KErrNone" );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// RAccessoryConnection::GetAccessoryConnectionStatus
//
// Synchronous version.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RAccessoryConnection::GetAccessoryConnectionStatus(
    TAccPolGenericIDArray& aGenericIdArray ) const
    {
    API_TRACE_( "[AccFW:Client] RAccessoryConnection::GetAccessoryConnectionStatus()" );

    TAccSrvGenericIDArrayPckg genericIdArrayPckg( aGenericIdArray );

    TInt err = SendReceive( EAccSrvGetAccessoryConnectionStatus,
                            TIpcArgs( &genericIdArrayPckg ) );

    API_TRACE_1( "[AccFW:Client] RAccessoryConnection::GetAccessoryConnectionStatus - return 0x%x", err );

    return err;
    }

// -----------------------------------------------------------------------------
// RAccessoryConnection::GetAccessoryConnectionStatus
//
// Asynchronous version.
// -----------------------------------------------------------------------------
//
EXPORT_C void RAccessoryConnection::GetAccessoryConnectionStatus(
    TRequestStatus& aStatus,
    TAccPolGenericIDArray& aGenericIdArray )
    {
    API_TRACE_( "[AccFW:Client] RAccessoryConnection::GetAccessoryConnectionStatus() async" );

    if( iFlags & KAccSrvGetAccessoryConnectionStatusBit )
        {
        API_TRACE_( "[AccFW:Client] RAccessoryConnection::GetAccessoryConnectionStatus - Request allready exists" );
        TRequestStatus* status = &aStatus;
        User::RequestComplete( status, KErrAlreadyExists );
        }
    else
        {
        TAccSrvGenericIDArrayPckg genericIDArrayPckg( aGenericIdArray );
        iGetConnectionStatusGIDArrayPckg.Set( genericIDArrayPckg );

        iFlags += KAccSrvGetAccessoryConnectionStatusBit;
        TPckg<TInt> flagsPckg( iFlags );
        iFlagsPckg.Set( flagsPckg );

        aStatus = KRequestPending;

        SendReceive( EAccSrvGetAccessoryConnectionStatusAsync,
                     TIpcArgs( &iGetConnectionStatusGIDArrayPckg,
                               TIpcArgs::ENothing,
                               &iFlagsPckg ),
                     aStatus );
        }

    API_TRACE_( "[AccFW:Client] RAccessoryConnection::GetAccessoryConnectionStatus async - return void" );
    }

// -----------------------------------------------------------------------------
// RAccessoryConnection::NotifyAccessoryConnectionStatusChanged
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void RAccessoryConnection::NotifyAccessoryConnectionStatusChanged(
    TRequestStatus& aStatus,
    TAccPolGenericIDArray& aGenericIdArray )
    {
    API_TRACE_( "[AccFW:Client] RAccessoryConnection::NotifyAccessoryConnectionStatusChanged()" );

    if( iFlags & KAccSrvNotifyAccConnStatusChangedBit )
        {
        API_TRACE_( "[AccFW:Client] RAccessoryConnection::NotifyAccessoryConnectionStatusChanged - Request allready exists" );
        TRequestStatus* status = &aStatus;
        User::RequestComplete( status, KErrAlreadyExists );
        }
    else
        {
        TAccSrvGenericIDArrayPckg genericIDArray( aGenericIdArray );
        iNotifyConnectionStatusChangedGIDArrayPckg.Set( genericIDArray );

        iFlags += KAccSrvNotifyAccConnStatusChangedBit;
        TPckg<TInt> flagsPckg( iFlags );
        iFlagsPckg.Set( flagsPckg );

        aStatus = KRequestPending;

        SendReceive( EAccSrvNotifyAccessoryConnectionStatusChanged,
                     TIpcArgs( &iNotifyConnectionStatusChangedGIDArrayPckg,
                               TIpcArgs::ENothing,
                               &iFlagsPckg ),
                     aStatus );
        }

    API_TRACE_( "[AccFW:Client] RAccessoryConnection::NotifyAccessoryConnectionStatusChanged - return void" );
    }

// -----------------------------------------------------------------------------
// RAccessoryConnection::CancelNotifyAccessoryConnectionStatusChanged
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void RAccessoryConnection::CancelNotifyAccessoryConnectionStatusChanged() const
    {
    API_TRACE_( "[AccFW:Client] RAccessoryConnection::CancelNotifyAccessoryConnectionStatusChanged()" );

    SendReceive( EAccSrvCancelNotifyAccessoryConnectionStatusChanged,
                 TIpcArgs( TIpcArgs::ENothing ) );

    API_TRACE_( "[AccFW:Client] RAccessoryConnection::CancelNotifyAccessoryConnectionStatusChanged - return void" );
    }

// -----------------------------------------------------------------------------
// RAccessoryConnection::CancelGetAccessoryConnectionStatus
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void RAccessoryConnection::CancelGetAccessoryConnectionStatus() const
    {
    API_TRACE_( "[AccFW:Client] RAccessoryConnection::CancelGetAccessoryConnectionStatus()" );

    SendReceive( EAccSrvCancelGetAccessoryConnectionStatus,
                 TIpcArgs( TIpcArgs::ENothing ) );

    API_TRACE_( "[AccFW:Client] RAccessoryConnection::CancelGetAccessoryConnectionStatus - return void" );
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

// End of File
