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



#ifndef _CMMONSTOREGSMEXT_H_
#define _CMMONSTOREGSMEXT_H_

//  INCLUDES
#include <et_phone.h>
#include "cmmonstoreextinterface.h"

// FORWARD DECLARATIONS
class CMmONStoreTsy;

// CLASS DECLARATION
/**
 * CMmONStoreGsmExt contains mode-dependent phone functionality for GSM phone.
 * Extension request handles are stored in MM Phone object.
 */
NONSHARABLE_CLASS( CMmONStoreGsmExt ) : public CMmONStoreExtInterface
    {
    public:  // Constructors and destructor

        /**
         * C++ default constructor.
         */
        CMmONStoreGsmExt();

        /**
         * Destructor
         */
        virtual ~CMmONStoreGsmExt();

        /**
         * Creates a new GSM ON store specific object instance
         *          
         *
         * @param aMmONStoreTsy pointer to EON store TSY
         * @return pointer to ON store GSM extension
         */
        static CMmONStoreGsmExt* NewL( CMmONStoreTsy* aMmONStoreTsy );

    public: // Functions from base classes

        /**
         * ReadL an entry
         *          
         *
         * @param aIpc IPC
         * @param aIndex index to be red
         * @return success/failure
         */
        virtual TInt ReadL( TInt aIpc, TInt aIndex );

        /**
         * WriteL an entry, used also in delete
         *          
         *
         * @param aIpc IPC
         * @param aEntry entry to be wrote
         * @return success/failure
         */
        virtual TInt WriteL( TInt aIpc, TDesC8* aEntry );

        /**
         * Writes an entry in write all sequence
         *          
         *
         * @param aIpc IPC
         * @param aEntry Data package containing an ON Store entry
         * @return success/failure
         */
        TInt WriteAllL( TInt aIpc, const CMmDataPackage* aEntry );
        
        /**
         * Get info
         *          
         *
         * @param aIpc IPC
         * @param aServiceType service type
         * @return success/failure
         */
        virtual TInt GetInfoL( TInt aIpc, TUint8 aServiceType );

        /**
         * Deletes all phonebook's entries. Deleting all entries is
         * multi-phase request, and this phase handles getting the numbers of
         * specified phonebookstore. If the service type is VMBX, SIM Server
         * does not support getting the numbers of entries, and in this case
         * function forwards request to Delete -method.
         *          
         *
         * @param aIpc IPC
         * @return KErrNone or error
         */
        virtual TInt DeleteAllL( TInt aIpc );

        /**
         * Deletes an entry by the location index
         * This method deletes an entry from SIM related to specific
         * phonebooktype and the location index. Request is forwarded
         * to PBStoreMessageHandlerGsmExt.
         *          
         *
         * @param aIpc IPC
         * @param aIndex  Location index
         * @return KErrNone or error
         */
        virtual TInt DeleteL( TInt aIpc, TInt aIndex );

        /**
         * Saves received num of entries from SIM Server for DeleteAll
         * When SIM Server gives the number of entries for DeleteAll -request,
         * this function sets this value to class attribute.
         *          
         *
         * @param TInt aNumOfEntries Number of entries
         */
        void ReceivedNumOfEntries( TInt aNumOfEntries );

        /**
         * Get the number of entries from specified PBStore
         * This function returns the number of entries. This function is used
         * by DeleteAll -request and called from PBStoreMessHandlerGsmExt
         *          
         *
         * @param  None
         * @return Number of entries
         */
        TInt GetDeleteAllNumOfEntries();

        /**
         * Gets and updates the num of entries for DeleteAll
         *          
         *
         * @param None
         * @return Index to delete
         */
        TInt GetAndUpdateDeleteAllIndex();

        /**
         * Gets number of entries for Write -request
         *          
         *
         * @param None
         * @return Num of entries
         */
        TInt GetWriteNumOfEntries();

        /**
         * Gets the num of used entrires for GetInfo
         * The purpose of this method is count the used entries store.
         *          
         *
         * @param None
         * @return Num of used entries
         */
        virtual TInt NumOfUsedEntries();

        /**
         * Gets and increases the num of used entries for GetInfo
         * The purpose of this method is count the used entries in ON store.
         * Num of used entries is returned and the value is increased.
         *          
         *
         * @param None
         * @return Number of used entries
         */
        TInt IncreaseAndGetNumOfUsedEntries();

    private:

        /**
         * By default Symbian 2nd phase constructor is private.
         */
        void ConstructL();

    private:     // Data

        /**
         * Pointer to the Phone TSY object
         */         
        CMmONStoreTsy* iMmONStoreTsy;

        /**
         * Attribute to handling delete all
         */         
        TInt iDeleteAllIndex;

        /**
         * Attribute to keep number of entries
         */         
        TInt iNumberOfEntries;

        /**
         * Attribute to keep the num of used entries
         */         
        TInt iNumOfUsedEntries;
    };

#endif /* _CMMONSTOREGSMEXT_H_ */

// End of File
