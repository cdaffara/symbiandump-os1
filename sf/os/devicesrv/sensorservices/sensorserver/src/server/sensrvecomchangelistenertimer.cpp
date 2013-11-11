/*
* Copyright (c) 2002-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Rescan timer for ECOM changes
*
*/


// INCLUDE FILES
#include "sensrvecomchangelistenertimer.h"
#include "sensrvecomchangelistener.h"


#include "sensrvdefines.h"
#include "ssycontrol.h"

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
// CSensrvEcomChangeListenerTimer::CSensrvEcomChangeListenerTimer
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSensrvEcomChangeListenerTimer::CSensrvEcomChangeListenerTimer( CSensrvEcomChangeListener* aListener )
    : CTimer( CActive::EPriorityHigh ),
      iListener( aListener )
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvEcomChangeListenerTimer::CSensrvEcomChangeListenerTimer()" ) ) );

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvEcomChangeListenerTimer::CSensrvEcomChangeListenerTimer - return void" ) ) );
    }

// -----------------------------------------------------------------------------
// CSensrvEcomChangeListenerTimer::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSensrvEcomChangeListenerTimer::ConstructL()
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvEcomChangeListenerTimer::ConstructL()" ) ) );

    CTimer::ConstructL();
    CActiveScheduler::Add( this );

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvEcomChangeListenerTimer::ConstructL - return void" ) ) );
    }

// -----------------------------------------------------------------------------
// CSensrvEcomChangeListenerTimer::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSensrvEcomChangeListenerTimer* CSensrvEcomChangeListenerTimer::NewL( CSensrvEcomChangeListener* aListener )
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvEcomChangeListenerTimer::NewL()" ) ) );

    CSensrvEcomChangeListenerTimer* self = new (ELeave) CSensrvEcomChangeListenerTimer( aListener );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvEcomChangeListenerTimer::NewL - return" ) ) );

    return self;
    }

// Destructor
CSensrvEcomChangeListenerTimer::~CSensrvEcomChangeListenerTimer()
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvEcomChangeListenerTimer::~CSensrvEcomChangeListenerTimer()" ) ) );

    Cancel();

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvEcomChangeListenerTimer::~CSensrvEcomChangeListenerTimer" ) ) );
    }

// -----------------------------------------------------------------------------
// CSensrvEcomChangeListenerTimer::RunL
// Initiates ECOM changelistener to rescan changes
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSensrvEcomChangeListenerTimer::RunL()
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvEcomChangeListenerTimer::RunL()" ) ) );

    iListener->TimerCallBack();

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvEcomChangeListenerTimer::RunL - return void" ) ) );
    }

// -----------------------------------------------------------------------------
// CSensrvEcomChangeListenerTimer::RunError
// Returns KErrNone
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CSensrvEcomChangeListenerTimer::RunError( TInt aError )
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvEcomChangeListenerTimer::RunError(%d)"), aError ) );

     // Avoid Panic in CActiveScheduler
    aError = KErrNone;

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvEcomChangeListenerTimer::RunError - return KErrNone" ) ) );

    return aError;
    }

// -----------------------------------------------------------------------------
// CSensrvEcomChangeListenerTimer::StartTimer
// Starts timer
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSensrvEcomChangeListenerTimer::StartTimer()
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvEcomChangeListenerTimer::StartTimer()" ) ) );

    After( ( TTimeIntervalMicroSeconds32 ) KSensrvEcomRescanTimerPeriod );

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvEcomChangeListenerTimer::StartTimer - return void" ) ) );
    }

// -----------------------------------------------------------------------------
// CSensrvEcomChangeListenerTimer::StartTimer
// Starts timer with specific timeout
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CSensrvEcomChangeListenerTimer::StartTimer(TTimeIntervalMicroSeconds32 timeout)
    {
    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvEcomChangeListenerTimer::StartTimer()" ) ) );

    After( timeout );

    COMPONENT_TRACE( ( _L( "Sensor Server - CSensrvEcomChangeListenerTimer::StartTimer - return void" ) ) );
    }
// ========================== OTHER EXPORTED FUNCTIONS =========================

// End of File
