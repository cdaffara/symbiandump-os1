/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Call Notify
*
*/



//  Include Files
#include    <CPhCltEmergencyCall.h> // for TPhCltEmergencyNumber
#include    "CPhCltCallNotify.h"
#include    "RPhCltServer.h"
#include    "CPhCltExtPhoneDialData.h"


// ============================= LOCAL FUNCTIONS ===============================



// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPhCltCallNotify::CPhCltCallNotify
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPhCltCallNotify::CPhCltCallNotify() 
    :  CActive( EPriorityStandard ),
    iDataLengthPckg ( iDialDataLength ),
    iEmergencyCallNumberPtr( NULL, 0, 0 )
    {
    CActiveScheduler::Add( this );
    }
    
// -----------------------------------------------------------------------------
// CPhCltCallNotify::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CPhCltCallNotify* CPhCltCallNotify::NewL()
    {
    CPhCltCallNotify* self = new ( ELeave ) CPhCltCallNotify; 
    return self;
    }
    
    // Destructor
EXPORT_C CPhCltCallNotify::~CPhCltCallNotify()
    {  
    Cancel();
    }

// -----------------------------------------------------------------------------
// CPhCltCallNotify::Open
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPhCltCallNotify::Open( RPhCltServer& aServer )
    {
    TInt retVal = iCallNotify.Open( aServer );
	return retVal;
    }

// -----------------------------------------------------------------------------
// CPhCltCallNotify::Close
// -----------------------------------------------------------------------------
//
EXPORT_C void CPhCltCallNotify::Close()
    {
    iCallNotify.Close();
    }

// -----------------------------------------------------------------------------
// CPhCltCallNotify::NotifyCallAttempt
// 
// -----------------------------------------------------------------------------
//
EXPORT_C void CPhCltCallNotify::NotifyCallAttempt( 
    TRequestStatus& aStatus,
    CPhCltExtPhoneDialData& aCallArgs )
    {
    aStatus = KRequestPending;
    iDialData = &aCallArgs; 
    iRequestStatus = &aStatus;
    iCallNotify.NotifyCallAttempt( iStatus, iDataLengthPckg ); 
    SetActive();
    }

// -----------------------------------------------------------------------------
// CPhCltCallNotify::CancelNotifyCallAttempt
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CPhCltCallNotify::CancelNotifyCallAttempt() const
    {
    iCallNotify.CancelNotifyCallAttempt();
    }


// -----------------------------------------------------------------------------
// CPhCltCallNotify::RespondClient
// 
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPhCltCallNotify::RespondClient( const TInt aResultCode )
    {
    return iCallNotify.RespondClient( aResultCode  );
    }

// -----------------------------------------------------------------------------
// CPhCltCallNotify::RunL()
// 
// -----------------------------------------------------------------------------
//    
void CPhCltCallNotify::RunL()
    {
    TInt error = iStatus.Int();
    if ( error == KErrCancel )
        {
        User::RequestComplete( iRequestStatus, KErrCancel );
        }
    else
        {
        HBufC8* package = HBufC8::NewLC ( iDialDataLength );
	    TPtr8 ptr ( package->Des());
	    //Dial data from server
	    iCallNotify.GetDialData(  ptr );
	    iDialData->SetParamsL( *package );
	    //Completes phone request with dialdata
	    User::RequestComplete( iRequestStatus, KErrNone );
	    CleanupStack::PopAndDestroy( package );   
        }
    }

// -----------------------------------------------------------------------------
// CPhCltCallNotify::DoCancel()
// 
// -----------------------------------------------------------------------------
//    
void CPhCltCallNotify::DoCancel()
    {
    }

// -----------------------------------------------------------------------------
// CPhCltCallNotify::NotifyEmergencyCall()
// 
// -----------------------------------------------------------------------------
//  
EXPORT_C void CPhCltCallNotify::NotifyEmergencyCall( 
	TRequestStatus& aStatus,
    TDesC& aEmergencyNumber )
	{
	iEmergencyCallNumberPtr.Set( reinterpret_cast<TText8*>( &aEmergencyNumber ), 
                         sizeof( TPhCltEmergencyNumber ),
                         sizeof( TPhCltEmergencyNumber ) );
	
    iCallNotify.NotifyEmergencyCall( 
    	aStatus, 
    	iEmergencyCallNumberPtr
    	 );
	}

// -----------------------------------------------------------------------------
// CPhCltCallNotify::CancelNotifyEmergencyCall()
// 
// -----------------------------------------------------------------------------
//	
EXPORT_C void CPhCltCallNotify::CancelNotifyEmergencyCall() const
	{
	iCallNotify.CancelNotifyEmergencyCall();
	}

// -----------------------------------------------------------------------------
// CPhCltCallNotify::RespondEmergencyToClient()
// 
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPhCltCallNotify::RespondEmergencyToClient( 
	const TInt aResultCode )
	{
	return iCallNotify.RespondEmergencyToClient( aResultCode  );
	}

// -----------------------------------------------------------------------------
// CPhCltCallNotify::NotifyComHandCommand
// 
// -----------------------------------------------------------------------------
//
EXPORT_C void CPhCltCallNotify::NotifyComHandCommand( 
    TRequestStatus& aStatus, 
    TDes8& aCommandArgs )
    {
    iCallNotify.NotifyComHandCommand( 
        aStatus, 
        aCommandArgs );
    }
    
// -----------------------------------------------------------------------------
// CPhCltCallNotify::CancelNotifyComHandCommand
// 
// -----------------------------------------------------------------------------
//
EXPORT_C void CPhCltCallNotify::CancelNotifyComHandCommand() const
    {
    iCallNotify.CancelNotifyComHandCommand();
    }

// -----------------------------------------------------------------------------
// CPhCltCallNotify::RespondComHandClient
// 
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPhCltCallNotify::RespondComHandClient( 
    const TInt aResultCode )
    {
    return iCallNotify.RespondComHandClient( aResultCode );
    }

// End of File
