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
* Description:  Ussd Sat Client.
*
*/



// INCLUDE FILES
#include    "CPhCltUssdImp.h"
#include    "PhCltClientServer.h"
#include    "CPhCltUssdSatClient.h"



// ============================= LOCAL FUNCTIONS ===============================

#ifndef CPHCLTUSSDSATACTIVE_H
#define CPHCLTUSSDSATACTIVE_H

/**
* The waiter class for SAT sessions.
*/
NONSHARABLE_CLASS( CPhCltUssdSatActive ) : public CActive
    {
    public:

        /**
        * Constructor.
        */
        CPhCltUssdSatActive();

        /**
        * Destructor.
        */
        ~CPhCltUssdSatActive();

        /**
        * Set this active object active.
        * Hides the function derived from CActive.
        */
        void SetActive();

        /**
        * Wait until the Sat Session is ended.
        * @return The error code of the session.
        */
        TInt WaitSatSessionToEnd();


    protected:  // From base classes

        /**
        * @see CActive::RunL
        */
        void RunL();

        /**
        * @see CActive::DoCancel
        */
        void DoCancel();


    private:

        // The Active scheduler waiter.
        CActiveSchedulerWait iSatWaiter;

        // The error code storage place.
        TInt* iError;
    };

#endif // CPHCLTUSSDSATACTIVE_H


// -----------------------------------------------------------------------------
// CPhCltUssdSatActive::CPhCltUssdSatActive
//
// -----------------------------------------------------------------------------
//
CPhCltUssdSatActive::CPhCltUssdSatActive() : CActive( EPriorityStandard )
    {
    CActiveScheduler::Add( this );
    }


// Destructor
CPhCltUssdSatActive::~CPhCltUssdSatActive()
    {
    Cancel();
    if ( iSatWaiter.IsStarted() )
        {
        iSatWaiter.AsyncStop();
        }

    }


// -----------------------------------------------------------------------------
// CPhCltUssdSatActive::SetActive
//
// -----------------------------------------------------------------------------
//
void CPhCltUssdSatActive::SetActive()
    {
    CActive::SetActive();
    iError = NULL;
    }


// -----------------------------------------------------------------------------
// CPhCltUssdSatActive::WaitSatSessionToEnd
//
// -----------------------------------------------------------------------------
//
TInt CPhCltUssdSatActive::WaitSatSessionToEnd()
    {
    if ( !IsActive() )
        {
        if ( iError )
            {
            return *iError;
            }
        else
            {
            return KErrNone;
            }
        }

    TInt error = KErrNone;
    iError = &error;
    iSatWaiter.Start();
    return error;
    }


// -----------------------------------------------------------------------------
// CPhCltUssdSatActive::RunL
//
// -----------------------------------------------------------------------------
//
void CPhCltUssdSatActive::RunL()
    {
    if ( iError )
        {
        *iError = iStatus.Int();
        }
    if ( iSatWaiter.IsStarted() )
        {
        iSatWaiter.AsyncStop();
        }
    }


// -----------------------------------------------------------------------------
// CPhCltUssdSatActive::DoCancel
//
// -----------------------------------------------------------------------------
//
void CPhCltUssdSatActive::DoCancel()
    {
    if ( iError )
        {
        *iError = KErrCancel;
        }
    }



// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPhCltUssdSatClient::CPhCltUssdSatClient
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPhCltUssdSatClient::CPhCltUssdSatClient()
    {
    }


// -----------------------------------------------------------------------------
// CPhCltUssdSatClient::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPhCltUssdSatClient::ConstructL( TBool aShowNotes )
    {
    iUssdWrapper = CPhCltUssdImp::NewL( aShowNotes );

    iShowNotes = aShowNotes;

    iSatActive = new( ELeave ) CPhCltUssdSatActive();
    }


// -----------------------------------------------------------------------------
// CPhCltUssdSatClient::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CPhCltUssdSatClient* CPhCltUssdSatClient::NewL( TBool aShowNotes )
    {
    CPhCltUssdSatClient* self = new ( ELeave ) CPhCltUssdSatClient;
    
    CleanupStack::PushL( self );
    self->ConstructL( aShowNotes );
    CleanupStack::Pop();
    
    return self;
    }


