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



#ifndef _CMMPHONEBOOKSTOREEXTINTERFACE_H
#define _CMMPHONEBOOKSTOREEXTINTERFACE_H

//  INCLUDES
#include <e32def.h>
#include <et_tsy.h>
#include <etelmm.h>

//  FORWARD DECLARATIONS
class TDataPackage;
class TPhonebookEntry;
class CMmPhoneBookStoreMessHandler;
class CPhoneBookStoreEntry;

// CLASS DECLARATION

/**
*  CMmPhoneBookStoreExtInterface is used to provide an extension interface for
*  system specific phonebook store object extensions.
*/
NONSHARABLE_CLASS( CMmPhoneBookStoreExtInterface ) : public CBase
    {
    public:

        /**
        * Destructor.
        */
        virtual ~CMmPhoneBookStoreExtInterface();

        /**
        * Deletes all entries from the specified PBStore
        *         
        *
        * @param aIpc  Ipc number
        * @param aPhoneBookName Phonebook name
        * @return Request returnvalue
        */
        virtual TInt DeleteAllL(
                    TInt /*aIpc*/, const TDesC& /*aPhoneBookName*/ )
        { return KErrNotSupported; }

        /**
        * Reads an entry from specified PBStore
        *         
        *
        * @param aIpc Ipc number
        * @param aPhoneBookName Phonebook name
        * @param aIndexAndEntries Index and entries
        * @return TInt  Request returnvalue 
        */
        virtual TInt ReadL(
                    TInt /*aIpc*/,
                    const TDesC& /*aPhoneBookName*/, 
                    RMobilePhoneBookStore::TPBIndexAndNumEntries* /*aIndexAndEntries*/)
        { return KErrNotSupported; }

        /**
        * Reads an entry from PBStore to cache
        *         
        *
        * @param aIpc Ipc number
        * @param aPhoneBookName Phonebook name
        * @return TInt : Request returnvalue
        */
        virtual TInt ReadToCacheL( 
                    TInt /*aIpc*/, 
                    const TDesC& /*aPhoneBookName*/)
        { return KErrNotSupported; }

        /**
        * Writes an entry specified by the location index
        *         
        *
        * @param aIpc  Ipc number
        * @param aPhoneBookName  Phonebook name
        * @param aIndex  Location index
        * @param aDataToWrite Pointer to data to be written
        * @return TInt  Request returnvalue
        */
        virtual TInt WriteL(
            TInt /*aIpc*/,
            const TDesC& /*aPhoneBookName*/,
            TInt /*aIndex*/,
            CPhoneBookStoreEntry* /*aDataToWrite*/ )
        { return KErrNotSupported; }

        /**
        * Deletes an entry by the location index
        *         
        *
        * @param aIpc  Ipc number
        * @param aPhoneBookName Phonebook name
        * @param aIndex Location index
        * @return TInt Request returnvalue
        */
        virtual TInt DeleteL(
                    TInt /*aIpc*/,
                    const TDesC& /*aPhoneBookName*/, TInt /*aIndex*/ )
        { return KErrNotSupported; }

        /**
        * Inits phonebook
        *         
        *
        * @param aIpc Ipc number
        * @param aPhoneBookName Phonebook name
        * @return TInt Request returnvalue
        */
        virtual TInt InitPhonebook( TInt /*aIpc*/, const TDesC& /*aPhoneBookName*/ )
        { return KErrNotSupported; }

        /**
        * Counts phonebook entries by phonebook type
        *         
        *
        * @param aIpc Ipc number
        * @param aPhoneBookName Phonebook name
        * @return TInt Request returnvalue
        */
        virtual TInt CountPhonebookEntriesL( TInt /*aIpc*/, 
                                                  TName& /*aPhoneBookName*/ )
        { return KErrNotSupported; }

    protected:
    
         /**
        * C++ default constructor.
        */
        CMmPhoneBookStoreExtInterface();


    private:
    
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL(){}

};

#endif // _CMMPHONEBOOKSTOREEXTINTERFACE_H

// End of File
