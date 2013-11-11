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
* Description:  Observer getting notifications about command handler requests
*                from Phone Server.
*
*/



//  INCLUDES

#include    "RPhCltCommandHandlerNotify.h"
#include    <RPhCltServer.h>
#include    <PhCltTypes.h>
#include    "PhCltClientServer.h"
#include    <e32base.h>



// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// RPhCltCommandHandlerNotify::RPhCltCommandHandlerNotify
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
RPhCltCommandHandlerNotify::RPhCltCommandHandlerNotify()
:   RSubSessionBase()
    {
    }


// -----------------------------------------------------------------------------
// RPhCltCommandHandlerNotify::Open
// 
// Open subsession to Phone Server.
// -----------------------------------------------------------------------------
//
TInt RPhCltCommandHandlerNotify::Open( RPhCltServer& aServer )
    {
    return CreateSubSession( 
        aServer, 
        EPhoneServerComHandNotifySubSessionOpen, 
        TIpcArgs() );
    }


// -----------------------------------------------------------------------------
// RPhCltCommandHandlerNotify::Close
// 
// Close subsession.
// -----------------------------------------------------------------------------
//
void RPhCltCommandHandlerNotify::Close()
    {
    CloseSubSession( EPhoneServerComHandNotifySubSessionClose );
    }
    

// -----------------------------------------------------------------------------
// RPhCltCommandHandlerNotify::NotifyComHandCommand
// 
// Notifies when a Command handler request arrives.
// -----------------------------------------------------------------------------
//
void RPhCltCommandHandlerNotify::NotifyComHandCommand( 
    TRequestStatus& aStatus, 
    TDes8& aCommandArgs )
    {
    SendReceive( 
        EPhoneServerComHandNotifySubSessionComHandRequest, 
        TIpcArgs( &aCommandArgs ),
        aStatus );
    }


// -----------------------------------------------------------------------------
// RPhCltCommandHandlerNotify::CancelNotifyComHandCommand
// 
// Cancel pending command handler request notification.
// -----------------------------------------------------------------------------
//
void RPhCltCommandHandlerNotify::CancelNotifyComHandCommand() const
    {
    SendReceive( 
        EPhoneServerComHandNotifySubSessionComHandRequestCancel, 
        TIpcArgs() );
    }


// -----------------------------------------------------------------------------
// RPhCltCommandHandlerNotify::RespondClient
// 
// Respond to client's command handler request.
// -----------------------------------------------------------------------------
//
TInt RPhCltCommandHandlerNotify::RespondClient( 
    const TInt aResultCode )
    {
    return Send( 
        EPhoneServerComHandNotifySubSessionReportComHandResult, 
        TIpcArgs( aResultCode ) );
    }


// End of File
