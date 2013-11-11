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
* Description:  Ext Phone for dialing.
*
*/



// INCLUDE FILES
#include    <e32std.h>
#include    "RPhCltExtCall.h"
#include    "RPhCltServer.h"
#include    "MPhCltExtPhoneObserver.h"
#include    "CPhCltExtPhone.h"


// LOCAL CONSTANTS AND MACROS

// Panic codes.
enum TPhCltExtPhonePanic
    {
    EPhCltExtPhonePanicNoObserver,           // No observer found.
    EPhCltExtPhonePanicInvalidOperationMode  // Operation mode was invalid.
    };


// LOCAL FUNCTION PROTOTYPES

// Panic function. Called if panic situation happens.
void Panic( TPhCltExtPhonePanic aPanic );



// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// Panic
// 
// Panics the client
//
// -----------------------------------------------------------------------------
//
void Panic( TPhCltExtPhonePanic aPanic )
    {
    _LIT(KPhCltExtPhonePanicCategory, "CPhCltExtPhone");
    User::Panic( KPhCltExtPhonePanicCategory, aPanic );
    }



// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPhCltExtPhone::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPhCltExtPhone* CPhCltExtPhone::NewL( MPhCltExtPhoneObserver* aObserver )
    {
    CPhCltExtPhone* self = new ( ELeave ) CPhCltExtPhone( aObserver );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }   


// Destructor
CPhCltExtPhone::~CPhCltExtPhone()
    {
    // Cancel, close subsessions and finally close the session.

    Cancel();
    iCall.Close();
    iServer.Close();
    }




// -----------------------------------------------------------------------------
// CPhCltExtPhone::DialL
// 
// If a request is pending, leave with error code KErrInUse.
// Otherwise, create a dial request. 
// -----------------------------------------------------------------------------
//
void CPhCltExtPhone::DialL( const TPhCltExtPhoneDialData& aData )
    {
    DialPreconditionCheckL();
    iCall.Dial( iStatus, aData );
    }


// -----------------------------------------------------------------------------
// CPhCltExtPhone::CPhCltExtPhone
// -----------------------------------------------------------------------------
//
CPhCltExtPhone::CPhCltExtPhone( MPhCltExtPhoneObserver* aObserver )
    : CPhCltExtPhoneBase( aObserver ), iMode( EIdle )
    {
    CActiveScheduler::Add( this );
    }


// -----------------------------------------------------------------------------
// CPhCltExtPhone::ConstructL
// -----------------------------------------------------------------------------
//
void CPhCltExtPhone::ConstructL() 
    {
    User::LeaveIfError( iServer.Connect() );
    User::LeaveIfError( iCall.Open( iServer ) );
    }


// -----------------------------------------------------------------------------
// CPhCltExtPhone::RunL
// 
// The method is called when a request is completed. Thus,
// it calls appropriate observer method.
// -----------------------------------------------------------------------------
//
void CPhCltExtPhone::RunL()
    {
    __ASSERT_ALWAYS( iObserver, Panic( EPhCltExtPhonePanicNoObserver ) );
    if( iStatus != KErrCancel )
        {
        switch( iMode )
            {                         
            case EDialling:
                iObserver->HandleDialL( iStatus.Int() );
                break;
            case EIdle:
                break;
            default:
                Panic( EPhCltExtPhonePanicInvalidOperationMode );
                break;
            }
        }
    }


// -----------------------------------------------------------------------------
// CPhCltExtPhone::DoCancel
// 
// Cancels the pending request.
// -----------------------------------------------------------------------------
//
void CPhCltExtPhone::DoCancel()
    {
    switch( iMode )
        {                         
        case EDialling:
            iCall.CancelDial();
            break;
        case EIdle:
            break;
        default:
            Panic( EPhCltExtPhonePanicInvalidOperationMode );
            break;
        }
    }


// -----------------------------------------------------------------------------
// CPhCltExtPhone::DialPreconditionCheckL
// 
// See header
// -----------------------------------------------------------------------------
//
void CPhCltExtPhone::DialPreconditionCheckL()
    {
    if( IsActive() )
        {
        User::Leave( KErrInUse );
        }
    if( iCall.SubSessionHandle() == 0 )
        {
        User::Leave( KErrBadHandle );
        }
    iMode = EDialling;
    SetActive();
    }


//  End of File  
