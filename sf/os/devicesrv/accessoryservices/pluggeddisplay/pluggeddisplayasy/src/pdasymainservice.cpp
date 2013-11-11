/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
 * Description:
 * CPDAsyMainService class implementation.
 *
 */

// SYSTEM INCLUDES

// USER INCLUDES
#include "pdengine.h"
#include "pdasymainservice.h"
#include "trace.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// C++ constructor.
// ---------------------------------------------------------------------------
//
CPDAsyMainService::CPDAsyMainService()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor.
// ---------------------------------------------------------------------------
//
void CPDAsyMainService::ConstructL()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// Symbian two phased constructor.
// ---------------------------------------------------------------------------
//
CPDAsyMainService* CPDAsyMainService::NewL()
    {
    FUNC_LOG;

    CPDAsyMainService* self = CPDAsyMainService::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Symbian two phased constructor.
// Leaves pointer in the cleanup stack.
// ---------------------------------------------------------------------------
//
CPDAsyMainService* CPDAsyMainService::NewLC()
    {
    FUNC_LOG;

    CPDAsyMainService* self = new ( ELeave ) CPDAsyMainService;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// C++ destructor.
// ---------------------------------------------------------------------------
//
CPDAsyMainService::~CPDAsyMainService()
    {
    FUNC_LOG;
    
    delete iEngine;
    }

//------------------------------------------------------------------------------
// CPDAsyMainService::Engine
//------------------------------------------------------------------------------
//
CPDEngine& CPDAsyMainService::Engine() const
    {
    FUNC_LOG;
    
    return *iEngine;
    }

//------------------------------------------------------------------------------
// CPDAsyMainService::StartL
//------------------------------------------------------------------------------
//
TInt CPDAsyMainService::StartL()
    {
    FUNC_LOG;
	
    // Create Plugged Display Engine
    TRAPD( err, iEngine = CPDEngine::NewL( Session() ) );
    if( err != KErrNone )
        {
        ERROR( err, "Failed to load Plugged Display Engine." );
        User::Leave( err );
        }
    INFO( "Plugged Display Engine instance created" );
    
    // Plugged Display ASY is ready to receive commands
    Signal();
    
    return KErrNone;
    }

// End of file
