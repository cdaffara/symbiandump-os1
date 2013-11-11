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
* Description:  Connection Status Handler
*
*/


// INCLUDE FILES
#include "AccSrvConnectionStatusHandler.h"
#include "AccSrvConnectionController.h" //MAccSrvConnectionControllerObserver
#include "acc_debug.h"

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
// CAccSrvConnectionStatusHandler::CAccSrvConnectionStatusHandler
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CAccSrvConnectionStatusHandler::CAccSrvConnectionStatusHandler()
    : CAccSrvHandlerBase ()
    {
    //Never called since only NewL(MAccSrvConnectionControllerObserver* aCallback) is defined
    TRACE_ASSERT_ALWAYS;
    }

// -----------------------------------------------------------------------------
// CAccSrvConnectionStatusHandler::CAccSrvConnectionStatusHandler
// -----------------------------------------------------------------------------
//
CAccSrvConnectionStatusHandler::CAccSrvConnectionStatusHandler(
    MAccSrvConnectionControllerObserver* aCallback )
    : CAccSrvHandlerBase (aCallback)
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvConnectionStatusHandler::CAccSrvConnectionStatusHandler()" );
    }

// -----------------------------------------------------------------------------
// CAccSrvConnectionStatusHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CAccSrvConnectionStatusHandler::ConstructL()
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvConnectionStatusHandler::ConstructL()" );
    }

// -----------------------------------------------------------------------------
// CAccSrvConnectionStatusHandler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CAccSrvConnectionStatusHandler* CAccSrvConnectionStatusHandler::NewL( 
    MAccSrvConnectionControllerObserver* aCallback )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvConnectionStatusHandler::NewL()" );
    CAccSrvConnectionStatusHandler* self = 
        new( ELeave ) CAccSrvConnectionStatusHandler( aCallback );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    COM_TRACE_( "[AccFW:AccServer] CAccSrvConnectionStatusHandler::NewL - return self" );
    return self;
    }

// Destructor
CAccSrvConnectionStatusHandler::~CAccSrvConnectionStatusHandler()
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvConnectionStatusHandler::~CAccSrvConnectionStatusHandler()" );
    }

// -----------------------------------------------------------------------------
// CAccSrvConnectionStatusHandler::RunL
//
// Start Accessory Connection Status update by calling call-back of
// Connection Controller. If RunL() leaves RunError from this class will be called.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CAccSrvConnectionStatusHandler::RunL()
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvConnectionStatusHandler::RunL()" );
    iCallback->HandleConnectionStatusChangedL();
    COM_TRACE_( "[AccFW:AccServer] CAccSrvConnectionStatusHandler::RunL - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvConnectionStatusHandler::RunError
//
// Called if RunL Leaves
// -----------------------------------------------------------------------------
//
TInt CAccSrvConnectionStatusHandler::RunError( TInt aError )
    {
    COM_TRACE_1( "[AccFW:AccServer] CAccSrvConnectionStatusHandler::RunError(%d)", aError );

#ifdef _DEBUG
    TRAPD( errHandle, iCallback->HandleAccessoryModeChangedL() );
    COM_TRACE_1( "[AccFW:AccServer] CAccSrvConnectionStatusHandler::RunError - errHandle == %d", errHandle );
#else
    TRAP_IGNORE( iCallback->HandleAccessoryModeChangedL() );
#endif

    // Avoid Panic in CActiveScheduler
    aError = KErrNone;
    COM_TRACE_( "[AccFW:AccServer] CAccSrvConnectionStatusHandler::RunError - return KErrNone" );
    return aError;
    }

// End of File
