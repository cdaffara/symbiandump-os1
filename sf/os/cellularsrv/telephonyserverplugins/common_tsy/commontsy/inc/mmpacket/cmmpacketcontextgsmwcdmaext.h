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



#ifndef CMMPACKETCONTEXTGSMWCDMAEXT_H
#define CMMPACKETCONTEXTGSMWCDMAEXT_H

// INCLUDES
#if defined (_MSC_VER) && (_MSC_VER >= 1000)
#pragma once
#endif

#include <ctsy/rmmcustomapi.h>
#include <ctsy/pluginapi/cmmdatapackage.h>
#include "cmmpacketqosgsmwcdmaext.h"
#include "CMmPrivateUtility.h"

// CONSTANTS

// FORWARD DECLARATIONS
class CMmPacketServiceTsy;
class CMmPacketContextTsy;
class RMmCustomAPI;


// CLASS DECLARATION

/**
*  CMmPacketContextGsmWcdmaExt contains mode specific packet context
*  functionality for GSM/WCDMA Packet Data.
*/
NONSHARABLE_CLASS( CMmPacketContextGsmWcdmaExt ) : public CBase
    {
   
  	public: // Constructor and destructor

        /**
         * Two-phased constructor.
         * @param aMmPacketContextTsy Pointer to the created object
         * @param aMmPacketServiceTsy Pointer to Servicetsy
         * @return CMmCallGsmWcdmaExt* Pointer to the created object
         */
         static CMmPacketContextGsmWcdmaExt* NewL(
                CMmPacketContextTsy* const aMmPacketContextTsy,
                CMmPacketServiceTsy* const aMmPacketServiceTsy );

        /**
         * Destructor.
         */
        virtual ~CMmPacketContextGsmWcdmaExt();

        /**
         * Initialise a context
         *         
         * @param aDataChannel Pointer to packet context channelId information
         * @return error code
         */
        TInt InitialiseContextL(RPacketContext::TDataChannelV2* aDataChannel);

#ifdef USING_CTSY_DISPATCHER        
        /**
         * Complete initialise of a context
         *         
         * @param aChannelId ChannelId from the L-TSY 
 		 * @param error code
 		 */
        void CompleteInitialiseContext(const TDesC& aChannelId,  
                				       const TInt aResult );  
#else        
        /**
         * Complete initialise of a context
         *         
         * @param error code
         */        
        void CompleteInitialiseContext( const TInt aResult);
#endif // #ifdef USING_CTSY_DISPATCHER       

        /**
         * Activate a context
         *         
         *
         * @return error code
         */    
        TInt ActivateL();

        /**
         * Complete the activate request
         *         
         *
         * @param aDataPackage Packaged parameters
         * @param error code
         */   
        void CompleteActivate( CMmDataPackage* aDataPackage,
                const TInt aResult );

        /**
         * Complete activate phase2
         *         
         *
         * @param aDataPackage Packaged parameters
         * @param error code
         */   
        void CompleteActivatePhase2( CMmDataPackage* aDataPackage,
            const TInt aResult );

         /**
         * Deactivate a context
         *         
         *
         * @return error code
         */    
        TInt DeactivateL();

        /**
         * Complete deactivation of the context.
         *         
         *
         * @param error code
         */  
        void CompleteDeactivate( const TInt aResult );

        
        /**
         * Delete a context.
         *         
         *
         * @return error code
         */    
        TInt DeleteL();

        /**
         * Complete deletion of the context.
         *         
         *
         * @param error code
         */ 
        void CompleteDelete( const TInt aResult );

        /**
         * Get the context configuration info.
         *         
         *
         * @return aConfig Current context configuration
         * @return error code
         */    
        TInt GetConfig( TPacketDataConfigBase* const aConfig );

        /**
         * Notify when context configuration info has been changed
         *         
         *
         * @param aConfig Current configuration mode
         * @return KErrNotSupported
         */
        TInt NotifyConfigChanged( TPacketDataConfigBase& aConfig );

        /**
         * Configure a context
         *         
         *
         * @param aConfig Current context configuration data
         * @return error code
         */ 
        TInt SetConfigL( TPacketDataConfigBase* const aConfig );

        /**
         * Complete SetConfig
         *         
         *
         * @param error code
         */ 
        void CompleteSetConfig( const TInt aResult );

        /**
         * Get the data volume transferred
         *         
         *
         * @return error code
         */
        TInt GetDataVolumeTransferredL();

        /**
         * Complete the GetDataVolumeTransferred request.
         *         
         *
         * @param aDataPackage Packaged parameters
         * @param error code
         */   
        void CompleteGetDataVolumeTransferred( 
            const CMmDataPackage* aDataPackage, const TInt aResult );

        /**
         * Prepare to set or modify packet filter(s) to context
         *         
         *
         * @param aPacketFilter Current context TFT filter data
         * @return error code
         */ 
        TInt AddPacketFilterL( TDes8* aPacketFilter );

        /**
         * Prepare to remove packet filter(s) from context.
         *         
         *
         * @param aId ID of the TFT filter to be removed
         * @return error code
         */ 
        TInt RemovePacketFilter( const TInt aId );

        /**
         * Add, remove or modify packetfilter(s) and qos to active context.
         *         
         *
         * @return error code
         */ 
        TInt ModifyActiveContextL();

        /**
         * Complete active context modify operation
         *         
         *
         * @param error code
         * @param aDataPackage Packaged parameters
         */ 
        void CompleteModifyActiveContext( 
        	CMmDataPackage* aDataPackage, const TInt aResult );

        /**
         * Count packet filters from context.
         *         
         *
         * @return aCount Count of the Packet Filters
         * @return error code
         */
        TInt EnumeratePacketFilters( TInt *aCount );

        /**
         * Gets packet filter info by index
         *         
         *
         * @param aIndex Index of the wanted packet filter
         * @return aPacketFilterInfo Inforamtion of the TFT filter specified by
         * index.
         * @return error code
         */
        TInt GetPacketFilterInfo( const TInt aIndex,
                TPacketBase* aPacketFilterInfo );

        /**
         * Return pointer to context configuration GPRS
         *         
         *
         * @return aConfig Current context configuration
         */    
        RPacketContext::TContextConfigGPRS* ContextConfig() const;

        /**
         * This method get the DNS info
         *         
         *
         * @param aDnsInfo Contains DNS information in return
         * @return KErrNone.
         */
        TInt GetDnsInfoEtel( TDes8* const aDnsInfo );

        /**
         * Complete the config changed notification
         *         
         *
         * @return None
         */
        void CompleteNotifyConfigChanged();

        /**
         * Complete the config changed notification
         *         
         *
         * @param aDataPackage Packaged parameters
         * @param error code
         */       
        void CompleteNotifyStatusChange( const CMmDataPackage*
            aDataPackage, const TInt aResult );

        /**
         * Sets iContextStatusBeforeActivation parameter
         *         
         *
         * @param aContextStatusBeforeDeactivation Contains status of the 
         * current context
         */
        void SetContextStatusBeforeDeactivation( 
        	const RPacketContext::TContextStatus
            aContextStatusBeforeDeactivation );

        /**
         * Tells if context activation is allowed
         *         
         *
         * @return TBool ETrue/EFalse
         */ 
        TBool IsContextActivationAllowed() const;

        /**
         * Get the context configuration data. This function is called from QoS
         * when the client has called the SetProfileParameters
         *
         * @return aContextName Name of the this context
         * @return aPdpType Pdp type of the context
         * @return aContextType Type of the Context
         * @return aPrimaryContextName Name of the primary context of this 
         * context
         * @return error code
         */ 
        TInt GetConfigurationData( TInfoName& aContextName,
            RPacketContext::TProtocolType& aPdpType, TBool& aContextType,
            TInfoName& aPrimaryContextName );


		/*
		* Adds media authorization params
		* 		
		*
		* @param aMediaAuthorization pointer to media authorization class
		* to be added
		* @return error code
		*/
		TInt AddMediaAuthorizationL( RPacketContext::CTFTMediaAuthorizationV3* 
							const aMediaAuthorization, TBool &aHasMediaAuthorizationBeenCached);
		
		/*
		* Removes media authorization params
		* 		
		*
		* @param aAuthorizationToken token to be removed
		* @return error code
		*/	
		TInt RemoveMediaAuthorization( RPacketContext::TAuthorizationToken* 
						const aAuthorizationToken );

        /**
         * Return context type
         *         
         *
         * @return TBool Context Type
         */ 
        TBool ContextType() const;

    private:

        /**
         * C++ default constructor.
         */
        CMmPacketContextGsmWcdmaExt();

        /**
         * Second phase constructor of CMmPacketContextGsmWcdmaExt.
         */
        void ConstructL();

        /**
         * Initialise miscellaneous internal attributes.
         *          
         *
         */
        void InitInternalAttributesL();

        
        /**
         * This is the second phase of context activation.
         *          
         *
         */
        void ActivatePhase2L();

    public: // Data
    
   		/**
         * packet context packet fílter operation states
         */       
	    enum TAddPacketFilterStatusStates
	        {
	        EAddPacketFilterStatusAdd,
	        EAddPacketFilterStatusModify,
	        EAddPacketFilterStatusRemove,
	        EAddPacketFilterStatusUnknown
	        };

    private: // Data

   		/**
         * Context configuration data GPRS
         * Own.         
         */
        RPacketContext::TContextConfigGPRS* iConfig;

   		/**
         * Pointer to the packet context tsy
         * Not own.         
         */
        CMmPacketContextTsy* iMmPacketContextTsy;

   		/**
         * Pointer to the Packet Service TSY
         * Not own.         
         */
        CMmPacketServiceTsy* iMmPacketServiceTsy;      
        
   		/**
         * Temporary context configuration data to GPRS
         */
        RPacketContext::TContextConfigGPRS* iTempConfig;

   		/**
         * Configuration data for NotifyConfigChanged to GPRS
         */        
        RPacketContext::TContextConfigGPRS iNotifyConfig;

   		/**
         * Configuration data for NotifyConfigChanged to R99_R4
         */        
        RPacketContext::TContextConfigR99_R4 iNotifyConfigR99_R4;

   		/**
         * Connection speed
         */
        TUint iConnectionSpeed;

   		/**
         * Context activation allowed
         */
        TBool iIsActivateAllowed;

   		/**
         * Context initialization allowed
         */
        TBool iIsInitialiseAllowed;
        
   		/**
         * Context status before deactivation
         */
        RPacketContext::TContextStatus iContextStatusBeforeDeactivation;

   		/**
         * Container to context packet filters
         * Own.         
         */    
        CArrayFixFlat<RPacketContext::TPacketFilterV2>* iFilterArray;

   		/**
         * Temporary container to context packet filter(s)
         * Own.         
         */    
        CArrayFixFlat<RPacketContext::TPacketFilterV2>* iFilterTemporaryArray;

   		/**
         * Temporary container to remove packet filter Id's
         */    
        TInt iFilterIdTemporaryArray[ KMaxNumberOfTftFilters ];

   		/**
         * Filter operation
         */    
        TAddPacketFilterStatusStates iFilterStatus;

   		/**
         * Previous filter operation
         */    
        TAddPacketFilterStatusStates iPrevFilterStatus;

   		/**
         * Includes context configuration information
         */    
        TInt iContextConfigurationType;

   		/**
         * Includes Qos, configuration and packet filter data
         */            
        TContextConfiguration iContextConfiguration;

   		/**
         * Temporary container for authorization class Rel5
         * Own.         
         */                    
        CArrayFixFlat<RPacketContext::CTFTMediaAuthorizationV3>*
        											iTempMediaAuthorizationArray;
   		/**
         * Indicator
         */    
        TBool isAddMediaAuthorizationCalledFirstTime;
        
        /** 
         *  DataChannel for a packet context
         */        

        RPacketContext::TDataChannelV2* iDataChannel; 
    
};

#endif // CMMPACKETCONTEXTGSMWCDMAEXT_H

// End of File