// Destructor
EXPORT_C CPhCltUssdSatClient::~CPhCltUssdSatClient()
    {
    if ( iSatActive )
        {
        if ( iSatActive->IsActive() )
            {
            iUssdWrapper->StopSAT();
            }
        delete iSatActive;
        iSatActive = NULL;
        }

    delete iUssdWrapper;
    iUssdWrapper = NULL;
    }


// -----------------------------------------------------------------------------
// CPhCltUssdSatClient::SendSatMessage
// Send SAT message and receive the last message.
// 
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPhCltUssdSatClient::SendSatMessage( 
    const TDesC& aSendMessage, 
    const TUint8 iSendDcs,
    TDes& aReceiveMessage,
    TBool& aSendCompletedFirst,
    TUint8& aReceivedDcs) 
    {
    // Do not process request if earlier request is ongoing.
    if ( iSatActive->IsActive() )
        {
        // Stop (complete) the earlier request so that we can send new
        // SATmessage during next function call.
        iUssdWrapper->StopSAT();
        return KErrInUse;
        }
    iUssdWrapper->SetDCS( iSendDcs );
    iSatError = KErrNone;
    iSendCompletedFirst = &aSendCompletedFirst;
    *iSendCompletedFirst = ETrue;
    iReceiveMessage.Set( aReceiveMessage );
    aReceiveMessage.Zero();
    aReceivedDcs = EPhCltDcsUnknown;
    
    iShowNotesAndDcs = 0; // reset the variable
    if ( iShowNotes )
        {
        iShowNotesAndDcs = KPhCltShowNotes;
        }

    TPckg< TUint > showNotesAndDcs( iShowNotesAndDcs );

    // Send receive message to the server. status is completed when Ussd session
    // ends.
    iUssdWrapper->StartSAT( 
        iSatActive->iStatus, 
        aReceiveMessage , 
        showNotesAndDcs );

    iSatActive->SetActive();

    
    // Now send the string according to the parameter.
    TPtrC pMessage( aSendMessage );
    iSatError = iUssdWrapper->SendUssd( pMessage );

    if ( iSatError != KErrNone )
        {
        iSendCompletedFirst = NULL;
        iUssdWrapper->StopSAT();

        iSatActive->WaitSatSessionToEnd();
        return iSatError; // Message send error
        }
    
    // Wait until the Ussd conversation is over:
    TInt satWaiterError = iSatActive->WaitSatSessionToEnd();
    if ( satWaiterError > KErrNone )
        {
        *iSendCompletedFirst = EFalse;
        }
    
    // Get the error code:
    iSatError = Min ( iSatError, satWaiterError );
    
    iSendCompletedFirst = NULL;

    // Return the DCS to the caller.
    DecodeDcs( aReceivedDcs );
    aReceivedDcs = iShowNotesAndDcs;
    return iSatError; // Receive error code
    }


// -----------------------------------------------------------------------------
// CPhCltUssdSatClient::SendSatMessageCancel
// 
// Cancels the out-standing request.
// -----------------------------------------------------------------------------
//
EXPORT_C void CPhCltUssdSatClient::SendSatMessageCancel()
    {
    iUssdWrapper->SendUssdCancel();
    iSendCompletedFirst = NULL;
    iUssdWrapper->StopSAT();
    iSatActive->Cancel();
    }

// -----------------------------------------------------------------------------
// CPhCltUssdSatClient::DecodeDcs
// 
// In this function the DCS received from PhoneServer is decoded.
// -----------------------------------------------------------------------------
//
void CPhCltUssdSatClient::DecodeDcs( TUint8& aReceivedDcs ) 
    {
    // Check what DCS was used, and update aReceivedDcs
    if ( iShowNotesAndDcs == KPhCltDcs7Bit )
        {
        aReceivedDcs = EPhCltDcs7Bit;
        }
    else if ( iShowNotesAndDcs == KPhCltDcs8Bit )
        {
        aReceivedDcs = EPhCltDcs8Bit;
        }
    else if ( iShowNotesAndDcs == KPhCltDcsUcs2 )
        {
        aReceivedDcs = EPhCltDcsUCS2;
        }
    else
        {
        aReceivedDcs = EPhCltDcsUnknown;
        }
    }


//  End of File  
