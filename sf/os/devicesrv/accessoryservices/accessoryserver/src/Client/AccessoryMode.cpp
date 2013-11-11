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
* Description:  Client-side handle to a sub-session of Accessory Mode
*
*/


// INCLUDE FILES
#include "AccessoryMode.h"
#include "AccessoryServer.h"
#include "AccClientServerMessages.h"
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
// RAccessoryMode::RAccessoryMode
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C RAccessoryMode::RAccessoryMode() :
    iNotifyAccessoryModeChangedPckg(
        *reinterpret_cast<TAccPolAccessoryMode*>( KDummyPointer ) ),
    iGetAccessoryModePckg(
        *reinterpret_cast<TAccPolAccessoryMode*>( KDummyPointer ) ),
    iFlags ( 0 ),
    iFlagsPckg( *reinterpret_cast<TInt*>( KDummyPointer ) ),
    iReserved1( *reinterpret_cast<TInt*>( KDummyPointer ) ),
    iReserved2( *reinterpret_cast<TInt*>( KDummyPointer ) )
    {
    API_TRACE_( "[AccFW:Client] RAccessoryMode::RAccessoryMode()" );
    }

// -----------------------------------------------------------------------------
// RAccessoryMode::CreateSubSession
// Creates the subsession
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RAccessoryMode::CreateSubSession(RAccessoryServer& aSession)
    {
    API_TRACE_( "[AccFW:Client] RAccessoryMode::CreateSubSession" );

    TInt ret = RSubSessionBase::CreateSubSession(
                                    aSession,
                                    EAccSrvCreateSubSessionAccessoryMode,
                                    TIpcArgs( TIpcArgs::ENothing ) );

    API_TRACE_1( "[AccFW:Client] RAccessoryMode::CreateSubSession - return 0x%x", ret );

    return ret;
    }

// -----------------------------------------------------------------------------
// RAccessoryMode::CloseSubSession
// Closes the subsession
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RAccessoryMode::CloseSubSession()
    {
    API_TRACE_( "[AccFW:Client] RAccessoryMode::CloseSubSession()" );

    RSubSessionBase::CloseSubSession( EAccSrvCloseSubSessionAccessoryMode );

    API_TRACE_( "[AccFW:Client] RAccessoryMode::CloseSubSession - return KErrNone" );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// RAccessoryMode::GetAccessoryMode
// Synchronous version.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RAccessoryMode::GetAccessoryMode(
    TAccPolAccessoryMode& aAccessoryMode ) const
    {
    API_TRACE_( "[AccFW:Client] RAccessoryMode::GetAccessoryMode()" );

    TAccSrvAccessoryModePckg accModePckg( aAccessoryMode );

    TInt err = SendReceive( EAccSrvGetAccessoryMode,
                            TIpcArgs( &accModePckg ) );

    API_TRACE_1( "[AccFW:Client] RAccessoryMode::GetAccessoryMode - return 0x%x", err );

    return err;
    }

// -----------------------------------------------------------------------------
// RAccessoryMode::GetAccessoryMode
// Asynchronous version.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void RAccessoryMode::GetAccessoryMode(
    TRequestStatus& aStatus,
    TAccPolAccessoryMode& aAccessoryMode )
    {
    API_TRACE_( "[AccFW:Client] RAccessoryMode::GetAccessoryMode() async" );

    if( iFlags & KAccSrvGetAccessoryModeBit )
        {
        API_TRACE_( "[AccFW:Client] RAccessoryMode::GetAccessoryMode - Request allready exists" );
        TRequestStatus* status = &aStatus;
        User::RequestComplete( status, KErrAlreadyExists );
        }
    else
        {
        TAccSrvAccessoryModePckg accessoryModePckg( aAccessoryMode );
        iGetAccessoryModePckg.Set( accessoryModePckg );

        iFlags += KAccSrvGetAccessoryModeBit;
        TPckg<TInt> flagsPckg( iFlags );
        iFlagsPckg.Set( flagsPckg );

        aStatus = KRequestPending;

        SendReceive( EAccSrvGetAccessoryModeAsync,
                     TIpcArgs( &iGetAccessoryModePckg, 
                               TIpcArgs::ENothing,
                               &iFlagsPckg ),
                     aStatus );
        }

    API_TRACE_( "[AccFW:Client] RAccessoryMode::GetAccessoryMode async - return void" );
    }

// -----------------------------------------------------------------------------
// RAccessoryMode::NotifyAccessoryModeChanged
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void RAccessoryMode::NotifyAccessoryModeChanged(
    TRequestStatus& aStatus,
    TAccPolAccessoryMode& aAccessoryMode )
    {
    API_TRACE_( "[AccFW:Client] RAccessoryMode::NotifyAccessoryModeChanged()" );

    if( iFlags & KAccSrvNotifyAccessoryModeChangedBit )
        {
        API_TRACE_( "[AccFW:Client] RAccessoryMode::NotifyAccessoryModeChanged - Request allready exists" );
        TRequestStatus* status = &aStatus;
        User::RequestComplete( status, KErrAlreadyExists );
        }
    else
        {
        TAccSrvAccessoryModePckg accessoryModePckg( aAccessoryMode );
        iNotifyAccessoryModeChangedPckg.Set( accessoryModePckg );

        iFlags += KAccSrvNotifyAccessoryModeChangedBit;
        TPckg<TInt> flagsPckg( iFlags );
        iFlagsPckg.Set( flagsPckg );

        aStatus = KRequestPending;

        SendReceive( EAccSrvNotifyAccessoryModeChanged,
                     TIpcArgs( &iNotifyAccessoryModeChangedPckg,
                               TIpcArgs::ENothing,
                               &iFlagsPckg ),
                     aStatus );
        }

    API_TRACE_( "[AccFW:Client] RAccessoryMode::NotifyAccessoryModeChanged - return void" );
    }

// -----------------------------------------------------------------------------
// RAccessoryMode::CancelNotifyAccessoryModeChanged
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void RAccessoryMode::CancelNotifyAccessoryModeChanged() const
    {
    API_TRACE_( "[AccFW:Client] RAccessoryMode::CancelNotifyAccessoryModeChanged()" );

    SendReceive( EAccSrvCancelNotifyAccessoryModeChanged,
                 TIpcArgs( TIpcArgs::ENothing ) );

    API_TRACE_( "[AccFW:Client] RAccessoryMode::CancelNotifyAccessoryModeChanged - return void" );
    }

// -----------------------------------------------------------------------------
// RAccessoryMode::CancelGetAccessoryMode
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void RAccessoryMode::CancelGetAccessoryMode() const
    {
    API_TRACE_( "[AccFW:Client] RAccessoryMode::CancelGetAccessoryMode()" );

    SendReceive( EAccSrvCancelGetAccessoryMode,
                 TIpcArgs( TIpcArgs::ENothing ) );

    API_TRACE_( "[AccFW:Client] RAccessoryMode::CancelGetAccessoryMode - return void" );
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

// End of File
