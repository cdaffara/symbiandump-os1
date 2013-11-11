/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Haptic server shutdown timer.
*
*/


#include "hwrmhapticsshutdown.h"
#include "hwrmhapticsconfiguration.h"

// ---------------------------------------------------------------------------
// Two phased constructor.
// ---------------------------------------------------------------------------
//
CHWRMHapticsShutdown* CHWRMHapticsShutdown::NewL()
    {
    CHWRMHapticsShutdown* self = new ( ELeave ) CHWRMHapticsShutdown();
    CleanupStack::PushL( self );

    self->ConstructL();

    CleanupStack::Pop( self );

    return self;
    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CHWRMHapticsShutdown::~CHWRMHapticsShutdown()
    {
    }

// ---------------------------------------------------------------------------
// Activates this timer.
// ---------------------------------------------------------------------------
//
void CHWRMHapticsShutdown::Start()
    {
    After( KHWRMHapticsShutdownDelay );
    }

// ---------------------------------------------------------------------------
// Constructor of the shutdown timer.
// ---------------------------------------------------------------------------
//
CHWRMHapticsShutdown::CHWRMHapticsShutdown()
    : CTimer( EPriorityStandard )
    {
    // add this active object to the active scheduler
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// Second phase construction.
// ---------------------------------------------------------------------------
//
void CHWRMHapticsShutdown::ConstructL()
    {
    CTimer::ConstructL();
    }

// ---------------------------------------------------------------------------
// Shuts down this server by stopping the active scheduler.
// ---------------------------------------------------------------------------
//
void CHWRMHapticsShutdown::RunL()
    {
    CActiveScheduler::Stop();
    }

//  End of File  
