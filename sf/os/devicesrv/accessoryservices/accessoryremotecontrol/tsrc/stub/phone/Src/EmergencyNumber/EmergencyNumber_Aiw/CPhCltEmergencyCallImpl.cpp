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
#include "RPhCltServer.h"
#include "CPhCltEmergencyCallImpl.h"
#include "MPhCltEmergencyCallObserver.h"
#include "PhCltTypes.h"


// ============================ MEMBER FUNCTIONS ===============================
// -----------------------------------------------------------------------------
// CPhCltEmergencyCallImpl::CPhCltEmergencyCallImpl
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPhCltEmergencyCallImpl::CPhCltEmergencyCallImpl( 
    MPhCltEmergencyCallObserver* aObserver ) : 
        CPhCltEmergencyCall( aObserver ), 
        iEmergencyCallNumberPtr( NULL, 0 )
    {
    CActiveScheduler::Add( this );
    }
    
// -----------------------------------------------------------------------------
// CPhCltEmergencyCallImpl::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPhCltEmergencyCallImpl* CPhCltEmergencyCallImpl::NewL(
    MPhCltEmergencyCallObserver* aObserver )
    {
    CPhCltEmergencyCallImpl* self = 
        new ( ELeave ) CPhCltEmergencyCallImpl( aObserver);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(); 
    return self;
    }
    
// Destructor
CPhCltEmergencyCallImpl::~CPhCltEmergencyCallImpl()
    { 
    Cancel();
    iEmergencyCall.Close(); 
    iServer.Close();
    }

// -----------------------------------------------------------------------------
// CPhCltEmergencyCallImpl::ConstructL
// 
// -----------------------------------------------------------------------------
//
void CPhCltEmergencyCallImpl::ConstructL()
    {
    User::LeaveIfError( iServer.Connect() );
    User::LeaveIfError( iEmergencyCall.Open( iServer ) );
    }

// -----------------------------------------------------------------------------
// CPhCltEmergencyCallImpl::RunL
// 
// -----------------------------------------------------------------------------
//
void CPhCltEmergencyCallImpl::RunL()
    {
    // inform observer
    iObserver->HandleEmergencyDialL( iStatus.Int() );
    }

// -----------------------------------------------------------------------------
// CPhCltEmergencyCallImpl::DoCancel
// 
// -----------------------------------------------------------------------------
//
void CPhCltEmergencyCallImpl::DoCancel()
    {
    iEmergencyCall.CancelDialEmergencyCall();
    }
    
// -----------------------------------------------------------------------------
// CPhCltEmergencyCallImpl::IsEmergenyPhoneNumber
// 
// -----------------------------------------------------------------------------
//
TInt CPhCltEmergencyCallImpl::IsEmergencyPhoneNumber( 
    const TDesC& aNumber, 
    TBool& aIsEmergencyNumber )
    {
    return iEmergencyCall.IsEmergencyPhoneNumber( 
    	aNumber, 
    	aIsEmergencyNumber );
    }

// -----------------------------------------------------------------------------
// CPhCltEmergencyCallImpl::FindEmergencyPhoneNumber
// 
// -----------------------------------------------------------------------------
//
TInt CPhCltEmergencyCallImpl::FindEmergencyPhoneNumber( 
    TDes& aNumber, 
    TBool& aIsEmergencyNumber )
    {
    return iEmergencyCall.FindEmergencyPhoneNumber( 
    	aNumber, 
    	aIsEmergencyNumber );
    }

// -----------------------------------------------------------------------------
// CPhCltEmergencyCallImpl::DialEmergencyCallL
// 
// Sends a dial request to the server.
// -----------------------------------------------------------------------------
//
void CPhCltEmergencyCallImpl::DialEmergencyCallL( 
    const TDesC& aNumber )
    {
    DialEmergencyPreconditionCheckL();
    iEmergencyCallNumber = aNumber;
    iEmergencyCallNumberPtr.Set( 
        reinterpret_cast<const TText8*>( &iEmergencyCallNumber ), 
        sizeof( TPhCltEmergencyNumber ) );
    iEmergencyCall.DialEmergencyCall( iStatus, iEmergencyCallNumberPtr );
    SetActive();
    }

// -----------------------------------------------------------------------------
// CPhCltEmergencyCallImpl::DialEmergencyPreconditionCheckL
// 
// -----------------------------------------------------------------------------
//
void CPhCltEmergencyCallImpl::DialEmergencyPreconditionCheckL()
    {
    if( IsActive() )
        {
        User::Leave( KErrInUse );
        }
    if( iEmergencyCall.SubSessionHandle() == 0 )
        {
        User::Leave( KErrBadHandle );
        }
    }

//  End of File  
