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
* Description:  Accessory Mode handler
*
*/


// INCLUDE FILES
#include "AccSrvModeHandler.h"
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
// CAccSrvModeHandler::CAccSrvModeHandler
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CAccSrvModeHandler::CAccSrvModeHandler(
    MAccSrvConnectionControllerObserver* aCallback )
    : CAccSrvHandlerBase ( aCallback )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvModeHandler::CAccSrvModeHandler()" );
    }

// -----------------------------------------------------------------------------
// CAccSrvModeHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CAccSrvModeHandler::ConstructL()
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvModeHandler::ConstructL()" );
    }

// -----------------------------------------------------------------------------
// CAccSrvModeHandler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CAccSrvModeHandler* CAccSrvModeHandler::NewL(
    MAccSrvConnectionControllerObserver* aCallback )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvModeHandler::NewL()" );

    CAccSrvModeHandler* self = new( ELeave ) CAccSrvModeHandler( aCallback );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    COM_TRACE_( "[AccFW:AccServer] CAccSrvModeHandler::NewL - return self" );
    return self;
    }

// Destructor
CAccSrvModeHandler::~CAccSrvModeHandler()
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvModeHandler::~CAccSrvModeHandler()" );
    }

// -----------------------------------------------------------------------------
// CAccSrvModeHandler::RunL()
//
// Start Accessory Mode update  by calling call-back of Connection Controller.
// If RunL() leaves RunError from base class(CAccSrvHandlerBase) will be called
// and Accessory Mode update is stopped.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CAccSrvModeHandler::RunL()
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvModeHandler::RunL()" );

    iCallback->HandleAccessoryModeChangedL();

    COM_TRACE_( "[AccFW:AccServer] CAccSrvModeHandler::RunL - return void" );
    }

// End of File
