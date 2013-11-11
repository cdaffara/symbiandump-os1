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



#ifndef CMMPACKETSERVICEGSMWCDMAEXT_H
#define CMMPACKETSERVICEGSMWCDMAEXT_H

// INCLUDES
#include "Cmmpacketservicetsy.h"

/**
*  CMmPacketServiceGsmWcdmaExt contains mode specific packet service
*  functionality for GSM/WCDMA Packet Data.
*/
NONSHARABLE_CLASS( CMmPacketServiceGsmWcdmaExt ) : public CBase
    {

    public: // Constructors and destructor

        /**
         * Two-phased constructor.
         *         
         *
		 * @param aMmPacketServiceTsy pointer to ServiceTsy
		 * @param aFactory pointer to Ltsy factory base
         */
        static CMmPacketServiceGsmWcdmaExt* NewL( 
			   CMmPacketServiceTsy* const aMmPacketServiceTsy,
			   MLtsyFactoryBase* aFactory );

        /**
         * Default destructor.
         *
         */        
        virtual ~CMmPacketServiceGsmWcdmaExt();

    public: // New functions

        /**
         * Attaches phone to the network.
         *         
         *
		* @return error value
         */
        virtual TInt AttachL();

        /**
         * Completes attach phone to the network request.
         *         
         *
		* @param aErrorValue error value for completion
         */
        virtual void CompleteAttach( TInt aErrorValue ) const;

        /**
         * Detaches phone to the network.
         *         
         *
		* @return error value
         */
        virtual TInt DetachL();

        /**
         * Completes detach phone to the network request.
         *         
         *
		* @param aErrorValue error value for completion
		* @return error value for completion
         */
        virtual void CompleteDetachL( TInt aErrorValue ) const;

        /**
         * Network registration status request to the network.
         *         
         *
		* @return error value for client 
         */
        virtual TInt GetNtwkRegStatusL();

        /**
         * Completes network registration status request.
         *         
        * @param aDataPackage
		* @param aResult result value for completion
         */
        void CompleteGetNtwkRegStatus( CMmDataPackage* aDataPackage, TInt aResult );

        /**
         * Sets Attach Mode to the DOS.
         *         
         *
		* @param aMode includes request related data
		* @return error value for client
         */
        virtual TInt SetAttachModeL( RPacketService::TAttachMode aMode );

        /**
         * Completes Set Attach Mode request.
         *         
         *
		* @param aErrorValue for completion
         */
        void CompleteSetAttachMode( const TInt aErrorValue ) const;

        /**
         * Gets Attach Mode from the DOS.
         *         
         *
		* @return error value for client
         */
        virtual TInt GetAttachModeL();

        /**
         * Completes Get Attach Mode request.
         *         
         *
		* @param aDataPackage contains request related data
         */
        void CompleteGetAttachMode( CMmDataPackage* aDataPackage,
        		                    TInt aResult) const;

        /**
         * Sets default parameters for a dial-up context.
         *         
         *
		* @param aPackage request related data
		* @return error value for client
         */
        TInt SetDefaultContextParamsL( const TDataPackage& aPackage );

        /**
         * Completes call SetDefaultContextParameters.
         *         
         *
		* @param aErrorCause error value
         */
        void CompleteSetDefaultContextParams( TInt aErrorCause );

        /**
         * Gets default parameters of dial-up context.
         *         
         *
		* @param aContextConfig pointer to context configuration data
		* @return error value for completion
         */
        TInt GetDefaultContextParams( TPacketDataConfigBase* aContextConfig );

        /**
         * Returns EGprs support in cell
         *         
         *
		* @return TBool value for GPRS support in current cell
         */
        virtual TBool EGprsSupportInCell();
		

#ifdef TF_LOGGING_ENABLED

        /**
         * Completes Set Attach Mode To Nvd memory request.
         *         
         *
         * @param aStatus not used
		*/
        void CompleteSetAttachModeToNvd( const TUint8 aStatus ) const;

#endif // TF_LOGGING_ENABLED

        /**
         * Sets Preferred Bearer to the DOS.
         *
		* @param aPackage includes request related data
         * @return error value for client 
		*/
        virtual TInt SetPreferredBearerL( const TDataPackage& aPackage );

        /**
         * Completes Set Preferred Bearer
         *         
         *
		* @param aErrorValue error value for completion
         */
        void CompleteSetPreferredBearer( TInt aErrorValue ) const;

        /**
         * Gets gsm/wcdma specific dynamic capabilities of the phone.
         *         
         *
		* @param aCaps pointer to dynamic capabilities
         */
        virtual void GetDynamicCaps( 
			RPacketService::TDynamicCapsFlags* const aCaps ) const;

        /**
         * Sets dynamic capabilities to the service tsy.
         *         
         *
		* @param aFlag flag to be changed
		* @param aIsActionPossible default value ETrue defines if
         *        Set is possible to do
         */
        virtual void SetDynamicCapsFlag(
            const RPacketService::TDynamicCapsFlags aFlag,
            const TBool aIsActionPossible );

        /**
         * Gets gsm/wcdma specific static capabilities of the phone.
         *         
         *
		* @param aCaps in which static caps is included
         */
        virtual void GetStaticCaps( TUint* const aCaps ) const;

		/**
         * Completes dynamic capabilities change to the service tsy.
         *         
         *
         * @param aPackage contains required data
         */
        void CompleteDynamicCapsChange( CMmDataPackage* aPackage );

		/**
		* Sets iLastEdgeSupport 
		* 		
        *
		* @param aLastEdgeSupport indicates if Edge is supported
		*/
		void SetEdgeGprsSupport( TBool aLastEdgeGprsSupport );

		/**
         * Completes transfer capabilities change to the service tsy.
         *         
	     * @param aPackage contains required data
         */
        void CompleteTransferCapsChange( CMmDataPackage* aPackage );
        
         /**
         * Completes get capabilities during to the service tsy startup.
         *         
	     * @param aPackage contains required data
         */
        void CompleteGetStaticCaps( CMmDataPackage* aPackage, TInt aError  );

            
    private: // New functions

        /**
         * C++ default constructor.
         */
        CMmPacketServiceGsmWcdmaExt();

        /**
         * Second phase constructor of CMmPacketServiceGsmWcdmaExt.
         *         
         * @param aFactory pointer to Ltsy factory base
         */
        void ConstructL( MLtsyFactoryBase* aFactory );

        /**
         * Initialise internal attributes.
         *         
         * @param aFactory pointer to Ltsy factory base
         */
        void InitInternalAttributesL( MLtsyFactoryBase* aFactory );


    private: // Data

        /**
         * Pointer to the packet service tsy
         * Not own.         
         */
        CMmPacketServiceTsy* iMmPacketServiceTsy;

        /**
         * Dynamic capabilities of gsm/wcdma extension
         */
        RPacketService::TDynamicCapsFlags iDynamicCaps;

        /**
         * Last status received from DOS
         */
        RPacketService::TStatus iLastTransferStatus;

        /**
         * Last info of circuit switch services availability from DOS
         */
        // 
		TBool iCsServicesIsSupportedOnCell;

        /**
         * Last info of gprs support in current cell from DOS
         */
		TBool iGprsIsSupportedOnCell;

        /**
         * Static capabilities of gsm/wcdma extension
         */
        TUint iStaticCaps;

        /**
         * SetContextDefaultParameter access point name
         */
        RPacketContext::TGSNAddress iAccessPointName;

        /**
         * SetContextDefaultParameter temporary access point name 
         */
        RPacketContext::TGSNAddress iTempAccessPointName;

        /**
         * Client side Pdp Type 
         */
        RPacketContext::TProtocolType iPdpType;

        /**
         * Client side temporary Pdp Type
         */
        RPacketContext::TProtocolType iTempPdpType;

        /**
         * SetContextDefaultParameter pdp compressions
         */
        TUint iPdpCompression;

        /**
         * SetContextDefaultParameter temporary pdp compressions
         */ 
        TUint iTempPdpCompression;

        /**
         * Is set default context params method called
         */
        TBool iIsSetDefaultContextParamsCalled;

        /**
         * Is edge gprs supported
         */
        TBool iLastEdgeGprsSupport;
        
    };

#endif // CMMPACKETSERVICEGSMWCDMAEXT_H

// End of File
