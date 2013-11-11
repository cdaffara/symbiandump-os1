/*
* Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef PHONEBOOKSTORE_H
#define PHONEBOOKSTORE_H

//  INCLUDES
#include <et_phone.h>
#include <mpbutil.h>
#include <ctsy/rmmcustomapi.h>
#include "cmmgsmphonestorageprivateutility.h"
#include "MmTsy_timeoutdefs.h"

//  FORWARD DECLARATIONS
class CMmTsyReqHandleStore;
class CMmPhoneBookStoreExtInterface;
class CPhoneBookDataPackage;
class CMmDataPackage;
class CMmPhoneTsy;

/**
 * CMmPhoneTsy contains phonebook storage functionality.
 * Extension request handles and parameters are stored as attributes.
 */
NONSHARABLE_CLASS( CMmPhoneBookStoreTsy ) : public CSubSessionExtBase
    {
    private:

        /**
         * Request types used for Phonebook store requests.
         */
        enum TPBRequestTypes
            {
            EMultimodePhoneBookStoreReqHandleUnknown, // 0
            EMultimodePhoneStoreRead,
            EMultimodePhoneStoreWrite,
            EMultimodePhoneBookStoreRead,
            EMultimodePhoneBookStoreWrite,
            EMultimodePhoneStoreDelete,
            EMultimodePhoneStoreDeleteAll,
            EMultimodePhoneStoreGetInfo,
            EMultimodePhoneStoreNotifyStoreEvent, // 8

            // This value must be 1 + exactly the same as the maximum enum
            // value of CMmPhoneBookStoreTsy::TPBRequestTypes.
            EMmTsyNumOfStorageRequests
            };

        /**
         * Array handling enums
         */
        enum TPBArrayTypeToRead
            {
            EPBStoreCacheArray,
            EPBStoreReadEntriesArray
            };

		struct TReadRequestStore
			   {
			   /** ETel request handle */
			   TTsyReqHandle iReqHandle;
			   /** index entries */
			   RMobilePhoneBookStore::TPBIndexAndNumEntries* iIndexAndEntries;
			   /** pointer to client-side	data */
    		   TDes8* iPBData;
    		   /** Boolean indicating that new cache is processing*/
    		   TBool iCaching;
			   };

    public:  // Constructors and destructor

        /**
         * Two-phased constructor.
         *
         *
         * @param aMmPhone pointer to the Phone object
         * @param aName Name of the phonebook
         * @return Created phonebook store object
         */
        static CMmPhoneBookStoreTsy* NewL( CMmPhoneTsy* aMmPhoneTsy,
            const TDesC& aName );

        /**
         * Destructor.
         */
        virtual ~CMmPhoneBookStoreTsy();

        // New functions

        /**
         * Methods that are inherited from ETel Server base classes
         * Initialisation method
         */
        virtual void Init();

        /**
         * Creates new PBStore object by name and returns a pointer to it.
         * New instances are created by given name, hence this method is
         * NOT SUPPORTED.
         *
         *
         * @param aName A name of the object to be opened.
         * @return Pointer to created instance by given name.
         */
        virtual CTelObject* OpenNewObjectByNameL( const TDesC& aName );

        /**
         * Opens extendable sub-sessions. This method is not supported.
         *
         *
         * @param aName A name of the object to be opened.
         * @return Pointer to created instance by given name.
         */
        virtual CTelObject* OpenNewObjectL( TDes& aName );

        /**
         * Handles extended client requests
         *
         *
         * @param aTsyReqHandle The request handle
         * @param aIpc Request IPC number
         * @param aPackage Packaged data from ETel server
         * @return KErrNone or error
         */
        virtual TInt ExtFunc(
            const TTsyReqHandle aTsyReqHandle,
            const TInt aIpc,
            const TDataPackage& aPackage );

        /**
         * To ask what type of flow control is used for the IPC number.
         *
         *
         * @param aIp Request IPC number
         * @return Request mode for given IPC
         */
        virtual CTelObject::TReqMode ReqModeL( const TInt aIpc );

        /**
         * To ask the TSY how big buffer it wants.
         *
         *
         * @param aIpc Request IPC number
         * @return Number of slots
         */
        virtual TInt NumberOfSlotsL( const TInt aIpc );

        /**
         * To cancel any extension requests.
         *
         *
         * @param aIpc Request IPC number
         * @param aTsyReqHandle Request handle
         * @return KErrNone or error value
         */
        virtual TInt CancelService(
            const TInt aIpc,
            const TTsyReqHandle aTsyReqHandle );

        /**
         * Enables the TSY to "turn on" any regular notification.
         *
         *
         * @param aIpc Request IPC number
         * @return KErrNone or KErrNotSupported
         */
        virtual TInt RegisterNotification( const TInt aIpc );

        /**
         * Deregister given notification
         *
         *
         * @param aIpc Request IPC number
         * @return KErrNone or KErrNotSupported
         */
        virtual TInt DeregisterNotification( const TInt aIpc );

        /**
         * Sets the active phonebook extension
         *
         *
         * @param aMmPBExt pointer to the new PB extension
         * @return KErrNone if extension set, else KErrGeneral
         */
        virtual TInt SetActivePBExtension(
            CMmPhoneBookStoreExtInterface* aMmPBExt );

        /**
         * Gets the active phonebook extension
         *
         *
         * @return Pointer to the active PB extension
         */
        virtual CMmPhoneBookStoreExtInterface* ActivePBExtension();

        /**
         * Completes read -request from SIM and fills descriptor
         * with requested entry.
         *
         *
         * @param aResult Read result
         */
        virtual void CompleteReadL( TInt aResult, CMmDataPackage* aDataPackage);

        /**
         * Completes write -request
         *
         *
         * @param aResult Result value
         * @param aDataPackage Data package
         */
        virtual void CompleteWriteL(
            TInt aResult,
            CMmDataPackage* aDataPackage );

        /**
         * Completes delete -request
         *
         *
         * @param TInt aResult Result value
         */
        virtual void CompleteDeleteL( TInt aError,
            CMmDataPackage* aDataPackage );

        /**
         * Completes GetInfo request
         *
         *
         * @param aResult Result value
         */
        virtual void CompleteGetInfo( TInt aResult );

        /**
         * Completes DeleteAll request
         *
         *
         * @param aResult Result value
         */
        virtual void CompleteDeleteAll( TInt aResult,
            CMmDataPackage* aDataPackage );

        /**
         * Completes Phonebook initialization
         *
         *
         * @param aResult Result value
         * @param aDataPackage
         */
        virtual void CompletePBStoreInitializationL( TInt aResult,
            CMmDataPackage* aDataPackage );

        /**
         * This method completes caching from SIM to TSY
         *
         *
         * @param aResult Result
         */
        virtual void CompleteCachingL( TInt aResult,
                CMmDataPackage* aDataPackage );

        /**
         * Sets PhoneBookType by name parameter
         *
         *
         * @param aName A name of the phonebook to be opened
         */
        void SetPhoneBookType( const TDesC& aName );

        /**
         * This method resets cache
         *
         *
         */
        virtual void ResetCache();

        /**
         * This method resets iCacheReady flag
         *
         *
         */
        virtual void ResetCacheReady();

        /**
         * This method resets iIsPhonebookInitialized flag
         *
         *
         */
        virtual void ResetInitialisationStatus();

        /**
         * This method returns pointer to CMmPhoneTsy -class
         *
         *
         * @return Pointer to CMmPhoneTsy -class
         */
        virtual CMmPhoneTsy* Phone();

#ifdef REQHANDLE_TIMER
        /**
         * Calls the needed complete method due timer expiration
         *
         *
         * @param aReqHandleType req handle type.
         * @param aError error value
         */
        virtual void Complete( TInt aReqHandleType, TInt aError );
#endif
		/**
         * This method continues Read
         *
         *
         */
		void ContinueReadL();
        
        /**
         * This method returns is the initialization done
         *
         * @param none
         * @return Phonebook init status
         */
        TBool IsPBInitDone();

        /**
         * This method returns phonebooktype
         *
         * @since S60 v3.2
         * @param none
         * @return Phonebook type
         */
         TUint8 GetPhonebookType();
         
         /**
          * This method will be activate by the phone to notify the SIM is ready
          */
         void SimIsReady();
         
    private:

        /**
         * C++ default constructor.
         */
        CMmPhoneBookStoreTsy();

        /**
         * By default Symbian 2nd phase constructor is private.
         */
        void ConstructL( const TDesC& aName );

        /**
         * Handles extended client requests. Uses TRAP to ensure functioning
         * on memory allocation failure.
         *
         *
         * @param aTsyReqHandle Request handle
         * @param aIpc Interprocess communication command identifier (IPC)
         * @param aPackage reference to a data package
         * @return KErrNone if request accepted, else several possibilities
         * depending on IPC
         */
        TInt DoExtFuncL( const TTsyReqHandle aTsyReqHandle,
            const TInt aIpc, const TDataPackage& aPackage );

        /**
         * Gets store information
         *
         *
         * @param aTsyReqHandle Request handle
         * @param aInfo package containing TMobilePhoneBookInfoV1 object
         * @return KErrNone or system wide error code
         */
        TInt GetInfoL( const TTsyReqHandle aTsyReqHandle, TDes8* aInfo );

        /**
         * Cancels get store information
         *
         *
         * @param aTsyReqHandle Request handle
         * @return KErrNone
         */
        TInt GetInfoCancel( const TTsyReqHandle aTsyReqHandle );

        /**
         * Phonebookstorage specific Read -Request
         *
         *
         * @param aTsyReqHandle Request handle
         * @param aIndexAndEntries
         * @param aPBData descriptor to fill entry(ies)
         * @param aContinuedRead ETrue means this method has been invoked from ContinueReadL. Otherwise, EFalse.
         * @return KErrNone
         */
        TInt ReadL( const TTsyReqHandle aTsyReqHandle,
            RMobilePhoneBookStore::TPBIndexAndNumEntries* aIndexAndEntries,
            TDes8* aPBData, TBool aContinuedRead=EFalse );

        /**
         * Fill buffered descriptor with data from SIM Server
         *
         *
         * @param aPhoneBookStoreResp
         * @return KErrNone
         */
        TInt FillBufferedDesL( CPhoneBookStoreEntry* aPhoneBookStoreResp );

        /**
         * Cancel reading process
         *
         *
         * @param aTsyReqHandle Request handle
         * @return KErrNone
         */
        TInt ReadCancel( const TTsyReqHandle aTsyReqHandle );

        /**
         * Phonebookstorage specific Write -Request
         *
         *
         * @param aTsyReqHandle Request handle
         * @param aPBData data holding entry to write
         * @param aIndex index to write
         * @return KErrNone
         */
        TInt WriteL( const TTsyReqHandle aTsyReqHandle, TDesC8* aPBData,
            TInt* aIndex );

        /**
         * Writes entry by given index, index should be mapped to (U)SIM world
         *
         *
         * @param aIndex
         * @return None
         */
        TInt WriteByIndexL( TInt aIndex );

        /**
         * Deletes an entry from SIM
         *
         *
         * @param const TTsyReqHandle aTsyReqHandle Request handle
         * @param aIndex index to delete
         * @return KErrNone
         */
        TInt DeleteL( const TTsyReqHandle aTsyReqHandle, TInt* aIndex );

        /**
         * Deletes all entries from SIM
         *
         *
         * @param aTsyReqHandle Request handle
         * @return KErrNone
         */
        TInt DeleteAllL( const TTsyReqHandle aTsyReqHandle );

        /**
         * Notifies an entry event
         *
         *
         * @param aEvent event to notify
         * @param aIndex location to notify
         * @return  KErrNone
         */
        TInt NotifyStoreEvent( RMobilePhoneStore::TMobileStoreEvent *aEvent,
            TInt* aIndex );

        /**
         * Cancels notify store event -request
         *
         *
         * @param aTsyReqHandle Request handle
         * @return KErrNone
         */
        TInt NotifyStoreEventCancel( const TTsyReqHandle aTsyReqHandle );

        /**
         * Completes NotifyStoreEvent request
         *
         *
         * @param aLocation index to notify
         */
        void CompleteNotifyStoreEvent( TInt aLocation );

        /**
         * Sets requested info parameters
         *
         *
         */
        void SetInfo();

        /**
         * This method resets class variables
         *
         *
         */
        void ResetVariables();

        /**
         * This method separates phonebook storage data from byte stream
         *
         *
         * @param aPBData data to separate
         * @param aWriteData data to be filled
         * @return KErrNone or error value
         */
        TInt SeparatePBDataL( TDesC8* aPBData,
            CPhoneBookStoreEntry* aWriteData );

        /**
         * This method starts the new caching request to (U)SIM
         *
         *
         */
        void ReadToCacheL();

        /**
         * This method updates cache
         *
         *
         * @param aEntry phonebook entry
         * @param aDeleteEntry Is the updating cache entry write or delete.
         */
        void UpdateCacheL( CPhoneBookStoreEntry* aEntry,
            TBool aDeleteEntry );

        /**
         * This method read num of entries by the phonebook type
         * and it is part of GetInfo functionality
         *
         *
         * @param aPhoneBookType phonebook type
         * @return KErrNone or error value
         */
        TInt CountEntriesL();

        /**
         * Sets maximum name & number length for the current phonebook
         *
         *
         */
        void SetMaxNameAndNumLenght();

    public:

        /**
         * This method returns the name of the phonebook
         *
         *
         * @param none
         * @return Phonebook name
         */
        TName* PhoneBookName();

        /**
         * This method Activates phonebook entry caching from SIM
         *
         *
         */
        void CacheEntriesL();

        /**
         * This method reads entries from array to fill client's read buffer
         *
         *
         * @param aArrayType Array type to be filled
         */
        void ReadEntriesFromArrayL( TPBArrayTypeToRead aArrayType );

        /**
         * This method completes read num of entries by the phonebook type
         *
         *
         * @param aResult Result value
         * @param aDataPackage Data package
         */
        virtual void CompleteCountEntries( TInt aResult,
            CMmDataPackage* aDataPackage );

#ifdef REQHANDLE_TIMER
        /**
         * Chooses the type of response, automatic or common
         *
         *
         * @param aReqHandleType req handle type.
         * @param aTsyReqHandle req handle to be stored.
         */
        void SetTypeOfResponse( const TInt aReqHandleType,
            const TTsyReqHandle aTsyReqHandle );
#endif

#ifdef TF_LOGGING_ENABLED
        /**
         * Overloads original ReqCompleted for logging pourpouses
         *
         *
         * @param aTsyReqHandle TSY request handle
         * @param aError Error value
         */
        virtual void ReqCompleted(
            const TTsyReqHandle aTsyReqHandle,
            const TInt aError );
#endif
        
        /**
		* This method copies entries from the LTSY cache to CTSYs own version of the cache
		*
		*
		* @param aEntryData Array type to be filled
		*/
	   void CopyLtsyCacheToCtsyCacheL( CArrayPtrSeg<CPhoneBookStoreEntry>* aEntryData );
        
        

    protected:  // Data

        /*
         * Notify PhoneBookStore event
         */
        RMobilePhoneStore::TMobileStoreEvent iMobilePhoneBookStoreEvent;

    private:    // Data

        /*
         * Name of the phonebook
         */
        TName iPhoneBookName;

        /*
         * Phonebook type
         */
        TUint8 iPhoneBookType;

        /*
         * A pointer to the Phone TSY
         * Own.
         */
        CMmPhoneTsy* iMmPhoneTsy;

        /*
         * A pointer to the active extension
         * Own.
         */
        CMmPhoneBookStoreExtInterface* iMmPhoneBookStoreExtInterface;

        /*
         * Pointer to PhoneBookInfo Received from ETel
         * Own.
         */
        RMobilePhoneBookStore::TMobilePhoneBookInfoV1* iPhoneBookInfoChanged;

        /*
         * PhoneBookInfo variable
         */
        RMobilePhoneBookStore::TMobilePhoneBookInfoV1 iPhoneBookInfo;

        /*
         * Variable for reading entries
         */
        TInt iNumEntriesToRead;

        /*
         * Variable to read entries
         * Own.
         */
        TDes8* iBufferToRead;

        /*
         * Variable for reading multiple entries
         */
        TInt iIndexToRead;

        /*
         * Attribute to hold the number of entries
         */
        TInt iNumEntriesFilled;

        /*
         * Attribute to hold the pointer to index and entries
         * Own.
         */
        RMobilePhoneBookStore::TPBIndexAndNumEntries* iIndexAndEntries;

        /*
         * Variable for return index value if needed
         * Own.
         */
        TInt* iWriteIndexToReturn;

        /*
         * Pointer to PBData
         * Own.
         */
        TDesC8* iPBData;

        /*
         * Class attribute to take a eventptr
         * Own.
         */
        TAny* iNotifyPhoneBookStoreEventPtr;

        /*
         * Class attribute to indexptr
         * Own.
         */
        TInt* iNotifyPhoneBookStoreEventIndexPtr;

        /*
         * A table for phonebook store request handles
         */
        TTsyReqHandle iStorageReqHandles[EMmTsyNumOfStorageRequests];

        /*
         * Pointer to ReqHandler instance
         * Own.
         */
        CMmTsyReqHandleStore* iTsyReqHandleStore;

        /*
         * Class attribute to PB request type
         */
        TPBRequestTypes iReqHandleType;

        /*
         * Attribute for reading multiple entries
         */
        TInt iMultipleReadPosTo;

        /*
         * Attribute to hold information about max num len
         * for current phonebook
         */
        TInt iMaxNumLen;

        /*
         * Attribute to hold information about max text len
         * for current phonebook
         */
        TInt iMaxTextLen;

        /*
         * Pointer to phonebook cache
         * Own.
         */
        CArrayPtrSeg<CPhoneBookStoreEntry>* iPBStoreCache;

        /*
         * Pointer to array for Read -request
         * Array is owned an destroyed by the LTSY 
         */
        CArrayPtrSeg<CPhoneBookStoreEntry>* iPBStoreReadEntries;

        /*
         * Attribute tells status of the cache
         */
        TBool iCacheReady;

        /*
         * Pointer to write entry
         * Own.
         */
        CPhoneBookStoreEntry* iWriteEntry;

        /*
         * Pointer to delete entry
         * Own.
         */
        CPhoneBookStoreEntry* iDeleteEntry;

        /*
         * Attribute to used entries
         */
        TInt iUsedEntries;

        /*
         * Attribute to hold info are used entries counted or not
         */
        TBool iIsUsedEntriesCounted;

        /*
         * Attribute to hold storage info
         * Own.
         */
        CStorageInfoData* iStoreInfoData;

        /*
         * Attribute to hold Read req. info
         * Own.
         */
        TReadRequestStore* iReadReqStore;

        /*
         * Boolean indicating that read is requested
         */
        TBool iRequestedOnPBRead;

        /*
         * Return value FillBufferedDesL
         */
        TInt iRetFillBufferedDes;
        
        /*
         * Attribute to tell the status of PB initialization
         */
        TBool iIsPBInitCompleted;
        
        TInt iInitError;
    };

#endif // PHONEBOOKSTORAGE_H

// End of File
