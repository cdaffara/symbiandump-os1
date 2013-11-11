/*
* Copyright (c) 2008-2008 Nokia Corporation and/or its subsidiary(-ies). 
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


#include "accsrvcentralrepositorywatch.h"
#include "acc_debug.h"


// ======== LOCAL FUNCTIONS ========


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------
// Symbian two-phased constructor
// ---------------------------------------------------------
//
CAccSrvCentralRepositoryWatch* CAccSrvCentralRepositoryWatch::NewL(
        MAccSrvCentralRepositoryObserver*   aObserver,
        TUid    aRepositoryId,
        TUint32 aKeyId )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvCentralRepositoryWatch::NewL()" );
    CAccSrvCentralRepositoryWatch* self = new ( ELeave ) CAccSrvCentralRepositoryWatch();
    CleanupStack::PushL( self );
    self->ConstructL( aObserver, aRepositoryId, aKeyId );
    CleanupStack::Pop( self );
    COM_TRACE_1( "[AccFW:AccServer] CAccSrvCentralRepositoryWatch::NewL - return %p", self );
    return self;
    }


// ---------------------------------------------------------------------------
// Symbian two-phased constructor
// ---------------------------------------------------------------------------
//
CAccSrvCentralRepositoryWatch* CAccSrvCentralRepositoryWatch::NewLC(
        MAccSrvCentralRepositoryObserver*   aObserver,
        TUid    aRepositoryId,
        TUint32 aKeyId )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvCentralRepositoryWatch::NewLC()" );
    CAccSrvCentralRepositoryWatch* self = new( ELeave ) CAccSrvCentralRepositoryWatch;
    CleanupStack::PushL( self );
    self->ConstructL( aObserver, aRepositoryId, aKeyId );
    COM_TRACE_1( "[AccFW:AccServer] CAccSrvCentralRepositoryWatch::NewLC - return %p", self );
    return self;
    }


// ---------------------------------------------------------
// Destructor
// ---------------------------------------------------------
//
CAccSrvCentralRepositoryWatch::~CAccSrvCentralRepositoryWatch()
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvCentralRepositoryWatch::~CAccSrvCentralRepositoryWatch()" );
    Cancel();
    if( iRepository )
        {
        delete iRepository;
        }
    COM_TRACE_( "[AccFW:AccServer] CAccSrvCentralRepositoryWatch::~CAccSrvCentralRepositoryWatch" );
    }

// ---------------------------------------------------------
// SubscribeNotification
// ---------------------------------------------------------
//
void CAccSrvCentralRepositoryWatch::Watch()
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvCentralRepositoryWatch::Watch()" );
    // Subscribe
    iRepository->NotifyRequest( iKey, iStatus );
    SetActive();
    COM_TRACE_( "[AccFW:AccServer] CAccSrvCentralRepositoryWatch::Watch - return void" );
    return;
    }


// ---------------------------------------------------------
// Get current value.
// ---------------------------------------------------------
//
TInt CAccSrvCentralRepositoryWatch::GetCurrentValue( TInt& aValue)
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvCentralRepositoryWatch::GetCurrentValue()" );
    TInt value(0);
    TInt aError = iRepository->Get(iKey, value); //try to get the value
    if(KErrNone == aError)
        {
        aValue = value;
        }
    else 
        {
        iObserver->CentRepGetKeyValueFailed( iRepositoryUid, iKey, aError );
        }
    COM_TRACE_1( "[AccFW:AccServer] CAccSrvCentralRepositoryWatch::GetCurrentValue - return aValue 0x%x", aValue );
    return aError;
    }


// ---------------------------------------------------------
// From class CActive.
// DoCancel
// ---------------------------------------------------------
//
void CAccSrvCentralRepositoryWatch::DoCancel()
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvCentralRepositoryWatch::DoCancel()" );
    iRepository->NotifyCancel( iKey );
    COM_TRACE_( "[AccFW:AccServer] CAccSrvCentralRepositoryWatch::DoCancel - return void" );
    }


// ---------------------------------------------------------
// C++ constructor
// ---------------------------------------------------------
//
CAccSrvCentralRepositoryWatch::CAccSrvCentralRepositoryWatch()
    :CActive ( EPriorityStandard),
    iObserver ( NULL )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvCentralRepositoryWatch::CAccSrvCentralRepositoryWatch()" );
    COM_TRACE_( "[AccFW:AccServer] CAccSrvCentralRepositoryWatch::CAccSrvCentralRepositoryWatch - return" );
    }


// ---------------------------------------------------------
// ConstructL
// ---------------------------------------------------------
//
void CAccSrvCentralRepositoryWatch::ConstructL(
        MAccSrvCentralRepositoryObserver*   aObserver,
        TUid    aRepositoryId,
        TUint32 aKeyId )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvCentralRepositoryWatch::ConstructL()" );
    iObserver = aObserver;
    iRepositoryUid = aRepositoryId;
    iKey = aKeyId;    
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
    COM_TRACE_( "[AccFW:AccServer] CAccSrvCentralRepositoryWatch::ConstructL - return void" );
    return;
   }

// ---------------------------------------------------------
// From class CActive.
// RunL
// ---------------------------------------------------------
//
void CAccSrvCentralRepositoryWatch::RunL()
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvCentralRepositoryWatch::RunL()" );

    // Get the value
    TInt aCurrentValue = 0;
    TInt errorCode = GetCurrentValue( aCurrentValue );
    if ( KErrNone == errorCode )
        {
        // Re-subscribe before notifying The Master
        iRepository->NotifyRequest( iKey, iStatus );
        SetActive();

        // Notify requester
        iObserver->CentRepKeyChanged(
                iRepositoryUid,
                iKey, 
                aCurrentValue );
        }
    else
        {
        COM_TRACE_1( "[AccFW:AccServer] CAccSrvCentralRepositoryWatch::RunL - RRepsitory::Get()failed aError %d", errorCode );
        iObserver->CentRepGetKeyValueFailed( iRepositoryUid, iKey, errorCode );
        }
    COM_TRACE_( "[AccFW:AccServer] CAccSrvCentralRepositoryWatch::RunL - return void" );
    }


// ---------------------------------------------------------
// From class CActive.
// RunL
// ---------------------------------------------------------
//
TInt CAccSrvCentralRepositoryWatch::RunError( TInt aError )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvCentralRepositoryWatch::RunError()" );
    // Avoid Panic in CActiveScheduler
    aError = KErrNone;
    COM_TRACE_1( "[AccFW:AccServer] CAccSrvCentralRepositoryWatch::RunError - return aError %d", aError );
    return aError;
    }


// ======== GLOBAL FUNCTIONS ========

