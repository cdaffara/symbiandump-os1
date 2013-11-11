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
* Description:  Generic sensor server timer
*
*/


#include "sensrvdefines.h"
#include "sensrvtimer.h"
#include "sensrvtrace.h"

// -----------------------------------------------------------------------------
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSensrvTimer* CSensrvTimer::NewL(MSensrvTimerCallback& aCallback,
                                 const TTimeIntervalMicroSeconds32& aMaxTime,
                                 TInt aTimerId)
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvTimer::NewL(0x%x, %d, %d)" ), &aCallback, aMaxTime.Int(), aTimerId ) );

    CSensrvTimer* self = new (ELeave) CSensrvTimer(aCallback, aMaxTime, aTimerId);
    CleanupStack::PushL(self);
    self->ConstructL(); // Base call, needed or will KERN-EXEC 0 at ::After()
    CleanupStack::Pop(self); 

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvTimer::NewL - return 0x%x" ), self ) );

    return self;
    }

// -----------------------------------------------------------------------------
// Constructor with callback class as a parameter.
// -----------------------------------------------------------------------------
//
CSensrvTimer::CSensrvTimer(MSensrvTimerCallback& aCallback, 
                           const TTimeIntervalMicroSeconds32& aMaxTime,
                           TInt aTimerId)
        : CTimer(EPriorityHigh), 
          iCallback(aCallback),
          iMaximumTime(aMaxTime),
          iTimerId(aTimerId)
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvTimer::CSensrvTimer()" ) ) );

    // Nothing to do
    
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvTimer::CSensrvTimer - return" ) ) );
    }

// -----------------------------------------------------------------------------
// Destructor.
// -----------------------------------------------------------------------------
//
CSensrvTimer::~CSensrvTimer()
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvTimer::~CSensrvTimer()" ) ) );
    
    Cancel();
    
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvTimer::~CSensrvTimer - return" ) ) );
    }

// -----------------------------------------------------------------------------
// Start the timer to complete after the specified number of microseconds.
// If the duration is zero, then timer is set to predefined maximum value.
// -----------------------------------------------------------------------------
//
void CSensrvTimer::Set(const TTimeIntervalMicroSeconds32& aInterval)
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvTimer::Set(%d)" ), aInterval.Int() ) );

    if (CActiveScheduler::Current())
        {
        if (!IsAdded())
            {
            CActiveScheduler::Add(this);
            }
        
        // If the timer is already running, cancel it... 
        if (IsActive())
            {
            Cancel();
            }
            
        // ...and set the new timer. If timer has specified maximum time, enforce that.
        if ( iMaximumTime.Int() != 0 && (0 == aInterval.Int() || aInterval > iMaximumTime) ) 
            {
            // Maximum time has been specified, and interval parameter is such that 
            // it should be enforced
            After(iMaximumTime);
            
            COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvTimer::Set - Using Maximum time %d" ), iMaximumTime.Int() ) );
            }
        else if ( iMaximumTime.Int() == 0 && aInterval.Int() == 0 )
            {
            // No maximum time specified and interval calls for infinite duration.
            // --> Do not set the timer at all.
            }
        else
            {    
            // Otherwise just set the time to specified interval.
            After(aInterval);
            }
        }
    else
        {
        COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvTimer::Set - Scheduler not active, set ignored" ) ) );
        // If there are loaded plugins when shutting down the server, timer setting does occur
        }    

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvTimer::Set - return" ) ) );
    }

// -----------------------------------------------------------------------------
// RunL() function will be run after the specified system time expires,
// i.e. time is set by After() method,
// -----------------------------------------------------------------------------
void CSensrvTimer::RunL()
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvTimer::RunL()" )) );

    iCallback.TimerFired(iTimerId);

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvTimer::RunL - return" ) ) );
    }

//  End of File  
