/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Timer for controlling asys loading
*
*/


// INCLUDE FILES
#include "ASYProxyLoaderTimer.h"
#include "ASYProxy.h"
#include "ASYProxyAsyLoader.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS
const TInt KASYLoadTimeOut = 5000000;

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CASYProxyLoaderTimer::CASYProxyLoaderTimer
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CASYProxyLoaderTimer::CASYProxyLoaderTimer( CASYProxyAsyLoader* aLoader )
    : CTimer( CActive::EPriorityHigh ),
      iLoader( aLoader )
    {
    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyLoaderTimer::CASYProxyLoaderTimer()" );

    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyLoaderTimer::CASYProxyLoaderTimer - return void" );
    }

// -----------------------------------------------------------------------------
// CASYProxyLoaderTimer::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CASYProxyLoaderTimer::ConstructL()
    {
    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyLoaderTimer::ConstructL()" );

    CTimer::ConstructL();
    CActiveScheduler::Add( this );

    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyLoaderTimer::ConstructL - return void" );
    }

// -----------------------------------------------------------------------------
// CASYProxyLoaderTimer::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CASYProxyLoaderTimer* CASYProxyLoaderTimer::NewL( CASYProxyAsyLoader* aLoader )
    {
    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyLoaderTimer::NewL()" );

    CASYProxyLoaderTimer* self = new (ELeave) CASYProxyLoaderTimer( aLoader );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    COM_TRACE_1( "[AccFW:AsyProxy] CASYProxyLoaderTimer::NewL - return 0x%x", self );

    return self;
    }

// Destructor
CASYProxyLoaderTimer::~CASYProxyLoaderTimer()
    {
    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyLoaderTimer::~CASYProxyLoaderTimer()" );

    Cancel();

    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyLoaderTimer::~CASYProxyLoaderTimer" );
    }

// -----------------------------------------------------------------------------
// CASYProxyLoaderTimer::RunL
// Notifies ASYProxy that time out in ASY loading came up
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CASYProxyLoaderTimer::RunL()
    {
    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyLoaderTimer::RunL()" );

    iLoader->iAsyProxy->LoadTimeOutL();

    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyLoaderTimer::RunL - return void" );
    }

// -----------------------------------------------------------------------------
// CASYProxyLoaderTimer::RunError
// Returns KErrNone
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CASYProxyLoaderTimer::RunError( TInt aError )
    {
    COM_TRACE_1( "[AccFW:AsyProxy] CASYProxyLoaderTimer::RunError(%d)", aError );

     // Avoid Panic in CActiveScheduler
    aError = KErrNone;

    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyLoaderTimer::RunError - return KErrNone" );

    return aError;
    }

// -----------------------------------------------------------------------------
// CASYProxyLoaderTimer::StartTimer
// Starts timer to control asy loading
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CASYProxyLoaderTimer::StartTimer()
    {
    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyLoaderTimer::StartTimer()" );

    After( ( TTimeIntervalMicroSeconds32 ) KASYLoadTimeOut );

    COM_TRACE_( "[AccFW:AsyProxy] CASYProxyLoaderTimer::StartTimer - return void" );
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

// End of File
