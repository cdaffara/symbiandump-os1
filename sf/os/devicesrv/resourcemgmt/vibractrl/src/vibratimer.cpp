/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Timer class for superviving Vibrating time
*
*/


// INCLUDE FILES
#include "vibratimer.h"
#include "vibractrl.h" //TVibraCtrlPanic

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CVibraTimer::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CVibraTimer* CVibraTimer::NewL(MVibraTimerCallback* aCallback, TInt aMaxVibTime)
    {
    CVibraTimer* p = new (ELeave) CVibraTimer(aCallback, aMaxVibTime);
    CleanupStack::PushL(p);
    p->ConstructL();//base call
    CleanupStack::Pop(); // p
    return p;
    }

// -----------------------------------------------------------------------------
// CVibraTimer::CVibraTimer(MVibraTimerCallback* aCallback)
// Constructor with callback class as a parameter.
// -----------------------------------------------------------------------------
//
CVibraTimer::CVibraTimer(MVibraTimerCallback* aCallback, TInt aMaxVibTime)
:CTimer(EPriorityHigh), 
iCallback(aCallback),
iMaximumVibraTimeMs(aMaxVibTime)
    {
    __ASSERT_ALWAYS(aCallback != NULL, User::Invariant());
    }

// -----------------------------------------------------------------------------
// CVibraTimer::~CVibraTimer
// Destructor.
// -----------------------------------------------------------------------------
//
CVibraTimer::~CVibraTimer()
    {
    }

// -----------------------------------------------------------------------------
// CVibraTimer::Set(TInt aIntervalInMilliSecs)
// Start the timer to complete after the specified number of microseconds.
// If the duration is zero, then timer is set to predefined maximum value.
// -----------------------------------------------------------------------------
//
TInt CVibraTimer::Set(TInt aIntervalInMilliSecs)
    {
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
    // And set the new timer... 
    // Convert to uS first -- which is, after all, why this method really exists...
    if ((0 == aIntervalInMilliSecs) || (aIntervalInMilliSecs > iMaximumVibraTimeMs))
        {
        After(iMaximumVibraTimeMs * 1000);
        }
    else
        {    
        After(aIntervalInMilliSecs * 1000);
        }
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CVibraTimer::RunL
// RunL() function will be run after the specified system time expires,
// i.e. time is set by After() method,
// -----------------------------------------------------------------------------
void CVibraTimer::RunL()
    {
    __ASSERT_ALWAYS(iCallback != NULL, User::Invariant());
    
    iCallback->TimerFired();
    }

//  End of File  
