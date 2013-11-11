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
#include    "TPhCltExtPhoneDialData.h"
#include    "PhCltClientServer.h"

// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// RPhCltExtCall::RPhCltExtCall
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
RPhCltExtCall::RPhCltExtCall() 
    : RSubSessionBase(), iArgs( TPhCltCallArgs() )
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
    const TPhCltTelephoneNumber& aNumber,
    const TPhCltNameBuffer& aName,
    const TInt aWindowGroupId,
    const TInt aRedial,
    const TPhCltContactId aContactId,
    const TBool aAllowNumberShowing )    
    {
    __ASSERT_ALWAYS( SubSessionHandle(), User::Panic( 
        KPhClientPanicCategory,
        EPhCltClientSidePanicNullHandle ) );

    // Populate a temporary arguments package
    TPhCltExtPhoneDialData args( aName, aNumber, aContactId, aWindowGroupId );
    args.SetRedial( aRedial );
    args.SetShowNumber( aAllowNumberShowing );
    
    // Do the dial
    Dial( aStatus, args );
    }


// -----------------------------------------------------------------------------
// RPhCltExtCall::Dial
// 
// Sends a dial request to the server.
// -----------------------------------------------------------------------------
//
void RPhCltExtCall::Dial( 
    TRequestStatus& aStatus, 
    const TPhCltExtPhoneDialData& aData )
    {
    __ASSERT_ALWAYS( SubSessionHandle(), User::Panic( KPhClientPanicCategory,
        EPhCltClientSidePanicNullHandle ) );
    
    // Save the arguments
    TPhCltCallArgs args;
    aData.AsIPCStruct( args );
    iArgs() = args;

    // Request the dial
    SendReceive( 
        EPhoneServerExtCallSubSessionMakeCall, 
        TIpcArgs( &iArgs ), 
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
