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
#include "cmmonstoregsmext.h"
#include "cmmonstoretsy.h"
#include <ctsy/pluginapi/cmmdatapackage.h>

// ======== MEMBER FUNCTIONS ========

CMmONStoreGsmExt::CMmONStoreGsmExt()
    {
    }

CMmONStoreGsmExt::~CMmONStoreGsmExt()
    {
    }

CMmONStoreGsmExt* CMmONStoreGsmExt::NewL( 
    CMmONStoreTsy* aMmONStoreTsy )
    {

    CMmONStoreGsmExt* aONStoreGsmExt = new ( ELeave ) CMmONStoreGsmExt();

    CleanupStack::PushL(aONStoreGsmExt);
    aONStoreGsmExt->iMmONStoreTsy = aMmONStoreTsy;
    aONStoreGsmExt->ConstructL();
    CleanupStack::Pop();

    return aONStoreGsmExt;
    }

void CMmONStoreGsmExt::ConstructL()
    {
    }

// ---------------------------------------------------------------------------
// CMmONStoreGsmExt::ReadL
// Reads an entry from the ONStore
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmONStoreGsmExt::ReadL( 
    TInt aIpc,
    TInt aIndex )
    {
    //Create package
    CMmDataPackage package;

    //Set call id and call mode
    package.PackData( &aIndex );

    //Send request to the Domestic OS layer.
    return iMmONStoreTsy->Phone()->MessageManager()->HandleRequestL( aIpc, 
        &package );
    }

// ---------------------------------------------------------------------------
// CMmONStoreGsmExt::WriteL
// Writes an entry to the ON store
// This function also used to delete one entry by index
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmONStoreGsmExt::WriteL( 
    TInt aIpc,
    TDesC8* aEntry )
    {
    //Create package
    CMmDataPackage package;

    //Set call id and call mode
    package.PackData( &aEntry );

    //Send request to the Domestic OS layer.
    return iMmONStoreTsy->Phone()->MessageManager()->HandleRequestL( aIpc,
        &package );
    }
    
// ---------------------------------------------------------------------------
// CMmONStoreGsmExt::WriteAllL
// Writes an entry to the ON store.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmONStoreGsmExt::WriteAllL(
	TInt aIpc,
	const CMmDataPackage* aEntry )
    {
    //Send request to the Domestic OS layer.
    return iMmONStoreTsy->Phone()->MessageManager()->HandleRequestL( aIpc, 
        aEntry );
    }

// ---------------------------------------------------------------------------
// CMmONStoreGsmExt::GetInfoL
// Returns information about ON store
// This method is used by many requests, because supplied servicetype
// can be used to get e.g. num of entries, length of number/name.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmONStoreGsmExt::GetInfoL( 
    TInt aIpc,
    TUint8 aServiceType ) 
    {
    //Create package
    CMmDataPackage package;

    //Set call id and call mode
    package.PackData( &aServiceType );

    //Send request to the Domestic OS layer.
    return iMmONStoreTsy->Phone()->MessageManager()->HandleRequestL( aIpc,
        &package );
    }

// ---------------------------------------------------------------------------
// CMmONStoreGsmExt::DeleteAllL
// Deletes all phonebook's entries.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmONStoreGsmExt::DeleteAllL(
    TInt aIpc )
    {
    //Create package
    CMmDataPackage package; // dummy package

    //Send request to the Domestic OS layer.
    return iMmONStoreTsy->Phone()->MessageManager()->HandleRequestL( aIpc, 
        &package );
    }

// ---------------------------------------------------------------------------
// CMmONStoreGsmExt::DeleteL
// Deletes an entry from the phonebook
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmONStoreGsmExt::DeleteL( 
    TInt aIpc,
    TInt aIndex )
    {
    //Create package
    CMmDataPackage package;

    //Set call id and call mode
    package.PackData( &aIndex );

    //Send request to the Domestic OS layer.
    return iMmONStoreTsy->Phone()->MessageManager()->HandleRequestL( aIpc, 
        &package );
    }

// ---------------------------------------------------------------------------
// CMmONStoreGsmExt::ReceivedNumOfEntries
// Inits the number of entries
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
void CMmONStoreGsmExt::ReceivedNumOfEntries(
    TInt aNumOfEntries )
    {
    // Set the number of entries for DeleteAll
    iNumberOfEntries = aNumOfEntries;
    }

// ---------------------------------------------------------------------------
// CMmONStoreGsmExt::GetDeleteAllNumOfEntries
// Gets the number of entries
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmONStoreGsmExt::GetDeleteAllNumOfEntries()
    {
    // Return the num of entries for DeleteAll
    return iNumberOfEntries;
    }

// ---------------------------------------------------------------------------
// CMmONStoreGsmExt::GetAndUpdateDeleteAllIndex
// This method returns and increases the index to be deleted
// for DeleteAll -request.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmONStoreGsmExt::GetAndUpdateDeleteAllIndex()
    {
    // Return and increase by one
    return iDeleteAllIndex++;
    }

// ---------------------------------------------------------------------------
// CMmONStoreGsmExt::IncreaseAndGetNumOfUsedEntries
// Increases and return used entry variable
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmONStoreGsmExt::IncreaseAndGetNumOfUsedEntries()
    {
    return ++iNumOfUsedEntries;
    }

// ---------------------------------------------------------------------------
// CMmONStoreGsmExt::NumOfUsedEntries
// Return used entry member variable
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmONStoreGsmExt::NumOfUsedEntries()
    {
    return iNumOfUsedEntries;
    }

// End of file
