/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Haptics generic timer class definition.
*
*/


#include <e32base.h>
#include "hwrmhapticsgenerictimer.h"
#include "hwrmhapticstrace.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CHWRMHapticsGenericTimer::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CHWRMHapticsGenericTimer* CHWRMHapticsGenericTimer::NewL(
        MHWRMHapticsGenericTimerCallback* aCallback,
        const TTimeIntervalMicroSeconds32& aMaxTime,
        TInt aTimerId )
    {
    COMPONENT_TRACE( ( _L( "HWRMHaptics Server - CHWRMHapticsGenericTimer::NewL(0x%x, 0x%x, 0x%x)" ), aCallback, aMaxTime.Int(), aTimerId ) );

    CHWRMHapticsGenericTimer* self = new ( ELeave ) CHWRMHapticsGenericTimer(
                                                                aCallback,
                                                                aMaxTime,
                                                                aTimerId );
    CleanupStack::PushL( self );
    self->ConstructL();   //base call
    CleanupStack::Pop( self );

    CActiveScheduler::Add( self );

    COMPONENT_TRACE( ( _L( "HWRMHaptics Server - CHWRMHapticsGenericTimer::NewL - return 0x%x" ), self ) );
    return self;
    }

// -----------------------------------------------------------------------------
// CHWRMHapticsGenericTimer::CHWRMHapticsGenericTimer
// Constructor with callback class as a parameter.
// -----------------------------------------------------------------------------
//
CHWRMHapticsGenericTimer::CHWRMHapticsGenericTimer(
        MHWRMHapticsGenericTimerCallback* aCallback, 
        const TTimeIntervalMicroSeconds32& aMaxTime,
        TInt aTimerId )
        : CTimer( EPriorityHigh ), 
          iCallback( aCallback ),
          iMaximumTime( aMaxTime ),
          iCutOff( EFalse ),
          iTimerId( aTimerId )
    {
    COMPONENT_TRACE( ( _L( "HWRMHaptics Server - CHWRMHapticsGenericTimer::CHWRMHapticsGenericTimer(0x%x, 0x%x)" ), aCallback, aMaxTime.Int() ) );

    __ASSERT_ALWAYS( aCallback, User::Invariant() );

    COMPONENT_TRACE( ( _L( "HWRMHaptics Server - CHWRMHapticsGenericTimer::CHWRMHapticsGenericTimer - return" ) ) );
    }

// -----------------------------------------------------------------------------
// CHWRMHapticsGenericTimer::~CHWRMHapticsGenericTimer
// Destructor.
// -----------------------------------------------------------------------------
//
CHWRMHapticsGenericTimer::~CHWRMHapticsGenericTimer()
    {
    COMPONENT_TRACE( ( _L( "HWRMHaptics Server - CHWRMHapticsGenericTimer::~CHWRMHapticsGenericTimer()" ) ) );
    
    iCallback = NULL;
    
    COMPONENT_TRACE( ( _L( "HWRMHaptics Server - CHWRMHapticsGenericTimer::~CHWRMHapticsGenericTimer - return" ) ) );
    }

// -----------------------------------------------------------------------------
// CHWRMHapticsGenericTimer::Set
// Start the timer to complete after the specified number of microseconds.
// If the duration is zero, then timer is set to predefined maximum value.
// -----------------------------------------------------------------------------
//
void CHWRMHapticsGenericTimer::Set(const TTimeIntervalMicroSeconds32& aInterval)
    {
    COMPONENT_TRACE( ( _L( "HWRMHaptics Server - CHWRMHapticsGenericTimer::Set(%d)" ), aInterval.Int() ) );

    __ASSERT_ALWAYS( CActiveScheduler::Current(), User::Invariant() );

    if ( IsActive() )
        {
        Cancel();
        }
        
    // And set the new timer. If timer has specified maximum time, enforce that.
    TTime now;
    now.UniversalTime();
    
    if ( iMaximumTime.Int() != 0 && ( 0 == aInterval.Int() || aInterval > iMaximumTime ) ) 
        {
        // Maximum time has been specified, and interval parameter is such that 
        // it should be enforced
        iActivationTime = now + iMaximumTime;
       
        After( iMaximumTime );
        
        COMPONENT_TRACE( ( _L( "HWRMHaptics Server - CHWRMHapticsGenericTimer::Set - Using Maximum time %d" ), iMaximumTime.Int() ) );
        iCutOff = ETrue;
        }
    else if ( iMaximumTime.Int() == 0 && aInterval.Int() == 0 )
        {
        // No maximum time specified and interval calls for infinite duration.
        // --> Do not set the timer at all.
        }
    else
        {    
        // Otherwise just set the time to specified interval.
        iActivationTime = now + aInterval;
        After( aInterval );
        iCutOff = EFalse;
        }

    COMPONENT_TRACE( ( _L( "HWRMHaptics Server - CHWRMHapticsGenericTimer::Set - return" ) ) );
    }

// -----------------------------------------------------------------------------
// CHWRMHapticsGenericTimer::Freeze
// Stop timer and returns the remaining time
// -----------------------------------------------------------------------------
//
TTimeIntervalMicroSeconds32 CHWRMHapticsGenericTimer::Freeze()
    {
    COMPONENT_TRACE( ( _L( "HWRMHaptics Server - CHWRMHapticsGenericTimer::Freeze()" ) ) );
    
    // Must use 64 bit version as no way to get TTimeIntervalMicroSeconds32 from TTime.
    TTimeIntervalMicroSeconds frozenTime = 0LL;
    
    if ( IsAdded() && IsActive() )
        {
        Cancel();

        TTime now;
        now.UniversalTime();
        frozenTime = iActivationTime.MicroSecondsFrom( now );
        if ( frozenTime.Int64() <= 0 )
            {
            // Should have activated by now, set freeze timer to 1 for instant activation
            // after unfreeze.
            frozenTime = 1LL;
            }
        }
        
    COMPONENT_TRACE( ( _L( "HWRMHaptics Server - CHWRMHapticsGenericTimer::Freeze - return %d" ), frozenTime.Int64() ) );    

    // Loss of precision ok, as frozentime never exceeds 32 bits.    
    return frozenTime.Int64();
    }

// -----------------------------------------------------------------------------
// CHWRMHapticsGenericTimer::RunL
// RunL() function will be run after the specified system time expires,
// i.e. time is set by After() method,
// -----------------------------------------------------------------------------
void CHWRMHapticsGenericTimer::RunL()
    {
    COMPONENT_TRACE( ( _L( "HWRMHaptics Server - CHWRMHapticsGenericTimer::RunL()" ) ) );

    __ASSERT_ALWAYS( iCallback, User::Invariant() );
    
    iCallback->GenericTimerFired( iTimerId, iCutOff );
    
    COMPONENT_TRACE( ( _L( "HWRMHaptics Server - CHWRMHapticsGenericTimer::RunL - return" ) ) );
    }

//  End of File  
