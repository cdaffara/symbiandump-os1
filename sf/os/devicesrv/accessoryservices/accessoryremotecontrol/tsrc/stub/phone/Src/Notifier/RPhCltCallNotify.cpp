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
#include    "RPhCltServer.h"


// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// Stub function to complete an asynchronous request
//
// -----------------------------------------------------------------------------
//
static void CompleteRequest( TRequestStatus& aStatus, TInt aCompletionCode )
    {
    TRequestStatus* status = &aStatus;
    User::RequestComplete( status, aCompletionCode );
    }


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// RPhCltCallNotify::RPhCltCallNotify
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C RPhCltCallNotify::RPhCltCallNotify() 
    : RSubSessionBase()
    {
    }


// -----------------------------------------------------------------------------
// RPhCltCallNotify::Open
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
// RPhCltCallNotify::Close
// -----------------------------------------------------------------------------
//
EXPORT_C void RPhCltCallNotify::Close()
    {
    CloseSubSession( EPhoneServerNotifySubSessionClose );
    }


// -----------------------------------------------------------------------------
// RPhCltCallNotify::NotifyCallAttempt
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
// RPhCltCallNotify::CancelNotifyCallAttempt
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
// RPhCltCallNotify::NotifyPhoneStatusRequest
// -----------------------------------------------------------------------------
//
EXPORT_C void RPhCltCallNotify::NotifyPhoneStatusRequest( 
    TRequestStatus& aStatus )
    {
    // Stub
    __ASSERT_DEBUG( EFalse, User::Invariant() );
    CompleteRequest( aStatus, KErrNone );
    }


// -----------------------------------------------------------------------------
// RPhCltCallNotify::CancelNotifyPhoneStatusRequest
//
// NOT SUPPORTED.
// -----------------------------------------------------------------------------
//
EXPORT_C void RPhCltCallNotify::CancelNotifyPhoneStatusRequest() const
    {
    // Stub, don't call
    __ASSERT_DEBUG( EFalse, User::Invariant() );
    }


// -----------------------------------------------------------------------------
// RPhCltCallNotify::ReportPhoneStatus
// 
// -----------------------------------------------------------------------------
//
EXPORT_C void RPhCltCallNotify::ReportPhoneStatus( 
    TRequestStatus& aStatus,
    const TInt aPhoneStatus )
    {
    // Stub
    __ASSERT_DEBUG( EFalse, User::Invariant() );
    CompleteRequest( aStatus, aPhoneStatus );
    }


// -----------------------------------------------------------------------------
// RPhCltCallNotify::CancelReportPhoneStatus
// 
// NOT SUPPORTED.
// -----------------------------------------------------------------------------
//
EXPORT_C void RPhCltCallNotify::CancelReportPhoneStatus() const
    {
    // Stub, don't call
    __ASSERT_DEBUG( EFalse, User::Invariant() );
    }


// -----------------------------------------------------------------------------
// RPhCltCallNotify::RespondClient
// 
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RPhCltCallNotify::RespondClient( const TInt aResultCode )
    {
    return Send( 
        EPhoneServerNotifySubSessionReportCallResult,
        TIpcArgs( aResultCode ) );
    }

// End of File
