// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <e32svr.h>
#include "HWRMVibra.h"
#include "HWRMVibraImpl.h"
#include "HWRMtrace.h"

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
// CHWRMVibra::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CHWRMVibra* CHWRMVibra::NewL()
    {
    CHWRMVibra* impl = NewLC();
    
    CleanupStack::Pop();

    return impl;
    }
   
// -----------------------------------------------------------------------------
// CHWRMVibra::NewLC
// Two-phased constructor. Leaves instance to CleanupStack.
// -----------------------------------------------------------------------------
//
EXPORT_C CHWRMVibra* CHWRMVibra::NewLC()
    {
    COMPONENT_TRACE1(_L("HWRM VibraClient - CHWRMVibra::NewLC()"));

    CHWRMVibraImpl* impl = new( ELeave ) CHWRMVibraImpl();
    
    CleanupStack::PushL( impl );
    impl->ConstructL(NULL);

    COMPONENT_TRACE2(_L( "HWRM VibraClient - CHWRMVibra::NewLC - return 0x%x" ), impl );

    return static_cast<CHWRMVibra*>(impl);
    }

// -----------------------------------------------------------------------------
// CHWRMVibra::NewL
// Two-phased constructor for creating a vibra client with callbacks.
// -----------------------------------------------------------------------------
//
EXPORT_C CHWRMVibra* CHWRMVibra::NewL(MHWRMVibraObserver* aCallback)
    {
    CHWRMVibra* impl = NewLC(aCallback);
    
    CleanupStack::Pop();

    return impl;
    }
   
// -----------------------------------------------------------------------------
// CHWRMVibra::NewLC
// Two-phased constructor for creating a vibra client with callbacks.
// Leaves instance to CleanupStack.
// -----------------------------------------------------------------------------
//
EXPORT_C CHWRMVibra* CHWRMVibra::NewLC(MHWRMVibraObserver* aCallback)
    {
    COMPONENT_TRACE2(_L( "HWRM VibraClient - CHWRMVibra::NewLC(0x%x)" ), aCallback );

    // Check that reference is not NULL
    if ( !aCallback )
        {
        User::Leave(KErrBadHandle);
        }

    CHWRMVibraImpl* impl = new( ELeave ) CHWRMVibraImpl();
    
    CleanupStack::PushL( impl );
    impl->ConstructL(aCallback);

    COMPONENT_TRACE2(_L( "HWRM VibraClient - CHWRMVibra::NewLC - return 0x%x" ), impl);

    return static_cast<CHWRMVibra*>(impl);
    }
// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File
