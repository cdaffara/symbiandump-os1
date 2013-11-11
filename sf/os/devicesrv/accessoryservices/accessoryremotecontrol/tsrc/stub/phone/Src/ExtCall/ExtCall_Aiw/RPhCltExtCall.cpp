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
* Description:  Ext Call
*
*/



// INCLUDE FILES
#include    "RPhCltServer.h"
#include    "RPhCltExtCall.h"
#include    "CPhCltExtPhoneDialData.h"
#include    "PhCltClientServer.h"

// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// RPhCltExtCall::RPhCltExtCall
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
RPhCltExtCall::RPhCltExtCall() 
    : RSubSessionBase()
    {    
    }


// -----------------------------------------------------------------------------
// RPhCltExtCall::Open
// 
// Creates a subsession, as part of the given session.
// -----------------------------------------------------------------------------
//
TInt RPhCltExtCall::Open( RPhCltServer& aServer )
    {
    __ASSERT_ALWAYS( aServer.Handle(), User::Panic( 
        KPhClientPanicCategory, 
        EPhCltClientSidePanicNullHandle ) );

    return CreateSubSession( 
        aServer, 
        EPhoneServerExtCallSubSessionOpen, 
        TIpcArgs() );
    }


// -----------------------------------------------------------------------------
// RPhCltExtCall::Close
// 
// Closes the subsession.
// -----------------------------------------------------------------------------
//
void RPhCltExtCall::Close()
    {
    CloseSubSession( EPhoneServerExtCallSubSessionClose );
    }


// -----------------------------------------------------------------------------
// RPhCltExtCall::Dial
// 
// Sends a dial request to the server.
// -----------------------------------------------------------------------------
//
void RPhCltExtCall::Dial( 
    TRequestStatus& aStatus, 
    TDesC8& aData, TInt aLength )
    {
    __ASSERT_ALWAYS( SubSessionHandle(), User::Panic( KPhClientPanicCategory,
        EPhCltClientSidePanicNullHandle ) );
    
    TPckg< TInt > pDataLength( aLength );
    SendReceive(  
        EPhoneServerExtCallSubSessionMakeCall, 
        TIpcArgs( &aData, aLength ), 
        aStatus ); 
    }


// -----------------------------------------------------------------------------
// RPhCltExtCall::CancelDial
// 
// Cancels the pending dial request.
// -----------------------------------------------------------------------------
//
void RPhCltExtCall::CancelDial() const
    {
    __ASSERT_ALWAYS( const_cast< RPhCltExtCall* >( this )->SubSessionHandle(), 
        User::Panic( 
            KPhClientPanicCategory, EPhCltClientSidePanicNullHandle ) );

    SendReceive( 
        EPhoneServerExtCallSubSessionMakeCallCancel, 
        TIpcArgs() );
    }


//  End of File  
