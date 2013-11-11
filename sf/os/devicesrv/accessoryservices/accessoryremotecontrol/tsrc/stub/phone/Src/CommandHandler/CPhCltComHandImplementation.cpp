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
* Description:  Implementation of the PhoneClient  interface.
*
*/



// INCLUDE FILES
#include    "CPhCltComHandImplementation.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPhCltCommandHandlerImplementation::CPhCltCommandHandlerImplementation
// 
// C++ constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CPhCltCommandHandlerImplementation::CPhCltCommandHandlerImplementation()
    {
    }


// -----------------------------------------------------------------------------
// CPhCltCommandHandlerImplementation::ConstructL
// 
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPhCltCommandHandlerImplementation::ConstructL()
    {
    User::LeaveIfError( iServer.Connect() );    
    User::LeaveIfError (iCommandHandler.Open( iServer ) ); 
    }


// -----------------------------------------------------------------------------
// CPhCltCommandHandlerImplementation::NewL
// 
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPhCltCommandHandlerImplementation* CPhCltCommandHandlerImplementation::NewL()
    {
    CPhCltCommandHandlerImplementation* self = 
        new (ELeave) CPhCltCommandHandlerImplementation();   

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }
    

// -----------------------------------------------------------------------------
// CPhCltCommandHandlerImplementation::~CPhCltCommandHandlerImplementation
// 
// Destructor.
// -----------------------------------------------------------------------------
//
CPhCltCommandHandlerImplementation::~CPhCltCommandHandlerImplementation()
    {
    // Close Subsession.
    iCommandHandler.Close();
    iServer.Close();
    }

// -----------------------------------------------------------------------------
// CPhCltCommandHandlerImplementation::Atd
// 
// Dial. Only voice call is supported.
// -----------------------------------------------------------------------------
//
void CPhCltCommandHandlerImplementation::Atd(
    TRequestStatus& aStatus, 
    const TPhCltTelephoneNumber& aTelephoneNumber )
    {
    TRequestStatus* ptrStat = &aStatus;
		User::RequestComplete( ptrStat, KErrNone );      
    }
 
// -----------------------------------------------------------------------------
// CPhCltCommandHandlerImplementation::Ata
// 
// Answer the call. 
// -----------------------------------------------------------------------------
//
void CPhCltCommandHandlerImplementation::Ata(
    TRequestStatus& aStatus )
    {
    TRequestStatus* ptrStat = &aStatus;
		User::RequestComplete( ptrStat, KErrNone );      
    }


// -----------------------------------------------------------------------------
// CPhCltCommandHandlerImplementation::Chld
// 
// Call hold and multiparty handling.
// -----------------------------------------------------------------------------
//
void CPhCltCommandHandlerImplementation::Chld(
    TRequestStatus& aStatus, 
    const TPhCltChldCommand aChldCommand,
    const TUint aCallNumber )
    {
    TRequestStatus* ptrStat = &aStatus;
		User::RequestComplete( ptrStat, KErrNone );      
    }


// -----------------------------------------------------------------------------
// CPhCltCommandHandlerImplementation::Chup
// 
// Hangup current call.
// -----------------------------------------------------------------------------
//
void CPhCltCommandHandlerImplementation::Chup(
    TRequestStatus& aStatus )
    {
    TRequestStatus* ptrStat = &aStatus;
		User::RequestComplete( ptrStat, KErrNone );      
    }


// -----------------------------------------------------------------------------
// CPhCltCommandHandlerImplementation::Vts
// 
// DTMF sending, starting and stopping in same function. 
// -----------------------------------------------------------------------------
//
void CPhCltCommandHandlerImplementation::Vts(
    TRequestStatus& aStatus, 
    const TPhCltDtmfTone aDtmfTone,
    const TPhCltDtmfAction aAction )
    {
    TRequestStatus* ptrStat = &aStatus;
		User::RequestComplete( ptrStat, KErrNone );      
    }


// -----------------------------------------------------------------------------
// CPhCltCommandHandlerImplementation::Bvra
// 
// Bluetooth Voice Recognition Activation.
// -----------------------------------------------------------------------------
//
void CPhCltCommandHandlerImplementation::Bvra( 
    TRequestStatus& aStatus, 
    const TBool aActivation )
    {
    TRequestStatus* ptrStat = &aStatus;
		User::RequestComplete( ptrStat, KErrNone );      
    }


// -----------------------------------------------------------------------------
// CPhCltCommandHandlerImplementation::CancelAsyncRequest
// 
// Cancel the ongoing asynchronous request.
// -----------------------------------------------------------------------------
//
void CPhCltCommandHandlerImplementation::CancelAsyncRequest( 
    const TPhCltComHandCommand aReqToCancel )
    {
    iCommandHandler.CancelAsyncRequest( aReqToCancel );
    }

// -----------------------------------------------------------------------------
// CPhCltCommandHandlerImplementation::MuteMicrophone
// -----------------------------------------------------------------------------
//

void CPhCltCommandHandlerImplementation::MuteMicrophone( 
    TRequestStatus& aStatus, const TBool aMute )
    {
    TRequestStatus* ptrStat = &aStatus;
		User::RequestComplete( ptrStat, KErrNone );      
    }

// -----------------------------------------------------------------------------
// CPhCltCommandHandlerImplementation::MuteRingingTone
// -----------------------------------------------------------------------------
//

void CPhCltCommandHandlerImplementation::MuteRingingTone( 
    TRequestStatus& aStatus  )
    {
    TRequestStatus* ptrStat = &aStatus;
		User::RequestComplete( ptrStat, KErrNone );      
    }
    


//  End of File
