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




// INCLUDE FILES
#include <e32base.h>
#include "HWRMGenericTimer.h"
#include "HWRMtrace.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CHWRMGenericTimer::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CHWRMGenericTimer* CHWRMGenericTimer::NewL(MHWRMGenericTimerCallback& aCallback,
                                       const TTimeIntervalMicroSeconds32& aMaxTime,
                                       TInt aTimerId)
    {
    COMPONENT_TRACE4(_L( "HWRM Server - CHWRMGenericTimer::NewL(0x%x, 0x%x, 0x%x)" ), &aCallback, aMaxTime.Int(), aTimerId );

    CHWRMGenericTimer* self = new (ELeave) CHWRMGenericTimer(aCallback, aMaxTime, aTimerId);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);

    COMPONENT_TRACE2(_L( "HWRM Server - CHWRMGenericTimer::NewL - return 0x%x" ), self );

    return self;
    }

// -----------------------------------------------------------------------------
// CHWRMGenericTimer::CHWRMGenericTimer
// Constructor with callback class as a parameter.
// -----------------------------------------------------------------------------
//
CHWRMGenericTimer::CHWRMGenericTimer(MHWRMGenericTimerCallback& aCallback, 
                                 const TTimeIntervalMicroSeconds32& aMaxTime,
                                 TInt aTimerId)
                            : CTimer(EPriorityHigh), 
                             iCallback(aCallback),
                             iMaximumTime(aMaxTime),
                             iCutOff(EFalse),
                             iTimerId(aTimerId)
    {
    COMPONENT_TRACE3(_L( "HWRM Server - CHWRMGenericTimer::CHWRMGenericTimer(0x%x, 0x%x)" ), &aCallback, aMaxTime.Int() );

    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMGenericTimer::CHWRMGenericTimer - return" ) );
    }

// -----------------------------------------------------------------------------
// CHWRMGenericTimer::~CHWRMGenericTimer
// Destructor.
// -----------------------------------------------------------------------------
//
CHWRMGenericTimer::~CHWRMGenericTimer()
    {
    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMGenericTimer::~CHWRMGenericTimer()" ) );
    
    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMGenericTimer::~CHWRMGenericTimer - return" ) );
    }

// -----------------------------------------------------------------------------
// CHWRMGenericTimer::Set
// Start the timer to complete after the specified number of microseconds.
// If the duration is zero, then timer is set to predefined maximum value.
// -----------------------------------------------------------------------------
//
void CHWRMGenericTimer::Set(const TTimeIntervalMicroSeconds32& aInterval)
    {
    COMPONENT_TRACE2(_L( "HWRM Server - CHWRMGenericTimer::Set(%d)" ), aInterval.Int() );

    __ASSERT_ALWAYS(CActiveScheduler::Current()!= NULL, User::Invariant());
    
    if (!IsAdded())
        {
        CActiveScheduler::Add(this);
        }
    
    // If the timer is already running, cancel it... 
    if (IsActive())
        {
        Cancel();
        }
        
    // And set the new timer. If timer has specified maximum time, enforce that.
    TTime now;
    now.UniversalTime();
    
    if ( iMaximumTime.Int() != 0 && (0 == aInterval.Int() || aInterval > iMaximumTime) ) 
        {
        // Maximum time has been specified, and interval parameter is such that 
        // it should be enforced
        iActivationTime = now + iMaximumTime;
       
        After(iMaximumTime);
        
        COMPONENT_TRACE2(_L( "HWRM Server - CHWRMGenericTimer::Set - Using Maximum time %d" ), iMaximumTime.Int() );
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
        After(aInterval);
        iCutOff = EFalse;
        }

    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMGenericTimer::Set - return" ) );
    }

// -----------------------------------------------------------------------------
// CHWRMGenericTimer::Freeze
// Stop timer and returns the remaining time
// -----------------------------------------------------------------------------
//
TTimeIntervalMicroSeconds32 CHWRMGenericTimer::Freeze()
    {
    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMGenericTimer::Freeze()" ));
    
    // Must use 64 bit version as no way to get TTimeIntervalMicroSeconds32 from TTime.
    TTimeIntervalMicroSeconds frozenTime = 0LL;
    
    if ( IsAdded() && IsActive() )
        {
        Cancel();

        TTime now;
        now.UniversalTime();
        frozenTime = iActivationTime.MicroSecondsFrom(now);
        if ( frozenTime.Int64() <= 0 )
            {
            // Should have activated by now, set freeze timer to 1 for instant activation
            // after unfreeze.
            frozenTime = 1LL;
            }
        }
        
    COMPONENT_TRACE2(_L( "HWRM Server - CHWRMGenericTimer::Freeze - return %d" ), frozenTime.Int64() );    

    // Loss of precision ok, as frozentime never exceeds 32 bits.    
    return frozenTime.Int64();
    }

// -----------------------------------------------------------------------------
// CHWRMGenericTimer::RunL
// RunL() function will be run after the specified system time expires,
// i.e. time is set by After() method,
// -----------------------------------------------------------------------------
void CHWRMGenericTimer::RunL()
    {
    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMGenericTimer::RunL()" ));

    iCallback.GenericTimerFired(iTimerId, iCutOff);

    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMGenericTimer::RunL - return" ) );
    }

void CHWRMGenericTimer::SetMaximumTime(TTimeIntervalMicroSeconds32& aMaximumTime)
	{
	iMaximumTime = aMaximumTime;
	}


//  End of File  
