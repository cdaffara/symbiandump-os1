// Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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



//  INCLUDE FILES
#include "CMmFaxExtInterface.h"

#ifdef FAX_IS_SUPPORTED
#include "cmmfaxext.h"
#endif

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CMmFaxExtInterface::NewL
// Create implementor object for CMmFaxExtInterface.
// Returns: pointer to interface object
// ---------------------------------------------------------------------------
//
#ifdef FAX_IS_SUPPORTED
CMmFaxExtInterface* CMmFaxExtInterface::NewL( 
        CMmCallTsy* aMmCall )
    {
    return CMmFaxExt::NewL( aMmCall );
    }
#else

CMmFaxExtInterface* CMmFaxExtInterface::NewL(
        CMmCallTsy* /*aMmCall*/ ) 
    {
    return NULL; // null marks non existing internal fax support
    }
    
#endif

// ================= OTHER EXPORTED FUNCTIONS ===============================

//  End of File 
