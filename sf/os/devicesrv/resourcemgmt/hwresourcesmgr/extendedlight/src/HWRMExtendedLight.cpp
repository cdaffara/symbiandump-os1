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
#include <e32svr.h>
#include "HWRMExtendedLight.h"
#include "HWRMExtendedLightImpl.h"
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
// CHWRMExtendedLight::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CHWRMExtendedLight* CHWRMExtendedLight::NewL()
    {
    CHWRMExtendedLight* impl = NewLC();
    
    CleanupStack::Pop();

    return impl;
    }
   
// -----------------------------------------------------------------------------
// CHWRMExtendedLight::NewLC
// Two-phased constructor. Leaves instance to CleanupStack.
// -----------------------------------------------------------------------------
//
EXPORT_C CHWRMExtendedLight* CHWRMExtendedLight::NewLC()
    {
    COMPONENT_TRACE1(_L( "HWRM ExtendedLightClient - CHWRMExtendedLight::NewLC()" ) );

    CHWRMExtendedLightImpl* impl = new( ELeave ) CHWRMExtendedLightImpl();
    
    CleanupStack::PushL( impl );
    impl->ConstructL(NULL);

    COMPONENT_TRACE2(_L( "HWRM ExtendedLightClient - CHWRMExtendedLight::NewLC - return 0x%x" ), impl );

    return static_cast<CHWRMExtendedLight*>(impl);
    }

// -----------------------------------------------------------------------------
// CHWRMExtendedLight::NewL
// Two-phased constructor for creating a Extended Light client with callbacks.
// -----------------------------------------------------------------------------
//
EXPORT_C CHWRMExtendedLight* CHWRMExtendedLight::NewL(MHWRMExtendedLightObserver* aCallback)
    {
    CHWRMExtendedLight* impl = NewLC(aCallback);
    
    CleanupStack::Pop();

    return impl;
    }
   
// -----------------------------------------------------------------------------
// CHWRMExtendedLight::NewLC
// Two-phased constructor for creating a Extended Light client with callbacks.
// Leaves instance to CleanupStack.
// -----------------------------------------------------------------------------
//
EXPORT_C CHWRMExtendedLight* CHWRMExtendedLight::NewLC(MHWRMExtendedLightObserver* aCallback)
    {
    COMPONENT_TRACE2(_L( "HWRM ExtendedLightClient - CHWRMExtendedLight::NewLC(0x%x)" ), aCallback );

    // Check that reference is not NULL
    if ( !aCallback )
        {
        User::Leave(KErrBadHandle);
        }

    CHWRMExtendedLightImpl* impl = new( ELeave ) CHWRMExtendedLightImpl();
    
    CleanupStack::PushL( impl );
    impl->ConstructL(aCallback);

    COMPONENT_TRACE2(_L( "HWRM LExtendedightClient - CHWRMExtendedLight::NewLC - return 0x%x" ), impl );

    return static_cast<CHWRMExtendedLight*>(impl);
    }

// -------------------------------------------------
// CHWRMExtendedLight::NewL
// Two-phased constructor for implementation class. 
// On opening passes permitted targets.
// -------------------------------------------------
//
EXPORT_C CHWRMExtendedLight* CHWRMExtendedLight::NewL(TInt aPermittedTargets)
	{		
	CHWRMExtendedLightImpl* impl = new(ELeave)CHWRMExtendedLightImpl;
	CleanupStack::PushL(impl);
	impl->ConstructL(NULL,aPermittedTargets);
	CleanupStack::Pop(impl);
	return impl;
	}

// -------------------------------------------------
// CHWRMExtendedLight::NewL
// Two-phased constructor for implementation class. 
// Use this method for creating a Light client with callbacks.
// On opening passes permitted targets.
// -------------------------------------------------
//
EXPORT_C CHWRMExtendedLight* CHWRMExtendedLight::NewL(MHWRMExtendedLightObserver* aCallback, TInt aPermittedTargets)
	{	
    // Check that reference is not NULL
    if ( !aCallback )
        {
        User::Leave(KErrBadHandle);
        }
	
	CHWRMExtendedLightImpl* impl = new(ELeave)CHWRMExtendedLightImpl;
	CleanupStack::PushL(impl);
	impl->ConstructL(aCallback,aPermittedTargets);
	CleanupStack::Pop(impl);
	return impl;
	}

// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File  
