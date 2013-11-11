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
* Description:
* Implementation of CSsmPsKeyObserver class.
*
*/


#include "ssmpskeyobserver.h"
#include "trace.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CSsmPsKeyObserver::CSsmPsKeyObserver
//
// ---------------------------------------------------------------------------
//
CSsmPsKeyObserver::CSsmPsKeyObserver( TUid aCategory, TUint aKey )
  : CActive( EPriorityHigh ),
    iCategory( aCategory ),
    iKey( aKey )
    {
    FUNC_LOG;

    CActiveScheduler::Add( this );
    }


// ---------------------------------------------------------------------------
// CSsmPsKeyObserver::~CSsmPsKeyObserver
//
// ---------------------------------------------------------------------------
//
CSsmPsKeyObserver::~CSsmPsKeyObserver()
    {
    FUNC_LOG;

    Cancel();
    iProperty.Close();
    Complete( KErrCancel ); // Just in case
    }


// ---------------------------------------------------------------------------
// CSsmPsKeyObserver::StartObserving
//
// ---------------------------------------------------------------------------
//
void CSsmPsKeyObserver::StartObserving( TRequestStatus& aStatus )
    {
    FUNC_LOG;
    ASSERT_TRACE( !IsActive() );

    aStatus = KRequestPending;
    iClientStatus = &aStatus;

    TInt errorCode = iProperty.Attach( iCategory, iKey );
    ERROR_2( errorCode, "Failed to attach to property 0x%08x::0x%08x", 
             iCategory.iUid, iKey );
    
    if ( errorCode == KErrNone )
        {
        CheckKeyValue();
        }
    else
        {
        Complete( errorCode );
        }
    }


// ---------------------------------------------------------------------------
// CSsmPsKeyObserver::DoCancel
//
// ---------------------------------------------------------------------------
//
void CSsmPsKeyObserver::DoCancel()
    {
    FUNC_LOG;

    iProperty.Cancel();
    Complete( KErrCancel );
    }


// ---------------------------------------------------------------------------
// CSsmPsKeyObserver::RunL
//
// ---------------------------------------------------------------------------
//
void CSsmPsKeyObserver::RunL()
    {
    FUNC_LOG;
    INFO_1( "CSsmPsKeyObserver::iStatus = %d", iStatus.Int() );

    if ( iStatus == KErrCancel ||
         iStatus == KErrServerTerminated ||
         iStatus ==KErrNotSupported )
        {
        Complete( iStatus.Int() );
        }
    else
        {
        CheckKeyValue();
        }
    }


// ---------------------------------------------------------------------------
// CSsmPsKeyObserver::CheckKeyValue
//
// ---------------------------------------------------------------------------
//
void CSsmPsKeyObserver::CheckKeyValue()
    {
    FUNC_LOG;
    ASSERT( !IsActive() );

    iProperty.Subscribe( iStatus );
    SetActive();

    TInt value( -KMaxTInt );
    TInt errorCode = iProperty.Get( value );
    ERROR_2( errorCode, "Failed to get value of property 0x%08x::0x%08x",
             iCategory.iUid, iKey );
    if ( errorCode != KErrNone || IsMatch( value ) )
        {
        INFO_3( "Property 0x%08x::0x%08x reached the target value %d.", 
                 iCategory.iUid, iKey, value );

        Complete( errorCode );
        Cancel();
        }
    else
        {
        INFO_3( "Value of property 0x%08x::0x%08x is %d. Waiting...",
                 iCategory.iUid, iKey, value );
        }
    }


// ---------------------------------------------------------------------------
// CSsmPsKeyObserver::Complete
//
// ---------------------------------------------------------------------------
//
void CSsmPsKeyObserver::Complete( const TInt aErrorCode )
    {
    FUNC_LOG;

    if ( iClientStatus )
        {
        INFO_1( "Completing request with status %d", aErrorCode );

        User::RequestComplete( iClientStatus, aErrorCode );
        iClientStatus = NULL;
        }
    }
