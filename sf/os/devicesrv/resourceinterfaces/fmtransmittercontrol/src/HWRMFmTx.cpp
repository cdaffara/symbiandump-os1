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
#include <hwrm/hwrmfmtx.h>
#include "HWRMFmTxImpl.h"
#include "HWRMtrace.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CHWRMFmTx::NewL
// Two-phased constructor for creating a FM Tx client with callbacks.
// -----------------------------------------------------------------------------
//
EXPORT_C CHWRMFmTx* CHWRMFmTx::NewL(MHWRMFmTxObserver* aCallback)
    {
    CHWRMFmTx* impl = NewLC(aCallback);
    
    CleanupStack::Pop( impl );

    return impl;
    }
   
// -----------------------------------------------------------------------------
// CHWRMFmTx::NewLC
// Two-phased constructor for creating a FM Tx client with callbacks.
// Leaves instance to CleanupStack.
// -----------------------------------------------------------------------------
//
EXPORT_C CHWRMFmTx* CHWRMFmTx::NewLC(MHWRMFmTxObserver* aCallback)
    {
    COMPONENT_TRACE2(_L( "HWRM FmTx - CHWRMFmTx::NewLC(0x%x)" ), aCallback );

    CHWRMFmTxImpl* impl = new( ELeave ) CHWRMFmTxImpl();
    
    CleanupStack::PushL( impl );
    impl->ConstructL(aCallback);

    COMPONENT_TRACE2( _L( "HWRM FM Tx Client - CHWRMFmTx::NewLC - return 0x%x" ), impl );

    return impl;
    }


// ========================== OTHER EXPORTED FUNCTIONS =========================

//  End of File  
