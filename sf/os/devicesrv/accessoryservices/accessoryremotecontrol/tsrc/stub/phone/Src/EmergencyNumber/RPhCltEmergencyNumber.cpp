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
* Description:  Emergency Number
*
*/


// INCLUDE FILES
#include    "RPhCltServer.h"
#include    "RPhCltEmergencyNumber.h"
#include    "PhCltClientServer.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// RPhCltEmergencyNumber::RPhCltEmergencyNumber
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C RPhCltEmergencyNumber::RPhCltEmergencyNumber() 
    : RSubSessionBase()
    {    
    }


// -----------------------------------------------------------------------------
// RPhCltEmergencyNumber::Open
// 
// Creates a subsession, as part of the given session.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RPhCltEmergencyNumber::Open( RPhCltServer& aServer )
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
// RPhCltEmergencyNumber::Close
// 
// Closes the subsession.
// -----------------------------------------------------------------------------
//
EXPORT_C void RPhCltEmergencyNumber::Close()
    {
    CloseSubSession( EPhoneServerEmergencyNumberSubSessionClose );
    }


// -----------------------------------------------------------------------------
// RPhCltEmergencyNumber::IsEmergenyPhoneNumber
// 
// Sends a dial request to the server.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RPhCltEmergencyNumber::IsEmergenyPhoneNumber( 
    const TPhCltTelephoneNumber& aNumber, 
    TBool& aIsEmergencyNumber,
    TPhCltEmergencyNumberQueryMode aMode )
    {
    TPckg< TBool > pIsEmergencyNumber( aIsEmergencyNumber );
    TPckgC< TPhCltTelephoneNumber > pNumber( aNumber );

    return SendReceive( 
        EPhoneServerEmergencyNumberSubSessionIsEmergencyNumber, 
        TIpcArgs( 
            &pNumber,
            &pIsEmergencyNumber,
            aMode ) );
    }

// -----------------------------------------------------------------------------
// RPhCltEmergencyNumber::IsEmergencyPhoneNumber
// 
// Sends a dial request to the server.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RPhCltEmergencyNumber::IsEmergencyPhoneNumber( 
    TPhCltTelephoneNumber& aNumber, 
    TBool& aIsEmergencyNumber,
    TPhCltEmergencyNumberQueryMode aMode )
    {
    TPckg< TBool > pIsEmergencyNumber( aIsEmergencyNumber );
    TPckg< TPhCltTelephoneNumber > pNumber( aNumber );    

    return SendReceive( 
        EPhoneServerEmergencyNumberSubSessionIsEmergencyNumber, 
        TIpcArgs( 
            &pNumber,
            &pIsEmergencyNumber,
            aMode ) );    
    }



//  End of File  
