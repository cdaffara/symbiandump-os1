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



#ifndef CMMPACKETCONTEXTTSY_H
#define CMMPACKETCONTEXTTSY_H

//  INCLUDES
#include "Cmmpacketservicetsy.h"
#include "cmmpacketcontextgsmwcdmaext.h"
#include "Cmmpacketqostsy.h"

// FORWARD DECLARATIONS
class CMmTsyReqHandleStore;
class CMmPacketQoSTsy;
class CMmPacketServiceTsy;
class CMmPacketContextGsmWcdmaExt;

// CLASS DECLARATION

/**
*  CMmPacketContextTsy contains GPRS context related functionality. 
*/
NONSHARABLE_CLASS( CMmPacketContextTsy ) : public CSubSessionExtBase
    {
    public: // Constructor and destructor    

        /**
         * The type of the context
         */
    	enum TPacketContextType
            {
            /**
             * Packet Context
             */
            EContextTypePacketContext,
            /**
             * MBMS Context
             */
            EContextTypeMBMS
            };    

        /**
         * NewL method is used to create a new instance of CMmPacketContextTsy
         * class.
         * 
         *                  
         * @param aMmPacketService Pointer to the Packet Service object
         * @param aHostCID Name of existing context
         * @param aName Context name
         * @param aMessageManager Pointer to the Message Manager object
         * @param aProxyId Proxy id
         */
        static CMmPacketContextTsy* NewL
                (
                CMmPacketServiceTsy* const aMmPacketService,
                const TInfoName& aHostCID,
                const TDes& aName,
                const TUint8 aProxyId = 0x00
                );

        /**
         * Destructor
         */
        ~CMmPacketContextTsy();

        /**
         * Initialisation method that is called from ETel Server.
         *         
         *
         */
        virtual void Init();

        /**
         * TRAP's all CMmPacketContextTsy related MM API requests in case that
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
        virtual TInt ExtFunc( const TTsyReqHandle aTsyReqHandle, 
            const TInt aIpc, const TDataPackage& aPackage );    

        /**
         * Returns request mode for given IPC number
         *          
         *         *         
         * @param aIpc IPC number of the request
         * @return CTelObject::TReqMode Request mode
         */
        virtual CTelObject::TReqMode ReqModeL( const TInt aIpc );

        /**
         * Returns number of slots to be used for a given IPC
         *          
         *         *         
         * @param aIpc IPC number of the request
         * @return TInt Number of slots for this request
         */
        virtual TInt NumberOfSlotsL( const TInt aIpc );

        /**
         * Cancels request that's IPC number and request handle are given as
         * parameters
         *         
         *   
         * @param aIpc: IPC number of the request
         * @param aTsyReqHandle Request handle from ETel server
         * @return KErrNone/KErrNotSupported
         */
        virtual TInt CancelService( const TInt aIpc, 
            const TTsyReqHandle aTsyReqHandle );

        /**
         * Register given notification
         *         
         *
         * @param aIpc IPC number of the request
         * @return TInt Result of the request
         */
        virtual TInt RegisterNotification( const TInt aIpc );

        /**
         * Deregister given notification
         *         
         *
         * @param aIpc IPC number of the request
         * @return TInt Result of the request
         */
        virtual TInt DeregisterNotification( const TInt aIpc );
    
        /**
         * Create new QoS object and returns a pointer to it
         *         
         *
         * @return aName On return contains the name of opened object
         * @return CTelObject* Opened object 
         */
        virtual CTelObject* OpenNewObjectL( TDes& aNewName );

        /**
         * Return a pointer to QoS if it exists
         *         
         *
         * @param aName Object name that should be opened
         * @return CTelObject* Opened object 
         */
        virtual CTelObject* OpenNewObjectByNameL( const TDesC& aName );

        /**
         * Complete the context initialisation
         *         
         *
         * @param aResult Result of the request
         */
        void CompleteInitialiseContext( const TInt aResult );

        /**
         * Complete the context activation.
         *         
         *
         * @param aResult Result of the request
         */
        void CompleteActivate( const TInt aResult );

        /**
         * Complete the context deactivation
         *         
         *
         * @param aResult Result of the request
         */
        void CompleteDeactivate( const TInt aResult );
 
        /**
         * Complete the context delete
         *         
         *
         * @param aResult Result of the request
         */
        void CompleteDelete( const TInt aResult );

        /**
         * Complete the config changed notification
         *         
         *
         * @param aResult Result of the request
         */
        void CompleteNotifyConfigChanged();
        
        /**
         * Complete the connection speed change notification
         *         
         *
         * @param aConnectionSpeed Contains connections speed
         */
        virtual void CompleteNotifyConnectionSpeedChange(
            TInt const aConnectionSpeed );
        
        /**
         * Complete the status change notification
         *         
         *
         * @param aContextStatus Contains status of the context
         */
        void CompleteNotifyStatusChange(
            const RPacketContext::TContextStatus aContextStatus );
        
        /**
         * Complete the context configuration
         *         
         *
         * @param aResult Result of the request
         * @param aIsAddMediaAuthorizationCalled indicator to check if
         *        Response is for AddMediaAuthorizationL
         */
        void CompleteSetConfig( const TInt aResult,
                        TBool aIsAddMediaAuthorizationCalled = EFalse );

        /**
         * Complete the get data volume transferred request.
         *         
         *
         * @param aResult Result of the request
         */
        void CompleteGetDataVolumeTransferred( const TInt aResult );

        /**
         * Sets the amount of data transmitted & received over the
         * airlink since the context was activated. 
         *         
         *
         * @param aResult Contains context configuration
         * @param dataVolume Contains transmitted & received data
         */
        virtual void SetDataVolume( const TInt aResult,
            RPacketContext::TDataVolume dataVolume );

        /**
         * Set the last error cause.
         *         
         *
         * @param aErrorCause Contains the last error cause
         */
        void SetLastErrorCause( const TInt aErrorCause );

        /**
         * Get the context status.
         *         
         *
         * return RPacketContext::TContextStatus
         */
        RPacketContext::TContextStatus ContextStatus() const;

        /**
         * Suspend the context. Called from Packet Service Tsy to notify client
         *         
         *
         */
        void ContextSuspended();

        /**
         * Resume the suspended context. This function is called from 
         * Packet Service Tsy to notify client
         *         
         *
         */
        void ContextResumed();

        /**
         * Return a pointer to CMmPacketContextGsmWcdmaExt
         *         
         *
         * @return Pointer to extension
         */
        virtual CMmPacketContextGsmWcdmaExt* PacketContextGsmWcdmaExt() const;

        /**
         * Return a pointer to CMmPacketQoSTsy
         *         
         *
         * @return CMmPacketQoSTsy* Pointer 
         */
        CMmPacketQoSTsy* PacketQoSTsy() const;

        /**
         * Remove a pointer to CMmPacketQoSTsy
         *         
         *
         */
        void RemoveQoS();

        /**
         * Check if this is a dial-up context, otherwise EFalse.
         *         
         *
         * @param aContextName Contains the name of this context
         * @return TBool ETrue/EFalse
         */
        TBool IsDialUpContext( const TInfoName* const aContextName = NULL
            ) const;

        /**
         * Return the HostCID class member
         *         
         *
         * @return TInfoName iHostCID containing host context name or none
         */
        TInfoName HostCID() const;

        /**
         * Completes contecxt Modification to client.
         *         
         *
         * @param aResult Result of the request
         */
        void CompleteModifyActiveContext( const TInt aResult );

        /**
         * Return the contextName
         *         
         *
         * @return TInfoName iContextName containing context name
         */
        TInfoName ContextName() const;

        /**
         * Reset the externally created dial-up context.
         *         
         *
         */
        void ResetDialUpContext();
        
        /** 
         * C++ default constructor.
         */
        CMmPacketContextTsy();
        

        /**
         * Returns type of context based on context name
         *
         * @param aInfoName Name of the context
         * @return TInt Type of context
         */
        virtual TPacketContextType ContextType() const;

#ifdef SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
        /**
         * Completes connection information change notification to the client and
         * updates new connection info to CTSY cache if information is changed.
         *         
         *
         * @param aDataPackage: new connection information
         */
        void CompleteNotifyConnectionInfoChange(
            TConnectionInfoBase* const aInfo );    
#endif // SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW

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

    private: // New functions
      

    
        /**
         * Class attributes are created in ConstructL.
         */
        void ConstructL();

        /**
         * Initialises extension modules for CMmPacketContextTsy.
         * 
         *                
         */
        void InitModulesL();

        /**
         * Initialise miscellaneous internal attributes.
         * 
         *                
         */
        void InitInternalAttributes();
        
        /**
         * TRAP's all CMmPacketContextTsy related MM Packet API requests in 
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
        TInt DoExtFuncL( const TTsyReqHandle aTsyReqHandle,
            const TInt aIpc, const TDataPackage& aPackage );

        /**
         * Initialise a context
         *
         * @param aDataChannel to be populated and returned to client  
         * @return TInt Success/failure value
         */
        TInt InitialiseContextL(RPacketContext::TDataChannelV2* aDataChannel);
        
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
         * Get the context configuration info
         *         
         *
         * @param aTsyReqHandle TSY request handle
         * @return aConfig Contains Context configuration info on return
         * @return TInt Success/failure value
         */
        TInt GetConfig( const TTsyReqHandle aTsyReqHandle,
            TPacketDataConfigBase* const aConfig );

        /**
         * Get the current connection speed availability
         *         
         *
         * @param aTsyReqHandle TSY request handle
         * @return aRate Contains connection speed on return
         * @return TInt Success/failure value
         */
        TInt GetConnectionSpeed( const TTsyReqHandle aTsyReqHandle,
            TUint* const aRate );

        /**
         * Get the data volume transferred
         *         
         *
         * @param aTsyReqHandle TSY request handle
         * @return aVolume Contains data volume transferred on return
         * @return TInt Success/failure value
         */
        TInt GetDataVolumeTransferredL( const TTsyReqHandle aTsyReqHandle,
            RPacketContext::TDataVolume* const aVolume );

        /**
         * Get the last error cause occurred
         *         
         *
         * @param aTsyReqHandle TSY request handle
         * @return aError Contains last error value on return
         * @return TInt Success/failure value
         */
        TInt GetLastErrorCause( const TTsyReqHandle aTsyReqHandle,
            TInt* const aError );

        /**
         * Get the QoS profile name
         *         
         *
         * @param aTsyReqHandle TSY request handle
         * @return aQoSProfile Contains QoS profile name on return
         * @return TInt Success/failure value
         */
        TInt GetProfileName( const TTsyReqHandle aTsyReqHandle,
            TInfoName* const aQoSProfile );

        /**
         * Get the current status of the context
         *         
         *
         * @param aTsyReqHandle TSY request handle
         * @return aContextStatus Contains current status of the context
         * @return TInt KErrNone
         */
        TInt GetStatus( const TTsyReqHandle aTsyReqHandle,
            RPacketContext::TContextStatus* const aContextStatus );

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
        TInt NotifyConfigChanged( TPacketDataConfigBase* const aConfig );

        /**
         * Allows clients to be notified of any change in the connection speed
         *
         *         
         * @return aRate Contains the connection speed on return
         * @return TInt Success/failure value
         */
        TInt NotifyConnectionSpeedChange( TUint* const aRate );

        /**
         * Allows clients to be notified of any change in the context status
         *
         *         
         * @return aContextStatus Contains the status of the context on return
         * @return TInt Success/failure value
         */
        TInt NotifyStatusChange( RPacketContext::TContextStatus* const
            aContextStatus );

        
        /**
         * Set context configuration
         *
         *         
         * @param aConfig Contains context configuration
         * @return TInt Success/failure value.
         */
        TInt SetConfigL( TPacketDataConfigBase* const aConfig );

        /**
         * Overloads original ReqCompleted for logging purposes
         *
         *         
         * @param aTsyReqHandle TSY request handle
         * @param aError error value
         */
        virtual void ReqCompleted( const TTsyReqHandle aTsyReqHandle,
            const TInt aError );
        
        /**
         * Set/Modify packet filter(s) to secondary context
         *
         *         
         * @param aTsyReqHandle TSY request handle
         * @param aPacketFilter Contains packet filter(s) info
         * @return TInt Success/failure value.
         */
        TInt AddPacketFilterL( const TTsyReqHandle aTsyReqHandle,
            TDes8* const aPacketFilter );
#ifdef USING_CTSY_DISPATCHER
        /**
         * Remove packet filter from context
         *
         *         
         * @param aID Contains packet filter ID to be removed
         * @return TInt Success/failure value.
         */
        TInt RemovePacketFilter( TInt* aID );
#else
        /**
         * Remove packet filter from context
         *
         *         
         * @param aTsyReqHandle TSY request handle
         * @param aID Contains packet filter ID to be removed
         * @return TInt Success/failure value.
         */
        TInt RemovePacketFilter( const TTsyReqHandle aTsyReqHandle, TInt* aID );
#endif
        /**
         * Method invokes MS-initiated modification of an active context in the 
         * network
         *         
         *
         * @return TInt Success/failure value.
         */
        TInt ModifyActiveContextL();


        /**
         * Enumerate context packet filters
         *         
         *
         * @param aTsyReqHandle TSY request handle
         * @param aCount Contains packet filter count in return
         * @return KErrNone
         */
        TInt EnumeratePacketFilters( const TTsyReqHandle aTsyReqHandle,
            TInt *aCount );

        /**
         * Gets filter info specified by index
         *         
         *
         * @param aTsyReqHandle TSY request handle
         * @param aIndex of the wanted filter index
         * @param aPacketFilterInfo Contains packet filter information
         * @return KErrNone
         */
        TInt GetPacketFilterInfo( const TTsyReqHandle aTsyReqHandle,
            TInt* aIndex, TPacketBase* aPacketFilterInfo );

        /**
         * Get the DNS info for Etel side request
         *         
         *
         * @param aTsyReqHandle TSY request handle
         * @param aDnsInfo Contains DNS information in return
         * @return KErrNone.
         */
        TInt GetDnsInfo( const TTsyReqHandle aTsyReqHandle,
            TDes8* const aDnsInfo );

          /**
         * Adds authorization params 
         *         
         *
         * @param aTsyReqHandle request handle
         * @param aMediaAuthorization params to be added
         * @return KErrNone or error code
         */     
        TInt AddMediaAuthorizationL( const TTsyReqHandle aTsyReqHandle,
        	TDes8* aMediaAuthorization );
        
        /**
         * Removes authorization params 
         *         
         *
         * @param aTsyReqHandle request handle
         * @param aMediaAuthorization params to be removed
         * @return KErrNone
         */  
        TInt RemoveMediaAuthorization( const TTsyReqHandle aTsyReqHandle,
         RPacketContext::TAuthorizationToken* const aAuthorizationToken );
            
#ifdef SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
        /**
         * Get current connection information. 
         *         
         *
         * @param aTsyReqHandle request handle
         * @param aInfo configuration info object that will be filled by TSY
         * @return KErrNone
         */  
        TInt GetConnectionInfo( const TTsyReqHandle aTsyReqHandle,
            TConnectionInfoBase* const aInfo );
          
        /**
         * Notify client when connection information changes.
         *         
         *
         * @param aInfo configuration info object that will be filled by TSY
         * @return KErrNone
         */              
        TInt NotifyConnectionInfoChange( TConnectionInfoBase* const aInfo );            
           
        /**
         * Fill connection info structure. 
         *         
         *
         * @param aInfo configuration info object that will be filled by TSY
         * @return KErrNone or KErrNotFound
         */              
        TInt FillConnectionInfo( TConnectionInfoBase* const aInfo );                              
#endif // SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW

#ifdef REQHANDLE_TIMER

        /**
         * Choose the type of response, automatic or common.
         * 
         *  
         * @param aReqHandleType
         * @param aTsyReqHandle                         
         */
        void SetTypeOfResponse( const TInt aReqHandleType, 
                const TTsyReqHandle aTsyReqHandle );

#endif //REQHANDLE_TIMER

    public: //Data
    
        // TPacketContextRequestTypes enumerates indexes to Packet's request
        // handle table. Request handles are stored there while waiting for 
        // completion of the request.
        enum TPacketContextRequestTypes
            {
            EMultimodePacketContextReqHandleUnknown,
            EMultimodePacketContextInitialiseContext,
            EMultimodePacketContextActivate,
            EMultimodePacketContextDeactivate,
            EMultimodePacketContextDelete,
            EMultimodePacketContextNotifyConfigChanged,
            EMultimodePacketContextNotifyConnectionSpeedChange,
            EMultimodePacketContextNotifyStatusChange,
            EMultimodePacketContextSetConfig,
            EMultimodePacketContextGetDataVolumeTransferred,
            EMultimodePacketContextGetPacketFilterInfo,
            EMultimodePacketContextEnumeratePacketFilters,
            EMultimodePacketContextAddPacketFilter,
            EMultimodePacketContextRemovePacketFilter,
            EMultimodePacketContextModifyActiveContext,
            EMultimodePacketContextNotifyConfigChanged99,
            EMultimodePacketContextAddMediaAuthorization,
#ifdef SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
            EMultimodePacketContextNotifyConnectionInfoChange,
#endif // SYMBIAN_ADAPTIVE_TCP_RECEIVE_WINDOW
            // Max number of requests
            // ADD NEW REQUESTS BEFORE THIS!
            EMaxNumOfPacketContextRequests

            };

    private: // Data

        /**
         * Req handle type
         */        
        TPacketContextRequestTypes iReqHandleType;

        /**
         *  Pointer to the Req handle store
         *  Own.        
         */        
        CMmTsyReqHandleStore* iTsyReqHandleStore;
        
        /**
         *  Table for packet context request handles
         */        
        TTsyReqHandle iPacketContextReqHandles[EMaxNumOfPacketContextRequests];
        
        /**
         *  Pointer to the Packet Service TSY
         *  Not own.        
         */
        CMmPacketServiceTsy* iMmPacketService;                                                
        
        /**
         *  Pointer to the gsm extension
         */        
        CMmPacketContextGsmWcdmaExt* iMmPacketContextGsmWcdmaExt;                                                

        /**
         *  Pointer to the QoS Profile
         *  Not own.        
         */
        CMmPacketQoSTsy* iQoSProfile;                                                

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
         *  Notify Connection Speed Change
         *  Not own.        
         */
        TUint* iRetNotifyConnectionSpeed;
        
        /**
         *  Notify Connection Speed
         */        
        TUint iNotifyConnectionSpeed;

        /**
         *  Connection Speed
         */
        TUint iConnectionSpeed;                                             

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
         *  Get Data Volume Transferred
         *  Not own.        
         */
        RPacketContext::TDataVolume* iRetDataVolume;                                                

        /**
         *  Data Volume Transferred
         */
        RPacketContext::TDataVolume iDataVolume;                                                

        /**
         *  QoS Profile name
         */

        TInfoName iQoSProfileName;                                                

        /**
         *  Context name      
         */

        TInfoName iContextName;                                                

        /**
         * Proxy Id       
         */

        TUint8 iProxyId;                                                

        /**
         * Context status before suspending       
         */

        RPacketContext::TContextStatus iContextStatusBeforeSuspending;        

        /**
         *  Last error cause      
         */

        TInt iLastErrorCause;       

        /**
         *  Is this Dial-Up Context    
         */
        TBool iIsDialUpContext;

        /**
         *  Name of existing context      
         */
        TInfoName iHostCID;
        
        /**
         *  Connection information received from LTSY.     
         */ 
        RPacketContext::TConnectionInfoV1 iConnectionInfo;
           
        /**
         *  Pointer to connection info held by client, to be populated when
         *  notification request is completed.
         *  Not own.        
         */            
        TConnectionInfoBase* iRetNotifyConnectionInfo;       
    };

#endif // CMMPACKETCONTEXTTSY_H
   
// End of File
