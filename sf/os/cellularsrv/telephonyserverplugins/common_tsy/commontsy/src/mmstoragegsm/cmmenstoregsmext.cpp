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



//  INCLUDE FILES
#include "cmmenstoregsmext.h"
#include "cmmenstoretsy.h"
#include <ctsy/pluginapi/cmmdatapackage.h>

// ======== MEMBER FUNCTIONS ========

CMmENStoreGsmExt::CMmENStoreGsmExt()
    {
    }

CMmENStoreGsmExt::~CMmENStoreGsmExt()
    {
    }

CMmENStoreGsmExt* CMmENStoreGsmExt::NewL( CMmENStoreTsy* aMmENStoreTsy )
    {
    CMmENStoreGsmExt* aENStoreGsmExt = new ( ELeave ) CMmENStoreGsmExt();

    CleanupStack::PushL( aENStoreGsmExt );
    aENStoreGsmExt->iMmENStoreTsy = aMmENStoreTsy;
    aENStoreGsmExt->ConstructL();
    CleanupStack::Pop();

    return aENStoreGsmExt;
    }

void CMmENStoreGsmExt::ConstructL()
    {
    }

// ---------------------------------------------------------------------------
// CMmENStoreGsmExt::Read
// Read reguest of EN store numbers
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmENStoreGsmExt::Read( 
    TInt aIpc,
    TInt aIndex )
    {
    TInt ret( KErrNone );
    
    //Create package
    CMmDataPackage package;

    //Set call id and call mode
    package.PackData( &aIndex );

    //Send request to the Domestic OS layer.
    TRAPD( trapError,
        ret = iMmENStoreTsy->Phone()->MessageManager()->HandleRequestL(
            aIpc, &package );
        );
        
    if ( KErrNone != trapError )
        {
        ret = trapError;
        }
    
    return ret;
    }

// End of file
