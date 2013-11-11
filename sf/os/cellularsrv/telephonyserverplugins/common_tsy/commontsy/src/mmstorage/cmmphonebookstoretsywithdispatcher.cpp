// Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifdef USING_CTSY_DISPATCHER

/**
 * This file contains additional function definitions for the CMmPhoneBookStoreTsy class
 * in CTSY for use when the CTSY is used with the CTSY Dispatcher. 
 */

// INCLUDE FILES

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cmmphonebookstoretsywithdispatcherTraces.h"
#endif

#include "cmmphonebookstoretsy.h"
#include "cmmphonebookstoreextinterface.h"
#include <ctsy/serviceapi/mmtsy_ipcdefs.h>
#include "cmmphonetsy.h"

// ---------------------------------------------------------------------------
// CMmPhoneBookStoreTsy::CacheEntriesL
// This method activates phonebook entry caching.
// if phonebook initialization is not done, it is done first.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//

//need to replace the whole function as the change is in a TRAPD which itself is a MACRO thus cannot ifdef


void CMmPhoneBookStoreTsy::CacheEntriesL()
    {
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_CACHEENTRIESL1_1, "TSY: CMmPhoneBookStoreTsy::CacheEntriesL - entered");
    // Get Phonebook related DLL -static data

    // Check if phonebook has been initialized successfully. Caching of ADN
    // entries does not work if phonebook initialize have been failed.
    if ( iStoreInfoData->iIsPhonebookInitialized &&
         ( EFalse == iStoreInfoData->iIsPhonebookInitializeFailed ||
           KADNPhoneBook != iPhoneBookType ) )
        {
        OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_CACHEENTRIESL1_2, "TSY: CMmPhoneBookStoreTsy::CacheEntriesL - Cache PB %S ", iPhoneBookName);
        TInt ret = KErrNone;
		CMmPhoneTsy::TNosBootState* bootState = iMmPhoneTsy->NosBootState();
		
		if ( !iCacheReady )
			{
			// Check if SIM is ready, and SMS has been cached already
			if ( ( KADNPhoneBook == iPhoneBookType ) ||
				 ( KFDNPhoneBook == iPhoneBookType ) &&
				 ( EFalse != bootState->iSIMReady ) )
				{
				// Cache must be reseted after the boot
				if ( NULL != iPBStoreCache )
					{
					iPBStoreCache->ResetAndDestroy();
					delete iPBStoreCache;
					iPBStoreCache = NULL;
OstTraceDef0(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_CACHEENTRIESL1_3, "TSY: CMmPhoneBookStoreTsy::CacheEntriesL - Old cache deleted");
					}
				
				//Inform phonebook that the cache is not ready for refresh
				ResetCacheReady();
	
				bootState->iCachingActive = ETrue;
				TRAP( ret, ReadToCacheL();)
				}
			}
            else if ( iCacheReady )
                {
OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_CACHEENTRIESL1_4, "TSY: CMmPhoneBookStoreTsy::CacheEntriesL - PB %S cache entries ready", iPhoneBookName);
                }
        if ( KErrNone != ret)
            {
OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_CACHEENTRIESL1_5, "TSY: CMmPhoneBookStoreTsy::CacheEntriesL - PB %S caching failed!", iPhoneBookName);
            }
        }
    // Phonebook not initialized -> let's do it before caching
    else if ( ( !iStoreInfoData->iIsPhonebookInitialized
        || iStoreInfoData->iIsPhonebookInitializeFailed )
        && !iMmPhoneTsy->IsPBInitActive() )
        {
OstTraceDefExt1(OST_TRACE_CATEGORY_DEBUG, TRACE_INTERNALS, CMMPHONEBOOKSTORETSY_CACHEENTRIESL1_6, "TSY: CMmPhoneBookStoreTsy::CacheEntriesL - Starting initialization PB: %S", iPhoneBookName);
        // before phonebook requests, phonebook must be initialized
        iMmPhoneBookStoreExtInterface->InitPhonebook(
            EMmTsyPhoneBookStoreInitIPC, iPhoneBookName );
        iMmPhoneTsy->SetPBInitActiveStatus( ETrue );
        }
    }

#endif //USING_CTSY_DISPATCHER
