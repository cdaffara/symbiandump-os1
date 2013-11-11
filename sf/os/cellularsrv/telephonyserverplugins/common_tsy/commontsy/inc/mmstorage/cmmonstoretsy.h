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



#ifndef ONSTORE_H
#define ONSTORE_H

//  INCLUDES
#include <et_phone.h>
#include <et_tsy.h>
#include "cmmphonetsy.h"
#include "cmmonstoregsmext.h"
#include <ctsy/serviceapi/mmtsy_defaults.h>
#include "cmmgsmphonestorageprivateutility.h"

// CONSTANTS
const TUint8 KSimMsisdn = 0x6D;
const TUint8 KSimNoServiceType = 0xFA;

// FORWARD DECLARATIONS
class CMmPhoneTsy;
class CMmONStoreGsmExt; 
class CMmTsyReqHandleStore;
class CMmDataPackage;

// CLASS DECLARATION

/**
*  CMmONStoreTsy contains extended ONStore functionality.
*/
NONSHARABLE_CLASS( CMmONStoreTsy ) : public CSubSessionExtBase
    {
    public:

        //NOTE! If you add new values to this enum type, remember to increase
        //the value of KMmTsyNumOfONStoreRequests constant!!!
        enum TONRequestTypes
            {
            EMultimodeONStoreReqHandleUnknown,  // 0
            EMultimodeONStoreRead,
            EMultimodeONStoreWrite,
            EMultimodeONStoreDelete,
            EMultimodeONStoreDeleteAll,
            EMultimodeONStoreGetInfo,
            EMultimodeONStoreReadAll,
            EMultimodeONStoreNotifyStoreEvent,
            EMultimodeONStoreStoreAll,          // 8
            EMmTsyNumOfONStoreRequests
            };

    public:  // Constructors and destructor

        /**
         * Two-phased constructor.
         */
        static CMmONStoreTsy* NewL( CMmPhoneTsy* aMmPhoneTsy );

        /**
         * Destructor.
         */
        virtual ~CMmONStoreTsy();

    public: // New functions

        /**
         * Methods that are inherited from ETel Server base classes
         * Initialisation method
         * 
         *
         */
        virtual void Init();

        /**
         * Opens extendable sub-sessions.
         * 
         *
         * @param aName A name of the object to be opened.
         * @return Pointer to created instance by given name.
         */
        virtual CTelObject* OpenNewObjectByNameL( const TDesC& /*aName*/ );

        /**
         * Creates new object and returns a pointer to it. 
         * New instances are created by given name, hence 
         * this method is NOT SUPPORTED.
         * 
         *
         * @param aName A name of the object to be opened.
         * @return Pointer to created instance by given name.
         */
        virtual CTelObject* OpenNewObjectL( TDes& /*aName*/ );

        /**
         * Handles extended client requests
         * 
         *
         * @param aTsyReqHandle The request handle
         * @param aIpc Request IPC number
         * @param aPackage Packaged data from ETel server
         * @return Error value
         */
        virtual TInt ExtFunc( const TTsyReqHandle aTsyReqHandle, 
            const TInt aIpc, const TDataPackage& aPackage );

        /**
         * To ask what type of flow control is used for the IPC number.
         * 
         *
         * @param aIpc Request IPC number
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
         * @return Error value
         */
        virtual TInt CancelService( const TInt aIpc, 
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
         * @param aIpc Request IPC number
         * @return KErrNone or KErrNotSupported
         */
        virtual TInt DeregisterNotification( const TInt aIpc );

        /**
         * Sets the active ONStore extension
         * 
         *
         * @param aMmONStoreExt pointer to the new ONStore extension.
         * return KErrNone if extension set, else KErrGeneral
         */
        virtual TInt SetActiveONStoreExtension(
            CMmONStoreExtInterface* aMmONStoreExt );

        /**
         * Gets the active ON extension
         * 
         *
         * @return Pointer to the active ON extension
         */
        virtual CMmONStoreExtInterface* ActiveONExtension(); 

        /**
         * Completes read -request, called when an entry has been read
         * or error from SIM
         * 
         *
         * @param aResult Error value
         * @param aDataPackage data from SIM
         */
        virtual void CompleteRead( TInt aResult, 
            CMmDataPackage* aDataPackage );

        /**
         * Completes write -request
         * 
         *
         * @param aResult Error value
         */
        virtual void CompleteWrite( TInt aResult, 
            CMmDataPackage* aDataPackage );

        /**
         * Completes delete -request
         * 
         *
         * @param aResult Error value
         */
        virtual void CompleteDelete( TInt aResult );

        /**
         * Completes DeleteAll request
         * 
         *
         * @param aResult Error value
         */
        virtual void CompleteDeleteAll( TInt aResult );

        /**
         * Completes DeleteAllPhase1 request: the num of entries
         * 
         *
         * @param aResult Error value
         */
        virtual void CompleteDeleteAllPhase1L( TInt aResult );

        /**
         * Completes GetInfo request
         * 
         *
         * @param aResult Error value
         */
        virtual void CompleteGetInfo( TInt aResult, 
            CMmDataPackage* aDataPackage );

        /**
         * Completes StoreAll -request
         * 
         *
         * @param aResult Error value
         */
        virtual void CompleteStoreAll( TInt aResult );

        /**
         * Reads ONStore entry for ReadAllPhase1
         * 
         *
         * @param aError Error value
         * @param ONStoreMsg ONStore entry
         */
        void InternalRetrieveONListReadEntryL( TInt aError, 
            TONStoreMsg* ONStoreMsg );

        /**
         * Reads ONStore entry for ReadAllPhase1
         * 
         * @param aError Error value
         * @param aDataPackage Data package containing ONStore entry
         */
        void InternalRetrieveONListReadEntryL( TInt aError, 
            CMmDataPackage* aDataPackage );


        /**
         * Reads ONStore entry for ReadAllPhase1
         * 
         *
         * @param TInt aError Error value
         * @param aDataPackage Datapackage containing ONSore size
         */
        void InternalRetrieveONListReadSizeL( TInt aError, 
            CMmDataPackage* aDataPackage );
                
        /**
         * Completes internally ReadAll -request
         * 
         *
         * @param aResult Error value
         */
           virtual void CompleteReadAllPhase1Req( TInt aResult );

        /**
         * Complete phase1 of retrieving ONStore entry list
         * 
         *
         * @return Error value
         */
           virtual TInt CompleteReadAllPhase1L();

        /**
         * Gets ONStore size for StoreAll -request
         * 
         *
         * @param aError Error value
         * @param aSize Size of the ONStore
         */
        void InternalStoreAllGetSizeL( TInt aError, TUint8 aSize );

        /**
         * Gets ONStore size for StoreAll -request
         * 
         *
         * @param aError Error value
         * @param aDataPackage Datapackage containing ONSore size
         */
        void InternalStoreAllGetSizeL( TInt aError, 
            CMmDataPackage* aDataPackage );

        /**
         * Writes ONStore entry for StoreAll -req
         * 
         *
         * @param aError Error value
         */
        virtual void InternalStoreAllWriteEntryL( TInt aError );

        /**
         * Gets pointer to CMmPhoneTsy
         * 
         *
         * @return Pointer to CMmPhoneTsy
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

        /*
         * This method used to notify the phone that the phone book store initilization was complete
         *
         * @param  aError Error value
         */
        void PhoneBookStoreInitCompleteL(TInt aError);
    private:
    
        /**
         * C++ default constructor.
         */
        CMmONStoreTsy();

        /**
         * By default Symbian 2nd phase constructor is private.
         */
        void ConstructL();

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
        TInt DoExtFuncL( const TTsyReqHandle aTsyReqHandle, const TInt aIpc, 
            const TDataPackage& aPackage );

        /**
         * Reads an entry from ONStore
         * A supplied aEntry parameter will be filled before the 
         * request is completed
         * 
         *
         * @param aEntry Entry to fill
         * @return Error value
         */
        TInt ReadL( TDes8* aEntry );

        /**
         * Cancels Read -request if it's still active
         * 
         *
         * @param aTsyReqHandle Request handle
         * @return KErrNone
         */
        TInt ReadCancel( const TTsyReqHandle aTsyReqHandle );

        /**
         * Writes the entry supplied within the entry parameter into the
         * ONStore, at an index specified by entry index. If the client
         * specifies an index of -1, then the TSY will store the entry in the
         * first free location and then return this location within an entry 
         * index when it completes the request
         * 
         *
         * @param aTsyReqHandle Request handle
         * @param aEntry An entry to writeL 
         * @return KErrNone
         */
        TInt WriteL( const TTsyReqHandle aTsyReqHandle, TDesC8* aEntry );

        /**
         * Searchs the first free loacation from SIM
         * 
         *
         * @return Error value
         */
        TInt SearchIndexAndWrite();

        /**
         * Cancels Write -request, if it's still active
         * 
         *
         * @param aTsyReqHandle Request handle
         * @return KErrNone
         */
        TInt WriteCancel( const TTsyReqHandle aTsyReqHandle );

        /**
         * Deletes an entry from ONStorage
         * 
         *
         * @param aTsyReqHandle Request handle
         * @param aIndex Pointer to index to be deleted 
         * @return KErrNone
         */
        TInt DeleteL( const TTsyReqHandle aTsyReqHandle, TInt* aIndex );

        /**
         * Deletes all entries from ONStorage
         * 
         *
         * @param aTsyReqHandle Request handle
         * @return KErrNone
         */
        TInt DeleteAllL( const TTsyReqHandle aTsyReqHandle );

        /**
         * Cancels DeleteAll -request, if it's still active
         * 
         *
         * @param aTsyReqHandle Request handle
         * @return KErrNone
         */
        TInt DeleteAllCancel( const TTsyReqHandle aTsyReqHandle );

        /**
         * This method returns information about the ONStore.
         * This information includes the store's name, the type of thing it 
         * stores, the total number of storage slots and the number of used
         * slots. It does not give information about which slots are filled
         * and which are empty.
         * 
         *
         * @param aTsyReqHandle  Request handle
         * @param  aInfo Info to be filled 
         * @return KErrNone
         */
        TInt GetInfoL( const TTsyReqHandle aTsyReqHandle, 
            RMobileONStore::TMobileONStoreInfoV1* aInfo );

        /**
         * Sets ini values to TSY's internal info -parameter
         * 
         *
         */      
        void SetInfo();

        /**
         * Cancels GetInfo -request, if it's still active
         * 
         *
         * @return KErrNone
         */
        TInt GetInfoCancel( const TTsyReqHandle aTsyReqHandle );

        /**
         * This method allows a client to be notified when one of the events
         * defined in TMobileStoreEvent occurs to the store.  These events
         * allow a client to know the overall status of a store 
         * (full, empty etc) and also when a slot within a store changes.
         * A slot could change because another client or the phone has 
         * created, updated or deleted an entry.
         * 
         *
         * @param aEvent Event to be filled
         * @param aIndex Index to be filled
         * @return KErrNone
         */
        TInt NotifyStoreEvent( RMobilePhoneStore::TMobileStoreEvent *aEvent, 
            TInt* aIndex );

        /**
         * Completes NotifyStoreEvent request
         * 
         *
         * @param aLocation index to notify
         */
        void CompleteNotifyStoreEvent( TInt aLocation );

        /**
         * Cancels notifying ONStore event
         * 
         *
         * @param aTsyReqHandle Request handle
         * @return KErrNone
         */
        TInt NotifyStoreEventCancel( const TTsyReqHandle aTsyReqHandle );

        /**
         * Makes phase1 request to ReadAll entires from ONStore
         * 
         *
         * @param aTsyReqHandle Request handle
         * @param aId Pointer to client Id
         * @param aBufSize Pointer to buffer size
         * @return KErrNone
         */
        TInt ReadAllPhase1L( const TTsyReqHandle aTsyReqHandle, 
            const RMobilePhone::TClientId* aId, TInt* aBufSize );

        /**
         * Makes a request for reading entry for ReadAll -request
         * 
         *
         */
        void MakeInternalRetrieveONListReadReqL();

        /**
         * Delete ONStore entry list array elements
         * 
         *
         */
        void DeleteONStoreListArrayElements();

        /**
         * Phase2 request to ReadAll entires from ONStore
         * 
         *
         * @param aTsyReqHandle Request handle
         * @return KErrNone
         */
        TInt ReadAllPhase2( TTsyReqHandle aTsyReqHandle, 
            const RMobilePhone::TClientId* aId, TDes8* aBuffer );

        /**
         * Cancels ReadAll -request if it's still outstanding
         * 
         *
         * @param aTsyReqHandle Request handle
         * @return KErrNone
         */
        TInt ReadAllCancel( const TTsyReqHandle aTsyReqHandle );

        /**
         * This method stores the whole ONStore in one go. Client
         * gives a buffered ONList, which contains ONStore elements, and this
         * list should not contain more entries that ONStore can store. 
         * It must be notified that if client supplies an empty ONList to 
         * ONStoreTSY, the whole ONStore will be destroyed.
         * 
         *
         * @param aTsyReqHandle Request handle
         * @param aBuffer buffered ONList 
         * @return KErrNone
         */
        TInt StoreAllL( const TTsyReqHandle aTsyReqHandle, TDes8* aBuffer );

        /**
         * Makes write request for storing all entries to StoreAll -request
         * 
         *
         */
        void MakeInternalStoreAllWriteReqL();

        /**
         * Cancels StoreAll -request if it's still outstanding
         * 
         *
         * @param aTsyReqHandle Request handle
         * @return KErrNone
         */
        TInt StoreAllCancel( TTsyReqHandle aTsyReqHandle );

        /**
         * This method resets class variables
         * 
         *
         */
        void ResetVariables();

#ifdef REQHANDLE_TIMER
        /**
         * Chooses the type of response, automatic or common
         * 
         *
         * @param ReqHandleType req handle type.
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
         * @param aTsyReqHandle Request handle
         * @param aError Error value
         */
        virtual void ReqCompleted( const TTsyReqHandle aTsyReqHandle, 
            const TInt aError );
#endif

    protected:  // Data

        /*
         * Notify PhoneBookStore event
         */
        RMobilePhoneStore::TMobileStoreEvent iMobileONStoreEvent;

    private:    // Data

        /*
         * Pointer to unpacked ONStore entry, Read -req.
         * Own.
         */
        TDes8* iReadONStoreEntryPtr;

        /*
         * Pointer to unpacked ONStore entry, Write -req.
         * Own.
         */
        TDesC8* iWriteONStoreEntryPtr;

        /*
         * Attribute to handle write request, search first empty location
         */
        TInt iWriteIndexToReturn;

        /*
         * Attribute to handle cancelling write
         */
        TBool iIsWriteCancellingPossible;

        /*
         * Delete ONStore Index
         */
        TInt iDeleteONStoreIndex;

        /*
         * Attribute for cancellation deleteall
         */
        TBool iDeleteAllContinue;

        /*
         * Class attribute to take a eventptr
         * Own.
         */
        TAny* iNotifyONStoreEventPtr;

        /*
         * Class attribute to indexptr
         * Own.
         */
        TInt* iNotifyONStoreEventIndexPtr;

        /*
         * PhoneBookInfo variable
         */
        RMobileONStore::TMobileONStoreInfoV1 iONStoreInfo;

        /*
         * Pointer to ONStoreInfo Received from ETel
         * Own.
         */
        RMobileONStore::TMobileONStoreInfoV1* iONStoreInfoChanged;

        /*
         * ReadAll message pointer
         * Own.
         */
        TInt* iReadAllBufSizePtr;

        /*
         * Read all message Id
         */
        RMobilePhone::TClientId iReadAllId;

        /*
         * Read all ONStore size
         */
        TInt iReadAllONStoreSize;

        /*
         * Read all ONStore size; hold the location index
         */
        TInt iReadAllONStoreIndex;

        /*
         * ONStoreTSY internal list to save entries for ReadAll
         * Own.
         */
        CArrayFixFlat<TONStoreMsg>* iONStoreListArray;

        /*
         * Read all ONStore entries array
         * Own.
         */
        CArrayPtrFlat<CListReadAllAttempt>* iONStoreReadAllArray;

        /*
         * Attribute to hold StoreAll buffer
         * Own.
         */
        TDes8* iStoreAllBuffer;

        /*
         * Holds the store size from client's ONList
         */
        TInt iStoreAllONListSize;

        /*
         * Store all ONStore size; hold the store size
         */
        TInt iStoreAllONStoreSize;

        /* 
         * Store all ONStore size; hold the index to write
         */
        TInt iStoreAllONStoreIndex;

        /*
         * Type of this phonebook: ONStore
         */
        TUint8 iPhoneBookType;

        /*
         * Attribute to used entries
         */
        TInt iUsedEntries;

        /*
         * is CountEntries used?
         */
        TBool iIsUsedEntriesCounted;

        /*
         * Service Type for GetInfo
         */
        TServiceType iServiceType;

        /*
         * A pointer to the Phone TSY
         * Own.
         */
        CMmPhoneTsy* iMmPhone;

        /*
         * A pointer to the active ONStore extension
         * Own.
         */
        CMmONStoreExtInterface* iMmONStoreExtInterface;

        /*
         * A pointer to the GSM ONStore extension
         * Own.
         */
        CMmONStoreExtInterface* iMmONStoreGsmExt;

        /*
         * A table for phonebook store request handles
         */
        TTsyReqHandle iStorageReqHandles[EMmTsyNumOfONStoreRequests];

        /*
         * Pointer to ReqHandler instance
         * Own.
         */
        CMmTsyReqHandleStore* iTsyReqHandleStore;

        /*
         * Attribute to handling ReqHandleType
         */
        TONRequestTypes iReqHandleType;
    };

#endif      // ONSTORE_H

// End of File
