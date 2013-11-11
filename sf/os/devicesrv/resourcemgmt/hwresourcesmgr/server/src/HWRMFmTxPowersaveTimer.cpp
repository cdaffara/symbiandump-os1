// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//



#include "HWRMtrace.h"
#include "HWRMFmTxPowersaveTimer.h"


// -----------------------------------------------------------------------------
// C++ default constructor
// -----------------------------------------------------------------------------
//
CHWRMFmTxPowerSaveTimer::CHWRMFmTxPowerSaveTimer( MHWRMFmTxPowerSaveTimerObserver& aTimerObserver ):
    CTimer( CActive::EPriorityStandard ),
    iTimerObserver( aTimerObserver )
    {
    COMPONENT_TRACE1( ( _L( "HWRM Server - CHWRMFmTxPowerSaveTimer::CHWRMFmTxPowerSaveTimer" ) ) );
    }

// -----------------------------------------------------------------------------
// CHWRMFmTxPowerSaveTimer::~CHWRMFmTxPowerSaveTimer
// Destructor
// -----------------------------------------------------------------------------
//
CHWRMFmTxPowerSaveTimer::~CHWRMFmTxPowerSaveTimer()
    {
    COMPONENT_TRACE1( ( _L( "HWRM Server - CHWRMFmTxPowerSaveTimer::~CHWRMFmTxPowerSaveTimer" ) ) );
    
    Cancel();
    
    COMPONENT_TRACE1( ( _L( "HWRM Server - CHWRMFmTxPowerSaveTimer::~CHWRMFmTxPowerSaveTimer - return" ) ) );
    }

// -----------------------------------------------------------------------------
// PowerSaveTimer::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//	
CHWRMFmTxPowerSaveTimer* CHWRMFmTxPowerSaveTimer::NewL( MHWRMFmTxPowerSaveTimerObserver& aTimerObserver )
    {
    COMPONENT_TRACE1( ( _L( "HWRM Server - CHWRMFmTxPowerSaveTimer::NewL" ) ) );
   
    CHWRMFmTxPowerSaveTimer* self = new ( ELeave ) CHWRMFmTxPowerSaveTimer( aTimerObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    COMPONENT_TRACE2( _L( "HWRM Server - CHWRMFmTxPowerSaveTimer::NewL - return 0x%x" ), self );
    return self;
    }

// -----------------------------------------------------------------------------
// CHWRMFmTxPowerSaveTimer::ConstructL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
void CHWRMFmTxPowerSaveTimer::ConstructL()
    {
    COMPONENT_TRACE1( _L( "HWRM Server - CHWRMFmTxPowerSaveTimer::ConstructL" ) );

    CTimer::ConstructL();
    CActiveScheduler::Add( this );
    
    COMPONENT_TRACE1( _L( "HWRM Server - CHWRMFmTxPowerSaveTimer::ConstructL - return" ) );
    }
	
// -----------------------------------------------------------------------------
// CHWRMFmTxPowerSaveTimer::Start
// Starts timer, timeout value given as a parameter.
// -----------------------------------------------------------------------------
//
void CHWRMFmTxPowerSaveTimer::Start( const TTimeIntervalMicroSeconds32 aTimeout )
    {
    COMPONENT_TRACE1( _L( "HWRM Server - CHWRMFmTxPowerSaveTimer::Start" ) );
    
    Cancel();
    CTimer::After( aTimeout );
    
    COMPONENT_TRACE1( _L( "HWRM Server - CHWRMFmTxPowerSaveTimer::Start - return" ) );
    }

// -----------------------------------------------------------------------------
// CHWRMFmTxPowerSaveTimer::Stop
// Stops timer
// -----------------------------------------------------------------------------
void CHWRMFmTxPowerSaveTimer::Stop()	
    {
    COMPONENT_TRACE1( _L( "HWRM Server - CHWRMFmTxPowerSaveTimer::Stop" ) );
    
    Cancel();
    
    COMPONENT_TRACE1( _L( "HWRM Server - CHWRMFmTxPowerSaveTimer::Stop - return" ) );
    }

// -----------------------------------------------------------------------------
// CHWRMFmTxPowerSaveTimer::RunL
// Handles an active object's request completion event.
// -----------------------------------------------------------------------------
//
void CHWRMFmTxPowerSaveTimer::RunL()
    {
    COMPONENT_TRACE1( _L( "HWRM Server - CHWRMFmTxPowerSaveTimer::RunL" ) );

    if ( iStatus == KErrNone )
        {
        iTimerObserver.TimerExpired();
        }
        
    COMPONENT_TRACE1( _L( "HWRM Server - CHWRMFmTxPowerSaveTimer::RunL - return" ) );
    }

// End of file
