/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  A class to watch changes of key values in Central Repository.
*
*/



#include    <e32base.h>
#include    <e32def.h>
#include    <centralrepository.h>


#include "centralrepositorywatch.h"
#include "trace.h"


// ======== LOCAL FUNCTIONS ========


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------
// Symbian two-phased constructor
// ---------------------------------------------------------
//
CCentralRepositoryWatch* CCentralRepositoryWatch::NewL(
        MCentralRepositoryObserver&   aObserver,
        TUid aRepositoryUid,
        TUint32 aKeyId )
    {
    FUNC_LOG;
    CCentralRepositoryWatch* self = new ( ELeave ) CCentralRepositoryWatch(
            aObserver,
            aRepositoryUid,
            aKeyId );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// Symbian two-phased constructor
// ---------------------------------------------------------------------------
//
CCentralRepositoryWatch* CCentralRepositoryWatch::NewLC(
        MCentralRepositoryObserver&   aObserver,
        TUid aRepositoryUid,
        TUint32 aKeyId )
    {
    FUNC_LOG;
    CCentralRepositoryWatch* self = new( ELeave ) CCentralRepositoryWatch(
            aObserver,
            aRepositoryUid,
            aKeyId );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// ---------------------------------------------------------
// Destructor
// ---------------------------------------------------------
//
CCentralRepositoryWatch::~CCentralRepositoryWatch()
    {
    FUNC_LOG;
    Cancel();
    delete iRepository;
    }

// ---------------------------------------------------------
// SubscribeNotification
// ---------------------------------------------------------
//
void CCentralRepositoryWatch::Watch()
    {
    FUNC_LOG;
    // Subscribe
    if ( !IsActive() )
        {
        iRepository->NotifyRequest( iKey, iStatus );
        SetActive();        
        }
    return;
    }


// ---------------------------------------------------------
// Get current value.
// ---------------------------------------------------------
//
TInt CCentralRepositoryWatch::GetCurrentValue( TInt& aValue)
    {
    FUNC_LOG;
    TInt value(0);
    TInt aError = iRepository->Get(iKey, value); //try to get the value
    if(KErrNone == aError)
        {
        aValue = value;
        }
    else 
        {
        iObserver.CentRepGetKeyValueFailed( iRepositoryUid, iKey, aError );
        }
    INFO_1( "Return aValue 0x%x", aValue );
    return aError;
    }


// ---------------------------------------------------------
// From class CActive.
// DoCancel
// ---------------------------------------------------------
//
void CCentralRepositoryWatch::DoCancel()
    {
    FUNC_LOG;
    iRepository->NotifyCancel( iKey );
    }


// ---------------------------------------------------------
// C++ constructor
// ---------------------------------------------------------
//
CCentralRepositoryWatch::CCentralRepositoryWatch(
        MCentralRepositoryObserver& aObserver,
        TUid aRepositoryUid,
        TUint32 aKeyId )
    : CActive ( EPriorityStandard),
    iObserver ( aObserver )
    {
    FUNC_LOG;
    iRepositoryUid = aRepositoryUid;
    iKey = aKeyId;
    }


// ---------------------------------------------------------
// ConstructL
// ---------------------------------------------------------
//
void CCentralRepositoryWatch::ConstructL( )
    {
    FUNC_LOG;
    // Connect CenRep
    iRepository = CRepository::NewL( iRepositoryUid );
    // Check that key is valid
    TInt aCurrentValue;
    TInt errorCode = GetCurrentValue( aCurrentValue );
    if ( KErrNone != errorCode )
        {
        User::Leave( errorCode );
        }
    CActiveScheduler::Add( this );
    return;
   }

// ---------------------------------------------------------
// From class CActive.
// RunL
// ---------------------------------------------------------
//
void CCentralRepositoryWatch::RunL()
    {
    FUNC_LOG;

    // Get the value
    TInt aCurrentValue = 0;
    
    // Re-subscribe before notifying The Master
    iRepository->NotifyRequest( iKey, iStatus );
    SetActive();
    
    TInt errorCode = GetCurrentValue( aCurrentValue );
    if ( KErrNone == errorCode )
        {
        // Notify requester
        iObserver.CentRepKeyChanged(
                iRepositoryUid,
                iKey );
        }
    else
        {
        INFO_1( "RRepsitory::Get()failed aError %d", errorCode );
        iObserver.CentRepGetKeyValueFailed( iRepositoryUid, iKey, errorCode );
        }
    }


// ---------------------------------------------------------
// From class CActive.
// RunL
// ---------------------------------------------------------
//
TInt CCentralRepositoryWatch::RunError( TInt aError )
    {
    FUNC_LOG;
    // Avoid Panic in CActiveScheduler
    INFO_1( "aError %d", aError );
    INFO("Issuing a new request for key value changes");
    Watch();
    aError = KErrNone;
    return aError;
    }


// ======== GLOBAL FUNCTIONS ========

