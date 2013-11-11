// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include "EPos_CPosServerDelayedShutdown.h"

// ================= MEMBER FUNCTIONS =======================

/**
 * C++ Constructor
 */
CPosServerDelayedShutdown::CPosServerDelayedShutdown()
    : CTimer(EPriorityLow)
    {
    CActiveScheduler::Add(this);
    }

/**
 * C++ Destructor
 */
CPosServerDelayedShutdown::~CPosServerDelayedShutdown()
    {
    Cancel();
    }

/**
 * Two-phase constructor
 */
CPosServerDelayedShutdown* CPosServerDelayedShutdown::NewL()
    {
    CPosServerDelayedShutdown* self = new (ELeave) CPosServerDelayedShutdown;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

/**
 * Symbian default constructor
 */
void CPosServerDelayedShutdown::ConstructL()
    {
    CTimer::ConstructL();
    }

/**
 * Start shutdown countdown.
 */
void CPosServerDelayedShutdown::Start(TTimeIntervalMicroSeconds32 aDelay)
    {
    After(aDelay);
    }

/**
 * From CActive.
 * Called when timer completes
 */
void CPosServerDelayedShutdown::RunL()
    {
    // Shutdown the server by shutting down the active scheduler.
    CActiveScheduler::Stop();
    }

// End of File
