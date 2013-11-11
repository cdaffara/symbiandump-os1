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



#ifndef CMMSMSTSY_H
#define CMMSMSTSY_H

// INCLUDES
#include <etelmm.h>
#include <et_phone.h>          
#include "MmTsy_timeoutdefs.h" 
#include <ctsy/serviceapi/mctsysatservice.h>

//  FORWARD DECLARATIONS
class CMmSmsStorageTsy;
class CMmPhoneTsy;
class CMessStore;
class CMmSmsExtInterface;
class CMmSmsStorageTsy;
class CMmDataPackage;
class CSmsSendRequest;
class CListReadAllAttempt;
class CMmTsyReqHandleStore;
struct TSmsMsg;
struct TSmsParameters;

// CLASS DECLARATION

/**
*  CMmSmsTsy contains extended sms functionality that is mode-independent 
*  Extension request handles and parameters are stored as attributes
*/
NONSHARABLE_CLASS( CMmSmsTsy ) : public CSubSessionExtBase,
    public MCtsySatService
    {
    public:

        enum TSmsRequestTypes
            {     
            EMultimodeSmsReqHandleUnknown,          //0
            //SMS specific requests
            EMultimodeSmsReceiveMessage,
            EMultimodeSmsSetReceiveMode,
            EMultimodeSmsAckStored,
            EMultimodeSmsNackStored,
            EMultimodeSmsResumeReception,
            EMultimodeSmsSendMessage,
            EMultimodeSmsSendMessageNoFdnCheck,
            EMultimodeSmsSendSatMessage,
            EMultimodeSmsGetMessageStoreInfo,
            EMultimodeSmsReadSmspListPhase1,
            EMultimodeSmsStoreSmspList, 
            EMultimodeSmsNotifyReceiveModeChange,   
            EMultimodeSmsNotifyMoSmsBearerChange,
            EMultimodeSmsSetMoSmsBearer,
            EMultimodeSmsNotifySmspListChange, 
            //SMS storage specific requests
            EMultimodeSmsGetInfo,
            EMultimodeSimStSmsReadSms, 
            EMultimodeSimStSmsWriteSms,            
            EMultimodeSimStSmsEraseSms,
            EMultimodeSimStSmsEraseAllSms,
            EMultimodeSimSmsNotifyStoreEvent,
            EMultimodeSimStSmsReadAllSms, 
            EMultimodeSimStSmsGetNumOfLoc,

            EMultimodeSmsMaxNumOfRequests 
            };

    private:

        /** SMS routing activity */
        enum TSmsRoutingActivity
            {
            ERoutingNotActivated,
            ERoutingActivating,
            ERoutingActivated
            };

    public:

        /**
         * Two-phased constructor.
         * @param aMmPhone Pointer to the MmPhone
         * @return Pointer to the created CMmSmsTsy object
         */
        static CMmSmsTsy* NewL( CMmPhoneTsy* aMmPhone );

        /**
         * Destructor
         */
        ~CMmSmsTsy();

        /** 
         * Initialisation method that is called from ETel Server
         *          
         * 
         */
        virtual void Init();

        /**
         * New instances are created by given name
         *          
         * 
         * @param aName Name of the object to be created
         * @return Pointer to created object casted to CTelObject*
         */
        virtual CTelObject* OpenNewObjectByNameL( const TDesC& aName ); 

        /**
         * New instances are created by given name
         * 
         * 
         * @param aNewName Name of the object to be created
         * @return Pointer to created object casted to CTelObject*
         */
        virtual CTelObject* OpenNewObjectL( TDes& aNewName ); 
        
        /** 
         * Handles extended client requests by calling ExtFunc method
         *           
         * 
         * @param aTsyReqHandle Request handle
         * @param aIpc IPC number of request
         * @param aPackage Parameters for request
         * @return Error value
         */
        virtual TInt ExtFunc( const TTsyReqHandle aTsyReqHandle, 
            const TInt aIpc, 
            const TDataPackage& aPackage );

        /** 
         * When the ETel server receives an "extension" client request, 
         * it will pass the IPC request number down to the TSY in order to find
         * out what type of request it is
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
        virtual TInt NumberOfSlotsL( const TInt aIpc );

        /**
         * Cancels request that's IPC number and request handle are given in 
         * parameters
         *          
         * 
         * @param aIpc IPC number of request
         * @param aTsyReqHandle Request handle
         * @return Error value
         */
        virtual TInt CancelService( const TInt aIpc, 
            const TTsyReqHandle aTsyReqHandle );

        /** 
         * Register given notification
         *          
         * 
         * @param aIpc Number of slots
         * @return Error value
         */
        virtual TInt RegisterNotification( const TInt aIpc );

        /**
         * Deregister given notification
         *          
         * 
         * @param aIpc IPC number of request
         * @return Error value
         */
        virtual TInt DeregisterNotification( const TInt aIpc );

        /**
         * Return pointer to the SMS Storage sub-session
         *          
         * 
         * @return Pointer to SMS Storage sub-session
         */
        virtual CMmSmsStorageTsy* GetSmsStorageTsy();

        /**
         * Activate SMS routing
         *          
         * 
         */
        virtual void ActivateSmsRoutingL();

        /**
         * This method completes reception of incoming unstored SMS
         *          
         * 
         * @param aError Error code
         * @param aDataPackage A data package containing TBool (indicates if 
         * some errors have occured while handling or acknowledging incoming Sms) 
         * and a TSmsMsg* (pointer to Sms message)
         */
        virtual void CompleteReceiveMessage( TInt aError, 
            CMmDataPackage* aDataPackage );

        /**
         * Handle internally response for ActivateSmsRouting
         *          
         * 
         * @param aError Error code
         * @param aDataPackage Package data containing TUint8, 
         * status of the routing (active or not)
         */
        virtual void CompleteActivateSmsRouting( TInt aError, 
            CMmDataPackage* aDataPackage );

        /** 
         * Complete AckSmsStored request
         *          
         * 
         * @param aError Error code
         */
        virtual void CompleteAckSmsStored( TInt aError );

        /** 
         * Complete NackSmsStored request
         *          
         * 
         * @param aError Error code
         */
        virtual void CompleteNackSmsStored( TInt aError );

        /** 
         * Complete CompleteSendMessage request
         *          
         * 
         * @param aError Error code
         * @param aDataPackage
         */
        virtual void CompleteSendMessage( TInt aError, 
            CMmDataPackage* aDataPackage );

        /** 
         * Complete CompleteSendMessageNoFdnCheck request
         *          
         *
         * @param aError Error code
         * @param aDataPackage
         */
        virtual void CompleteSendMessageNoFdnCheck( TInt aError, 
            CMmDataPackage* aDataPackage );

        /**
         * From MCtsySatService
         * Send SAT SMS message to the network
         *          
         * 
         * @param aStkTsySatService Pointer to SAT service interface
         * @param aSmsTpdu Pointer to Sms TPDU
         * @param aScAddress Pointer to service address
         * @param aMobileTON Pointer to type of number
         * @param aMobileNPI Pointer to number plan
         * @param aMoreToSend True if there is more to send
         * @param aTsyReqHandle Request handle
         * @return KErrNone 
         */
        virtual TInt SendSatMessage( 
            MStkTsySatService& aStkTsySatService, 
            TDes8* aSmsTpdu, 
            TDes16* aScAddress, 
            RMobilePhone::TMobileTON* aMobileTON, 
            RMobilePhone::TMobileNPI* aMobileNPI, 
            TBool aMoreToSend, 
            TTsyReqHandle aTsyReqHandle );

        /**
         * Complete SendSatMessage request
         *          
         * 
         * @param aError Error code
         */
        virtual void CompleteSendSatMessage( TInt aError );

        /**
         * Complete ResumeSmsReception request
         *          
         * 
         * @param aError Error code
         */
        virtual void CompleteResumeSmsReception( TInt aError );

        /**
         * Complete get sms message store info
         *          
         * 
         * @param aError Error code
         * @param aDataPackage
         */
        virtual void CompleteGetMessageStoreInfo( TInt aError, 
            CMmDataPackage* aDataPackage );

        /**
         * Complete first phase of read all SMSP sets
         *          
         * 
         * @param aError Error code
         * @param aDataPackage
         */
        virtual void CompleteReadAllSmspPhase1( TInt aError, 
            CMmDataPackage* aDataPackage );

        /**
         * Handle internally SMS servers response to SMS Parameter update          
         * request (Store SMSP list)
         *          
         * 
         * @param aError Error code
         */
        virtual void InternalStoreSmspList( TInt aError );

        /**
         * Set SMS Storage TSY
         *          
         * 
         * @param aNewPointer Pointer to Storage Tsy
         */
        virtual void SetSmsStorageTsy( CMmSmsStorageTsy* aNewPointer );

        /**
         * Return number of SMS location on SIM card 
         *          
         * 
         * @return Number of SMS location on SIM card
         */
        virtual TInt16 GetSmsNumOfLoc();

        /**
         * Set Sms Number of Location on SIM Card
         *          
         * 
         * @param aSmsNumOfUsedEntries Number of SMS location
         */
        virtual void SetSmsNumOfLoc( TInt aSmsNumOfUsedEntries );

        /**
         * This method set Sms Send Attributes values for sending MMSAT 
         * message
         *          
         * 
         * @param aMmPhone
         * @param aSmsTpdu Pointer to Sms TPDU
         * @param aScAddress Pointer to service address
         * @param aMobileTON Pointer to type of number
         * @param aMobileNPI Pointer to number plan
         * @param aMoreToSend True if there is more to send
         * @return Error code
         */
        virtual TInt SendSatMessageL( 
            CMmPhoneTsy* aMmPhone,
            TDes8* aSmsTpdu,
            TDes16* aScAddress,
            RMobilePhone::TMobileTON* aMobileTON,
            RMobilePhone::TMobileNPI* aMobileNPI,
            TBool aMoreToSend );

        /**
         * Set the information about the CS hardware status: True if off-line, 
         * false if on-line
         *          
         * 
         * @param aIsOffline
         */
        void SetOffline( TBool aIsOffline );

        /**
         * Complete set bearer type for sending SMS messages 
         *          
         *
         * @param aResult Error value
         */
        void CompleteSetMoSmsBearer( TInt aResult );

#ifdef REQHANDLE_TIMER
        /**
         * Chooses the type of response, automatic or common
         *          
         * 
         * @param aReqHandleType Req handle type
         * @param aTsyReqHandle Req handle to be stored
         */
        void SetTypeOfResponse( const TInt aReqHandleType, 
            const TTsyReqHandle aTsyReqHandle );

        /**
         * Complete request because of timer expiration
         *          
         * 
         * @param aReqHandleType Request handle type
         * @param aError Error value
         */    
        virtual void Complete( TInt aReqHandleType, 
            TInt aError );
#endif // REQHANDLE_TIMER

    private:

        /**
         * C++ default constructor.
         */
        CMmSmsTsy();

        /**
         * By default Symbian 2nd phase constructor is private.
         */
        void ConstructL();

        /**
         * Resend an unsent message
         *          
         * 
         */
        void ResendSms();

        /** 
         * Handles extended client requests
         *          
         * 
         * @param aTsyReqHandle Request handle
         * @param aIpc IPC number of request
         * @param aPackage Data Package
         * @return Error value
         */
        TInt DoExtFuncL( const TTsyReqHandle aTsyReqHandle, 
            const TInt aIpc, 
            const TDataPackage& aPackage );

        /**
         * Get SMS messaging capabilities
         *          
         * 
         * @param aTsyReqHandle Request handle
         * @param aCaps Pointer to the messaging capabilities
         * @return KErrNone 
         */
        TInt GetCaps( const TTsyReqHandle aTsyReqHandle, 
            TDes8* aCaps );

        /**
         * Get receive mode
         *          
         * 
         * @param aTsyReqHandle Request handle
         * @param aReceiveMode Pointer to receive mode
         * @return KErrNone 
         */
        TInt GetReceiveMode( const TTsyReqHandle aTsyReqHandle, 
            RMobileSmsMessaging::TMobileSmsReceiveMode* aReceiveMode );

        /**
         * Set receive mode
         *          
         * 
         * @param aTsyReqHandle Request handle
         * @param aReceiveMode Pointer to receive mode
         * @return KErrNone 
         */
        TInt SetReceiveMode( const TTsyReqHandle aTsyReqHandle, 
            RMobileSmsMessaging::TMobileSmsReceiveMode const* aReceiveMode );

        /**
         * Set receive mode cancel
         *          
         * 
         * @param aTsyReqHandle Request handle
         * @return KErrNone 
         */
        TInt SetReceiveModeCancel( const TTsyReqHandle aTsyReqHandle );

        /**
         * Notify from receive mode changes
         *          
         * 
         * @param aReceiveMode Pointer to receive mode
         * @return KErrNone 
         */
        TInt NotifyReceiveModeChange( 
            RMobileSmsMessaging::TMobileSmsReceiveMode* aReceiveMode );

        /**
         * Cancel notify reveice mode change request
         *          
         * 
         * @param aTsyReqHandle Request handle
         * @return KErrNone 
         */
        TInt NotifyReceiveModeChangeCancel( 
            const TTsyReqHandle aTsyReqHandle );

        /**
         * Complete notify reveice mode change request
         *          
         * 
         */
        void CompleteNotifyReceiveModeChange();

        /**
         * This method starts the reception of incoming unstored SMS
         *          
         * 
         * @param aTsyReqHandle Request handle
         * @param aMsgData Message data
         * @param aMsgAttributes Message attributes
         * @return Error value
         */
        TInt ReceiveMessageL( const TTsyReqHandle aTsyReqHandle,
            TDes8* aMsgData, 
            TDes8* aMsgAttributes );

        /**
         * Wait for next incoming unstored message cancel
         *          
         * 
         * @param aTsyReqHandle Request handle
         * @return Error value
         */
        TInt ReceiveMessageCancel( const TTsyReqHandle aTsyReqHandle );

        /**
         * Store Class 2 message in TSY's internal memory
         *          
         * 
         * @param aSmsMsg Pointer to TSmsMsg struct
         */
        void StoreClass2MessageL( TSmsMsg const* aSmsMsg );

        /**
         * Deliver Stored Class 2 message to the SMS stack
         * 
         */
        void DeliverClass2ToSmsStack();

        /**
         * Ack message
         *          
         * 
         * @param aTsyReqHandle Request handle
         * @param aMsgData SMS PDU
         * @param aMemoryFull Is SMS stack's memory now full
         * @return Error value
         */
        TInt AckSmsStoredL( const TTsyReqHandle aTsyReqHandle, 
            const TDesC8* aMsgData, 
            TBool const* aMemoryFull );

        /**
         * Route RP-Error request to messagehandler
         *          
         * 
         * @param aTsyReqHandle Request handle
         * @param aMsgData Pointer to message data
         * @param aRpCause Pointer to RP cause value
         * @return Error value
         */
        TInt NackSmsStoredL( const TTsyReqHandle aTsyReqHandle, 
            const TDesC8* aMsgData, 
            TInt* aRpCause );

        /**
         * Resume sms message reception
         *          
         * 
         * @param aTsyReqHandle Request handle
         * @return Error value
         */
        TInt ResumeSmsReceptionL( const TTsyReqHandle aTsyReqHandle );

        /**
         * Retrieve the current setting for the bearer type used 
         * for sending SMS messages 
         *          
         * 
         * @param aTsyReqHandle Request handle
         * @param aBearer Pointer to bearer
         * @return KErrNone
         */
        TInt GetMoSmsBearer( const TTsyReqHandle aTsyReqHandle, 
            RMobileSmsMessaging::TMobileSmsBearer* aBearer );

        /**
         * Set the bearer type for sending SMS messages 
         *          
         * 
         * @param aTsyReqHandle Request handle
         * @param aBearer Pointer to bearer
         * @return KErrNone
         */
        TInt SetMoSmsBearer( const TTsyReqHandle aTsyReqHandle, 
            RMobileSmsMessaging::TMobileSmsBearer* aBearer );

        /**
         * Set the bearer type cancel
         *          
         * 
         * @param aTsyReqHandle Request handle
         * @return KErrNone
         */
        TInt SetMoSmsBearerCancel( const TTsyReqHandle aTsyReqHandle );

        /**
         * Notify from SMS bearer change
         *          
         * 
         * @param aBearer Pointer to bearer
         * @return KErrNone
         */
        TInt NotifyMoSmsBearerChange( 
            RMobileSmsMessaging::TMobileSmsBearer* aBearer );

        /**
         * Notify from SMS bearer change cancel
         *          
         * 
         * @param aTsyReqHandle Request handle
         * @return KErrNone
         */
        TInt NotifyMoSmsBearerChangeCancel( 
            const TTsyReqHandle aTsyReqHandle );

        /**
         * Send message
         *          
         * 
         * @param aTsyReqHandle Request handle
         * @param aMsgData Pointer to Message Data
         * @param aMsgAttributes Pointer to Message Attributes
         * @return Error value
         */
        TInt SendMessageL( const TTsyReqHandle aTsyReqHandle, 
            TDes8* aMsgData, 
            TDes8* aMsgAttributes );
            
        /** 
         * Cancels an outstanding SendMessageNoFdnCheck request
         *          
         * 
         * @param aTsyReqHandle Request handle
         * @return Error value
         */
        TInt SendMessageNoFdnCheckCancel( const TTsyReqHandle aTsyReqHandle );
               
        /** 
         * Send sat message
         *          
         * 
         * @param aSmsTpdu Pointer to SMS PDU
         * @param aScAddress Pointer to SC address
         * @param aMobileTON Type of number
         * @param aMobileNPI Numberin plan identific
         * @param aTsyReqHandle Request handle
         */
        void DoSendSatMessageL( TDes8* aSmsTpdu, 
            TDes16* aScAddress,                    
            RMobilePhone::TMobileTON* aMobileTON, 
            RMobilePhone::TMobileNPI* aMobileNPI, 
            TBool aMoreToSend,
            TTsyReqHandle aTsyReqHandle );

        /** 
         * Enumerate sms message stores
         *          
         * 
         * @param aTsyReqHandle Request handle
         * @param aCount Pointer to message count
         * @return KErrNone    
         */
        TInt EnumerateMessageStores( const TTsyReqHandle aTsyReqHandle, 
            TInt* aCount );

        /**
         * Get sms message store info
         *          
         * 
         * @param aTsyReqHandle Request handle
         * @param aIndex Index
         * @param aInfo Information
         * @return Error value
         */
        TInt GetMessageStoreInfoL( const TTsyReqHandle aTsyReqHandle, 
            TInt const* aIndex, 
            TDes8* aInfo );

        /**
         * Get sms message store info cancel
         *          
         * 
         * @param aTsyReqHandle Request handle
         * @return KErrNone
         */
        TInt GetMessageStoreInfoCancel( const TTsyReqHandle aTsyReqHandle );

        /**
         * Read SMSP list phase 1
         *          
         * 
         * @param aTsyReqHandle Request handle
         * @param aId Pointer to client id
         * @param aBufSize Pointer to buffer size
         * @return Error value
         */
        TInt ReadSmspListPhase1L( const TTsyReqHandle aTsyReqHandle, 
            RMobilePhone::TClientId const* aId, 
            TInt* aBufSize );

        /**
         * Read SMSP list phase 2
         *          
         * 
         * @param aTsyReqHandle Request handle
         * @param aId Pointer to client id
         * @param aBuffer Pointer to buffer
         * @return KErrNone
         */
        TInt ReadSmspListPhase2( TTsyReqHandle aTsyReqHandle, 
            RMobilePhone::TClientId const* aId, 
            TDes8* aBuffer );

        /**
         * Cancel read SMSP list
         *          
         * 
         * @param aTsyReqHandle Request handle
         * @return KErrNone
         */
        TInt ReadAllSmspCancel( const TTsyReqHandle aTsyReqHandle );

        /**
         * Create SMSP list from read entries
         *          
         * 
         * @param aSmspList List of SMSP entries
         */
        void CreateSmspListL( CArrayPtrFlat<TSmsParameters>& aSmspList );

        /**
         * Store SMSP list
         *          
         * 
         * @param aTsyReqHandle Request handle
         * @param aBuffer Pointer to buffer
         * @return KErrNone
         */
        TInt StoreSmspList( const TTsyReqHandle aTsyReqHandle, 
            TDes8* aBuffer );

        /**
         * This method trap and calls ProcessStoreSmspListL-method
         *          
         * 
         */
        void CallProcessStoreSmspList();

        /**
         * Call CompleteStoreSmspList-method or make new SMS parameter update
         * request to DOS
         *          
         * 
         */
        void ProcessStoreSmspListL();

        /**
         * Complete StoreAllSmsp request to the client
         *          
         * 
         * @param aError Cause value that is used while completing request
         */
        void CompleteStoreSmspList( TInt aError );

        /**
         * Notify SMSP list change
         *          
         * 
         * @return KErrNone
         */
        TInt NotifySmspListChange();

        /**
         * Notify SMSP list change cancel
         *          
         * 
         * @param aTsyReqHandle Request handle
         * @return KErrNone
         */
        TInt NotifySmspListChangeCancel( const TTsyReqHandle aTsyReqHandle );

        /**
         * Complete notify SMSP list
         *          
         * 
         */
        void CompleteNotifySmspListChange();
    
        /** 
         * Reset all class attributes
         *          
         * 
         */
        void ResetVariables();

        /**
         * Checks wether or not a ETel request can be performed while 
         * offline mode is enabled
         *          
         * 
         * @param aIpc Ipc number of the request
         * @return TBool 
         */
        TBool IsRequestPossibleInOffline( TInt aIpc ) const;
        
        /**
         * Checks if error code is a relay protocol error
         *          
         * 
         * @param aError Cause value received from LTSY
         * @return TBool
         */
        static TBool IsRPError(TInt aError);

    private:

        /**
         * A pointer to the Phone TSY
         * Not own.         
         */
        CMmPhoneTsy* iMmPhone;

        /**
         * A pointer to the SMS extension
         * Own.         
         */
        CMmSmsExtInterface* iMmSmsExtInterface;

        /**
         * A pointer to the SMS storage sub-session
         * Not own.         
         */
        CMmSmsStorageTsy* iMmSmsStorageTsy;

        /**
         * A pointer to the SAT service interface provided by the SAT
         * TSY.
         * Not own.
         */
        MStkTsySatService* iTsySatMessaging;

        /**
         * Routing activity
         */
        TSmsRoutingActivity iServerRoutingActivity;

        /**
         * Receive mode
         */
        RMobileSmsMessaging::TMobileSmsReceiveMode iMobileSmsReceiveMode;

        /**
         * Notify receive mode pointer
         * Not own.         
         */
        RMobileSmsMessaging::TMobileSmsReceiveMode* iNotifyReceiveModeChangePtr;

        /**
         * Wait for unstored message pointer
         * Not own.         
         */
        TAny* iReceiveMessageParamsPtr;

        /**
         * Wait for unstored message pointer
         * Not own.         
         */
        TDes8* iReceiveMessagePduPtr;

        /**
         * Notify SMS bearer pointer
         * Not own.         
         */
        RMobileSmsMessaging::TMobileSmsBearer* iNotifySmsBearerPtr;

        /**
         * A pointer to send message attributes
         * Not own.         
         */
        TDes8* iSendMessageMsgAttrPckgPtr;

        /**
         * A pointer to get message store info
         * Not own.         
         */
        TDes8* iGetMessageStoreInfoPtr;

        /**
         * Store all parameter sets buffer
         * Not own.         
         */
        TDes8* iStoreSmspBufferPtr;

        /**
         * Store all parameter sets index         
         */
        TInt iStoreSmspIndex;
         
        /**
         * Request handle type         
         */
        TSmsRequestTypes iReqHandleType;

        /**
         * Pointer to TSY request handle store
         * Own.                  
         */
        CMmTsyReqHandleStore* iTsyReqHandleStore;

        /**
         * A table for Sms request handles                  
         */
        TTsyReqHandle iSmsReqHandles[EMultimodeSmsMaxNumOfRequests];

        /**
         * SMSP list, in streamed format, for two phased list retrieval
         * Not own.                  
         */
        CBufBase* iSMSPList;

        /**
         * SMSP size buffer, for two phased list retrieval                
         */        
        TInt* iRetSMSPSize;
        
        /**
         * SMSP client id for two phased list retrieval                
         */             
        RMobilePhone::TClientId* iSMSPClientId;

        /**
         * SMS message array for incoming messages that are waiting
         * acknowledging from SMS stack     
         * Own.                               
         */
        CArrayPtrFlat<TSmsMsg>* iSmsMsgArray;

        /**
         * Flag that indicates if client storage is full                  
         */        
        TBool iClientStorageFull;

        /**
         * Number of SMS Locations on Sim Card                  
         */
        TUint8 iSmsNumOfLoc;

        /**
         * SMS send request
         * stores the last sent message for possible re-send attempts
         * Own.                                    
         */
        CSmsSendRequest* iSmsSendReq;

        /**
         * On-line/off-line status of CS hardware.
         * At this moment, only needed for method ResumeSmsReception                           
         */
        TBool iIsOffline;

        /**
         * true if a ResumeSmsReception request has come during off-line
         * mode. See method ResumeSmsReception                           
         */
        TBool iResumeSmsReceptionPending;

        /** 
         * Indicating status of NoFdnCheck
         */ 
        enum TSmsNoFdnCheckStatus
            {
            ESmsNoFdnCheckUnknown, 
            ESmsNoFdnCheckUsed,
            ESmsNoFdnCheckNotUsed                      
            }; 
            
        /** 
         * Indicating if send message no fdn check request is ongoing
         */ 
        TSmsNoFdnCheckStatus iSmsNoFdnCheckFlag;          

        TInt iExpectAckOrNack;
        
};
#endif // CMMSMSTSY_H

//  End of File
