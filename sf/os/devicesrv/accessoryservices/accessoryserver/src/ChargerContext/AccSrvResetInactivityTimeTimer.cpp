/*
* Copyright (c) 2008-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation of a timer interface.
*
*/



#include "AccSrvResetInactivityTimeTimer.h"
#include "acc_debug.h"


// ======== LOCAL FUNCTIONS ========


// ======== MEMBER FUNCTIONS ========


// -----------------------------------------------------------------------------
// CAccSrvResetInactivityTimeTimer::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CAccSrvResetInactivityTimeTimer* CAccSrvResetInactivityTimeTimer::NewL(
        MResetActivityTimeTimerObserver* aResetInactivityTimeTimerObserver )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvResetInactivityTimeTimer::NewL()" );

    CAccSrvResetInactivityTimeTimer* self = new ( ELeave ) CAccSrvResetInactivityTimeTimer(
            aResetInactivityTimeTimerObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    COM_TRACE_1( "[AccFW:AccServer] CAccSrvResetInactivityTimeTimer::NewL - return %p", self );

    return self;
    }


// Destructor
CAccSrvResetInactivityTimeTimer::~CAccSrvResetInactivityTimeTimer()
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvResetInactivityTimeTimer::~CAccSrvResetInactivityTimeTimer()" );

    Cancel();

    COM_TRACE_( "[AccFW:AccServer] CAccSrvResetInactivityTimeTimer::~CAccSrvResetInactivityTimeTimer- return" );
    }

// -----------------------------------------------------------------------------
// CAccSrvResetInactivityTimeTimer::StartTimer
// .
// -----------------------------------------------------------------------------
//
void CAccSrvResetInactivityTimeTimer::StartTimer(
        TInt aTime )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvResetInactivityTimeTimer::StartTimer()" );

    iTime = aTime;
    After( ( TTimeIntervalMicroSeconds32 ) iTime );

    COM_TRACE_( "[AccFW:AccServer] CAccSrvResetInactivityTimeTimer::StartTimer - return void" );

    return;
    }

// -----------------------------------------------------------------------------
// CAccSrvResetInactivityTimeTimer::RunL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CAccSrvResetInactivityTimeTimer::RunL()
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvResetInactivityTimeTimer::RunL()" );
    
    iResetInactivityTimeTimerObserver->InactivityTimerTimeout();

    COM_TRACE_( "[AccFW:AccServer] CAccSrvResetInactivityTimeTimer::RunL - return void" );
    }


// -----------------------------------------------------------------------------
// CAccSrvResetInactivityTimeTimer::RunError
// Returns KErrNone
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CAccSrvResetInactivityTimeTimer::RunError( TInt aError )
    {
    COM_TRACE_1( "[AccFW:AsyProxy] CAccSrvResetInactivityTimeTimer::RunError(%d)", aError );

     // Avoid Panic in CActiveScheduler
    aError = KErrNone;

    COM_TRACE_( "[AccFW:AsyProxy] CAccSrvResetInactivityTimeTimer::RunError - return KErrNone" );

    return aError;
    }


// -----------------------------------------------------------------------------
// CAccSrvResetInactivityTimeTimer::CAccSrvResetInactivityTimeTimer
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CAccSrvResetInactivityTimeTimer::CAccSrvResetInactivityTimeTimer(
        MResetActivityTimeTimerObserver* aResetInactivityTimeTimerObserver ) : 
    CTimer( CActive::EPriorityStandard ),
    iTime( NULL ),
    iResetInactivityTimeTimerObserver( aResetInactivityTimeTimerObserver )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvResetInactivityTimeTimer::CAccSrvResetInactivityTimeTimer()" );

    COM_TRACE_( "[AccFW:AccServer] CAccSrvResetInactivityTimeTimer::CAccSrvResetInactivityTimeTimer - return" );
    }

// -----------------------------------------------------------------------------
// CAccSrvProcessCommandTimer::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CAccSrvResetInactivityTimeTimer::ConstructL()
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvResetInactivityTimeTimer::ConstructL()" );

    CTimer::ConstructL();
    CActiveScheduler::Add( this );

    COM_TRACE_( "[AccFW:AccServer] CAccSrvResetInactivityTimeTimer::ConstructL - return void" );
    }



// ======== GLOBAL FUNCTIONS ========

