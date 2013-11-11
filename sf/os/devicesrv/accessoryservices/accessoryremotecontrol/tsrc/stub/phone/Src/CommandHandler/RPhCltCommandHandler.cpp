/*
* Copyright (c) 2003-2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This class is used to perform command handler originated 
*                commands.
*
*/



// INCLUDES

#include    "RPhCltCommandHandler.h"
#include    <RPhCltServer.h>
#include    <PhCltTypes.h>
#include    "PhCltClientServer.h"
#include    <e32std.h>
#include    <e32base.h>



// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// RPhCltCommandHandler::RPhCltCommandHandler
// 
// C++ constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
RPhCltCommandHandler::RPhCltCommandHandler()
:   RSubSessionBase(),
    iComHandParams( TPhCltComHandCommandParameters() )
    {
    }


// -----------------------------------------------------------------------------
// RPhCltCommandHandler::Open
// 
// Open subsession to Phone Server.
// -----------------------------------------------------------------------------
//
TInt RPhCltCommandHandler::Open( RPhCltServer& aServer )
    {
    __ASSERT_ALWAYS( aServer.Handle(), User::Panic( 
            KPhClientPanicCategory, 
            EPhCltClientSidePanicNullHandle ) );

    return CreateSubSession( 
        aServer, 
        EPhoneServerComHandSubSessionOpen, 
        TIpcArgs() );
    }


// -----------------------------------------------------------------------------
// RPhCltCommandHandler::Close
// 
// Close subsession.
// -----------------------------------------------------------------------------
//
void RPhCltCommandHandler::Close()
    {
    CloseSubSession( EPhoneServerComHandSubSessionClose );
    }

// -----------------------------------------------------------------------------
// RPhCltCommandHandler::Atd
// 
// Dial. Only voice call is supported.
// -----------------------------------------------------------------------------
//
void RPhCltCommandHandler::Atd(
    TRequestStatus& aStatus, 
    const TPhCltTelephoneNumber& aTPhCltTelephoneNumber )
    {
    __ASSERT_ALWAYS( SubSessionHandle(), User::Panic( 
        KPhClientPanicCategory, EPhCltClientSidePanicNullHandle ) );

    // This is Atd function call.
    iComHandParams().iCommandHandlerCommand = EPhCltCommandAtd;
	iComHandParams().iTelNumber = aTPhCltTelephoneNumber;
    SendReceive( 
        EPhoneServerComHandSubSessionAtd, 
        TIpcArgs( &iComHandParams ), 
        aStatus );
    }

// -----------------------------------------------------------------------------
// RPhCltCommandHandler::Ata
// 
// Answer the call. 
// -----------------------------------------------------------------------------
//
void RPhCltCommandHandler::Ata(
    TRequestStatus& aStatus )
    {
    __ASSERT_ALWAYS( 
        SubSessionHandle(), User::Panic( 
            KPhClientPanicCategory, EPhCltClientSidePanicNullHandle ) );

    // This is Ata function call.
    iComHandParams().iCommandHandlerCommand = EPhCltCommandAta;

    // Ata method does not have parameters <=> Uniformity reason for this.
    SendReceive( 
        EPhoneServerComHandSubSessionAta, 
        TIpcArgs( &iComHandParams ),
        aStatus );
    }


// -----------------------------------------------------------------------------
// RPhCltCommandHandler::Chld
// 
// Call hold and multiparty handling.
// -----------------------------------------------------------------------------
//
void RPhCltCommandHandler::Chld(
    TRequestStatus& aStatus, 
    const TPhCltChldCommand aChldCommand,
    const TUint aCallNumber )
    {
    __ASSERT_ALWAYS( 
        SubSessionHandle(), User::Panic( 
            KPhClientPanicCategory, EPhCltClientSidePanicNullHandle ) );

    // Save the chld arguments.
    iComHandParams().iChldCommand    = aChldCommand;
    iComHandParams().iChldCallNumber = aCallNumber;
    
    // This is Chld function call.
    iComHandParams().iCommandHandlerCommand = EPhCltCommandChld;

    SendReceive( 
        EPhoneServerComHandSubSessionChld, 
        TIpcArgs( &iComHandParams ),
        aStatus );
    }


// -----------------------------------------------------------------------------
// RPhCltCommandHandler::Chup
// 
// Hangup current call.
// -----------------------------------------------------------------------------
//
void RPhCltCommandHandler::Chup(
    TRequestStatus& aStatus )
    {
    __ASSERT_ALWAYS( 
        SubSessionHandle(), User::Panic( 
            KPhClientPanicCategory, EPhCltClientSidePanicNullHandle ) );

    // This is Chup function call.
    iComHandParams().iCommandHandlerCommand = EPhCltCommandChup;

    // Chup method does not have parameters <=> Uniformity reason for this.
    SendReceive( 
        EPhoneServerComHandSubSessionChup, 
        TIpcArgs( &iComHandParams ),
        aStatus );
    }


