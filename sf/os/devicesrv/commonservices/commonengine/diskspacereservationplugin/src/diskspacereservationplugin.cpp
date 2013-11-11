/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation of CDiskSpaceReservationPlugIn class.
*
*/


#include "diskspacereservationplugin.h"

// Parameters
_LIT( KReserve, "reserve" );
_LIT( KFree, "free" );

// Free disk space requested for boot
const TInt KRequestedDiskSpaceForBoot = 0x23000; // 140 kB

// Debug macros
#ifdef _DEBUG
    #include <e32debug.h>
    #define TRACE_0( _s ) RDebug::Print( _L( _s ) );
    #define TRACE_1( _s, _t ) RDebug::Print( _L( _s ), _t );
    #define TRACE_IF_ERROR( _s, _e ) if ( _e != KErrNone ) RDebug::Print( _L( _s ), _e );
#else // _DEBUG
    #define TRACE_0( _s );
    #define TRACE_1( _s, _t );
    #define TRACE_IF_ERROR( _s, _e )
#endif // _DEBUG

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Constructor
//
// ---------------------------------------------------------------------------
//
CDiskSpaceReservationPlugIn* CDiskSpaceReservationPlugIn::NewL(
    TAny* aConstructionParameters )
    {
    CDiskSpaceReservationPlugIn* self =
        new( ELeave ) CDiskSpaceReservationPlugIn( aConstructionParameters );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// Destructor
//
// ---------------------------------------------------------------------------
//
CDiskSpaceReservationPlugIn::~CDiskSpaceReservationPlugIn()
    {
    TRACE_0( "CDiskSpaceReservationPlugIn::~CDiskSpaceReservationPlugIn" );

    iSharedData.Close();

    TRACE_0( "CDiskSpaceReservationPlugIn::~CDiskSpaceReservationPlugIn finished" );
    }


// ---------------------------------------------------------------------------
// CDiskSpaceReservationPlugIn::ExecuteL
//
// ---------------------------------------------------------------------------
//
void CDiskSpaceReservationPlugIn::ExecuteL(
    TRequestStatus& aStatus,
    const TDesC& aParams )
    {
    TRACE_1( "CDiskSpaceReservationPlugIn::ExecuteL( %S )", &aParams );

    aStatus = KRequestPending;
    TRequestStatus* status = &aStatus;

    if ( aParams == KReserve )
        {
        iSharedData.RequestFreeDiskSpace( KRequestedDiskSpaceForBoot );
        }
    else if ( aParams == KFree )
        {
        iSharedData.CancelFreeDiskSpaceRequest();
        }

    User::RequestComplete( status, KErrNone );

    TRACE_0( "CDiskSpaceReservationPlugIn::ExecuteL finished" );
    }


// ---------------------------------------------------------------------------
// CDiskSpaceReservationPlugIn::Cancel
//
// ---------------------------------------------------------------------------
//
void CDiskSpaceReservationPlugIn::Cancel()
    {
    // Nothing to do.
    }

// ---------------------------------------------------------------------------
// First phase constructor
//
// ---------------------------------------------------------------------------
//
CDiskSpaceReservationPlugIn::CDiskSpaceReservationPlugIn(
    TAny* aConstructionParameters )
  : CSystemStartupExtension( aConstructionParameters )
    {
    }


// ---------------------------------------------------------------------------
// Second phase constructor
//
// ---------------------------------------------------------------------------
//
void CDiskSpaceReservationPlugIn::ConstructL()
    {
    TInt errorCode = iSharedData.Connect();
    TRACE_IF_ERROR( "Failed to connect to SharedData", errorCode );
    User::LeaveIfError( errorCode );
    }
