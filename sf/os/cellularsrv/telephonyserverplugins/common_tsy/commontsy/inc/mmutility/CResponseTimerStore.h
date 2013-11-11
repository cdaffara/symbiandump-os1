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



#ifndef CRESPONSETIMERSTORE_H
#define CRESPONSETIMERSTORE_H

// INCLUDES
#include "MmTsy_timeoutdefs.h"

#ifdef REQHANDLE_TIMER

#include <et_phone.h>

// FORWARD DECLARATIONS
class CMmTsyReqHandleStore;
class CMmPhoneTsy;

// CLASS DECLARATION

/**
 *  CResponseTimerStore class.
 */
NONSHARABLE_CLASS( CResponseTimerStore ) : public CBase
    {
    public: // 

    class CTableEntry
        {
        public: // New functions
        
            /**
             * Returns order between two entries
             * 
             *                           
             * @param const CTableEntry& arg1
             * @param const CTableEntry& arg2
             * @return Tint 0 if arg1 = arg2
                           1 if arg1 > arg2
                          -1 else
             */
            static TInt OrderEntries ( const CTableEntry& arg1,
                const CTableEntry& arg2 );
            /**
             * Returns if two entries are equal
             * 
             *                           
             * @param const CTableEntry& arg1
             * @param const CTableEntry& arg2
             * @return TBool ETrue if two entries are equal
             */   
            static TBool CompareEntries( const CTableEntry& arg1,
                const CTableEntry& arg2 );
            /**
             * Non default Constructor
             * 
             *                           
             * @param TInt handleType
             * @param TUint32 timeStamp
             * @param CMmTsyReqHandleStore *handleStore
             * @param CTelObject *userObject
             */
            CTableEntry( TInt aHandleType,
				TUint32 aTimeStamp,
				CMmTsyReqHandleStore *aHandleStore,
				CTelObject *aUserObject,
				TInt aIpc = 0);
    
            /**
             * returns iTimeStamp from a CTableEntry
             * 
             *                           
             * @return TUint32 value of the iTimeStamp attribute
             */
            TUint32 GetTimeStamp();

            /**
             * returns iHandleStore from a CTableEntry.
             * 
             *                           
             * @return CMmTsyReqHandleStore* value of the iHandleStore
             * attribute
             */  
            CMmTsyReqHandleStore* GetHandleStore();
    
            /**
             * returns iUserObject from a CTableEntry.
             *  
             *                           
             * @return CTelObject* value of the iUserObject attribute
             */
            CTelObject* GetUserObject();
    
            /**
             * returns iHandleType from a CTableEntry.
             * 
             *                           
             * @return TInt value of the iHandleType attribute
             */
            TInt GetHandleType();

            /**
             * returns iIPC from a CTableEntry.
             * 
             *                           
             * @return TInt value of the iIPC attribute
             */
            TInt GetIPC();


        private:

            /**
             * Value of the Handle type
             */             
            TInt iHandleType;
    
            /**
             * Time stamp (current beat amount + timeout beats)
             */             
            TUint32 iTimeStamp;
    
            /**
             * Pointer to handle store
             * Own.
             */                          
            CMmTsyReqHandleStore* iHandleStore;
    
            /**
             * Pointer to user TSY object
             * Not own.
             */                          
            CTelObject* iUserObject;
    
            /**
             * Value of the IPC request
             */             
            TInt iIPC;
            
        };

    public: // Constructors and destructor
    
        /**
         * Two-phased constructor.
         * @param *aPhone Pointer to phone object
         * @return CResponseTimerStore* created timer store object
         */
        static CResponseTimerStore* NewL( CMmPhoneTsy *aPhone );
    
        /**
         * Destructor
         */
        virtual ~CResponseTimerStore();
    
        // New functions
        
        /**
         * Adds an entry in the store
         * 
         *                   
         * @param aHandleIndex
         * @param *aHandleStore
         * @param *aUserObject
         * @param TUint32 aWaitingTime    
         */
        void AddTimeEntry( TInt aHandleIndex,
			CMmTsyReqHandleStore *aHandleStore,
			CTelObject *aUserObject,
			TUint32 aWaitingTime,
			TInt aIpc = 0 );
        /**
         * Deletes an entry by Handle
         * 
         *                   
         * @param aHandleIndex
         * @param *aHandleStore
         * @return TInt Success of the operation
         */
        TInt DeleteEntryByHandle( TInt aHandleIndex,
            CMmTsyReqHandleStore *aHandleStore );
    
        /**
         * Deletes an entry by index
         * 
         *                   
         * @param TInt aHandleIndex
         * @return Success of the operation
         */
        TInt DeleteEntryByIndex( TInt aIndex );

         /**
         * Gets an entry by index
         * 
         *                   
         * @param aHandleIndex
         * @param *aHandleStore
         * @param *aUserObject
         * @param TUint32 aWaitingTime    
         */
        void  ResetEntryTimeout( TInt aHandleIndex,
	        CMmTsyReqHandleStore *aHandleStore,
	        CTelObject *aUserObject,
	        TUint32 aWaitingTime);
    
         /**
         * Resets the timout value for an entry
         * 
         *                   
         * @param aHandleIndex
         * @return CTableEntry* pointer to the entry in the given index
         */
        CResponseTimerStore::CTableEntry* GetEntry( TInt aIndex );
    
    
        /**
         * Gives the amount of entries in the table
         * 
         *                   
         * @return Amount of entries in the table
         */
        TInt NumberOfEntries();

       /**
         * Checks if a handleIndex has a timer
         * 
         *                   
         * @param TInt aHandleIndex:
         * @param CMmTsyReqHandleStore *aHandleStore
         * @return TBool ETrue if the handle has a timer asigned EFalse else.
         */
        TBool CheckEntry( TInt aHandleIndex,
            CMmTsyReqHandleStore *aHandleStore );
    
       /**
         * Deletes all the entries related to a TSY object
         * 
         *                   
         * @param *aHandleStore
         * @return TInt Success of the operation
         */
        TInt DeleteByHandleStoreAllEntries(
            CMmTsyReqHandleStore *aHandleStore );

    private: // Constructors and destructor
    
        /**
         * Default constructor
         */
        CResponseTimerStore();
        
        /**
         * By default Symbian 2nd phase constructor is private.
         */
        void ConstructL();

    private: // Data
    
        /**
         * Pointer to the entry array
         */         
        RArray<CTableEntry> *iEntryTable;
    
        /** 
         * Pointer to phone
         */         
        CMmPhoneTsy *iPhone;
};

#endif //REQHANDLE_TIMER
#endif //CRESPONSETIMERESTORE

//  End of File
