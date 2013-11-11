// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//




// INCLUDE FILES
#include "HWRMServerActiveScheduler.h"
#include "HWRMtrace.h"


// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS
_LIT( KPanicCategory, "HWRMActiveSched" );

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CHWRMServerActiveScheduler::CHWRMServerActiveScheduler
// C++ default constructor
// -----------------------------------------------------------------------------
//
CHWRMServerActiveScheduler::CHWRMServerActiveScheduler()
    : CActiveScheduler()
    {
    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMServerActiveScheduler::CHWRMServerActiveScheduler()" ));
    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMServerActiveScheduler::CHWRMServerActiveScheduler - return" ) );
    }

// -----------------------------------------------------------------------------
// CHWRMServerActiveScheduler::NewL
// Public constructor. Leaves if out of memory.
// -----------------------------------------------------------------------------
//
CHWRMServerActiveScheduler* CHWRMServerActiveScheduler::NewL()
    {
    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMServerActiveScheduler::NewL()" ));

    CHWRMServerActiveScheduler* scheduler = NewLC();
    
    CleanupStack::Pop(scheduler);

    COMPONENT_TRACE2(_L( "HWRM Server - CHWRMServerActiveScheduler::NewL - return 0x%x" ), scheduler );

    return scheduler;
    }

// -----------------------------------------------------------------------------
// CHWRMServerActiveScheduler::NewLC
// Public constructor. Leaves if out of memory. Pushes object to cleanup stack.
// -----------------------------------------------------------------------------
//
CHWRMServerActiveScheduler* CHWRMServerActiveScheduler::NewLC()
    {
    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMServerActiveScheduler::NewLC()" ));

    CHWRMServerActiveScheduler* scheduler = 
        new(ELeave) CHWRMServerActiveScheduler;

    CleanupStack::PushL( scheduler );

    COMPONENT_TRACE2(_L( "HWRM Server - CHWRMServerActiveScheduler::NewLC - return 0x%x" ), scheduler );

    return scheduler;
    }


// Destructor
CHWRMServerActiveScheduler::~CHWRMServerActiveScheduler()
    {
    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMServerActiveScheduler::~CHWRMServerActiveScheduler()" ));
    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMServerActiveScheduler::~CHWRMServerActiveScheduler - return" ) );
    }

// -----------------------------------------------------------------------------
// CHWRMServerActiveScheduler::Error
// Calls Panic function if occurred error is severe enough
// However, the execution should never come here unless the plugin 
// implementation doesn't TRAP all leaves it should.
// -----------------------------------------------------------------------------
//
void CHWRMServerActiveScheduler::Error( TInt aError ) const
    {
    COMPONENT_TRACE2(_L( "HWRM Server - CHWRMServerActiveScheduler::Error(%d)" ), aError );
    
    switch ( aError )
        {
        case KErrNoMemory:
            {
            // Recoverable, so ignore because we don't have anything to cleanup
            COMPONENT_TRACE1(_L( "HWRM Server - CHWRMServerActiveScheduler::Error : KErrNoMemory" ) );
            break;
            }
        default:
            {
            // Unrecoverable error for this thread. Panicing needed!
            COMPONENT_TRACE2(_L( "HWRM Server - CHWRMServerActiveScheduler::Error - Unrecoverable error: %d" ), aError );
    		User::Panic( KPanicCategory, aError );
            break;
            }
        }

    COMPONENT_TRACE1(_L( "HWRM Server - CHWRMServerActiveScheduler::Error - return" ));
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File  
