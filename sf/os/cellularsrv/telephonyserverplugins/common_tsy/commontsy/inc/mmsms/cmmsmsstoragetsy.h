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



#ifndef CMMSMSTORAGETSY_H
#define CMMSMSTORAGETSY_H

// INCLUDES
#include <et_phone.h>   
#include <mmretrieve.h> 
#include "cmmtsyreqhandlestore.h"  
#include <ctsy/serviceapi/cmmsmsutility.h>
#include "cmmsmstsy.h"

//  FORWARD DECLARATIONS
class CMmPhoneTsy;
class CMmSmsExtInterface;
class CListReadAllAttempt;
class CMmTsyReqHandleStore;
class CRetrieveMobilePhoneSmsList;
class CMmDataPackage;

// CLASS DECLARATION

/**
*  SMS Storage TSY
*/
NONSHARABLE_CLASS( CMmSmsStorageTsy ) : public CSubSessionExtBase
    {
    public:

        /**
         * Two-phased constructor.
         * @param aMmSmsTsy Pointer to Sms Tsy 
         * @param aMmPhone Pointer to MmPhone 
         * @param aMmSmsExtInterface Pointer to SmsExtInterface 
         * @param aMmSmsExtInterface Pointer to request handle store
         * @return Pointer to the created CMmSmsStorageTsy object
         */
        static CMmSmsStorageTsy* NewL( CMmSmsTsy* aMmSmsTsy, 
            CMmPhoneTsy* aMmPhone, 
            CMmSmsExtInterface* aMmSmsExtInterface,
            CMmTsyReqHandleStore* aMmTsyReqHandleStore );

        /**
         * Destructor
         */
        virtual ~CMmSmsStorageTsy();

        /**
         * Initialisation method
         *         
         * 
         */
        virtual void Init();
    
        /**
         * Handles extended client requests
         *         
         * 
         * @param aTsyReqHandle Tsy request handle
         * @param aIpc Ipc number of request
         * @param aPackage Package data
         * @return Error value
         */
        virtual TInt ExtFunc( const TTsyReqHandle aTsyReqHandle, 
            const TInt aIpc,
            const TDataPackage& aPackage );

        /**
         * Returns request mode for given IPC number
         *         
         * 
         * @param aIpc IPC number of request
         * @return Request mode for given IPC
         */
        virtual CTelObject::TReqMode ReqModeL( const TInt aIpc );

        /**
         * Returns number of slots to be used for given IPC
         *         
         * 
         * @param aIpc IPC number of request
         * @return Number of slots
         */
        virtual TInt NumberOfSlotsL( const TInt aIpc);

        /**
         * Cancels request that's IPC number and request handle are given         
         * in parameters
         *         
         * 
         * @param aIpc IPC number of request
         * @param aTsyReqHandle Request handle
         * @return Error value
         */
        virtual TInt CancelService( const TInt aIpc, 
            const TTsyReqHandle aTsyReqHandle );

        /**
         * New instances are created by given name
         *         
         * 
         * @param aName Name of the object to be created
         * @return CTelObject*
         */
        virtual CTelObject* OpenNewObjectL( TDes& aName );

        /**
         * New instances are created by given name
         *         
         * 
         * @param aName Name of the object to be created
         * @return CTelObject*
         */
        virtual CTelObject* OpenNewObjectByNameL( const TDesC& aName );

        /**
         * Register given notification
         *         
         * 
         * @param aIpc IPC number of request
         * @return KErrNone / KErrNotSupported 
         */
        virtual TInt RegisterNotification( const TInt aIpc );

        /**
         * Deregister given notification
         *         
         * 
         * @param aIpc IPC number of request
         * @return KErrNone / KErrNotSupported  
         */
        virtual TInt DeregisterNotification( const TInt aIpc );

        /**
         * Complete GetInfo request
         *         
         * 
         * @param aError Error code
         * @param aDataPackage Package data
         */
        virtual void CompleteGetInfo( TInt aError, 
            CMmDataPackage* aDataPackage ); 

        /** 
         * Complete ReadSms request
         *         
         * 
         * @param aError Error code
         * @param aDataPackage Package data
         */
        virtual void CompleteReadSms( TInt aError, 
            CMmDataPackage* aDataPackage );

        /**
         * Complete WriteSms request
         *         
         * 
         * @param aError Error code
         * @param aDataPackage Package data
         */
        virtual void CompleteWriteSms( TInt aError, 
            CMmDataPackage* aDataPackage );

        /**
         * Complete DeleteSms request
         *         
         * 
         * @param aError Error code
         */
        virtual void CompleteDeleteSms( TInt aError );

        /**
         * Complete DeleteAllSms request
         *         
         * 
         * @param aError Error code
         */
        virtual void CompleteDeleteAllSms( TInt aError );

		/** 
         * Call when the last location of SMS slot on SIM card was empty
		* to retrieve the Sms list
		* 		
         * 
         * @param aError Error code
         * @param aDataPackage Package data
         */
		virtual void RetrieveSmsListReadSmsL ( TInt aError, 
            CMmDataPackage* aDataPackage );

        /** 
         * Complete first phase of read all SMSes
         *         
         * 
         * @param aError Error code
         * @param aReceivedClass2ToBeReSent boolean which notifies if a class2 
         * has to be resent by SMS Server or not
         */
        virtual void CompleteReadAllSmsPhase1( TInt aError, 
			TBool aReceivedClass2ToBeReSent );
		
        /**
         * Unpacks a entry package and set contents of a message to the entry
         *         
         * 
         * @param iReadSmsEntryPtr Pointer to sms entry
         * @param aSmsMsg Pointer to the sms message
         * @return KErrNone
         */
        virtual TInt UnpackSmsReadEntryPackage( TAny* iReadSmsEntryPtr,     
            TSmsMsg* aSmsMsg );

        /**
         * Unpacks a entry package and set location of the slot to the entry
         *         
         * 
         * @param iWriteSmsEntryPtr Pointer to write sms entry
         * @param aLocation Location of the slot
         * @return KErrNone
         */
        virtual TInt UnpackSmsWriteEntryPackage( TAny* iWriteSmsEntryPtr,
            TInt aLocation );

		/**
         * Get pointer to Tsy request Handle Store 
         *         
         * 
         * @return Pointer to Tsy request Handle Store 
         */
		CMmTsyReqHandleStore* GetTsyReqHandleStore();
    
        /**
         * Complete NotifyStoreEvent request
         *         
         * 
         * @param aLocation Location of the slot
         * @param aEvent Sum of RMobilePhoneStore::TMobileStoreEvent event types
         */
        void CompleteNotifyStoreEvent( TInt aLocation,
       		TUint32 aEvent );

    private:

        /**
         * C++ default constructor.
         */
        CMmSmsStorageTsy();
        
        /**
         * By default Symbian 2nd phase constructor is private.
         */
        void ConstructL();

        /**
         * Handles extended client requests
         *         
         * 
         * @param aTsyReqHandle Tsy request handle
         * @param aIpc IPC number of request
         * @param aPackage Packaged data
         * @return Error value
         */
        TInt DoExtFuncL( const TTsyReqHandle aTsyReqHandle, 
            const TInt aIpc, 
            const TDataPackage& aPackage );

        /**
         * Get info from SMS message store
         *         
         * 
         * @param aTsyReqHandle Request handle
         * @param aSmsStoreInfoPckg Pointer to the sms store infopackage
         * @return Error value
         */
        TInt GetInfoL( const TTsyReqHandle aTsyReqHandle, 
            TDes8* aSmsStoreInfoPckg );

        /**
         * Cancel the outstanding asynchronous GetInfo request
         *         
         * 
         * @param aTsyReqHandle Request handle
         * @return KErrNone
         */
        TInt GetInfoCancel( const TTsyReqHandle aTsyReqHandle );

        /**
         * Read one SMS message
         *         
         * 
         * @param aTsyReqHandle Request handle
         * @param aSmsPckg A pointer to the sms package
         * @return Error value
         */
        TInt ReadSmsL( const TTsyReqHandle aTsyReqHandle, 
            TDes8* aSmsPckg );

        /**
         * Cancel an outstanding asynchronous ReadSms request
         *         
         * 
         * @param aTsyReqHandleRequest handle
         * @return KErrNone
         */
        TInt ReadSmsCancel( const TTsyReqHandle aTsyReqHandle );

        /**
         * Write one SMS message
         *         
         * 
         * @param aTsyReqHandle Request handle
         * @param aSmsPckg Pointer to the sms package
         * @return Error value
         */
        TInt WriteSmsL( const TTsyReqHandle aTsyReqHandle, 
            TDesC8* aSmsPckg );

        /**
         * Delete one SMS message
         *         
         * 
         * @param aTsyReqHandle Request handle
         * @param aIndex Pointer to the slot location
         * @return Error value
         */
        TInt DeleteSmsL( const TTsyReqHandle aTsyReqHandle, 
            TInt* aIndex );

        /**
         * Delete all SMS messages
         *         
         * 
         * @param aTsyReqHandle Request handle
         * @return Error value
         */
        TInt DeleteAllSmsL( const TTsyReqHandle aTsyReqHandle );

        /**
         * Notify SMS message store event
         *         
         * 
         * @param aEvent Pointer to the event
         * @param aIndex Pointer to the index
         * @return KErrNone
         */
        TInt NotifySmsStoreEvent( RMobilePhoneStore::TMobileStoreEvent* aEvent,
            TInt* aIndex );

        /**
         * Cancel an outstanding asynchronous NotifyStoreEvent request
         *         
         * 
         * @param aTsyReqHandle Request handle 
         * @return KErrNone
         */
        TInt NotifySmsStoreEventCancel( const TTsyReqHandle aTsyReqHandle );

        /**
         * Retrieve SMS message list phase 1
         *         
         * 
         * @param aTsyReqHandle Request handle 
         * @param aRequest Pointer to the request
         * @param aBufSize Pointer to the buffer size
         * @return Error value
         */
        TInt ReadAllSmsPhase1L( const TTsyReqHandle aTsyReqHandle, 
            CRetrieveMobilePhoneSmsList::TBatchRequestData const* aRequest, 
            TInt* aBufSize );
 
        /**
         * Retrieve SMS message list phase 2
         *         
         * 
         * @param aTsyReqHandle Request handle 
         * @param aId Pointer to client id
         * @param aBuffer Pointer to client buffer
         * @return KErrNone
         */
        TInt ReadAllSmsPhase2( TTsyReqHandle aTsyReqHandle, 
            RMobilePhone::TClientId const* aId, 
            TDes8* aBuffer );

        /**
         * Cancel a two-phase list retrieval before completion
         *         
         * 
         * @param aTsyReqHandle Request handle
         * @return KErrNone
         */
        TInt ReadAllSmsCancel( const TTsyReqHandle aTsyReqHandle );
    
        /**
         * Reset all SMS storage variables
         *         
         * 
         */
        void ResetVariables();

    protected:

        /*
         * SMS message list storage array
         * Own.         
         */         
        CArrayPtrFlat<TSmsMsg>* iSmsListArray;

        /*
         * Read all SMS messages array
         * Own.         
         */ 
        CArrayPtrFlat<CListReadAllAttempt>* iSmsReadAllArray;

    private:

        /*
         * Pointer to request handle store
         * Own.         
         */ 
        CMmTsyReqHandleStore* iMmTsyReqHandleStore;

        /*
         * Pointer to the Phone TSY
         * Not Own.         
         */ 
        CMmPhoneTsy* iMmPhone;

        /*
         * Pointer to the SMS TSY
         * Not Own.         
         */ 
        CMmSmsTsy* iMmSmsTsy;

        /*
         * Pointer to the SMS extension
         * Not Own.         
         */ 
        CMmSmsExtInterface* iMmSmsExtInterface;

        /*
         * Get SMS store info pointer
         * Not Own.         
         */ 
        TAny* iGetInfoInfoPtr;

        /*
         * Read SMS message pointer
         * Not Own.         
         */ 
        TDes8* iReadSmsEntryPtr;    

        /*
         * Write SMS message pointer
         * Not Own.         
         */ 
        TAny* iWriteSmsEntryPtr;

        /*
         * Delete SMS message location index
         */ 
        TInt iDeleteSmsIndex;

        /*
         * Notify SMS store event pointer
         * Not own.         
         */ 
        TAny* iNotifySmsStoreEventPtr;

        /*
         * Notify SMS store event index
         * Not own.         
         */ 
        TInt* iNotifySmsStoreEventIndexPtr;

        /*
         * Read all SMS messages pointer
         * Not own.         
         */ 
        TInt* iReadAllBufSizePtr;

        /*
         * Read all SMS messages Id
         */ 
        RMobilePhone::TClientId iReadAllId;

        /*
         * Request handle type         
         */ 
        CMmSmsTsy::TSmsRequestTypes iReqHandleType;

        /*
         * Number of total SIM SMS entries         
         */         
        TUint8 iTotalEntries;
        
        /*
         * Number of used SIM SMS entries         
         */                 
        TUint8 iUsedEntries;
    };

#endif  // CMMSMSTORAGETSY_H

// End of File
