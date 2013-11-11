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
* Description:  Call Notify
*
*/



//  Include Files

#include    "RPhCltCallNotify.h"
#include    "PhCltClientServer.h"
#include    "RPhCltServer.h"



// ============================= LOCAL FUNCTIONS ===============================



// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// RPhCltExtCallNotify::RPhCltExtCallNotify
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C RPhCltCallNotify::RPhCltCallNotify() 
    : RSubSessionBase()
    {
    }


// -----------------------------------------------------------------------------
// RPhCltExtCallNotify::Open
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RPhCltCallNotify::Open( RPhCltServer& aServer )
    {
    return CreateSubSession( 
        aServer,
        EPhoneServerNotifySubSessionOpen,
        TIpcArgs() );
    }


// -----------------------------------------------------------------------------
// RPhCltExtCallNotify::Close
// -----------------------------------------------------------------------------
//
EXPORT_C void RPhCltCallNotify::Close()
    {
    CloseSubSession( EPhoneServerNotifySubSessionClose );
    }


// -----------------------------------------------------------------------------
// RPhCltExtCallNotify::NotifyCallAttempt
// 
// -----------------------------------------------------------------------------
//
EXPORT_C void RPhCltCallNotify::NotifyCallAttempt( 
    TRequestStatus& aStatus,
    TDes8& aCallArgs )
    {
    SendReceive( 
        EPhoneServerNotifySubSessionNotifyCallRequest, 
        TIpcArgs( &aCallArgs ), 
        aStatus );
    }


// -----------------------------------------------------------------------------
// RPhCltExtCallNotify::CancelNotifyCallAttempt
//
// -----------------------------------------------------------------------------
//
EXPORT_C void RPhCltCallNotify::CancelNotifyCallAttempt() const
    {
    SendReceive( 
        EPhoneServerNotifySubSessionNotifyCallRequestCancel,
        TIpcArgs() );
    }


// -----------------------------------------------------------------------------
// RPhCltExtCallNotify::RespondClient
// 
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RPhCltCallNotify::RespondClient( const TInt aResultCode )
    {
    return Send( 
        EPhoneServerNotifySubSessionReportCallResult,
        TIpcArgs( aResultCode ) );
    }
  
 // -----------------------------------------------------------------------------
// RPhCltExtCallNotify::NotifyComHandCommand
// 
// Notifies when a Command handler request arrives.
// -----------------------------------------------------------------------------
//
EXPORT_C void RPhCltCallNotify::NotifyComHandCommand( 
    TRequestStatus& aStatus, 
    TDes8& aCommandArgs )
    {
    SendReceive( 
        EPhoneServerComHandNotifySubSessionComHandRequest, 
        TIpcArgs( &aCommandArgs ),
        aStatus );
    }


// -----------------------------------------------------------------------------
// RPhCltExtCallNotify::CancelNotifyComHandCommand
// 
// Cancel pending command handler request notification.
// -----------------------------------------------------------------------------
//
EXPORT_C void RPhCltCallNotify::CancelNotifyComHandCommand() const
    {
    SendReceive( 
        EPhoneServerComHandNotifySubSessionComHandRequestCancel, 
        TIpcArgs() );
    }


// -----------------------------------------------------------------------------
// RPhCltExtCallNotify::RespondComHandClient
// 
// Respond to client's command handler request.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RPhCltCallNotify::RespondComHandClient( 
    const TInt aResultCode )
    {
    return Send( 
        EPhoneServerComHandNotifySubSessionReportComHandResult, 
        TIpcArgs( aResultCode ) );
    }


// End of File
