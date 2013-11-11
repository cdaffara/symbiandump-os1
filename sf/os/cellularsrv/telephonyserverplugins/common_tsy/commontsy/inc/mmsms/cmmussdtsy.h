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



#ifndef CMMUSSDTSY_H
#define CMMUSSDTSY_H

//  INCLUDES
#include <et_phone.h>               // CSubSessionExtBase 
#include "cmmphonetsy.h"    

//  FORWARD DECLARATIONS
class CMmPhoneTsy;
class CMessStore;
class RMobileUssdMessaging::TMobileUssdAttributesV1;

// CLASS DECLARATION

/**
*  USSD TSY
*/
NONSHARABLE_CLASS( CMmUssdTsy ) : public CSubSessionExtBase
    {
    public:
    
        enum TUssdRequestTypes
            {
            EMultimodeUssdReqHandleUnknown,
            EMultimodeUssdSendMessage,
            EMultimodeUssdReceiveMessage,
            EMultimodeUssdSendRelease,
            EMultimodeUssdNotifyNetworkRelease,            
            EMultimodeUssdSendMessageNoFdnCheck,
            // ATTENTION: Declare constant for those requests that need
            // own request handle record in iTsyReqHandleStore above the
            // following!
            EMultimodeUssdMaxNumOfRequests
            };
        
    public:

        /**
         * Two-phased constructor.
         */
        static CMmUssdTsy* NewL( CMmPhoneTsy* aMmPhone );

        /** 
         * Destructor.
         */
        virtual ~CMmUssdTsy();

        /**
         * Initialisation method
         *          
         * 
         */
        virtual void Init();

        /**
         * Returns number of slots to be used for given IPC
         *          
         * 
         * @param aIpc Request IPC number
         * @return Number of slots to be used
         */
        virtual TInt NumberOfSlotsL( const TInt aIpc );
    
        /**
         * Register given notification
         *          
         * 
         * @param aIpc Request IPC number
         * @return Result of the request
         */
        virtual TInt RegisterNotification( const TInt aIpc );

        /**
         * Deregister given notification
         *          
         * 
         * @param aIpc Request IPC number
         * @return Result of the request
         */
        virtual TInt DeregisterNotification( const TInt aIpc );

        /**
         * Dispatches multimode API function calls
         *          
         * 
         * @param aTsyReqHandle Request handle from ETel server
         * @param aIpc IPC number of the request
         * @param aPackage Reference to the input parameters.
         * @return KErrNone / KErrNotSupported
         */
        virtual TInt ExtFunc( const TTsyReqHandle aTsyReqHandle, 
            const TInt aIpc,
            const TDataPackage& aPackage );

        /**
         * Called by the server when it has a "extended".i.e. 
         * non-core ETel request for the TSY. To process a request handle, 
         * request type and request data are passed to the TSY. 
         *          
         * 
         * @param aTsyReqHandle Request handle from ETel server
         * @param aIpc IPC number of the request
         * @param aPackage Reference to the input parameters.
         * @return KErrNone / KErrNotSupported
         */
        virtual TInt DoExtFuncL( const TTsyReqHandle aTsyReqHandle, 
            const TInt aIpc, 
            const TDataPackage& aPackage );

        /**
         * Returns request mode for given IPC number
         *          
         * 
         * @param aIpc Request IPC number
         * @return Request mode
         */
        virtual CTelObject::TReqMode ReqModeL( const TInt aIpc );

        /**
         * Cancels request that's IPC number and request handle are given in
         *          
         * 
         * @param aIpc Request IPC number
         * @param aTsyReqHandle Request handle from ETel server
         * @return Error value
         */
        virtual TInt CancelService( const TInt aIpc, 
            const TTsyReqHandle aTsyReqHandle );

        /**
         * Opens new object and names it
         *          
         * 
         * @param aName Return contains name of opened object
         * @return Pointer to created object casted to CTelObject*
         */
        virtual CTelObject* OpenNewObjectL( TDes& aName );

        /**
         * Opens a object using given name
         *          
         * 
         * @param aName On return contains name of opened object
         * @return NULL
         */
        virtual CTelObject* OpenNewObjectByNameL( const TDesC& aName );
    
        /**
         * Complete SendMessage
         *          
         * 
         * @param aError Completes send message request
         */
        virtual void CompleteSendMessage( TInt aError );

        /**
         * Complete SendMessageNoFdnCheck
         *          
         * 
         * @param aError Completes send message request
         */
        virtual void CompleteSendMessageNoFdnCheck( TInt aError );
                  
        /**
         * Complete SendRelease
         *          
         * 
         * @param aError Error code
         * @param aDataPackage Contains packed 
         * RMobilePhone::TMobilePhoneSendSSRequestV3
         */
        virtual void CompleteSendRelease( TInt aError, 
            CMmDataPackage* aDataPackage );
    
        /**
         * Complete ReceiveMessage request
         *          
         * 
         * @param aError Error code
         * @param aDataPackage Contains TDesC8& (USSD string data) and 
         * packed RMobilePhone::TMobileUssdAttributesV1
         */
        virtual void CompleteReceiveMessage( TInt aError, 
            CMmDataPackage* aDataPackage );
            
        /**
         * Notification network release
         *           
         *
         * @param aTsyReqHandle Request handle
         * @param aMsgData Contains packed RMobileUssdMessaging::TMobilePhoneSendSSRequestV3
         * @param aMsgAttributes Contains poacked RMobilePhone::TMobileUssdAttributesV1
         * @return Error value
         */
        virtual TInt NotifyNetworkRelease( const TTsyReqHandle aTsyReqHandle, 
            TDes8* aMsgData,
            TDes8* aMsgAttributes);
        
        /**
         * Complete network release request
         *          
         *
         * @param aError Error code
         * @param aDataPackage Contains TDesC8& (USSD string data) and 
         * packed RMobilePhone::TMobileUssdAttributesV1
         */    
        virtual void CompleteNotifyNetworkRelease( TInt aErrorCode, 
            CMmDataPackage* aDataPackage );
        
        /**
         * Notify network release cancel
         *          
         *
         * @return KErrNone
         */    
        virtual TInt NotifyNetworkReleaseCancel();                

#ifdef REQHANDLE_TIMER
        /**
         * Calls the needed complete method due timer expiration
         *          
         * 
         * @param aReqHandleType Req handle type.
         * @param aError Error value
         */
        virtual void Complete( TInt aReqHandleType, 
            TInt aError );
#endif // REQHANDLE_TIMER

        /**
         * Returns pointer to the Phone object
         *          
         * 
         * @return Pointer to the Phone object
         */
        virtual CMmPhoneTsy* Phone();
        
    private:

        /**
         * C++ default constructor.
         */
        CMmUssdTsy();

        /**
         * By default Symbian 2nd phase constructor is private.
         * @param CMmPhoneTsy* aMmPhoneTsy, pointer to phone tsy object.
         */
        void ConstructL( CMmPhoneTsy* aMmPhoneTsy );

        /**
         * Returns the ussd messaging capabilities to the client
         *          
         * 
         * @param aTsyReqHandle Request handle
         * @param aCaps Packaged RMobilePhone::TMobileUssdCapsV1
         * @return Return value to the ETel Server
         */ 
        TInt GetCaps( const TTsyReqHandle aTsyReqHandle, 
            TDes8* aCaps );

        /**
         * Receive ussd message
         *          
         * 
         * @param aTsyReqHandle Request handle
         * @param aMsgData Message data
         * @param aMsgAttributes Message attributes
         * @return Error value
         */ 
        TInt ReceiveMessage( const TTsyReqHandle aTsyReqHandle, 
            TDes8* aMsgData, 
            TDes8* aMsgAttributes );
    
        /**
         * Receive ussd message cancel
         *          
         * 
         * @param aTsyReqHandle Request handle
         */
        void ReceiveMessageCancel( const TTsyReqHandle aTsyReqHandle );

        /**
         * Send ussd message
         *          
         * 
         * @param aTsyReqHandle Request handle
         * @param aData Message data
         * @param aAttributes Contains packaged
         * RMobileUssdMessaging::TMobileUssdAttributesV1Pckg
         * @return Error value
         */ 
        TInt SendMessageL( const TTsyReqHandle aTsyReqHandle, 
            TDes8* aData, 
            TDes8* aAttributes );
            
        /**
         * Cancels an outstanding SendMessage request
         *          
         * 
         * @param aTsyReqHandle Request handle
         * @return KErrNone
         */
        TInt SendMessageCancel( const TTsyReqHandle aTsyReqHandle );
              
        /** 
         * Cancels an outstanding SendMessageNoFdnCheck request
         *          
         * 
         * @param aTsyReqHandle Request handle
         * @return KErrNone
         */
        TInt SendMessageNoFdnCheckCancel( const TTsyReqHandle aTsyReqHandle );

        /**
         * Send ussd session end -message
         *          
         * 
         * @param aTsyReqHandle Request handle
         * @param aReturnResult Contains packed 
         * RMobileUssdMessaging::TMobilePhoneSendSSRequestV3
         * @return Error value
         */ 
        TInt SendReleaseL( const TTsyReqHandle aTsyReqHandle, 
            TDes8* aReturnResult );

        /**
         * Cancels an outstanding SendRelease request
         *          
         * 
         * @return KErrNone
         */
        TInt SendReleaseCancel();

        /**
         * Reset used member variables
         * 
         */
        void ResetVariables();

        /**
         * Checks whether or not a ETel request can be performed while offline 
         * mode is enabled
         *          
         * 
         * @param aIpc Ipc number of the request
         * @return TBool 
         */
        TBool IsRequestPossibleInOffline( TInt aIpc ) const;

#ifdef REQHANDLE_TIMER
        /**
         * Chooses the type of response, automatic or common
         *          
         * 
         * @param aReqHandleType Req handle type.
         * @param aTsyReqHandle Eeq handle to be stored.
         */
        void SetTypeOfResponse(
            const TInt aReqHandleType, 
            const TTsyReqHandle aTsyReqHandle );
#endif // REQHANDLE_TIMER

#ifdef TF_LOGGING_ENABLED       
        /**
         * Overloads original ReqCompleted for logging purposes
         *          
         * 
         * @param aTsyReqHandle
         * @param aError
         */
        virtual void ReqCompleted(
            const TTsyReqHandle aTsyReqHandle,
            const TInt aError );
#endif // TF_LOGGING_ENABLED

    /**
     * A table for call request handles
     */
    TTsyReqHandle iUssdReqHandles[EMultimodeUssdMaxNumOfRequests];

    /**
     * Pointer to request handle store
     * Own.     
     */
    CMmTsyReqHandleStore* iTsyReqHandleStore;

    /**
     * A pointer iReceiveUssdMessagePtr
     * Not own.     
     */        
    TDes8* iReceiveUssdMessagePtr;

    /**
     * A pointer to packed ussd message attributes
     * Not own.     
     */        
    RMobileUssdMessaging::TMobileUssdAttributesV1* 
        iReceiveUssdMessageAttributesPtr;

    /**
     * A pointer to packed operation code and additional info
     * Not own.     
     */        
    RMobilePhone::TMobilePhoneSendSSRequestV3* iReturnResultPtr;

    /**
     * A pointer to packed operation code and additional info
     * Not own.     
     */        
    RMobilePhone::TMobilePhoneSendSSRequestV3* iReturnNotifyPtr;

    /**
     * A pointer to packed attributes of a USSD message, returned in
     * NotifyNetworkRelease notifications
     * Not own.     
     */      
    RMobileUssdMessaging::TMobileUssdAttributesV1* iReturnNotifyUssdMessageAttributesPtr;
    
    /**
     * A pointer to the Phone TSY
     * Not own.     
     */        
    CMmPhoneTsy* iMmPhone;

    /**
     * Flow control is removed from EMobileUssdMessagingSendMessage.
     * iSsTransactionOngoing flag is used instead.     
     */  
    TBool iSsTransactionOngoing;
    
    /** 
     * Indicating status of NoFdnCheck
     */ 
    enum TUssdNoFdnCheckStatus
            {
            EUssdNoFdnCheckUnknown, 
            EUssdNoFdnCheckUsed,
            EUssdNoFdnCheckNotUsed                      
            }; 

    /** 
     * Indicating if SendMessageNoFdnCheck request is ongoing                         
     */ 
    TUssdNoFdnCheckStatus iUssdNoFdnCheckFlag;
};

#endif  // CMMUSSDTSY_H

//  End of File

