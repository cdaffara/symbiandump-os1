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



#ifndef CMMPACKETQOSGSMWCDMAEXT_H
#define CMMPACKETQOSGSMWCDMAEXT_H

// INCLUDES
#include "Cmmpacketqostsy.h"
#include "cmmpacketcontextgsmwcdmaext.h"

// FORWARD DECLARATIONS
class CMmPacketQoSTsy;
class CMmPacketServiceTsy;

// CLASS DECLARATION

/**
*  CMmPacketQoSGsmWcdmaExt contains mode specific QoS 
*  functionality for GSM/WCDMA Packet Data.
*/
NONSHARABLE_CLASS( CMmPacketQoSGsmWcdmaExt ) : public CBase
    {
    public: // Constructors and destructor

        /**
         * Two-phased constructor. NewL method is used to create a new 
         * instance of CMmPacketQoSGsmWcdmaExt class.
         *  
         *                 
		 * @param aMmPacketQoSTsy pointer to PacketQosTsy
		 * @param aMmPacketService pointer to PacketServiceTsy
         */
        static CMmPacketQoSGsmWcdmaExt* NewL
            (
            CMmPacketQoSTsy* aMmPacketQoSTsy,
            CMmPacketServiceTsy* aMmPacketService
            );
        
        /**
         * Destructor.
         */
        ~CMmPacketQoSGsmWcdmaExt();

        /**
         * Set Profile Parameters
         *         
         *
		 * @param aProfile profile clinet side QoS data
		 * @return error value
         */
        TInt SetProfileParametersL( TDes8* aProfile );

        /**
         * Get Profile Parameters
         *          
         *
		 * @param aProfileCaps Profile capabilities
		 * @return error value
         */
        TInt GetProfileParameters( TDes8* aProfileCaps );

        /**
         * Notify Profile Changed
         *          
         *
		 * @param aProfile Profile capabilities
		 * @return error value
         */
        TInt NotifyProfileChanged( TDes8* aProfile );

        /**
         * Cancel Notify Profile Changed
         *          
         *
         */
        void NotifyProfileChangedCancel();


        /**
         * Complete Set Profile Parameters
         *          
         *
		 * @param aErrorValue error value for completion
         */
        void CompleteSetProfileParameters( const TInt aErrorValue );

        /**
         * Get Profile Capabilities
         *          
         *         
		 * @param aProfileCaps Profile capabilities
		 * @return error value
         */
        TInt GetProfileCapabilities( TDes8* aProfileCaps );

		/*
         * Saves negotiated Qos values and completes notification to client
         *          
         *
         * @param aDataPackage contains initial data
		 */
		void CompleteNotifyProfileChanged( CMmDataPackage* aDataPackage );
        /**
		 * QoS is asked
		 * 		 
         *
		 * @return true if client has succesfully set up Qos values 
         *         else false
		 */
        TBool ProfileAsked();

        /**
		 * Gets Qos profile
		 * 		 
         *
		 * @return Requested 99 QoS parameters in return
		 */
        RPacketQoS::TQoSGPRSRequested* Requested97();

        /**
		 * Gets Qos profile
		 * 		 
         *
		 * @return Requested 97/98 QoS parameters in return
		 */
        RPacketQoS::TQoSR99_R4Requested* Requested99();
    
		/**
		 * Gets Qos profile
		 * 		 
         *
		 * @return Requested Rel5 QoS parameters in return
		 */
		RPacketQoS::TQoSR5Requested* RequestedR5();
        /**
         * Gets configuration type
         *          
         *
         */
        TInt ConfigurationType() const;

    private:

        /**
         * C++ default constructor.
         */
        CMmPacketQoSGsmWcdmaExt();

        /**
         * Second phase constructor of CMmPacketQoSGsmWcdmaExt.
         */
        void ConstructL();


           
    private: // Data


        /**
         * A pointer to the Packet Service Tsy
         */
        CMmPacketServiceTsy* iMmPacketService;

        /**
         * Pointer to the Packet QoS Tsy
         */
        CMmPacketQoSTsy* iMmPacketQoSTsy;
        
        /**
         * Negotieated QoSGPRS profile
        */        
        RPacketQoS::TQoSGPRSNegotiated iQoSGPRSNegotiated;
        
        /**
         * Negotieated QoS 99 parameters
        */            
        RPacketQoS::TQoSR99_R4Negotiated iQoSR99_R4Negotiated;
        
        /**
         * Negotiated QoS rel5 parameters
        */             
        RPacketQoS::TQoSR5Negotiated iQoSR5Negotiated;

        /**
         * Pointer to the notify pckg
         * Not own.         
        */      
        TDes8* iQoSGPRSNotifyPckg;

        /**
         * for saving client request
        */            
		TBool iQoSGPRSRequested;

        /**
         * for saving client request      
        */            
		TBool iQoSR99_R4Requested;	
		
        /**
         * for saving client request      
        */        
		TBool iQoSR5Requested;			

        /**
         * Tells if QoS is asked.      
        */        
        TBool iQosAsked;

        /**
         * Struct containing reguested Qos R5 params      
        */        
        RPacketQoS::TQoSR5Requested iRequestedR5;
        
        /**
         * Struct containing reguested Qos R99 params      
        */     
        RPacketQoS::TQoSR99_R4Requested iRequested99; 

        /**
         * Struct containing reguested Qos 97/98 params      
        */                    
        RPacketQoS::TQoSGPRSRequested iRequested;

        /**
         * Configuraton type      
        */   
        TInt iConfigurationType;

    };

#endif //CMMPACKETQOSGSMWCDMAEXT_H 

// End of File
