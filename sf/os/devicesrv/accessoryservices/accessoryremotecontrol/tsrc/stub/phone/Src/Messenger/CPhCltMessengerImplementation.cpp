/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation of the PhoneClient Messenger interface.
*
*/



// INCLUDE FILES
#include    "CPhCltMessengerImplementation.h"



// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPhCltMessengerImplementation::CPhCltMessengerImplementation
// 
// C++ constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CPhCltMessengerImplementation::CPhCltMessengerImplementation()
    {
    }

// -----------------------------------------------------------------------------
// CPhCltMessengerImplementation::ConstructL
// 
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPhCltMessengerImplementation::ConstructL( const TUid& aCategoryUid,
            const TInt& aDefaultMessageSize )
    {
    User::LeaveIfError( iServer.Connect() );    
    User::LeaveIfError (iMessenger.Open( iServer, aCategoryUid,
        aDefaultMessageSize ) ); 
    }

// -----------------------------------------------------------------------------
// CPhCltMessengerImplementation::NewL
// 
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPhCltMessengerImplementation* CPhCltMessengerImplementation::NewL( const TUid& aCategoryUid,
            const TInt& aDefaultMessageSize )
    {
    CPhCltMessengerImplementation* self = 
        new ( ELeave ) CPhCltMessengerImplementation;   
   CleanupStack::PushL( self );
    self->ConstructL( aCategoryUid, aDefaultMessageSize );
    CleanupStack::Pop();
    return self;
    }
    

// -----------------------------------------------------------------------------
// CPhCltMessengerImplementation::~CPhCltMessengerImplementation
// 
// Destructor.
// -----------------------------------------------------------------------------
//
CPhCltMessengerImplementation::~CPhCltMessengerImplementation()
    {
    // Ensure that clean-up is done properly.
    iMessenger.Close();
    iServer.Close();
    }

// -----------------------------------------------------------------------------
// CPhCltMessengerImplementation::Receive
// 
// Receive message.
// -----------------------------------------------------------------------------
//
void CPhCltMessengerImplementation::Receive(
    TRequestStatus& aStatus,
    TInt& aMessageSize,
    TDes8& aMessagePayload )
    {
    iMessenger.Receive(
        aStatus,
        aMessageSize,
        aMessagePayload );
    }


// -----------------------------------------------------------------------------
// CPhCltMessengerImplementation::Send
// 
// Send message.
// -----------------------------------------------------------------------------
//
void CPhCltMessengerImplementation::Send(
    TRequestStatus& aStatus,
    const TDesC8& aMessagePayload )
    {
    iMessenger.Send(
        aStatus,
        aMessagePayload );
    }


// -----------------------------------------------------------------------------
// CPhCltMessengerImplementation::CancelRequest
// 
// Cancel ongoing request.
// -----------------------------------------------------------------------------
//
void CPhCltMessengerImplementation::CancelRequest(
    const TPhCltMessengerRequest& aRequest )
    {
    iMessenger.CancelRequest( aRequest );
    }


// -----------------------------------------------------------------------------
// CPhCltMessengerImplementation::Skip
// 
// Skip the current message.
// -----------------------------------------------------------------------------
//
void CPhCltMessengerImplementation::Skip()
    {
    iMessenger.Skip();
    }


//  End of File
