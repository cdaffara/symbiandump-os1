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
* Description:  USSD handler class.
*
*/


// INCLUDE FILES
#include "RPhCltServer.h"
#include "RPhCltUssd.h"
#include "PhCltClientServer.h"

// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// RPhCltUssd::RPhCltUssd
// Constructor
// 
// -----------------------------------------------------------------------------
//
RPhCltUssd::RPhCltUssd()
    : RSubSessionBase()
    {
    }


// -----------------------------------------------------------------------------
// RPhCltUssd::Open
// Open USSD subsession to the Phone Server.
// 
// -----------------------------------------------------------------------------
//
TInt RPhCltUssd::Open( RPhCltServer& aServer )
    {
    return CreateSubSession( 
        aServer,
        EPhoneServerUSSDSubSessionOpen,
        TIpcArgs() );
    }


// -----------------------------------------------------------------------------
// RPhCltUssd::Close
// 
// Closes the subsession
// -----------------------------------------------------------------------------
//
void RPhCltUssd::Close()
    {    
    CloseSubSession( EPhoneServerUSSDSubSessionClose );
    }


// -----------------------------------------------------------------------------
// RPhCltUssd::StartUssdEditor
// 
// Request Ussd App to start
// ---------------------------------------------------------------------------
//
TInt RPhCltUssd::StartUssdEditor() const
    {
    return SendReceive(
        EPhoneServerUSSDSubSessionStartEditing,
        TIpcArgs() );
    }


// -----------------------------------------------------------------------------
// RPhCltUssd::AppStarting
// 
// App informs the server about the start-up.
// -----------------------------------------------------------------------------
//
TInt RPhCltUssd::AppStarting()
    {
    return SendReceive(
        EPhoneServerUSSDSubSessionAppStarting,
        TIpcArgs() );
    }


// -----------------------------------------------------------------------------
// RPhCltUssd::AppTerminating
// 
// App informs the server about the termination
// -----------------------------------------------------------------------------
//
TInt RPhCltUssd::AppTerminating( 
    TPhCltUssdAppExitReason aExitReason )
    {
    TPckg< TPhCltUssdAppExitReason > exitReasonPckg( aExitReason );

    return SendReceive(
        EPhoneServerUSSDSubSessionAppTerminating,
        TIpcArgs( &exitReasonPckg ) );
    }


// -----------------------------------------------------------------------------
// RPhCltUssd::AppToForeground
// 
// App informs the server about the foreground event
// -----------------------------------------------------------------------------
//
TBool RPhCltUssd::AppToForeground()
    {
    TInt ret = SendReceive(
        EPhoneServerUSSDSubSessionAppToForeground, 
        TIpcArgs() );
    if ( ret > 0 )
        {
        return ETrue;
        }
    return EFalse;
    }


// -----------------------------------------------------------------------------
// RPhCltUssd::AppToBackgroud
// 
// App informs the server about the background event
// -----------------------------------------------------------------------------
//
TInt RPhCltUssd::AppToBackground()
    {
    return SendReceive(
        EPhoneServerUSSDSubSessionAppToBackground,
        TIpcArgs() );
    }


// -----------------------------------------------------------------------------
// RPhCltUssd::SendUssd
// 
// Send Ussd string to the server - async
// -----------------------------------------------------------------------------
//
void RPhCltUssd::SendUssd(
    TRequestStatus& aReqStatus, 
    const TDesC8& aMsgData, 
    const TDesC8& aMsgAttribute )
    {
    SendReceive(
        EPhoneServerUSSDSubSessionSendMessage,
        TIpcArgs( 
            &aMsgData,
            aMsgData.Length(),
            &aMsgAttribute ),
        aReqStatus );
    }


// -----------------------------------------------------------------------------
// RPhCltUssd::CancelSend
// 
// Cancels the send
// -----------------------------------------------------------------------------
//
void RPhCltUssd::CancelSend()
    {
    SendReceive(
        EPhoneServerUSSDSubSessionSendMessageCancel,
        TIpcArgs() );
    }


// -----------------------------------------------------------------------------
// RPhCltUssd::StartSAT
// 
// Starts SAT interaction.
// -----------------------------------------------------------------------------
//
void RPhCltUssd::StartSAT( 
    TRequestStatus& aStatus , 
    TDes& aReceiveMessage, 
    TPckg< TUint >& aShowNotesAndDcs )
    {
    __ASSERT_ALWAYS( 
        SubSessionHandle() , 
        User::Panic( KPhClientPanicCategory , KErrBadHandle ) );

    SendReceive(
        EPhoneServerUSSDSubSessionStartSAT,
        TIpcArgs(
            &aShowNotesAndDcs,
            aReceiveMessage.MaxLength(),
            &aReceiveMessage ),
        aStatus );
    }


// -----------------------------------------------------------------------------
// RPhCltUssd::StopSAT
// 
// End SAT interaction.
// -----------------------------------------------------------------------------
//
void RPhCltUssd::StopSAT()
    {
    __ASSERT_ALWAYS( 
        SubSessionHandle() , 
        User::Panic( KPhClientPanicCategory , KErrBadHandle ) );

    SendReceive(
        EPhoneServerUSSDSubSessionStopSAT,
        TIpcArgs() );
    }


// End of File
