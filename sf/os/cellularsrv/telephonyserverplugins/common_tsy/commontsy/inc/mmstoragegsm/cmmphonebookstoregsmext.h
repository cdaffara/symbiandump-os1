/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/



#ifndef _CMMPHONEBOOKSTOREGSMEXT_H
#define _CMMPHONEBOOKSTOREGSMEXT_H

//  INCLUDE
#include "cmmphonebookstoreextinterface.h"
#include "cmmgsmphonestorageprivateutility.h"
#include <ctsy/rmmcustomapi.h>

//  FORWARD DECLARATIONS
class CMmPhoneBookStoreTsy;
class CMmPhoneTsy;

// CLASS DECLARATION

/**
 * CMmPhoneBookStoreGsmExt contains mode-dependent
 * phone functionality for GSM/WCDMA phone.
 */
NONSHARABLE_CLASS( CMmPhoneBookStoreGsmExt ): public CMmPhoneBookStoreExtInterface
    {
    public:  // Constructors and destructor

        /**
         * Two-phased constructor.
         * @return created phonebook store object 
         */
        static CMmPhoneBookStoreGsmExt* NewL(
            CMmPhoneBookStoreTsy* aMmPhoneBookStoreTsy );

        /**
         * Destructor.
         */
        ~CMmPhoneBookStoreGsmExt();

        /**
         * Deletes all entries from the specified PBStore
         *          
         *
         * @param aIpc Ipc number
         * @param aPhoneBookName Phonebook name
         * @return Request returnvalue
         */
        virtual TInt DeleteAllL( TInt aIpc, const TDesC& aPhoneBookName );

        /**
         * Reads an entry from specified PBStore
         *          
         *
         * @param aIpc Ipc number
         * @param aPhoneBookName Phonebook name
         * @param aIndexAndEntries Index and entries
         * @return Request returnvalue 
         */
        virtual TInt ReadL( TInt aIpc, const TDesC& aPhoneBookName, 
            RMobilePhoneBookStore::TPBIndexAndNumEntries* aIndexAndEntries);

        /**
         * Reads an entry from PBStore to cache
         *          
         *
         * @param aIpc Ipc number
         * @param aPhoneBookName Phonebook name
         * @return Request returnvalue
         */
        virtual TInt ReadToCacheL( TInt aIpc, const TDesC& aPhoneBookName );

        /**
         * Writes an entry specified by the location index
         *          
         *
         * @param aIpc Ipc number
         * @param aPhoneBookName Phonebook name
         * @param aIndex Location index
         * @param aDataToWrite Pointer to data to be written
         * @return Request returnvalue
         */
        virtual TInt WriteL( TInt aIpc, const TDesC& aPhoneBookName, TInt aIndex,
            CPhoneBookStoreEntry* aDataToWrite );

        /**
         * Deletes an entry by the location index
         *          
         *
         * @param aIpc Ipc number
         * @param aPhoneBookName Phonebook name
         * @param aIndex Location index
         * @return Request returnvalue
         */
        virtual TInt DeleteL( TInt aIpc, const TDesC& aPhoneBookName, TInt aIndex );

        /**
         * Inits phonebook
         *          
         *
         * @param aIpc Ipc number
         * @param aPhoneBookName Phonebook name
         * @return Request returnvalue
         */
        virtual TInt InitPhonebook( TInt aIpc, const TDesC& aPhoneBookName );

        /**
         * Counts the number of specific phonebook used entries
         *          
         *
         * @param aIpc Ipc number
         * @param aPhoneBookName Phonebook name
         * @return Request returnvalue
         */
        virtual TInt CountPhonebookEntriesL( TInt aIpc, 
            TName& aPhoneBookName );
    private:

        /**
         * C++ default constructor.
         */
        CMmPhoneBookStoreGsmExt();

        /**
         * By default Symbian 2nd phase constructor is private.
         */
        void ConstructL();
        
    private:    // Data

        /**
         * Pointer to the Phone TSY object
         */         
        CMmPhoneBookStoreTsy* iMmPhoneBookStoreTsy;
    };

#endif // _CMMPHONEBOOKSTOREGSMEXT_H 

// End of File
