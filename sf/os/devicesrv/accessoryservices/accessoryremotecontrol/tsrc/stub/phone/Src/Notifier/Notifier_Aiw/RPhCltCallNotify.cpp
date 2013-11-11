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
#include "RPhCltCallNotify.h"
#include "RPhCltServer.h"
#include    "PhCltClientServer.h"

// ============================= LOCAL FUNCTIONS ===============================



// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// RPhCltCallNotify::RPhCltCallNotify
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
RPhCltCallNotify::RPhCltCallNotify() 
    : RSubSessionBase()
    {
    }


// -----------------------------------------------------------------------------
// RPhCltCallNotify::Open
// -----------------------------------------------------------------------------
//
TInt RPhCltCallNotify::Open( RPhCltServer& aServer )
    {
    return CreateSubSession( 
        aServer,
        EPhoneServerNotifySubSessionOpen,
        TIpcArgs() );
    }


// -----------------------------------------------------------------------------
// RPhCltCallNotify::Close
// -----------------------------------------------------------------------------
//
void RPhCltCallNotify::Close()
    {
    CloseSubSession( EPhoneServerNotifySubSessionClose );
    }


// -----------------------------------------------------------------------------
// RPhCltCallNotify::NotifyCallAttempt
// 
// -----------------------------------------------------------------------------
//
void RPhCltCallNotify::NotifyCallAttempt( 
    TRequestStatus& aStatus,
    TPckg< TInt >& aDataLength )
    {
    SendReceive( 
        EPhoneServerNotifySubSessionNotifyCallRequest, 
        TIpcArgs( &aDataLength ), 
        aStatus );
    }


// -----------------------------------------------------------------------------
// RPhCltCallNotify::CancelNotifyCallAttempt
//
// -----------------------------------------------------------------------------
//
void RPhCltCallNotify::CancelNotifyCallAttempt() const
    {
    SendReceive( 
        EPhoneServerNotifySubSessionNotifyCallRequestCancel,
        TIpcArgs() );
    }


// -----------------------------------------------------------------------------
// RPhCltCallNotify::RespondClient
// 
// -----------------------------------------------------------------------------
//
TInt RPhCltCallNotify::RespondClient( const TInt aResultCode )
    {
    return Send( 
        EPhoneServerNotifySubSessionReportCallResult,
        TIpcArgs( aResultCode ) );
    }
 
 // -----------------------------------------------------------------------------
// RPhCltCallNotify::GetDialData
// 
// -----------------------------------------------------------------------------
//   
 void RPhCltCallNotify::GetDialData( TDes8& aData )
    {
    
    SendReceive( 
        EPhoneServerNotifySubSessionDialData, 
        TIpcArgs( &aData) );
    }

// -----------------------------------------------------------------------------
// RPhCltCallNotify::NotifyEmergencyCall
// 
// -----------------------------------------------------------------------------
//
void RPhCltCallNotify::NotifyEmergencyCall( 
    TRequestStatus& aStatus,
    TDes8& aEmergencyNumber
     )
    {
    SendReceive( 
        EPhoneServerNotifyEmergencySubSessionNotifyCallRequest, 
        TIpcArgs( &aEmergencyNumber ), 
        aStatus );
    }

// -----------------------------------------------------------------------------
// RPhCltCallNotify::CancelNotifyEmergencyCall
//
// -----------------------------------------------------------------------------
//
void RPhCltCallNotify::CancelNotifyEmergencyCall() const
    {
    SendReceive( 
        EPhoneServerNotifyEmergencySubSessionNotifyCallRequestCancel,
        TIpcArgs() );
    }


// -----------------------------------------------------------------------------
// RPhCltCallNotify::RespondEmergencyToClient
// 
// -----------------------------------------------------------------------------
//
TInt RPhCltCallNotify::RespondEmergencyToClient( const TInt aResultCode )
    {
    return Send( 
        EPhoneServerNotifyEmergencySubSessionReportCallResult,
        TIpcArgs( aResultCode ) );
    }

 // -----------------------------------------------------------------------------
// RPhCltCallNotify::NotifyComHandCommand
// 
// Notifies when a Command handler request arrives.
// -----------------------------------------------------------------------------
//
void RPhCltCallNotify::NotifyComHandCommand( 
    TRequestStatus& aStatus, 
    TDes8& aCommandArgs )
    {
    SendReceive( 
        EPhoneServerComHandNotifySubSessionComHandRequest, 
        TIpcArgs( &aCommandArgs ),
        aStatus );
    }


// -----------------------------------------------------------------------------
// RPhCltCallNotify::CancelNotifyComHandCommand
// 
// Cancel pending command handler request notification.
// -----------------------------------------------------------------------------
//
void RPhCltCallNotify::CancelNotifyComHandCommand() const
    {
    SendReceive( 
        EPhoneServerComHandNotifySubSessionComHandRequestCancel, 
        TIpcArgs() );
    }


// -----------------------------------------------------------------------------
// RPhCltCallNotify::RespondComHandClient
// 
// Respond to client's command handler request.
// -----------------------------------------------------------------------------
//
TInt RPhCltCallNotify::RespondComHandClient( 
    const TInt aResultCode )
    {
    return Send( 
        EPhoneServerComHandNotifySubSessionReportComHandResult, 
        TIpcArgs( aResultCode ) );
    }

// End of File