// -----------------------------------------------------------------------------
// RPhCltCommandHandler::Vts
// 
// DTMF sending, starting and stopping in same function. 
// -----------------------------------------------------------------------------
//
void RPhCltCommandHandler::Vts(
    TRequestStatus& aStatus, 
    const TPhCltDtmfTone aDtmfTone,
    const TPhCltDtmfAction aAction )
    {
    __ASSERT_ALWAYS( 
        SubSessionHandle(), User::Panic( 
            KPhClientPanicCategory, EPhCltClientSidePanicNullHandle ) );

    // Save the Vts arguments.
    iComHandParams().iDtmfTone   = aDtmfTone;
    iComHandParams().iDtmfAction = aAction;
    
    // This is Vts function call.
    iComHandParams().iCommandHandlerCommand = EPhCltCommandVts;

    SendReceive( 
        EPhoneServerComHandSubSessionVts, 
        TIpcArgs( &iComHandParams ),
        aStatus );
    }


// -----------------------------------------------------------------------------
// RPhCltCommandHandler::Bvra
// 
// Bluetooth Voice Recognition Activation.
// -----------------------------------------------------------------------------
//
void RPhCltCommandHandler::Bvra( 
    TRequestStatus& aStatus, 
    const TBool aActivation )
    {
    __ASSERT_ALWAYS( SubSessionHandle(), User::Panic( 
        KPhClientPanicCategory, EPhCltClientSidePanicNullHandle ) );

    // Save the Bvra arguments.
    iComHandParams().iBvraActivation = aActivation;

    // This is Bvra function call.
    iComHandParams().iCommandHandlerCommand = EPhCltCommandBvra;

    SendReceive( 
        EPhoneServerComHandSubSessionBvra, 
        TIpcArgs( &iComHandParams ),
        aStatus );
    }


// -----------------------------------------------------------------------------
// RPhCltCommandHandler::CancelAsyncRequest
// 
// Cancel the ongoing asynchronous request.
// -----------------------------------------------------------------------------
//
void RPhCltCommandHandler::CancelAsyncRequest( 
    const TPhCltComHandCommand aReqToCancel )
    {
    __ASSERT_ALWAYS( SubSessionHandle(), User::Panic( 
        KPhClientPanicCategory, EPhCltClientSidePanicNullHandle ) );

    // Save the cancel arguments.
    iComHandParams().iCommandHandlerCommand = aReqToCancel;

    SendReceive( 
        EPhoneServerComHandSubSessionCancel, 
        TIpcArgs( &iComHandParams ) );
    }

// -----------------------------------------------------------------------------
// RPhCltCommandHandler::MuteMicrophone
// 
// -----------------------------------------------------------------------------
//
void RPhCltCommandHandler::MuteMicrophone(
    TRequestStatus& aStatus, const TBool aMute )
    {
    __ASSERT_ALWAYS( SubSessionHandle(), User::Panic( 
        KPhClientPanicCategory, EPhCltClientSidePanicNullHandle ) );
    
    // Save the MuteMic arguments.
    iComHandParams().iMute = aMute;

     // This is MuteMic function call.
    iComHandParams().iCommandHandlerCommand = EPhCltCommandMuteMic;

    // MuteMic method does not have parameters <=> Uniformity reason for this.
    SendReceive( 
        EPhoneServerComHandSubSessionMuteMic, 
        TIpcArgs( &iComHandParams ),
        aStatus );

    }

// -----------------------------------------------------------------------------
// RPhCltCommandHandler::MuteRingingTone
// 
// -----------------------------------------------------------------------------
//
void RPhCltCommandHandler::MuteRingingTone(
    TRequestStatus& aStatus )
    {
    __ASSERT_ALWAYS( SubSessionHandle(), User::Panic( 
        KPhClientPanicCategory, EPhCltClientSidePanicNullHandle ) );

     // This is MuteRingingTone function call.
    iComHandParams().iCommandHandlerCommand = EPhCltCommandMuteRingingTone;

    // MuteRingingTone method does not have parameters <=> Uniformity reason for this.
    SendReceive( 
        EPhoneServerComHandSubSessionMuteRingingTone, 
        TIpcArgs( &iComHandParams ),
        aStatus );

    }    
            
// End of File
