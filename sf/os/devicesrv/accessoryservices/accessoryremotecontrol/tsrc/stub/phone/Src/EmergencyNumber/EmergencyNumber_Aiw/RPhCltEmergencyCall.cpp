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
* Description:  
*
*/


// INCLUDE FILES
#include    <CPhCltEmergencyCall.h> // for TPhCltEmergencyNumber
#include    "RPhCltEmergencyCall.h"
#include    "PhCltClientServer.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// RPhCltEmergencyCall::RPhCltEmergencyCall
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
RPhCltEmergencyCall::RPhCltEmergencyCall() 
    : RSubSessionBase()
    {    
    }

// -----------------------------------------------------------------------------
// RPhCltEmergencyCall::Open
// 
// Creates a subsession, as part of the given session.
// -----------------------------------------------------------------------------
//
TInt RPhCltEmergencyCall::Open( RPhCltServer& aServer )
    {
    __ASSERT_ALWAYS( aServer.Handle(), User::Panic( 
        KPhClientPanicCategory, 
        EPhCltClientSidePanicNullHandle ) );

    return CreateSubSession( 
        aServer, 
        EPhoneServerEmergencyNumberSubSessionOpen,
        TIpcArgs() );
    }

// -----------------------------------------------------------------------------
// RPhCltEmergencyCall::Close
// 
// Closes the subsession.
// -----------------------------------------------------------------------------
//
void RPhCltEmergencyCall::Close()
    {
    CloseSubSession( EPhoneServerEmergencyNumberSubSessionClose );
    }

// -----------------------------------------------------------------------------
// RPhCltEmergencyCall::IsEmergenyPhoneNumber
// 
// Sends a dial request to the server.
// -----------------------------------------------------------------------------
//
TInt RPhCltEmergencyCall::IsEmergencyPhoneNumber( 
    const TDesC& aNumber, 
    TBool& aIsEmergencyNumber )
    {
    TPckg< TBool > pIsEmergencyNumber( aIsEmergencyNumber );
    
    if ( aNumber.Length() > KPhCltEmergencyNumberSize )
        {
        return KErrArgument;
        }
    
    TPckgC< TPhCltEmergencyNumber > pNumber( aNumber );

    return SendReceive( 
        EPhoneServerEmergencyNumberSubSessionIsEmergencyNumber, 
        TIpcArgs( 
            &pNumber,
            &pIsEmergencyNumber,
            EPhCltEmergencyNumberQueryModeNormal ) );
    }
    
// -----------------------------------------------------------------------------
// RPhCltEmergencyCall::IsEmergencyPhoneNumber
// 
// Sends a dial request to the server.
// -----------------------------------------------------------------------------
//
TInt RPhCltEmergencyCall::FindEmergencyPhoneNumber( 
    TDes& aNumber, 
    TBool& aIsEmergencyNumber )
    {
    TPckg< TBool > pIsEmergencyNumber( aIsEmergencyNumber );
    
    if ( aNumber.Length() > KPhCltTelephoneNumberLength )
        {
        return KErrArgument;
        }
    
    TPckg< TPhCltTelephoneNumber > pNumber( 
        static_cast<TPhCltTelephoneNumber&>( aNumber ) );
    return SendReceive( 
        EPhoneServerEmergencyNumberSubSessionIsEmergencyNumber, 
        TIpcArgs( 
            &pNumber,
            &pIsEmergencyNumber,
            EPhCltEmergencyNumberQueryModeAdvanced ) );
    }
    
// -----------------------------------------------------------------------------
// RPhCltEmergencyCall::DialEmergencyCall
// 
// Sends a dial request to the server.
// -----------------------------------------------------------------------------
//
void RPhCltEmergencyCall::DialEmergencyCall( 
	TRequestStatus& aReqStatus, 
	TDesC8& aNumber )
    {
    return SendReceive( 
        EPhoneServerEmergencyNumberSubsessionEmergencyDial,
        TIpcArgs( &aNumber ),
        aReqStatus );
    }

// -----------------------------------------------------------------------------
// RPhCltEmergencyCall::CancelDialEmergencyCall
// 
// Cancels an emergency call.
// -----------------------------------------------------------------------------
//
void RPhCltEmergencyCall::CancelDialEmergencyCall()
	{
	SendReceive( 
		EPhoneServerEmergencyNumberSubsessionEmergencyDialCancel,
		TIpcArgs() );
	}

//  End of File  
