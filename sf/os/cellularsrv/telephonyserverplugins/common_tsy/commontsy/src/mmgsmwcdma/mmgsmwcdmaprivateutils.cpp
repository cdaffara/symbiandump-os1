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
#include "mmgsmwcdmaprivateutils.h"
#include <ctsy/serviceapi/mmtsy_defaults.h>
#include "MmTsy_conf.h"
#include <ctsy/mmtsy_names.h>
#include "CMmCommonStaticUtility.h"

// ======== LOCAL FUNCTIONS ========

// ---------------------------------------------------------------------------
// MapLinenameToMmMobileService
// Map multimode api linename to mobile service.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt MapLinenameToMmMobileService(
    const TDesC& aLinename, 
    RMobilePhone::TMobileService* aMobileService )
    {
    if ( 0 == aLinename.CompareF( KMmTsyVoice1LineName )  )
        {
        *aMobileService = RMobilePhone::ETelephony;
        }
    else if ( 0 == aLinename.CompareF( KMmTsyVoice2LineName ) )
        {
        *aMobileService = RMobilePhone::EAuxVoiceService;
        }
    else if ( 0 == aLinename.CompareF( KMmTsyDataLineName ) )
        {
        *aMobileService = RMobilePhone::EAsyncData;
        }
    else if ( 0 == aLinename.CompareF( KMmTsyFaxLineName ) )
        {
        *aMobileService = RMobilePhone::EFaxService;
        }
    else 
        {
        // this can't be KErrArgument because basic
        // service code isn't usually given as argument
        return KErrGeneral;                 
        }

    return KErrNone;
    }


//  End of File
