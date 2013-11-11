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
#include "cmmphonebookstoregsmext.h"
#include <ctsy/pluginapi/cmmdatapackage.h>
#include "cmmphonetsy.h"
#include "cmmphonebookstoretsy.h"

// ======== MEMBER FUNCTIONS ========

CMmPhoneBookStoreGsmExt::CMmPhoneBookStoreGsmExt()
    {
    }

void CMmPhoneBookStoreGsmExt::ConstructL()
    {
    }

CMmPhoneBookStoreGsmExt* CMmPhoneBookStoreGsmExt::NewL( CMmPhoneBookStoreTsy* aMmPhoneBookStoreTsy )
    {
    CMmPhoneBookStoreGsmExt* aPhoneBookStoreGsmExt = 
                new ( ELeave ) CMmPhoneBookStoreGsmExt();

    CleanupStack::PushL( aPhoneBookStoreGsmExt );
    aPhoneBookStoreGsmExt->iMmPhoneBookStoreTsy = aMmPhoneBookStoreTsy;
    aPhoneBookStoreGsmExt->ConstructL();
    CleanupStack::Pop();

    return aPhoneBookStoreGsmExt;
    }

CMmPhoneBookStoreGsmExt::~CMmPhoneBookStoreGsmExt()
    {
    }

// ---------------------------------------------------------------------------
// CMmPhoneBookStoreGsmExt::DeleteAllL
// Deletes all phonebook's entries.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneBookStoreGsmExt::DeleteAllL(
    TInt aIpc,
    const TDesC& aPhoneBookName )
    {
    //Create package
    CPhoneBookDataPackage package;
    // Set package data
    package.SetPhoneBookName( aPhoneBookName );

    //Send request to the Domestic OS layer.
    return iMmPhoneBookStoreTsy->Phone()->MessageManager()->
        HandleRequestL( aIpc, &package );
    }

// ---------------------------------------------------------------------------
// CMmPhoneBookStoreGsmExt::ReadL
// This method Reads an entry from (U)SIM with phonebooktype,
// location index and number of entries to be read.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneBookStoreGsmExt::ReadL(
    TInt aIpc,
    const TDesC& aPhoneBookName,
    RMobilePhoneBookStore::TPBIndexAndNumEntries* aIndexAndEntries)
    {
    //Create package
    CPhoneBookDataPackage package;
    // Set package data
    package.SetPhoneBookName( aPhoneBookName );
    package.PackData( &aIndexAndEntries );
    
    //Send request to the Domestic OS layer.
    return iMmPhoneBookStoreTsy->Phone()->MessageManager()->
        HandleRequestL( aIpc, &package );
    }

// ---------------------------------------------------------------------------
// CMmPhoneBookStoreGsmExt::ReadToCacheL
// Reads entries from the (U)SIM phonebook to cache array.
// Cache is owned by PhoneBookStoreTsy.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneBookStoreGsmExt::ReadToCacheL( 
    TInt aIpc,
    const TDesC& aPhoneBookName)
    {
    //Create package
    CPhoneBookDataPackage package;
    // Set package data
    package.SetPhoneBookName( aPhoneBookName );

    //Send request to the Domestic OS layer.
    return iMmPhoneBookStoreTsy->Phone()->MessageManager()->
                                HandleRequestL( aIpc, &package );
    }

// ---------------------------------------------------------------------------
// CMmPhoneBookStoreGsmExt::WriteL
// This method writes an entry to (U)SIM with specific phonebooktype,
// location index and an entry to be written.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneBookStoreGsmExt::WriteL( 
    TInt aIpc,
    const TDesC& aPhoneBookName,
    TInt aIndex,
    CPhoneBookStoreEntry* aDataToWrite )
    {
    //Create package
    CPhoneBookDataPackage package;
    // Set package data
    package.SetPhoneBookName( aPhoneBookName );
    package.PackData( &aIndex, &aDataToWrite );

    //Send request to the Domestic OS layer.
    return iMmPhoneBookStoreTsy->Phone()->MessageManager()->
        HandleRequestL( aIpc, &package );
    }

// ---------------------------------------------------------------------------
// CMmPhoneBookStoreGsmExt::DeleteL
// This method deletes an entry from (U)SIM with phonebooktype and
// the location index to be deleted.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneBookStoreGsmExt::DeleteL( 
    TInt aIpc,
    const TDesC& aPhoneBookName,
    TInt aIndex )
    {
    //Create package
    CPhoneBookDataPackage package;
    // Set package data
    package.SetPhoneBookName( aPhoneBookName );
    package.PackData( &aIndex );

    //Send request to the Domestic OS layer.
    return iMmPhoneBookStoreTsy->Phone()->MessageManager()->
        HandleRequestL( aIpc, &package );
    }

// ---------------------------------------------------------------------------
// CMmPhoneBookStoreGsmExt::InitPhonebook
// This method initializes phonebook. As a return value we get information
// about (U)SIM phonebook.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneBookStoreGsmExt::InitPhonebook(
    TInt aIpc,
    const TDesC& aPhoneBookName )
    {
    TInt ret( KErrNotFound );

    //Create package
    CPhoneBookDataPackage package;
    // Set package data
    package.SetPhoneBookName( aPhoneBookName );

    //Send request to the Domestic OS layer.
    TRAPD( trapError,
        ret = iMmPhoneBookStoreTsy->Phone()->MessageManager()->HandleRequestL(
            aIpc, &package );
        );
        
    if ( KErrNone != trapError )
        {
        ret = trapError;
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// CMmPhoneBookStoreGsmExt::CountPhonebookEntriesL
// This method counts phonebook entries and it is part of
// GetInfo -functionality.
// (other items were commented in a header).
// ---------------------------------------------------------------------------
//
TInt CMmPhoneBookStoreGsmExt::CountPhonebookEntriesL( 
    TInt aIpc,
    TName& aPhoneBookName )
    {
    //Create package
    CPhoneBookDataPackage package;
    // Set package data
    package.SetPhoneBookName( aPhoneBookName );

    //Send request to the Domestic OS layer.
    return iMmPhoneBookStoreTsy->Phone()->MessageManager()->
        HandleRequestL( aIpc, &package );
    }

//  End of File
