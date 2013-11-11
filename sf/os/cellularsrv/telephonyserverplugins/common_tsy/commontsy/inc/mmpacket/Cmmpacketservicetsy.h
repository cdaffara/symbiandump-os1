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



#ifndef CMMPACKETSERVICETSY_H
#define CMMPACKETSERVICETSY_H

// INCLUDES
#include "cmmphonetsy.h"
#include "Cmmpacketcontexttsy.h"
#include "Cmmpacketqostsy.h"
#include <ctsy/serviceapi/gsmerror.h>
#include <ctsy/rmmcustomapi.h>
#include "cmmmessagemanagerbase.h"
#include <pcktlist.h>

// CONSTANTS

// String literals
_LIT( KStringExternal,  "External"  );
_LIT( KStringExternal2, "External2" );

// DUMMY_NIF flag should be off in builds and releases.
// DUMMY_NIF flag should be off by default in builds.
// DUMMY_NIF flag should be on if want test DUMMY_NIF_PEP_FOR_PACKET_DATA.
//#define DUMMY_NIF_PEP_FOR_PACKET_DATA_TESTING

// FORWARD DECLARATIONS
class CMmMessageManagerBase;
class CMmPacketContextList;
class CMmPacketContextTsy;
class CMmMBMSContextList;
class CMmMBMSMonitoredList;
class CMmPacketContextGsmWcdmaExt;
class CMmPacketServiceGsmWcdmaExt;
class CMmPacketQosTsy;

// CLASS DECLARATION    

