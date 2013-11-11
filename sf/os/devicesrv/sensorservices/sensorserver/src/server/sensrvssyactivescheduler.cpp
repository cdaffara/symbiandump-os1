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
* Description:  Active scheduler for SSY threads. 
*                This is required, since it cannot be guaranteed SSY 
*                implementations handle RunL errors properly. 
*
*/



// INCLUDE FILES
#include "sensrvssyactivescheduler.h"
#include "sensrvdefines.h"
#include "sensrvtrace.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSensrvSsyActiveScheduler::CSensrvSsyActiveScheduler
// C++ default constructor
// -----------------------------------------------------------------------------
//
CSensrvSsyActiveScheduler::CSensrvSsyActiveScheduler()
    : CActiveScheduler()
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvSsyActiveScheduler::CSensrvSsyActiveScheduler()" )) );
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvSsyActiveScheduler::CSensrvSsyActiveScheduler - return" ) ) );
    }

// -----------------------------------------------------------------------------
// CSensrvSsyActiveScheduler::NewL
// Public constructor. Leaves if out of memory.
// -----------------------------------------------------------------------------
//
CSensrvSsyActiveScheduler* CSensrvSsyActiveScheduler::NewL()
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvSsyActiveScheduler::NewL()" )) );

    CSensrvSsyActiveScheduler* scheduler = new(ELeave) CSensrvSsyActiveScheduler;

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvSsyActiveScheduler::NewL - return 0x%x" ), scheduler ) );

    return scheduler;
    }

// -----------------------------------------------------------------------------
// CSensrvSsyActiveScheduler::NewLC
// Public constructor. Leaves if out of memory. Pushes object to cleanup stack.
// -----------------------------------------------------------------------------
//
CSensrvSsyActiveScheduler* CSensrvSsyActiveScheduler::NewLC()
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvSsyActiveScheduler::NewLC()" )) );

    CSensrvSsyActiveScheduler* scheduler = 
        new(ELeave) CSensrvSsyActiveScheduler;

    CleanupStack::PushL( scheduler );

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvSsyActiveScheduler::NewLC - return 0x%x" ), scheduler ) );

    return scheduler;
    }


// Destructor
CSensrvSsyActiveScheduler::~CSensrvSsyActiveScheduler()
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvSsyActiveScheduler::~CHWRMServerActiveScheduler()" )) );
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvSsyActiveScheduler::~CHWRMServerActiveScheduler - return" ) ) );
    }

// -----------------------------------------------------------------------------
// CSensrvSsyActiveScheduler::Error
// Panic SSY thread if unhandled error occurs in SSY.
// -----------------------------------------------------------------------------
//
void CSensrvSsyActiveScheduler::Error( TInt aError ) const
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvSsyActiveScheduler::Error - Unrecoverable error: %d" ), aError ) );

    // It should never get here, but in case SSY plugin does not handle RunL leaves properly,
    // it is possible.
    User::Panic( KSensrvPanicCategory, aError );

    COMPONENT_TRACE( ( _L( "HWRM Server - CHWRMServerActiveScheduler::Error - return" )) );
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File  
