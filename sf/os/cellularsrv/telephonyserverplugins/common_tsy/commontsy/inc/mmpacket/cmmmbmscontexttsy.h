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



#ifndef CMmMBMSContextTsy_H
#define CMmMBMSContextTsy_H

//  INCLUDES
#include "Cmmpacketcontexttsy.h"
#include "mbmstypes.h"


// FORWARD DECLARATIONS
class CMmTsyReqHandleStore;
class CMmPacketServiceTsy;

// CLASS DECLARATION

/**
*  CMmMBMSContextTsy contains MBMS context related functionality. 
*/
NONSHARABLE_CLASS( CMmMBMSContextTsy ) : public CMmPacketContextTsy
    {
    public: // Constructor and destructor    
    
  
        /**
         * NewL method is used to create a new instance of CMmMBMSContextTsy
         * class.
         * 
         *                  
         * @param aMmPacketService Pointer to the Packet Service object
         * @param aName Context name
         * @param aProxyId Proxy id
         */
        static CMmMBMSContextTsy* NewL
                ( CMmPacketServiceTsy* aMmPacketService,                
                  const TDes& aName,  
                  const TUint8 aProxyId );

        /**
         * Destructor
         */
        ~CMmMBMSContextTsy();

        /**
         * TRAP's all CMmMBMSContextTsy related MM API requests in case that
         * they fail. This method functions only as a centralized TRAP for the
         * DoExtFuncL method that does the actual mapping of IPC number to
         * TSY method call.
         *         
         *
         * @param aTsyReqHandle TSY request handle from ETel server
         * @param aIpc IPC number of the request
         * @param aPackage Reference to the input parameters.
         * @return KErrNone/KErrNotSupported
         */
        virtual TInt ExtFunc( TTsyReqHandle aTsyReqHandle, 
            TInt aIpc, const TDataPackage& aPackage );    

        /**
         * Returns request mode for given IPC number
         *          
         *                 
         * @param aIpc IPC number of the request
         * @return CTelObject::TReqMode Request mode
         */
        virtual CTelObject::TReqMode ReqModeL( TInt aIpc );

        /**
         * Returns number of slots to be used for a given IPC
         *          
         *                  
         * @param aIpc IPC number of the request
         * @return TInt Number of slots for this request
         */
        virtual TInt NumberOfSlotsL( TInt aIpc );

        /**
         * Cancels request that's IPC number and request handle are given as
         * parameters
         *         
         *   
         * @param aIpc: IPC number of the request
         * @param aTsyReqHandle Request handle from ETel server
         * @return KErrNone/KErrNotSupported
         */
        virtual TInt CancelService( TInt aIpc, 
            TTsyReqHandle aTsyReqHandle );

        /**
         * Register given notification
         *         
         *
         * @param aIpc IPC number of the request
         * @return TInt Result of the request
         */
        virtual TInt RegisterNotification( TInt aIpc );

        /**
         * Deregister given notification
         *         
         *
         * @param aIpc IPC number of the request
         * @return TInt Result of the request
         */
        virtual TInt DeregisterNotification( TInt aIpc );

        /**
         * Complete the context initialisation
         *         
         *
         * @param aResult Result of the request
         * @param aDataChannel the data channel to return to the client
         */
        void CompleteInitialiseContext( TInt aResult ,RPacketContext::TDataChannelV2* aDataChannel );

        /**
         * Complete the context activation.
         *         
         *
         * @param aResult Result of the request
         */
        void CompleteActivate(TInt aResult );
        
        /**
         * Complete activate phase2
         *         
         *
         * @param aTsyReqHandle The ETEL request handle
         * @param aClient The client id
         * @param aBuffer A descriptor to put the active services into
         * @return error code
         */
        TInt GetMbmsActiveServicesPhase2L(
        	TTsyReqHandle aTsyReqHandle,
        	RMobilePhone::TClientId* aClient,
	        TDes8* aBuffer );
	        
	    /**
	     * Cancel get service list request
	     */
        TInt CancelGetMbmsActiveServicesPhase1();

        /**
         * Complete the context deactivation
         *         
         *
         * @param aResult Result of the request
         * @param aDataPackage NULL or a list of failed entries in case there are failed entries 
         */
        void CompleteDeactivate( CMmDataPackage* aDataPackage, TInt aResult );
 
        /**
         * Complete the context delete
         *         
         *
         * @param aResult Result of the request
         */
        void CompleteDelete( TInt aResult );

        /**
         * Complete the config changed notification
         *         
         * @param aDataPackage The data that was send from the LTSY
         * @param aResult The result of the status change 
         */
        void CompleteNotifyConfigChanged( 
        		const CMmDataPackage* aDataPackage = NULL,
                TInt aResult= 0);
        
        /**
         * Complete the status change notification
         *         
         * @param aDataPackage The data that was send from the LTSY
         * @param aResult The result of the status change 
         */
        void CompleteNotifyStatusChange(
            const CMmDataPackage* aDataPackage,
            TInt aResult  );      
            
        /**
         * Complete the status change notification
         *         
         *
         * @param aContextStatus Contains status of the context
         */
        void CompleteNotifyStatusChange(
            RPacketContext::TContextStatus aContextStatus  );                 
        
        /**
         * Complete the context configuration
         *         
         *
         * @param aResult Result of the request
         */
        void CompleteSetConfig( TInt aResult );

        /**
         * Set the last error cause.
         *         
         *
         * @param aErrorCause Contains the last error cause
         */
        void SetLastErrorCause( TInt aErrorCause );

        /**
         * Get the context status.
         *         
         *
         * return RPacketContext::TContextStatus
         */
        RPacketContext::TContextStatus ContextStatus() const;

        /**
         * Tells if context activation is allowed
         *         
         *
         * @return TBool ETrue/EFalse
         */ 
        TBool IsContextActivationAllowed() const;

        /**
         * Return the contextName
         *         
         *
         * @return The context name
         */
        const TInfoName &ContextName() const;
        
        /**
         * Completets sessions list change request back to client
         *
         * @param aResult Actual result of complete
         */
        void CompleteUpdateMbmsSessionList( 
            const TInt aResult );          

        /**
         * Return the context index
         *         
         *
         * @return The context index
         */
        TInt ContextIndex() const;        
        
        /**
         * First phase to retrieve active services
         *
         * @param aTsyReqHandle the ETEL request handle
         * @param aClient Etel's client ID
         * @param aBufSize Buffer size to client
         * @return error code
         */
        TInt GetMbmsActiveServicesPhase1L(
	        TTsyReqHandle aTsyReqHandle,
	        RMobilePhone::TClientId* aClient,
	        TInt* aBufSize );
            
        
        /**
         * Sets dynamic capability flags to this context
         * 
         * @param aFlag Flags to set
         * @param aIsActionPossible Is action possible
         */
        void SetDynamicCapsFlag(
            const RPacketService::TDynamicCapsFlags aFlag,  
            const TBool aIsActionPossible );   
            
        /**
         * Returns number of session in this context
         *
         * @return TInt Number of session in this context
         */            
        TInt SessionCount();
        

        /**
         * Returns type of context based on context name
         *
         * @param aInfoName Name of the context
         * @return TInt Type of context
         */
        virtual TPacketContextType ContextType() const;

#ifdef REQHANDLE_TIMER
        /**
         * Call the needed complete method due the timer expiration.
         *         
         *
         * @param aReqHandleType  
         * @param aError                      
         */
        void Complete
                (
                const TInt aReqHandleType,
                const TInt aError 
                );
#endif //REQHANDLE_TIMER

    private: // functions
      
        /** 
         * C++ default constructor.
         */
        CMmMBMSContextTsy();     
        

   
        /**
         * Class attributes are created in ConstructL.
         * 
         * 
         * @param aMmPacketService Pointer to the Packet Service object
         * @param aMessageManager Pointer to the Message Manager object
         * @param aName Context name
         * @param aProxyId Proxy id
         */
        void ConstructL(CMmPacketServiceTsy* const aMmPacketService,                
                const TDes& aName,  
                TUint8 aProxyId);


        /**
         * Initialise miscellaneous internal attributes.
         * 
         *                
         */
        void InitInternalAttributes();
        
        /**
         * TRAP's all CMmMBMSContextTsy related MM Packet API requests in 
         * case that they fail. This method functions only as a centralized
         * TRAP for the DoExtFuncL method that does the actual mapping of IPC 
         * number to TSY method call.
         *         
         *
         * @param aTsyReqHandle TSY request handle from ETel server
         * @param aIpc IPC number of the request
         * @param aPackage Reference to the input parameters.
         * @return KErrNone/KErrNotSupported
         */
        TInt DoExtFuncL( TTsyReqHandle aTsyReqHandle,
            TInt aIpc, const TDataPackage& aPackage );

        /**
         * Initialise a context
         *         
         * @param aDataChannel A pointer to a client object to return the data channel
         * @return TInt Success/failure value
         */
        TInt InitialiseContextL(RPacketContext::TDataChannelV2* aDataChannel );
        
        
        /**
         * Activate a context
         *         
         *
         * @return TInt Success/failure value
         */
        TInt ActivateL();

        /**
         * Deactivate a context
         *         
         *
         * @return TInt Success/failure value
         */
        TInt DeactivateL();

        /**
         * Deletes a context
         *         
         *
         * @return TInt Success/failure value
         */
        TInt DeleteL();
        
        /**
         * Updates session list of this context. Possible actions are;
         * add, remove or remove all items
         *
         * @param aAction Update action; add, remove or remove all
         * @param aSession The id of the session  
         * @return TInt Possible error value
         */
        TInt UpdateMbmsSessionList( TMbmsAction* aAction,
            TMbmsSessionId* aSession );
            
          
    
        /**
         * Get the context configuration info
         *         
         *
         * @param aTsyReqHandle TSY request handle
         * @return aConfig Contains Context configuration info on return
         * @return TInt Success/failure value
         */
        TInt GetConfig(TTsyReqHandle aTsyReqHandle,
            TPacketDataConfigBase* aConfig );

        /**
         * Get the last error cause occurred
         *         
         *
         * @param aTsyReqHandle TSY request handle
         * @return aError Contains last error value on return
         * @return TInt Success/failure value
         */
        TInt GetLastErrorCause( TTsyReqHandle aTsyReqHandle,
            TInt* aError );

        /**
         * Get the current status of the context
         *         
         *
         * @param aTsyReqHandle TSY request handle
         * @return aContextStatus Contains current status of the context
         * @return TInt KErrNone
         */
        TInt GetStatus( TTsyReqHandle aTsyReqHandle,
            RPacketContext::TContextStatus* aContextStatus );

        /**
         * Allows clients to be notified of any change in context 
         * configuration. Allows a client to be notified whenever a change in 
         * the configuration of the current context is detected and passes 
         * back the updated  configuration. Note that once the notification 
         * completes, the clients must then re-post the notification if they 
         * wish to continue receiving the notification.
         *         
         *
         * @return aConfig Contains context configuration info on return
         * @return TInt Success/failure value
         */
        TInt NotifyConfigChanged( TPacketDataConfigBase* aConfig );

        /**
         * Allows clients to be notified of any change in the context status
         *
         *         
         * @return aContextStatus Contains the status of the context on return
         * @return TInt Success/failure value
         */
        TInt NotifyStatusChange( RPacketContext::TContextStatus* aContextStatus );

        
        /**
         * Set context configuration
         *
         *         
         * @param aConfig Contains context configuration
         * @return TInt Success/failure value.
         */
        TInt SetConfigL( TPacketDataConfigBase* aConfig );

        /**
         * Overloads original ReqCompleted for logging purposes
         *
         *         
         * @param aTsyReqHandle TSY request handle
         * @param aError error value
         */
        virtual void ReqCompleted( TTsyReqHandle aTsyReqHandle,
            TInt aError );

        /**
         * Method invokes MS-initiated modification of an active context in the 
         * network
         *         
         *
         * @return TInt Success/failure value.
         */
        TInt ModifyActiveContextL();

#ifdef REQHANDLE_TIMER

        /**
         * Choose the type of response, automatic or common.
         * 
         *  
         * @param aReqHandleType
         * @param aTsyReqHandle                         
         */
        void SetTypeOfResponse( TInt aReqHandleType, 
                TTsyReqHandle aTsyReqHandle );

#endif //REQHANDLE_TIMER

    public: //Data
    
        // TPacketContextRequestTypes enumerates indexes to Packet's request
        // handle table. Request handles are stored there while waiting for 
        // completion of the request.
        enum TPacketContextRequestTypes
            {
            EMultimodePacketMbmsReqHandleUnknown,
            EMultimodePacketMbmsInitialiseContext,
            EMultimodeMbmsContextActivate,
            EMultimodeMbmsContextDeactivate,
            EMultimodeMbmsContextDelete,
            EMultimodeMbmsContextNotifyConfigChanged,
            EMultimodePacketContextNotifyConnectionSpeedChange,
            EMultimodeMbmsContextNotifyStatusChange,
            EMultimodePacketMbmsContextSetConfig,
            EMultimodePacketContextGetDataVolumeTransferred,
            EMultimodePacketContextGetPacketFilterInfo,
            EMultimodePacketContextEnumeratePacketFilters,
            EMultimodePacketContextAddPacketFilter,
            EMultimodePacketContextRemovePacketFilter,
            EMultimodePacketContextModifyActiveContext,
            EMultimodePacketContextNotifyConfigChanged99,
            EMultimodeGetMbmsSessionListPhase1,
            EMultimodeGetMbmsSessionListPhase2,
            EMultimodeMbmsContextUpdateMbmsSessionList,
            

            // Max number of requests
            // ADD NEW REQUESTS BEFORE THIS!
            EMaxNumOfMBMSContextRequests

            };

    private: // Data

   		/**
         * Context configuration data GPRS
         * Own.         
         */
        RPacketMbmsContext::TContextConfigMbmsV1* iConfig;

        /**
         * Req handle type
         */        
        TPacketContextRequestTypes iReqHandleType;
        
        /**
         *  TSY request handle
         */
        TTsyReqHandle iTsyReqHandle;           

        /**
         *  Pointer to the Req handle store
         *  Own.        
         */        
        CMmTsyReqHandleStore* iTsyReqHandleStore;
        
        /**
         *  Table for packet context request handles
         */        
        TTsyReqHandle iPacketContextReqHandles[EMaxNumOfMBMSContextRequests];
        
        /**
         *  Pointer to the Packet Service TSY
         *  Not own.        
         */
        CMmPacketServiceTsy* iMmPacketService;                                                
                                             
        /**
         *  Notify Config Changed
         *  Not own.        
         */
        TPacketDataConfigBase* iRetNotifyConfig;
        
        /**
         *  Notify Config Changed
         *  Not own.        
         */        
        TPacketDataConfigBase* iRetNotifyConfig2;

                                            
        /**
         *  Notify Context Status Change
         *  Not own.        
         */
        RPacketContext::TContextStatus* iRetNotifyStatus;                                               

        /**
         * Context status   
         */
        RPacketContext::TContextStatus iContextStatus;
                                               
        /**
         *  Context name      
         */
        TInfoName iContextName;    

        /**
         *  Initialisation state flag    
         */        
        TBool iIsInitialiseAllowed;                                            
        
        /**
         *  Activation state flag    
         */        
        TBool iIsActivateAllowed; 
        
        /**
         * Context status before deactivation
         */
        RPacketContext::TContextStatus iContextStatusBeforeDeactivation;
        
        /**
         * Unique object Id       
         */
        TUint8 iObjectId;                                                                

        /**
         * Context status before suspending       
         */
        RPacketContext::TContextStatus iContextStatusBeforeSuspending;        

        /**
         *  Last error cause      
         */
        TInt iLastErrorCause;       
        
        /**
         *  Array containing all services
         */
        RPacketMbmsContext::CMbmsSession* iServicesArray; 
	    
        
        /**
         * Temporary configuration structure
         */
        RPacketMbmsContext::TContextConfigMbmsV1* iTempConfig;
        
        
   		/**
         * Session list action to complete
         */      
        TMbmsAction iAction;
        
        /**
         * Item to add or remove to/from session list
         */
        TMbmsSessionId iSession;
        
        
        /**
         * Buffer to store active session info
         */
        HBufC8* iActiveSessionInfoBuffer;
        
        /**
         * The client id for GetMbmsActiveServices 
         */ 
        RMobilePhone::TClientId iClientId;
        
        /**
         * A pointer to the client object, to return the data channel after init
         */
        RPacketContext::TDataChannelV2 *iDataChannelV2;

    };

#endif // CMmMBMSContextTsy_H
   
// End of File
