/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Ussd Notify Handler
*
*/



// INCLUDE FILES
#include "CPhCltUssdRequestHandler.h"
#include "MPhCltUssdRequestHandler.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPhCltUssdRequestHandler::CPhCltUssdRequestHandler
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPhCltUssdRequestHandler::CPhCltUssdRequestHandler(
    MPhCltUssdRequestHandler& aHandler,
    TInt aPriority )
    : CActive( aPriority ), iHandler( aHandler )
    {
    CActiveScheduler::Add( this );
    }


// -----------------------------------------------------------------------------
// CPhCltUssdRequestHandler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPhCltUssdRequestHandler* CPhCltUssdRequestHandler::NewL( 
    MPhCltUssdRequestHandler& aHandler,
    TInt aPriority )
    {
    CPhCltUssdRequestHandler* self = 
        new( ELeave ) CPhCltUssdRequestHandler( aHandler , aPriority );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(); // self
    return self;
    }


// -----------------------------------------------------------------------------
// CPhCltUssdRequestHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPhCltUssdRequestHandler::ConstructL()
    {
    User::LeaveIfError( iPhClient.Connect() );
    User::LeaveIfError( iUssdClient.Open( iPhClient ) );
    }


// Destructor
CPhCltUssdRequestHandler::~CPhCltUssdRequestHandler()
    {
    Cancel();

    iUssdClient.CancelSend();
    iUssdClient.Close();

    iPhClient.Close();
    }


// -----------------------------------------------------------------------------
// CPhCltUssdRequestHandler::SendUssd
//
// Called when Sending
// -----------------------------------------------------------------------------
void CPhCltUssdRequestHandler::SendUssd( 
    const TDesC8& aMsgData, 
    const TDesC8& aMsgAttribute )
    {
    iUssdClient.SendUssd( iStatus , aMsgData , aMsgAttribute );
    SetActive();
    }


// -----------------------------------------------------------------------------
// CPhCltUssdRequestHandler::SendUssdCancel
// 
// -----------------------------------------------------------------------------
void CPhCltUssdRequestHandler::SendUssdCancel()
    {
    if ( IsActive() )
        {
        Cancel();
        TRAPD( ignore , iHandler.HandleSendEventL( KErrCancel ) );
        User::LeaveIfError ( ignore );
        }
    }

// -----------------------------------------------------------------------------
// CPhCltUssdRequestHandler::UssdClient
// 
// Returns USSD Client instance
// -----------------------------------------------------------------------------
RPhCltUssd& CPhCltUssdRequestHandler::UssdClient()
    {
    return iUssdClient;
    }


// -----------------------------------------------------------------------------
// CPhCltUssdRequestHandler::RunL
//
// Called when Send completes
// -----------------------------------------------------------------------------
void CPhCltUssdRequestHandler::RunL()
    {
    iHandler.HandleSendEventL( iStatus.Int() );
    }


// -----------------------------------------------------------------------------
// CPhCltUssdRequestHandler::DoCancel
//
// Called when Send is cancelled
// -----------------------------------------------------------------------------

void CPhCltUssdRequestHandler::DoCancel()
    {
    iUssdClient.CancelSend();
    }


//  End of File  