/**
*  CMmPacketServiceTsy contains GPRS service related functionality.
*/
NONSHARABLE_CLASS( CMmPacketServiceTsy ) : public CSubSessionExtBase
    {
    public: // Data
    
        
        enum TPacketServiceRequestTypes
            {
            EMultimodePacketServiceReqHandleUnknown,                     //0
            EMultimodePacketServiceAttach,                               //1
            EMultimodePacketServiceDetach,                               //2
            EMultimodePacketServiceGetNtwkRegStatus,                     //3
            EMultimodePacketServiceNotifyNtwkRegStatusChange,            //4
            EMultimodePacketServiceNotifyContextActivationRequested,     //5
            EMultimodePacketServiceNotifyContextAdded,                   //6
            EMultimodePacketServiceNotifyDynamicCapsChange,              //7
            EMultimodePacketServiceNotifyMSClassChange,                  //8
            EMultimodePacketServiceNotifyStatusChange,                   //9
            EMultimodePacketServiceSetPreferredBearer,                   //10
            EMultimodePacketServiceSetAttachMode,                        //11
            EMultimodePacketServiceRejectActivationRequest,              //12
            EMultimodePacketServiceGetAttachMode,                        //13
            EMultimodePacketServiceSetDefaultContextParams,              //14
            EMultimodePacketServiceGetDefaultContextParams,              //15
            EMultimodePacketServiceNotifyContextActivationRequested99,   //16
            EMultimodePacketSetAlwaysOn,                                 //17
            EMultimodePacketServiceNotifyMBMSStatusChange,	             //18				 										
            EMultimodePacketServiceGetMBMSStatus,                        //19
			EMultimodePacketServiceNotifyMbmsServiceAvailabilityChange,  //20
			EMultimodePacketServiceEnumerateMbmsActiveServices,          //21
			EMultimodePacketServiceUpdateMBMSMonitorServiceList,         //22
            EMultimodePacketServiceNotifyContextActivationRequested5,    //23

            // Add new requests here. ENumOfMultimodePacketServiceRequests shows
            // maximum number of requests in TPacketServiceRequestTypes
            ENumOfMultimodePacketServiceRequests                         //24
            };

    private: // Data
        
        /**
         * Internal saved notify related data pointers in Packet Service
         * @param iChangeOfNtwkRegStatus pointer to RPacketService::TRegistrationStatus
         * @param iDynamicCapsChange pointer to RPacketService::TDynamicCapsFlags
         * @param iMSClassChange pointer to RPacketService::TMSClass
         * @param iStatusChange pointer to RPacketService::TStatus
         * @param iContextActivationRequested pointer to RPacketContext::TContextConfigGPRS
         * @param iContextActivationRequested99 pointer to RPacketContext::TContextConfigR99_R4
         * @param iContextActivationRequested5 pointer to RPacketContext::TContextConfig_R5
         * @param iContextAdded pointer 
         */
        struct TMmNotifyDataPointers
            {
            RPacketService::TRegistrationStatus*  iChangeOfNtwkRegStatus;
            RPacketService::TDynamicCapsFlags*    iDynamicCapsChange;
            RPacketService::TMSClass*             iMSClassChange;
            RPacketService::TStatus*              iStatusChange;
            RPacketContext::TContextConfigGPRS*   iContextActivationRequested;
            RPacketContext::TContextConfigR99_R4* iContextActivationRequested99;
            RPacketContext::TContextConfig_R5*    iContextActivationRequested5;
            TDes*                                 iContextAdded;
            /**
             * A client pointer to the MbmsNetworkServiceStatus for notify any status change
             */
			TMbmsNetworkServiceStatus*            iMBMSStatusChange;
			
			TMmNotifyDataPointers()
				{
				iChangeOfNtwkRegStatus = NULL;
				iDynamicCapsChange = NULL;
				iMSClassChange = NULL;
				iStatusChange = NULL;
				iContextActivationRequested = NULL;
				iContextActivationRequested99 = NULL;
				iContextAdded = NULL;
				iMBMSStatusChange = NULL;
				}
            };

        /**
         * Internal saved data in Packet Service
         * @param iMsClass  contains MsClass type
         * @param iPreferredBearer contains current PreferredBearer
         * @param iServiceStatus contains service status
         * @param iServiceStatusBeforeSuspend contains current status before 
         * suspend         
         * @param iRegistrationStatus contains registration status
         */
        struct TMmPacketServiceInternalSavedData
            {
            RPacketService::TMSClass iMsClass;
            RPacketService::TPreferredBearer iPreferredBearer;
            RPacketService::TStatus iServiceStatus;
            RPacketService::TStatus iServiceStatusBeforeSuspend;
            RPacketService::TRegistrationStatus iRegistrationStatus;
            /**
            * contain current MBMS network status
            */    
            TMbmsNetworkServiceStatus iMbmsStatus;
            };

        /**
         * Temporary saved data in Packet Service
         * @param iGetAttachMode  pointer to RPacketService::TAttachMode
         * @param iPreferredBearer preferred bearer
         */
        struct TMmPacketServiceTemporarySavedData
            {
            RPacketService::TAttachMode* iGetAttachMode;
            RPacketService::TPreferredBearer iPreferredBearer;
            /**
             * Pointer to the MBMS Status
             */
            TMbmsNetworkServiceStatus* iMbmsStatus;
            };
         
     public: // Constructors and destructor
    
        /**
         * Two-phased constructor.
         * @param aMmPhone pointer to PhoneTsy
         * @param aMessageManager pointer to Messagemanager
         * @param aFactory pointer to Ltsy factory base
         */        
        static CMmPacketServiceTsy* NewL( CMmPhoneTsy* const aMmPhone,
            CMmMessageManagerBase* const aMessageManager,
            MLtsyFactoryBase* aFactory );

        /**
         * Default destructor.
         */        
        virtual ~CMmPacketServiceTsy();

    public: // New functions


        /**
         * Returns MBMS context list 
         *
         * @return CMmMBMSContextList MBMS context list object
         */   
        virtual CMmMBMSContextList* MBMSContextList();
        
        /**
         * Request to update monitored service list. 
         *
         * @param aAction Action to make changes (add/remove or remove all)
         * @param aList A descriptor that holds the list of monitored services to update, or NULL for an empty list
         * @return TInt Error value
         */
        TInt UpdateMbmsMonitorServiceListL( TMbmsAction aAction,   
            TDes8* aList );
        
        /**
         * Completes update monitored list request
         *
         * @param aResult the result of the Update
         * @param aDataPackage a packed list of all the monitor entries that failed to update
         */
        virtual void CompleteUpdateMbmsMonitorServiceList( CMmDataPackage* aDataPackage, 
            const TInt aResult );
        
        /**
         * Enumrates monitored service list
         *
         * @param aCount Actual number of monitored services is list
         * @param aMax Maximum number of services in list
         * @return error code
         */
        TInt EnumerateMonitorServiceList( TInt& aCount, TInt& aMaxAllowed );
        
        
        /**
         * Enumerates all active services 
         */
        TInt EnumerateMbmsActiveServices();     

        /**
         * Returns a pointer to CMmCustomTsy object.
         *          
         *
         * @return CMmCustomTsy*
         */
        virtual CMmCustomTsy* CustomTsy() const;

        /**
         * Returns a pointer to the active packet service extension.
         *          
         *
         * @return CMmPacketServiceGsmWcdmaExt*
         */
        virtual CMmPacketServiceGsmWcdmaExt* ActivePacketServiceExtension();

        /**
         * Returns a pointer to the tsy request handle store.
         *          
         *
         * @return CMmTsyReqHandleStore*
         */
        virtual CMmTsyReqHandleStore* TsyReqHandleStore();

        /**
         * Returns a pointer to the packet context list.
         *          
         *
         * @return CMmPacketContextList*
         */
        virtual CMmPacketContextList* PacketContextList();

        /**
         * Returns current status of Packet Service.
         *          
         *
         * @return RPacketService::TStatus
         */
        virtual RPacketService::TStatus ServiceStatus() const;

        /**
         * Context status has changed from the Service point of view.
         *          
         *
         * @param aContextStatus
         */
        void ContextStatusChanged(
            const RPacketContext::TContextStatus aContextStatus );

        /**
         * Starts asynchronous attach request to the network.
         *          
         *
         * @return Errorvalue
         */
        TInt AttachL();

        /**
         * Completes asynchronous attach request.
         *          
         *
         * @param aError error value for completion
         */
        virtual void CompleteAttach( const TInt aError );

        /**
         * Starts asynchronous detach request to the network.
         *          
         *
         * @return error value
         */
        TInt DetachL();

        /**
         * Completes asynchronous detach request.
         *          
         *
         * @param aError, errovalue for completion
         */
        virtual void CompleteDetachL( const TInt aError );

        /**
         * Enumerates contexts and completes.
         *          
         *
         * @param aCount, number of created contexts
         * @param aMaxAllowed, maximum number of contexts
         */
        TInt EnumerateContexts( TInt* const aCount,TInt* const aMaxAllowed );

        /**
         * Returns current mode of the GPRS Attach operation.
         *          
         *
         * @param aMode attach mode
         * @return current mode of the GPRS
         */
        TInt GetAttachModeL( RPacketService::TAttachMode* const aMode );

        /**
         * Completes Get GPRS Attach Info operation.
         *          
         *
         * @param aAttachMode attach mode
         */
        virtual void CompleteGetAttachMode(const RPacketService::TAttachMode aAttachMode,
        		                           TInt aResult);

        /**
         * Gets Context info of the context defined by aIndex.
         *          
         *
         * @param aIndex, index to the context
         * @param aInfo, context info
         * @return error value
         */
        TInt GetContextInfo( const TInt* const aIndex,
            RPacketService::TContextInfo* const aInfo );

        /**
         * Gets the dynamic capabilities of the phone.
         *          
         *
         * @param aCaps, dynamic capabilities
         * @return error value
         */
        TInt GetDynamicCaps( RPacketService::TDynamicCapsFlags* const aCaps );

        /**
         * Gets current and maximum value of the Mobile Station Class.
         *          
         *
         * @param aCurrentClass, currently used ms class
         * @param aMaxClass, maximum possible ms class
         * @return error value
         */
        TInt GetMSClass( RPacketService::TMSClass* const aCurrentClass, 
            RPacketService::TMSClass* const aMaxClass );

        /**
         * Gets network registration status.
         *          
         *
         * @param aRegistrationStatus, status
         * @return error value
         */
        TInt GetNtwkRegStatusL( 
            RPacketService::TRegistrationStatus* const aRegistrationStatus );

        /**
         * Completes get network registration status request.
         *          
         *
         * @param aRegistrationStatus, registration status
         * @param aError, error code default as KErrNone
         */
        virtual void CompleteGetNtwkRegStatus(
            const RPacketService::TRegistrationStatus aRegistrationStatus,
            const TInt aError );

        /**
         * Returns preferred bearer.
         *          
         *
         * @param aBearer, preferred bearer
         * @return error value
         */
        TInt GetPreferredBearer(
            RPacketService::TPreferredBearer* const aBearer );

        /**
         * Retrieves those capabilities of the TSY that are constant.
         *          
         *
         * @param aCaps, static capabilities
         * @param aPdpType, current Pdp type
         * @return error value
         */
        TInt GetStaticCaps( TUint* const aCaps,
            const RPacketContext::TProtocolType* const aPdpType );

        /**
         * Returns current status of the packet service.
         *          
         *
         * @param aPacketStatus,packet status
         * @return error value
         */
        TInt GetStatus( RPacketService::TStatus* const aPacketStatus );

        /**
         * Requests notification of network registration status change.
         *          
         *
         * @param aRegistrationStatus, registrationstatus
         * @return errorvalue
         */
        TInt NotifyChangeOfNtwkRegStatus( 
            RPacketService::TRegistrationStatus* const aRegistrationStatus );

        /**
         * Requests notification of network requests context activation.
         *          
         *
         * @param aContextParameters, context parameters
         * @return error value
         */
        TInt NotifyContextActivationRequested(
            TPacketDataConfigBase* const aContextParameters );

        /**
         * Requests notification of context has been added.
         *          
         *
         * @param aPackage, contains parameters for request
         * @return error value
         */
        TInt NotifyContextAdded( const TDataPackage& aPackage );

        /**
         * Requests notification of dynamic capabilities change.
         *          
         *
         * @param aCaps, dynamic capabilities
         * @return error value
         */
        TInt NotifyDynamicCapsChange
            (
            RPacketService::TDynamicCapsFlags* const aCaps
            );

        /**
         * Requests notification of mobile station class change.
         *          
         *
         * @param aNewClass, new mobile station class
         * @return error value
         */
        TInt NotifyMSClassChange( RPacketService::TMSClass* const aNewClass );

        /**
         * Informs client that service status change.
         *          
         *
         * @param aPacketStatus, packet status
         * @return error value
         */
        TInt NotifyStatusChange( 
            RPacketService::TStatus* const aPacketStatus );

        /**
         * Reject activation request from the network.
         *          
         *
         * @return error value
         */
        TInt RejectActivationRequestL();

        /**
         * Completes asynchronous reject activation request.
         *          
         *
         */
        virtual void CompleteRejectActivationRequest();

        /*
         * Sets attach mode given in parameter to the server.
         *          
         *
         * @param aPackage, contains parameters for request
         * @return error value
         */
        TInt SetAttachModeL( const RPacketService::TAttachMode* aMode );

        /**
         * Completes the Set Attach Mode request.
         *          
         *
         * @param aError, error value for completion
         */
        virtual void CompleteSetAttachMode( const TInt aError );

        /**
         * Sets preferred bearer given in parameter to the server.
         *          
         *
         * @param aPackage, contains parameters for request
         * @return error value
         */
        TInt SetPreferredBearerL( const TDataPackage& aPackage );

        /**
         * Completes the Set Preferred Bearer request.
         *          
         *
         * @param aError, error value for completion
         */
        virtual void CompleteSetPreferredBearer( const TInt aError );

        /** 
         * ExtFunc is called by the ETel when it has request for the TSY.
         *          
         *
         * @param aTsyReqHandle, request handle
         * @param aIpc, ipc number of request
         * @param aPackage, data package
         * @return error value
         */
        virtual TInt ExtFunc( const TTsyReqHandle aTsyReqHandle, 
            const TInt aIpc, const TDataPackage& aPackage );    

        /**
         * Returns request mode for given IPC.
         *          
         *
         * @param aIpc, ipc number of request
         * @return request mode
         */
        virtual CTelObject::TReqMode ReqModeL( const TInt aIpc );

        /**
         * Returns number of slots to be used for given IPC.
         *          
         *
         * @param aIpc, ipc number of request
         * @return number of slots used for given IPC 
         */
        virtual TInt NumberOfSlotsL( const TInt aIpc );

        /**
         * Cancels request defined by aIpc.
         *          
         *
         * @param aIpc, ipc number of request 
         * @param aTsyReqHandle, request handle of given request
         * @return error value
         */
        virtual TInt CancelService( const TInt aIpc, 
            const TTsyReqHandle aTsyReqHandle );

        /**
         * This function subscribes given notification from DOS.
         *          
         *
         * @param aIpc, ipc number of request
         * @return error value
         */
        virtual TInt RegisterNotification( const TInt aIpc );

        /**
         * This function unsubscribes given notification from DOS.
         *          
         *
         * @param aIpc, ipc number of request
         */
        virtual TInt DeregisterNotification( const TInt aIpc );
    
        /**
         * Creates new Context object and returns a pointer to it.
         *          
         *
         * @param aNewName, a new name of the Context object to be created
         * @return pointer to created context
         */
        virtual CTelObject* OpenNewObjectL( TDes& aNewName );

        /**
         * Returns context object defined in parameter aName.
         *          
         *
         * @param aName, a name of the object to be created
         * @return pointer to context object 
         */
        virtual CTelObject* OpenNewObjectByNameL( const TDesC& aName );

        /**
         * Next created Context will be secondary, gives primary context name
         *          
         *
         * @param aPrimaryContextName, Name of the host context
         * @return error value
         */
        TInt PrepareOpenSecondary( TDes16* aPrimaryContextName );

        /**
         * Initialisation method that is called from ETel Server.
         *          
         *
         */
        virtual void Init();

        /**
         * Completes network registration status notification to client.
         *          
         *
         * @param aRegistrationStatus, reg status
         */
        virtual void CompleteNotifyChangeOfNtwkRegStatus(
            const RPacketService::TRegistrationStatus aRegistrationStatus );

        /**
         * Completes context activation requested notification to client.
         *          
         *
         * @param aDataPackage
         */
        virtual void CompleteNotifyContextActivationRequested(
            CMmDataPackage* aDataPackage );

        /**
         * Completes the context added notification to client.
         *          
         *
         * @param aPackage contains parameters for request
         */
        virtual void CompleteNotifyContextAdded( CMmDataPackage* aPackage  );   
           

        /**
         * Completes dynamic capabilities change notification to client.
         *          
         *
         * @param aCaps, dynamic capabilities
         */
        virtual void CompleteNotifyDynamicCapsChange(
            const RPacketService::TDynamicCapsFlags aCaps );
            
         /**
         * Completes max value of monitored services change.
         *          
         *
         * @param aMaxMonitorValue, maximum value of monitored services
         */
        virtual void CompleteMaxMonitoredServicesChange( TInt aMaxMonitorValue );

        /**
         * Completes mobile station class change notification to client.
         *          
         *
         * @param aDataPackage contains data used for this notification
         */
        virtual void CompleteNotifyMSClassChange( 
            CMmDataPackage* aDataPackage );

        /**
         * Completes service status change notification to client.
         *          
         *
         * @param aPacketStatus, packet service status
         * @param aErrorCode contains error value if packet status changed because of error
         */
        virtual void CompleteNotifyStatusChange(
            const RPacketService::TStatus& aPacketStatus,
            TInt aErrorCode );

        /*
         * Completes packet status change.
         *          
         *
         * @param aDataPackage        
         * @param aErrorCode contains error value if packet status changed because of error
         */
        virtual void CompletePacketStatusChanged( 
            CMmDataPackage* aDataPackage,
            TInt aErrorCode );

        /**
         * Completes preferred bearer change.
         *          
         *
         * @param aPreferredBearer preferredbearer
         * @param aPackage includes request related data
         */
        virtual void PreferredBearerChanged(
            CMmDataPackage* aPackage );

        /**
         * Returns externally created dial-up context.
         *          
         *
         * @param aContextName     
         */
        virtual CMmPacketContextTsy* DialUpContext(
            const TInfoName aContextName );

        /**
         * Resets pointer to dial-up context.
         *          
         *
         * @param aContextName         
         */
        void ResetPointerToDialUpContext( const TInfoName aContextName );
        
        /**
         * Enumerate network interfaces.
         *          
         *
         * @param aCount, pointer to number of nifs
         * @return error value
         */
        TInt EnumerateNifs( TInt* aCount );

        /**
         * Get network interface information.
         *
         *          
         * @param aIndex, index of wanted nif
         * @param aNifInfoV2, pointer to nif information
         * @return error value 
         */
        TInt GetNifInfo( TInt* aIndex, 
            RPacketService::TNifInfoV2* aNifInfoV2 );

        /**
         * Number of contexts in network interface.
         *          
         *
         * @param aExistingContextName, name of Nif
         * @param aCount, pointer to number of contexts
         * @return error value
         */
        TInt EnumerateContextsInNif( TDesC* aExistingContextName, 
            TInt* aCount );

        /**
         * Get name of context in network interface.
         *          
         *
         * @param aContextNameInNif, name of Nif and index of context
         * @param aContextName, wanted context name
         * @return error value
         */
        TInt GetContextNameInNif(
            RPacketService::TContextNameInNif* aContextNameInNif,
            TDes* aContextName );

        /**
         * Set default parameters to context.
         *          
         *
         * @param aPackage,contains parameters for request
         * @return error value 
         */
        TInt SetDefaultContextParamsL( const TDataPackage& aPackage );

        /**
         * Completes SetDefaultContextConfiguration request.
         *          
         *
         * @param aCause, Error cause
         */
        virtual void CompleteSetDefaultContextParams( TInt aCause );

        /**
         * Get context default parameters.
         *          
         *
         * @param aContextConfig, Configuration data
         * @return error value
         */
        TInt GetDefaultContextParams( TPacketDataConfigBase* aContextConfig );

        /**
         * Is activation of context allowed
         *          
         *
         */
        TInt IsActivationAllowed();

        /**
         * Returns pointer to CMmMessagemanagerBase.
         *          
         *
         * @return pointer to CMmMessagemanagerBase
         */
        virtual CMmMessageManagerBase* MessageManager();


        /**
         * Routes completion to customtsy
         *          
         *
         * @param aDataPackage
         */
        void CompleteNotifyEGprsInfoChange( CMmDataPackage* aDataPackage );

        /**
         * Completes network registration status notification to client.
         * Handles request coming from MessHandler
         *          
         *
         * @param aDataPackage includes request related data
         * @param aResult The result of the notification from the LTSY
         */
        void CompleteNotifyChangeOfNtwkRegStatus( 
            CMmDataPackage* aDataPackage, TInt aResult );

        /**
         * Returns pointer to right Qos
         *          
         *
         * @param aMmPacketContextName
         * @return pointer to right Qos object 
         */
        CMmPacketQoSTsy* QosTsy( TInfoName aMmPacketContextName );

        /**
         * Set current network to Service TSY.
         * 
         *                  
         * @param CDataPackage         
         */
        virtual void CompleteNetworkModeChange( CMmDataPackage* CDataPackage );

        /**
         * Returns context by name
         *          
         *
         * @param contextName, context Name
         * @return pointer to context object
         */
        CMmPacketContextTsy* ContextTsy( TInfoName& contextName  );
        
        virtual RHandleBase* GlobalKernelObjectHandle();
        
        /** 
         * Request Set Always On
         *
         *
         * @param aTsyReqHandle TSY ReqHandle
         * @param aMode requested mode
         * @return error value
         */
        TInt SetAlwaysOnL( TTsyReqHandle aTsyReqHandle, 
            RMmCustomAPI::TSetAlwaysOnMode aMode );
        
        /** 
         * Completes SetAlwaysOn request
         * 
         *
         * @param aError error value from LTSY 
         */
        void CompleteSetAlwaysOn( TInt aError );
        
        /** 
         * Cancels Set Always On
         *
         *
         * @param aTsyReqHandle TSY reghandle
         */
        void CancelSetAlwaysOn( 
            TTsyReqHandle aTsyReqHandle );
		 
        /**
         * Returns current status of MBMS Network Service.
         *          
         * @param aAttemptAttach a boolean to state whether the mbms should attemt to attach the service
         * @param aMBMSServiceStatus pointer to a client object to return the  MBMS network service status
         * @return KErrNone if success, system wide error code otherwise
         */
		TInt GetMbmsNetworkServiceStatus(TBool aAttemptAttach, TMbmsNetworkServiceStatus* aMBMSServiceStatus );
		
		 /**
         * Completes get MBMS network service status request.
         *          
         *
         * @param aDataPackage, service status
         * @param aError, error code default as KErrNone
         */
		void CompleteGetMbmsNetworkServiceStatus( CMmDataPackage* aDataPackage, TInt aError );
				
		 /**
         * Requests notification of network service status change.
         *          
         *
         * @param aMBMSServiceStatus
         * @return error value
         */
		TInt NotifyMbmsNetworkServiceStatusChange(TMbmsNetworkServiceStatus* aMBMSServiceStatus );
		
		 /**
         * Completes Network service status change requested notification to client.
         *          
         *
         * @param aDataPackage, contains parameters for request
         */
		void CompleteNotifyMbmsNetworkServiceStatusChange( CMmDataPackage* aDataPackage, TInt aResult );
		
		 /**
         * Requests notification of mbms service availability list change.
         *          
         *
         */
		TInt NotifyMbmsServiceAvailabilityChange();

		 /**
         * Completes service available status change requested notification to client.
         *          
         *
         * @param aDataPackage a packed list of all the available services, or NULL for empty list
         * @param aResult The result of the notification from the LTSY
         */
		void CompleteNotifyMbmsServiceAvailabilityChangeL(CMmDataPackage* aDataPackage, TInt aResult);
				
		 /**
         * Returns the number of Mbms active service list and the maximum possible number of active service list
         *          
         *
         * @param aCount The number of Mbms active service list [out]
         * @param aMaxAllowed The maximum number of active service lists [out]
         * @return error code
         */
		TInt EnumerateMbmsActiveServiceList(TInt &aCount, TInt &aMaxAllowed);
		
		 /**
         * Completes Enumrate active service list
		 *
         */
		void CompleteEnumerateMbmsActiveServiceList(CMmDataPackage* aDataPackage, TInt aResult);
		
		 /**
         * Requests size of the monitored service list.
         *          
         *
         * @param aClient a pointer to the client ID
         * @param aBufSize The return buffer size
         * @return error code
         */
		TInt GetMbmsMonitoredServicesPhase1L(RMobilePhone::TClientId* aClient,  TUint &aBufSize);	
 		 
 		 /**
         * Get size of monitored service list  
         *
         * @param aClient, client id  
         * @param aBufSize, TUint size of the buffer 
         * @return error code
         */ 
 		 TInt GetSizeOfMonitoredServiceListL(RMobilePhone::TClientId* aClient,  TUint& aBufSize);
             
		 /**
         * Requests the monitored service list.
         *          
         *
         * @param aClient, client id  
         * @param aBuf, client bufer to copy data to
         * @return error code
         */
		TInt GetMbmsMonitoredServicesPhase2(RMobilePhone::TClientId* aClient, TDes8* aBuf);	
					
	    /**
         * Return maximum number of active services allowed
         *
         * @return TInt Max number of active services
         */
        TInt MaximumActiveServices();

#ifdef REQHANDLE_TIMER
        /**
         * Returns a pointer to CMmPhoneTsy object.
         *          
         *
         */
        CMmPhoneTsy* PhoneTsy();

        /**
         * Called from MmPhoneTsy when timer expire.
         *          
         *
         * @param aObject
         * @param aReqHandleType
         * @param aError                      
         */
        virtual void Complete( const CTelObject* const aObject,
            const TInt aReqHandleType, const TInt aError );
            
#endif // REQHANDLE_TIMER


#ifdef TF_LOGGING_ENABLED       
        /**
         * Overloads original ReqCompleted for logging purposes.
         * 
         * 
         * @param aTsyReqHandle
         * @param aError                                
         */
        virtual void ReqCompleted( const TTsyReqHandle aTsyReqHandle,
            const TInt aError );
                                                           
#endif // TF_LOGGING_ENABLED
        
		/**
         * Resets an entry in the req handle store 
         * 
         * 
         * @param aTsyReqHandle req handle 
         * return EFalse if request handle not found
         */       
        virtual TBool ResetReqHandle( const TTsyReqHandle aTsyReqHandle );
        
    private: // New functions

        /** 
         * Default constructor.
         */
        CMmPacketServiceTsy();
    
        /**
         * Initialises object attributes.
         *
         * @param aFactory pointer to Ltsy factory base
         */
        void ConstructL( MLtsyFactoryBase* aFactory );

        /**
         * Initialises extension modules for CMmPacketServiceTsy.
         * 
         * @param aFactory pointer to Ltsy factory base
         */
        void InitModulesL( MLtsyFactoryBase* aFactory );

        /**
         * Initialises needed stores and lists.
         * 
         *                  
         */
        void InitStoresAndListsL();

        /**
         * Checks if context status is active from service point of view.
         * 
         *                  
         * @param aContextStatus, context status
         * @return Boolean True/False                  
         */
        TBool IsContextStatusActive(
            const RPacketContext::TContextStatus aContextStatus) const;

        /**
         * Sets and completes attached packet status if needed.
         * 
         *                  
         * @param aIsActiveContext, state of context
         */
        void UpdateAttachedPacketStatus( const TBool aIsActiveContext );

        /**
         * Calls correct method to handle ETel server request.
         * 
         *                  
         * @param aIpc
         * @param aPackage
         * @return error value
         */
        TInt DoExtFuncL( const TInt aIpc, const TDataPackage& aPackage );
        
        /**
         * Cancel the Get Mbms Monitored Services
         * 
         * @param aTsyReqHandle the TSY request handle
         * @return error code
         */
        TInt GetMbmsMonitoredServicesCancel( TTsyReqHandle aTsyReqHandle );

        /**
         * Cancel the Update Mbms Monitor Service List
         * 
         * @param aTsyReqHandle the TSY request handle
         * @return error code
         */
        TInt UpdateMbmsMonitorServiceListCancel(TTsyReqHandle aTsyReqHandle );
        

#ifdef REQHANDLE_TIMER
        /**
         * Chooses the type of response, automatic or common.
         * 
         *
         * @param aReqHandleType                           
         */
        void SetTypeOfResponse( const TInt aReqHandleType );

        /**
         * Calls the needed complete method due timer expiration.
         * 
         *  
         * @param aReqHandleType
         * @param aError                                 
         */
        void Complete( const TInt aReqHandleType, const TInt aError );
        
#endif // REQHANDLE_TIMER

    private: // Data
        
       
        /**
         * MBMS context object
         *
         */
        CMmMBMSContextList* iMBMSContextList;        
        
        /**
         * MBMS monitorted service list 
         *
         */
        CMmMBMSMonitoredList* iMBMSMonitoredList;
 
		       
        /**
         * Pointer to the packet context list
         * Own.         
         */
        CMmPacketContextList* iPacketContextList;

        /**
         * Pointer to the TRegistrationStatus enum defined in Packet Data API
         * Not Own.         
         */
        RPacketService::TRegistrationStatus* iRegistrationStatus;
              
        /**
         *  Packet Service Request Handle Type
         */
        TPacketServiceRequestTypes iReqHandleType;        
        
        /**
         *  TSY request handle
         */
        TTsyReqHandle iTsyReqHandle;        

        /**
         *  Pointer to the request handle store
         *  Own.         
         */
        CMmTsyReqHandleStore* iTsyReqHandleStore;        

        /**
         *  True if service is suspended, otherwise false
         */
        TBool iSuspended;    

        /**
         *  A table for packet service request handles
         */
        TTsyReqHandle iPacketServiceReqHandles[
                                         ENumOfMultimodePacketServiceRequests];
        /**
         *  A pointer to the Phone TSY
         *  Not own.         
         */
        CMmPhoneTsy* iMmPhone;        

        /**
         * A pointer to the Gsm/Wcdma spesific packet service extension
         * Own.         
         */
        CMmPacketServiceGsmWcdmaExt* iMmPacketServiceGsmWcdmaExt;

        /**
         * Structure contains notify related data pointers
         */
        TMmNotifyDataPointers iNotifyDataPointers;

        /**
         * Structure contains data that must be saved in Packet Service
         */
        TMmPacketServiceInternalSavedData iInternalSavedData;

        /**
         * Structure contains temporary saved data in Packet Service
         */
        TMmPacketServiceTemporarySavedData iTemporarySavedData;

        /**
         * A pointer to the externally created dial-up context
         * Own.         
         */
        CMmPacketContextTsy* iDialUpContext;

        /**
         * A pointer to the externally created dial-up context
         * Own.         
         */
        CMmPacketContextTsy* iSecondaryDialUpContext;

        /**
         * Primary Context Name         
         */
        TInfoName iHostCID;
        
        /**
         * Pointer to CMmMessageManagerBase
         * Not own.         
         */        
        CMmMessageManagerBase* iMessageManager;      

        /**
         * Current network mode
         */      
        RMobilePhone::TMobilePhoneNetworkMode iCurrentMode;        

        /**
         * Current attach mode 
         */      
        RPacketService::TAttachMode iAttachMode;
        
        /**
         * Unnamed mutex object owned by TSY
         */      
        RMutex iMutex;

        /**
         *  Boolean to indicate MBMS support 
         */
        TBool iMbmsSupported;        
 
         /**
         * Maximum number of monitored service lists 
         *
         */   
         TInt iMaxMonitoredServices; 
         
        /**
         * Maximum number of active services (= max number of contexts)
         */
        TInt iMaxActiveServices;
              
 		 /**
         * Action type of Update MBMS Monitored Service List  
         *
         */    
		TMbmsAction iActionType;
		
		 /**
         * indicator of MBMS network service status cache  
         *
         */  
		TBool iMbmsStatusCached;
	
		/**
         * Read all entries
         */  
        CArrayPtrFlat<CListReadAllAttempt>* iGetMbmsMonitoredServiceLists;	
		
         /**
         * indicator of MBMS monitorlist max value cache  
         *
         */  
		TBool iMbmsMonitorListMaxNumIsCached;
		
    };

#endif // CMMPACKETSERVICETSY_H

            
// End of File
