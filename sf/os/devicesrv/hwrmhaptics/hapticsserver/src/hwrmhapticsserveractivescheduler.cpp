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
* Description:  Haptic server active scheduler implementation.
*
*/


#include "hwrmhapticsserveractivescheduler.h"
#include "hwrmhapticstrace.h"

// ---------------------------------------------------------------------------
// C++ default constructor
// ---------------------------------------------------------------------------
//
CHWRMHapticsServerActiveScheduler::CHWRMHapticsServerActiveScheduler()
    : CActiveScheduler()
    {
    COMPONENT_TRACE( _L( "CHWRMHapticsServerActiveScheduler::CHWRMHapticsServerActiveScheduler()") );
    }

// ---------------------------------------------------------------------------
// Public constructor. Leaves if out of memory.
// ---------------------------------------------------------------------------
//
CHWRMHapticsServerActiveScheduler* CHWRMHapticsServerActiveScheduler::NewL()
    {
    COMPONENT_TRACE( ( _L( "CHWRMHapticsServerActiveScheduler::NewL()" )) );

    CHWRMHapticsServerActiveScheduler* scheduler = 
        new ( ELeave ) CHWRMHapticsServerActiveScheduler;

    COMPONENT_TRACE( ( _L( "CHWRMHapticsServerActiveScheduler::NewL - return 0x%x" ), scheduler ) );

    return scheduler;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CHWRMHapticsServerActiveScheduler::~CHWRMHapticsServerActiveScheduler()
    {
    COMPONENT_TRACE( _L( "CHWRMHapticsServerActiveScheduler::~CHWRMHapticsServerActiveScheduler()") );
    }

// ---------------------------------------------------------------------------
// A leave from any active object's RunL() causes active scheduler's 
// Error() -method to be called. The default implementation panics the thread.
// This implementation overrides the default implementation, and is defined 
// here as empty implementation, so that the server thread is never paniced
// in case of an error.
// ---------------------------------------------------------------------------
//
void CHWRMHapticsServerActiveScheduler::Error( TInt /*anError*/ ) const
    {
    }

//  End of File  
