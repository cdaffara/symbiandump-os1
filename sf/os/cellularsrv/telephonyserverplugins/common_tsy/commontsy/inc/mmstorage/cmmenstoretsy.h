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



#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif
#ifndef ENSTORE_H
#define ENSTORE_H

//  INCLUDES
#include "et_phone.h"
#include "et_tsy.h"
#include "cmmphonetsy.h"
#include "cmmenstoregsmext.h"
#include <ctsy/serviceapi/mmtsy_defaults.h>
#include "cmmgsmphonestorageprivateutility.h"

// CONSTANTS  
// This value must be 1 + exactly the same as the maximum enum value of 
// CMmENStoreTsy::TENRequestTypes.
const TInt KMmTsyNumOfENStoreRequests  = 8;
// Same constants used in MMIFACE, will be moved to place
// where both STORAGE and MMIFACE can use them.
const TUint8 KEccStorageNumberMaxCount = 5; // ETSI 11.11
// SIM Message Desc.
const TUint8 KEccStorageEntryLengthIsaSim = 8;

// FORWARD DECLARATIONS
class CMmPhoneTsy;
class CMmENStoreGsmExt; 
class CMmTsyReqHandleStore;
class CMmDataPackage;

// CLASS DECLARATION

/**
* CMmENStoreTsy contains extended ENStore functionality
*/
NONSHARABLE_CLASS( CMmENStoreTsy ) : public CSubSessionExtBase
    {
    public:

        // NOTE! If you add new values to this enum type, remember to increase
        // the value of KMmTsyNumOfONStoreRequests constant!!!
        enum TENRequestTypes
            {
            EMultimodeENStoreReqHandleUnknown,  // 0
            EMultimodeENStoreRead,
            EMultimodeENStoreWrite,
            EMultimodeENStoreDelete,
            EMultimodeENStoreDeleteAll,
            EMultimodeENStoreGetInfo,
            EMultimodeENStoreReadAll,
            EMultimodeENStoreNotifyStoreEvent,  // 7
            };

    public:  // Constructors and destructor

        /**
         * Two-phased constructor.
         */
        static CMmENStoreTsy* NewL( CMmPhoneTsy* aMmPhoneTsy );

        /**
         * Destructor.
         */
        virtual ~CMmENStoreTsy();

        // New functions

        /**
         * Initialisation method that is called from ETel Server.
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
         * Creates new object and returns a pointer to it. All 
         * objects opened from the phone are opened by name, hence 
         * this method is NOT SUPPORTED.
         * 
         *
         * @param aName 
         * @return Pointer to object
         */
        virtual CTelObject* OpenNewObjectL( TDes& /*aName*/ );

        /**
         * Handles extended client requests.
         * A TSY processes all client requests posted using extended API 
         * through this function. This function checks that the request 
         * number is valid and then farms out the request and parameters
         * to other functions.
         * 
         *
         * @param aTsyReqHandle The request handle
         * @param aIpc Request IPC number
         * @param aPackage Packaged data from ETel server
         * @return Error value
         */
        virtual TInt ExtFunc( 
            const TTsyReqHandle aTsyReqHandle, 
            const TInt aIpc, 
            const TDataPackage& aPackage );

        /**
         * To ask what type of flow control is used for the IPC number.
         * When the ETel server receives an "extension" client request, it 
         * will pass the IPC request number down to the TSY in order to 
         * find out what type of request it is. The virtual method that 
         * gets called is ReqModeL.
         *
         * The TSY can associate one or more of the following attributes
         * to the request.
         *    · KReqModeRePostImmediately
         *    · KReqModeFlowControlObeyed
         *    · KReqModeMultipleCompletionEnabled
         *    
         *
         * @param aIpc Request IPC number
         * @return Request mode for given IPC
         */
        virtual CTelObject::TReqMode ReqModeL( const TInt aIpc );

        /**
         * To ask the TSY how big buffer it wants. 
         * When the ETel server discovers that a request is "repost 
         * immediately" it will ask the TSY how big a buffer it wants. 
         * NumberOfSlotsL Returns number of slots to be used for 
         * given requests.
         * 
         *
         * @param aIpc Request IPC number
         * @return Number of slots
         */
        virtual TInt NumberOfSlotsL( const TInt aIpc );

        /**
         * To cancel any extension requests.
         * CancelService is called by the server when it is "cleaning-up"
         * any still outstanding asynchronous requests before closing
         * a client's sub-session. This will happen if a client closes 
         * its R-class handle without cancelling outstanding asynchronous
         * requests on.
         * 
         *
         * @param aIpc Request IPC number
         * @param aTsyReqHandle Request handle
         * @return Erro value
         */
        virtual TInt CancelService( 
            const TInt aIpc, 
            const TTsyReqHandle aTsyReqHandle );

        /**
         * Enables the TSY to "turn on" any regular notification.
         * Called when the server recognises that this notification is being
         * posted for the first time on this sub-session object. It enables 
         * the TSY to "turn on" any regular notification messages that it may 
         * receive from DOS.
         * 
         *
         * @param aIpc Request IPC number
         * @return Error value
         */
        virtual TInt RegisterNotification( const TInt aIpc );

        /**
         * Deregister given notification.
         * Called when the server recognises that this notification will not 
         * be posted again because the last client to have a handle on this 
         * sub-session object has just closed the handle. It enables the
         * TSY to "turn off" any regular notification messages that it may 
         * receive from the DOS.
         * 
         *
         * @param aIpc Request IPC number
         * @return Error value
         */
        virtual TInt DeregisterNotification( const TInt aIpc );

        /**
         * Complete read one EN store entry
         * 
         *
         * @param aResult CS return value
         * @param aDataPackage En store data
         */
        virtual void CompleteRead( TInt aResult, 
            CMmDataPackage* aDataPackage );

        /**
         * Complete get info
         * 
         *
         * @param aResult CS return value
         * @param aDataPackage num of used EN entries
         */
        virtual void CompleteGetInfo( TInt aResult, 
            CMmDataPackage* aDataPackage );

        /**
         * Complete read all EN entries phase 1
         * 
         *
         * @param aResult possible error value
         * @param aDataPackage containing TENStoreResponse items
         * @return Error value
         */
        virtual TInt CompleteReadAllPhase1L( TInt aResult, 
            CMmDataPackage* aDataPackage );
        
        /**
         * Get CMmPhoneTsy object
         * 
         *
         * @return CMmPhoneTsy object
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

    private:

        /**
         * C++ default constructor.
         */
        CMmENStoreTsy();

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
         * @param TDataPackage& aPackage reference to a data package
         * @return KErrNone if request accepted, else several possibilities 
         * depending on IPC
         */
        TInt DoExtFuncL( const TTsyReqHandle aTsyReqHandle,
            const TInt aIpc,
            const TDataPackage& aPackage );

        /**
         * Read one EN store entry
         * 
         *
         * @param aTsyReqHandle TSY request handle
         * @param aEntry entry
         * @return Error value
         */
        TInt Read( const TTsyReqHandle aTsyReqHandle, TDes8* aEntry );

        /**
         * Cancel read one EN store entry
         * 
         *
         * @param aTsyReqHandle TSY request handle
         * @return Error value
         */
        TInt ReadCancel( const TTsyReqHandle aTsyReqHandle );

        /**
         * Get EN store info
         * 
         *
         * @param aInfo info data store
         * @return Error value
         */
        TInt GetInfo( RMobilePhoneStore::TMobilePhoneStoreInfoV1* aInfo );

        /**
         * Cancel get EN store info
         * 
         *
         * @param const aTsyReqHandle TSY request handle
         * @return Error valur
         */
        TInt GetInfoCancel( const TTsyReqHandle aTsyReqHandle );

        /**
         * Read all EN entries first phase
         * 
         *
         * @param aTsyReqHandle TSY request handle
         * @param aId client ID
         * @param aBufSize buffer size 
         * @return Error value
         */
        TInt ReadAllPhase1( const TTsyReqHandle aTsyReqHandle, 
            const RMobilePhone::TClientId* aId, 
            TInt* aBufSize );

        /**
         * Read all EN entries second phase
         * 
         *
         * @param aTsyReqHandle TSY request handle
         * @param aId client ID
         * @param aBuffer EN number buffer 
         * @return Error value
         */
        TInt ReadAllPhase2( TTsyReqHandle aTsyReqHandle, 
            const RMobilePhone::TClientId* aId,
            TDes8* aBuffer );

        /**
         * Cancel Read all EN entries
         * 
         *
         * @param aTsyReqHandle TSY request handle
         * @return Error value
         */
        TInt ReadAllCancel( const TTsyReqHandle aTsyReqHandle );

#ifdef REQHANDLE_TIMER
        /**
         * Chooses the type of response, automatic or common
         * Automatic mode includes an automatic response in case
         * of non response from the DOS in a specified time.
         * 
         *
         * @param aReqHandleType req handle type.
         * @param aTsyReqHandle req handle to be stored.
         */
        void SetTypeOfResponse(
            const TInt aReqHandleType, 
            const TTsyReqHandle aTsyReqHandle );
#endif

    private:    // Data

        /* 
         * Pointer to unpacked ONStore entry, Read -req.
         * Own.
         */
        TDes8* iReadENStoreEntryPtr;

        /* 
         * Pointer to ENStoreInfo Received from ETel
         * Own.
         */
        RMobilePhoneStore::TMobilePhoneStoreInfoV1* iENStoreInfoChanged;

        /* 
         * Class attribute to hold a eventptr
         * Own.
         */
        TAny* iNotifyENStoreEventPtr;

        /* 
         * Class attribute to hold indexptr
         * Own.
         */
        TInt* iNotifyENStoreEventIndexPtr;

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
         * Read all ENStore entries array
         * Own.
         */
        CArrayPtrFlat<CListReadAllAttempt>* iENStoreReadAllArray;

        /* 
         * A pointer to the Phone TSY
         * Own.
         */
        CMmPhoneTsy* iMmPhone;

        /* 
         * A pointer to the active ENStore extension
         * Own.
         */
        CMmENStoreExtInterface* iMmENStoreExtInterface;

        /* 
         * A pointer to the GSM ENStore extension
         * Own.
         */
        CMmENStoreExtInterface* iMmENStoreGsmExt;

        /* 
         * A table for phonebook store request handles
         */
        TTsyReqHandle iStorageReqHandles[KMmTsyNumOfENStoreRequests];

        /* 
         * Pointer to ReqHandler instance
         * Own.
         */
        CMmTsyReqHandleStore* iTsyReqHandleStore;

        /* 
         * Attribute to handling ReqHandleType
         */
        TENRequestTypes iReqHandleType;
    };


#endif      // ENSTORE_H

// End of File
