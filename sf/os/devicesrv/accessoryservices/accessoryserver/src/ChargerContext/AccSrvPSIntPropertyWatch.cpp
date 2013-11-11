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
* Description:  A class to watch changes of a integer property.
*
*/



#include    <e32base.h>
#include    <e32def.h>


#include "AccSrvPSIntPropertyWatch.h"
#include "acc_debug.h"


// ======== LOCAL FUNCTIONS ========


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------
// Symbian two-phased constructor
// ---------------------------------------------------------
//
CAccSrvPSIntPropertyWatch* CAccSrvPSIntPropertyWatch::NewL(
        MAccSrvPSIntPropertyObserver*   aObserver,
        TUid    aCategory,
        TInt    aKey,
        TOwnerType aType )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvPSIntPropertyWatch::NewL()" );
    CAccSrvPSIntPropertyWatch* self = new ( ELeave ) CAccSrvPSIntPropertyWatch();
    CleanupStack::PushL( self );
    self->ConstructL( aObserver, aCategory, aKey, aType );
    CleanupStack::Pop( self );
    COM_TRACE_1( "[AccFW:AccServer] CAccSrvPSIntPropertyWatch::NewL - return %p", self );
    return self;
    }


// ---------------------------------------------------------------------------
// Symbian two-phased constructor
// ---------------------------------------------------------------------------
//
CAccSrvPSIntPropertyWatch* CAccSrvPSIntPropertyWatch::NewLC(
        MAccSrvPSIntPropertyObserver*   aObserver,
        TUid    aCategory,
        TInt    aKey,
        TOwnerType aType )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvPSIntPropertyWatch::NewLC()" );
    CAccSrvPSIntPropertyWatch* self = new( ELeave ) CAccSrvPSIntPropertyWatch;
    CleanupStack::PushL( self );
    self->ConstructL( aObserver, aCategory, aKey, aType );
    COM_TRACE_1( "[AccFW:AccServer] CAccSrvPSIntPropertyWatch::NewLC - return %p", self );
    return self;
    }


// ---------------------------------------------------------
// Destructor
// ---------------------------------------------------------
//
CAccSrvPSIntPropertyWatch::~CAccSrvPSIntPropertyWatch()
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvPSIntPropertyWatch::~CAccSrvPSIntPropertyWatch()" );
    Cancel();
    iProperty.Close();
    COM_TRACE_( "[AccFW:AccServer] CAccSrvPSIntPropertyWatch::~CAccSrvPSIntPropertyWatch" );
    }

// ---------------------------------------------------------
// SubscribeNotification
// ---------------------------------------------------------
//
void CAccSrvPSIntPropertyWatch::Watch()
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvPSIntPropertyWatch::Watch()" );
    iProperty.Subscribe(iStatus);
    SetActive();
    COM_TRACE_( "[AccFW:AccServer] CAccSrvPSIntPropertyWatch::Watch - return void" );
    return;
    }


// ---------------------------------------------------------
// Get current value.
// ---------------------------------------------------------
//
TInt CAccSrvPSIntPropertyWatch::GetCurrentValue( TInt& aValue)
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvPSIntPropertyWatch::GetCurrentValue()" );
    TInt value = 0;
    TInt error( iProperty.Get( value ) );
    if ( KErrNone == error )
        {
        // use new value ...
        COM_TRACE_1( "[AccFW:AccServer] CAccSrvPSIntPropertyWatch::GetCurrentValue - value 0x%x", value );
        aValue = value;
        }
    else if ( KErrNotFound == error )
        {
        // property deleted, do necessary actions here...
        iObserver->PSPropertyDeleted( iCategory, iKey );
        }
    COM_TRACE_1( "[AccFW:AccServer] CAccSrvPSIntPropertyWatch::GetCurrentValue - return aError %d", error );
    return error;
    }


// ---------------------------------------------------------
// From class CActive.
// DoCancel
// ---------------------------------------------------------
//
void CAccSrvPSIntPropertyWatch::DoCancel()
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvPSIntPropertyWatch::DoCancel()" );
    iProperty.Cancel();
    COM_TRACE_( "[AccFW:AccServer] CAccSrvPSIntPropertyWatch::DoCancel - return void" );
    }


// ---------------------------------------------------------
// C++ constructor
// ---------------------------------------------------------
//
CAccSrvPSIntPropertyWatch::CAccSrvPSIntPropertyWatch()
    :CActive ( EPriority )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvPSIntPropertyWatch::CAccSrvPSIntPropertyWatch()" );
    COM_TRACE_( "[AccFW:AccServer] CAccSrvPSIntPropertyWatch::CAccSrvPSIntPropertyWatch - return" );
    }


// ---------------------------------------------------------
// ConstructL
// ---------------------------------------------------------
//
void CAccSrvPSIntPropertyWatch::ConstructL(
        MAccSrvPSIntPropertyObserver*   aObserver,
        TUid    aCategory,
        TInt    aKey,
        TOwnerType aType )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvPSIntPropertyWatch::ConstructL()" );
    iObserver = aObserver;
    iCategory = aCategory;
    iKey = aKey;    
    User::LeaveIfError( iProperty.Attach(iCategory,iKey, aType ) );
    CActiveScheduler::Add(this);
    COM_TRACE_( "[AccFW:AccServer] CAccSrvPSIntPropertyWatch::ConstructL - return void" );
    return;
   }

// ---------------------------------------------------------
// From class CActive.
// RunL
// ---------------------------------------------------------
//
void CAccSrvPSIntPropertyWatch::RunL()
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvPSIntPropertyWatch::RunL()" );

    // resubscribe before processing new value to prevent missing updates
    iProperty.Subscribe(iStatus);
    SetActive();
    // property updated, get new value
    TInt value;
    if ( iProperty.Get( value ) == KErrNotFound )
        {
        // property deleted, do necessary actions here...
        iObserver->PSPropertyDeleted( iCategory, iKey );
        }
    else
        {
        // use new value ...
        iObserver->PSPropertyChanged( iCategory, iKey, value );
        }
    COM_TRACE_( "[AccFW:AccServer] CAccSrvPSIntPropertyWatch::RunL - return void" );
    }


// ---------------------------------------------------------
// From class CActive.
// RunL
// ---------------------------------------------------------
//
TInt CAccSrvPSIntPropertyWatch::RunError( TInt aError )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvPSIntPropertyWatch::RunError()" );
    // Avoid Panic in CActiveScheduler
    aError = KErrNone;
    COM_TRACE_1( "[AccFW:AccServer] CAccSrvPSIntPropertyWatch::RunError - return aError %d", aError );
    return aError;
    }


// ======== GLOBAL FUNCTIONS ========

