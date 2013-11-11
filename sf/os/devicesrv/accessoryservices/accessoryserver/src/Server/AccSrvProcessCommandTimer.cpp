/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Timer for ProcessCommand handling
*
*/


// INCLUDE FILES
#include "AccSrvProcessCommandObserver.h"
#include "AccSrvProcessCommandTimer.h"
#include "AccSrvConnectionController.h"
#include "AccSrvServerModel.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CAccSrvProcessCommandTimer::CAccSrvProcessCommandTimer
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CAccSrvProcessCommandTimer::CAccSrvProcessCommandTimer(
    MAccSrvProcessCommandObserver* aProcessCommandObserver,
    TInt aTime ) : 
    CTimer( CActive::EPriorityStandard ),
    iTime( aTime ),
    iProcessCommandObserver( aProcessCommandObserver )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvProcessCommandTimer::CAccSrvProcessCommandTimer()" );

    COM_TRACE_( "[AccFW:AccServer] CAccSrvProcessCommandTimer::CAccSrvProcessCommandTimer - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvProcessCommandTimer::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CAccSrvProcessCommandTimer::ConstructL()
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvProcessCommandTimer::ConstructL()" );

    CTimer::ConstructL();
    CActiveScheduler::Add( this );

    COM_TRACE_( "[AccFW:AccServer] CAccSrvProcessCommandTimer::ConstructL - return void" );
    }
    
// -----------------------------------------------------------------------------
// CAccSrvProcessCommandTimer::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CAccSrvProcessCommandTimer* CAccSrvProcessCommandTimer::NewL(
                            MAccSrvProcessCommandObserver* aProcessCommandObserver,
                            TInt aTime )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvProcessCommandTimer::NewL()" );

    CAccSrvProcessCommandTimer* self = new ( ELeave ) CAccSrvProcessCommandTimer(
                                                        aProcessCommandObserver,
                                                        aTime );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    COM_TRACE_1( "[AccFW:AccServer] CAccSrvProcessCommandTimer::NewL - return 0x%x", self );

    return self;
    }

// Destructor
CAccSrvProcessCommandTimer::~CAccSrvProcessCommandTimer()
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvProcessCommandTimer::~CAccSrvProcessCommandTimer()" );

    Cancel();

    COM_TRACE_( "[AccFW:AccServer] CAccSrvProcessCommandTimer::~CAccSrvProcessCommandTimer" );
    }

// -----------------------------------------------------------------------------
// CAccSrvProcessCommandTimer::RunL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CAccSrvProcessCommandTimer::RunL()
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvProcessCommandTimer::RunL()" );
    
    if ( iTime == KInitCmdTimeOut )
        {
        iProcessCommandObserver->CompleteProcessCommandInitL();
        }

    else
        {
        iProcessCommandObserver->ProcessCommandTimeOutCompleteL();
        }

    COM_TRACE_( "[AccFW:AccServer] CAccSrvProcessCommandTimer::RunL - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvProcessCommandTimer::RunError
// Returns KErrNone
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CAccSrvProcessCommandTimer::RunError( TInt aError )
    {
    COM_TRACE_1( "[AccFW:AsyProxy] CAccSrvProcessCommandTimer::RunError(%d)", aError );

     // Avoid Panic in CActiveScheduler
    aError = KErrNone;

    COM_TRACE_( "[AccFW:AsyProxy] CAccSrvProcessCommandTimer::RunError - return KErrNone" );

    return aError;
    }

// -----------------------------------------------------------------------------
// CAccSrvProcessCommandTimer::StartTimer
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CAccSrvProcessCommandTimer::StartTimer()
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvProcessCommandTimer::StartTimer()" );

    After( ( TTimeIntervalMicroSeconds32 ) iTime );

    COM_TRACE_( "[AccFW:AccServer] CAccSrvProcessCommandTimer::StartTimer - return void" );
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

// End of File
