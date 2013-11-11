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



#ifndef CMMBROADCASTTSY_H
#define CMMBROADCASTTSY_H


// INCLUDES
#include <et_phone.h>
#include <mmretrieve.h> 
#include "MmTsy_timeoutdefs.h"
#include <ctsy/serviceapi/cmmsmsutility.h>

// FORWARD DECLARATIONS
class CMmPhoneTsy;
class CMessStore;
class CListReadAllAttempt;
class CMmTsyReqHandleStore;
class CMmMessageManagerBase;
class CMmDataPackage;
struct TCbsCbmiAndLanglist;

// CLASS DECLARATION

/**
*  BroadcastTSY
*/
NONSHARABLE_CLASS( CMmBroadcastTsy ) : public CSubSessionExtBase
    {
    public:
    
        enum TBroadcastRequestTypes
            {
            EMultimodeBroadcastReqHandleUnknown,    //0
            EMultimodeBroadcastReceiveMessage,
            EMultimodeBroadcastReceiveMessageCancel,
            EMultimodeBroadcastSetFilterSetting,
            EMultimodeBroadcastNotifyFilterSetting,
            EMultimodeBroadcastSetLanguageFilter,
            EMultimodeBroadcastNotifyLanguageFilter,
            EMultimodeBroadcastStoreCbmiList,
            EMultimodeBroadcastNotifyCbmiList,
            EMultimodeBroadcastMaxNumOfRequests
            };

    public:

        /**
         * Two-phased constructor.
         */
        static CMmBroadcastTsy* NewL( CMmPhoneTsy* aMmPhone );

        /**
         * Destructor
         */
        virtual ~CMmBroadcastTsy();

        /**
         * Initialisation method that is called from ETel Server
         *          
         * 
         */
        virtual void Init();

        /**
         * Handles extended client requests
         *          
         * 
         * @param aTsyReqHandle Parameter handles the request
         * @param aIpc Ipc number of request
         * @param aPackage Packaged data
         * @return Error value
         */
        virtual TInt ExtFunc( const TTsyReqHandle aTsyReqHandle, 
            const TInt aIpc,
            const TDataPackage& aPackage );

        /** 
         * Returns request mode for given IPC number
         *          
         * 
         * @param aIpc Ipc number of request
         * @return Request mode for given IPC
         */
        virtual CTelObject::TReqMode ReqModeL( const TInt aIpc );

        /** 
         * Returns number of slots to be used for given IPC
         *          
         * 
         * @param aIpc Ipc number of request
         * @return Number of Slots
         */
        virtual TInt NumberOfSlotsL( const TInt aIpc );

        /** 
         * Cancels request that's IPC number and request handle are given          
         * in parameters
         *          
         * 
         * @param aIpc Ipc number of request
         * @param aTsyReqHandle Parameter handles the request
         * @return Error value
         */
        virtual TInt CancelService( const TInt aIpc, 
            const TTsyReqHandle aTsyReqHandle );

        /** 
         * New instances are created by given name
         *          
         * 
         * @param aName A name of the object to be created
         * @return Pointer to created XXX object casted to CTelObject*
         */
        virtual CTelObject* OpenNewObjectL( TDes& aName );

        /** 
         * New instances are created by given name
         *          
         * 
         * @param aName A name of the object to be created
         * @return Pointer to created object casted to CTelObject*
         */
        virtual CTelObject* OpenNewObjectByNameL( const TDesC& aName );

        /** 
         * Register given notification
         *          
         * 
         * @param aIpc Ipc number of request
         * @return Error value
         */
        virtual TInt RegisterNotification( const TInt aIpc );

        /**
         * Deregister given notification
         *          
         * 
         * @param aIpc Ipc number of request
         * @return Error value
         */
        virtual TInt DeregisterNotification( const TInt aIpc );

        /** 
         * Handle internally response to Broadcast routing request
         *          
         * 
         * @param aError A local error variable
         */
        virtual void InternalCompleteCbRoutingRequest( TInt aError );

        /** 
         * Complete ReceiveMessageCancel request 
         *          
         * 
         * @param aError A local error variable
         */ 
        virtual void CompleteReceiveMessageCancel( TInt aError );

        /** 
         * Complete ReceiveMessage Gsm Cbs request
         *          
         * 
         * @param aError A local error variable
         * @param aDataPackage
         */
        virtual void CompleteReceiveMessageGsmCbs( TInt aError, 
            CMmDataPackage* aDataPackage );

		/** 
         * Complete ReceiveMessage Wcdma Cbs request
         *          
         * 
         * @param aError A local error variable
         * @param aDataPackage
         */
        virtual void CompleteReceiveMessageWcdmaCbs( TInt aError, 
            CMmDataPackage* aDataPackage );
		
		/** 
         * Complete ReceiveWcdma Message Page left
         *          
         * 
         */
        virtual void CompleteReceivedWcdmaCbsMessagePageLeft(); 

        /** 
         * Complete SetFilterSetting request
         *          
         * 
         * @param aError A local error variable
         */
        virtual void CompleteSetFilterSetting( TInt aError );

#ifdef REQHANDLE_TIMER
        /** 
         * Complete request because of timer expiration
         *          
         * 
         * @param aReqHandleType A request handle type
         * @param aError A error value
         */    
        virtual void Complete( TInt aReqHandleType, TInt aError );
#endif // REQHANDLE_TIMER

    private:

        /**
         * C++ default constructor.
         */
        CMmBroadcastTsy();
        
        /**
         * By default Symbian 2nd phase constructor is private.
         */
        void ConstructL();

        /** 
         * Handles extended client requests
         *          
         * 
         * @param aTsyReqHandle Tsy request handle
         * @param aIpc IPC number
         * @param aPackage Packaged data
         * @return Error value
         */
        TInt DoExtFuncL( const TTsyReqHandle aTsyReqHandle, 
            const TInt aIpc, 
            const TDataPackage& aPackage );

        /**
         * Get Broadcast Messaging capabilities of the phone
         *          
         * 
         * @param aTsyReqHandle
         * @param aCaps
         * @return Error value
         */
        TInt GetCaps( const TTsyReqHandle aTsyReqHandle, 
            TDes8* aCaps );

        /**
         * Calls CbRoutingRequest-method that activates routing of CB messages
         *          
         * 
         * @param aTsyReqHandle Tsy request handle
         * @param aMsgData 
         * @param aMsgAttributes
         * @return Error value
         */ 
        TInt ReceiveMessageL( const TTsyReqHandle aTsyReqHandle, 
            TDes8* aMsgData, 
            TDes8* aMsgAttributes );

        /**
         * Releases routing of CB messages and cancel receiving of next 
         * incoming Broadcast Message
         *          
         * 
         * @param aTsyReqHandle Tsy request handle
         * @return Error value
         */
        TInt ReceiveMessageCancelL( const TTsyReqHandle aTsyReqHandle );

        /**
         * Get current setting for the receipt of broadcast messages
         *          
         * 
         * @param aTsyReqHandle Tsy request handle
         * @param aSetting
         * @return KErrNone
         */
        TInt GetFilterSetting( const TTsyReqHandle aTsyReqHandle, 
            RMobileBroadcastMessaging::TMobilePhoneBroadcastFilter* aSetting );
    
        /**
         * Set a new setting for the receipt of broadcast messages
         *          
         * 
         * @param aTsyReqHandle Tsy request handle
         * @param aSetting
         * @return Error value
         */
        TInt SetFilterSettingL( const TTsyReqHandle aTsyReqHandle,
            RMobileBroadcastMessaging::TMobilePhoneBroadcastFilter const* 
            aSetting );

        /**
         * Notify client if there is a change in the setting for the receipt 
         * of broadcast messages
         *          
         * 
         * @param aSetting
         * @return KErrNone
         */
        TInt NotifyFilterSettingChange( 
            RMobileBroadcastMessaging::TMobilePhoneBroadcastFilter* aSetting );

        /**
         * Cancel an outstanding asynchronous NotifyFilterSettingChange 
         * request
         *          
         * 
         * @param aTsyReqHandle
         * @return KErrNone
         */ 
        TInt NotifyFilterSettingChangeCancel( 
            const TTsyReqHandle aTsyReqHandle );

        /**
         * Complete NotifyFilterSettingChange request
         *          
         * 
         */
        void CompleteNotifyFilterSettingChange();

        /**
         * Retrieve CBMI list phase 1
         *          
         * 
         * @param aTsyReqHandle Tsy request handle
         * @param aReqData
         * @param aBufSize
         * @return Error value
         */
        TInt GetBroadcastIdListPhase1L( const TTsyReqHandle aTsyReqHandle,
            CRetrieveMobilePhoneBroadcastIdList::TGetBroadcastIdRequest const* 
                aReqData,
            TInt* aBufSize );

        /**
         * Retrieve CBMI list phase 2
         *          
         * 
         * @param aTsyReqHandle Tsy request handle
         * @param aClient
         * @param aBuffer
         * @return KErrNone
         */
        TInt GetBroadcastIdListPhase2( const TTsyReqHandle aTsyReqHandle,
            RMobilePhone::TClientId const* aClient,
            TDes8* aBuffer );
            
        /**
         * Store a new version of the entire list of CBMI entries  
         *          
         * 
         * @param aTsyReqHandle Tsy request handle
         * @param aBuffer
         * @return Error value
         */
        TInt StoreBroadcastIdListL( const TTsyReqHandle aTsyReqHandle, 
            TDes8 const* aBuffer );

        /** 
         * Reset all Broadcast variables
         *          
         * 
         */
        void ResetVariables();

#ifdef REQHANDLE_TIMER
        /**
         * Chooses the type of response, automatic or common
         *          
         * 
         * @param aReqHandleType Req handle type.
         * @param aTsyReqHandle Req handle to be stored
         */
        void SetTypeOfResponse( const TInt aReqHandleType, 
            const TTsyReqHandle aTsyReqHandle );
#endif // REQHANDLE_TIMER

    protected:

        /**
         * Broadcast Filter Setting temporary storage
         */         
        RMobileBroadcastMessaging::TMobilePhoneBroadcastFilter 
            iCbFilterTempSetting;

        /**
         * Broadcast Filter Setting storage
         */         
        RMobileBroadcastMessaging::TMobilePhoneBroadcastFilter 
            iCbFilterSetting;

    private:

        /**
         * A pointer to the Phone TSY
         * Not own.         
         */
        CMmPhoneTsy* iMmPhone;

        /**
         * Pointer to the message manager
         * Not own.         
         */
        CMmMessageManagerBase* iMessageManager;

        /**
         * Broadcast Messages routing from DOS activated         
         */
        TBool iCbRoutingActivated;

        /**
         * Receive Broadcast Message
         * Not own.                  
         */
        TDes8* iReceiveCbMessagePtr;

        /**
         * Receive Broadcast Message Attributes
         * Not own.                  
         */
        TDes8* iReceiveCbMessageAttributesPtr;

        /**
         * Notify Filter Setting Change
         * Not own.                  
         */
        RMobileBroadcastMessaging::TMobilePhoneBroadcastFilter* 
			iNotifyFilterSettingChangePtr;

        /**
         * Request handle type                  
         */
        TBroadcastRequestTypes iReqHandleType;

        /**
         * Pointer to TSY request handle store
         * Own.                           
         */
        CMmTsyReqHandleStore* iTsyReqHandleStore;

        /**
         * A table for cell broadcast request handles
         */
        TTsyReqHandle iBroadcastReqHandles[EMultimodeBroadcastMaxNumOfRequests];

        /**
         * Received WCDMA CBS message
         * Not own.         
         */
		CArrayPtrFlat< TWcdmaCbsMsg >* iCbsMsg;

        /**
         * Number of Pages of WCDMA CBS Message
         */
		TUint8 iWcdmaPageNumber;
		
		/**
		* Currently handled page WCDMA CBS Message
		*/
		TUint8 iWcdmaCurrentPage;

        /**
         * Flag which notifies if some WCDMA CDS Pages are waiting to be 
         * passed to client
         */
		TBool iWcdmaCbsPageLeft;

        /**
         * Index of the page of the WCDMA CBS Message
         */
		TUint8 iWcdmaCbsMsgPageIndex;

		
};

#endif  // CMMBROADCASTTSY_H

//  End of File
