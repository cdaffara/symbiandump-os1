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




/**
 @file
 @publishedPartner
 @released
*/
#ifndef CMmDataPackage_H
#define CMmDataPackage_H

#include <e32base.h>
#include <et_tsy.h>
#include <etelmm.h>
#include <etelpckt.h>
#include <etelqos.h>

/**
*  Class defining base data package 
*/
class CMmDataPackage : public CBase
    {
    public: //data

    /**  Data package type */
    enum TDataPackageType
        {
        /**Indicates that the package is basic Data type.*/
        KDataType,
        /**Indicates that the package is call specific.*/
        KCallType,
        /**Indicates that the package is phonebook specific.*/
        KPhoneBookType,
        /**Indicates that the package is packet specific.*/
#ifndef USING_CTSY_DISPATCHER 
        KPacketType
#else //USING_CTSY_DISPATCHER
        KPacketType,
         /**Indicates that the package is SAT specific.*/
        KSatType
#endif //USING_CTSY_DISPATCHER
        };

    public: // Constructors and destructor

        /**
         * C++ default constructor.
         */
        CMmDataPackage( void ){ iPtr1=NULL; iPtr2=NULL; iType= KDataType;} 

        /**
         * Set content of this data package
         *
         * @param aEtelDataPackage Etel data package
         */
        void SetPacketData( const TDataPackage *aEtelDataPackage  )
            {
            switch (aEtelDataPackage->Type())
                {
                case TDataPackage::EPackage1u2u:
                    iPtr1 = REINTERPRET_CAST( 
                        TAny*, aEtelDataPackage->Des1u() );
                    iPtr2 = REINTERPRET_CAST( 
                        TAny*, aEtelDataPackage->Des2u() );
                    break;
                case TDataPackage::EPackage1n2u:
                    iPtr1 = REINTERPRET_CAST( 
                        TAny*, aEtelDataPackage->Ptr1() );
                    iPtr2 = REINTERPRET_CAST( 
                        TAny*, aEtelDataPackage->Des2u() );
                    break;
                case TDataPackage::EPackage1u:
                    iPtr1 = REINTERPRET_CAST( 
                        TAny*, aEtelDataPackage->Des1u() );
                    iPtr2 = NULL;
                    break;
                case TDataPackage::EPackage1n:
                case TDataPackage::EPackage1n2n:
                case TDataPackage::EPackageUnknown:
                default:
                    iPtr1 = REINTERPRET_CAST( 
                        TAny*, aEtelDataPackage->Ptr1() );
                    iPtr2 = REINTERPRET_CAST( 
                        TAny*, aEtelDataPackage->Ptr2() );
                    break;
                }
            }

        /** Templated function to pack data for two parameters */
        template <typename T, typename S> inline void PackData( 
            T* aObject1, S* aObject2 )
            {
             iPtr1 = REINTERPRET_CAST( TAny*, aObject1 );
             iPtr2 = REINTERPRET_CAST( TAny*, aObject2 );
            };
        /** Templated function to pack data for one parameter */
        template <typename T> void PackData( T* aObject )
            {
             iPtr1 = REINTERPRET_CAST( TAny*, aObject );
             iPtr2 = NULL;
            };
            
        /** Templated function to unpack data for two parameters, when pointers used */
        template <typename T, typename S> inline void UnPackData( 
            T** aObject1, S** aObject2 ) const
            {
            *aObject1 = REINTERPRET_CAST( T*, this->iPtr1 ); 
            *aObject2 = REINTERPRET_CAST( S*, this->iPtr2 ); 
            };

        /** Templated function to unpack data for one parameter, when pointers used */
        template <typename T> inline void UnPackData( T** aObject1 ) const
            {
            *aObject1 = REINTERPRET_CAST( T*, this->iPtr1 );
            };
        
        /** Templated function to unpack data for two parameters, when references used */
        template <typename T, typename S> inline void UnPackData( 
            T& aObject1, S& aObject2 ) const
            {
            aObject1 = *( REINTERPRET_CAST( T*, this->iPtr1 ) ); 
            aObject2 = *( REINTERPRET_CAST( S*, this->iPtr2 ) ); 
            };

        /** Templated function to unpack data for one parameter, when references used */
        template <typename T> inline void UnPackData( T& aObject1 ) const
            {
            aObject1 = *( REINTERPRET_CAST( T*, this->iPtr1 ) );
            };
            
        /**
         * Returns type of this data package
         *
         * @return TDataPackageType Type of this package
         */
        TDataPackageType PackageType ( void )const { return iType; }

        /**
         * Destructor.
         */
        virtual ~CMmDataPackage( void ){};

    protected: // Data
        
        // Pointer to first piece of data
        TAny* iPtr1;
        
        // Pointer to second piece of data
        TAny* iPtr2;
        
        // Datapackage type
        TDataPackageType iType;
        
    };

/**
*  Class defining call specific data package.
*
*/
class CCallDataPackage : public CMmDataPackage
    {
        
    public: // Constructors and destructor
    
        /**
         * C++ default constructor.
         */    
        CCallDataPackage()
            { 
            //overwrite the iId to right value
            iType = KCallType;
            iCallId = -1;
            iCallMode = RMobilePhone::EServiceUnspecified;
            } 
            
        // New functions           
        
        /**
         * Set call identifier and call mode.
         *
         * @param aCallId Call identifier
         * @param aCallMode Call mode
         */    
        void SetCallIdAndMode( TInt aCallId, 
            RMobilePhone::TMobileService aCallMode )
            {
            iCallId = aCallId;
            iCallMode = aCallMode;
            }
        
        /**
         * Get call identifier and call mode.
         *
         * @param aCallId Call identifier
         * @param aCallMode Call mode
         */    
        void GetCallIdAndMode( TInt& aCallId, 
            RMobilePhone::TMobileService& aCallMode ) const
            {
            aCallId = iCallId;
            aCallMode = iCallMode;
            }

        /**
         * Destructor.
         */
        virtual ~CCallDataPackage( void ){};
    
    private: // Data
        
        // call ID
        TInt iCallId;
  
        // call mode
        RMobilePhone::TMobileService iCallMode;
        
    };

/**
*  Class defining PhoneBook specific data package
*
*/
class CPhoneBookDataPackage : public CMmDataPackage
    {
    public: // Constructors and destructor
    
        /**
         * C++ default constructor.
         */
        CPhoneBookDataPackage()
            { 
            iType = KPhoneBookType;
            _LIT( KStringZero, "0" );
            iPhoneBookName = ( KStringZero );
            } 
            
        // New functions 
                   
        /**
         * Set phonebook name.
         *
         * @param aPhoneBookName phonebook name
         */
        void SetPhoneBookName( const TName& aPhoneBookName )
            {
            iPhoneBookName = aPhoneBookName;
            }
        
        /**
         * Get call identifier and call mode.
         *
         * @param aPhoneBookName phonebook name
         */    
        void GetPhoneBookName( TName& aPhoneBookName ) const
            {
            aPhoneBookName = iPhoneBookName;
            }

        /**
         * Destructor.
         */
        virtual ~CPhoneBookDataPackage() {};

    private: // Data
        
        // name of the phone book
        TName iPhoneBookName;
        
    };

/**
*  Class defining Packet specific data package
*
*/
class CMmPacketDataPackage : public CMmDataPackage
    {
        
    public: // Constructors and destructor
    
        /**
         * C++ default constructor.
         */
        CMmPacketDataPackage()
            { 
            iType = KPacketType;
            iConfig = NULL;
            iConfigurationType = -1;
            } 
        
        /**
         * Set Configuration
         *
         * @param aConfig Pointer to context configurations for GPRS 
         * @param aConfigurationType Configuration type
         */ 
        void SetConfig( RPacketContext::TContextConfigGPRS* aConfig,
            TInt aConfigurationType )
            {
            iConfig = aConfig;
            iConfigurationType = aConfigurationType;
            }
        
        /**
         * Get Configuration
         *
         * @param aConfig Pointer to Context configurations for GPRS
         * @param aConfigurationType Configuration type
         */    
        void GetConfig( RPacketContext::TContextConfigGPRS* aConfig, 
            TInt& aConfigurationType ) const
            {
            if ( (iConfig != NULL) && (aConfig != NULL) )
            	{
            	*aConfig = *iConfig;
            	aConfigurationType = iConfigurationType;            	
            	}
            else
            	{
            	aConfigurationType = -1;
            	}
            }

        /**
         * Set profile changed
         *
         * @param aQoSGPRSNegotiated negotiated QoS values
         * @param aQoSR99_R4Negotiated negotiated values (Rel99 & Rel4)
         * @param aQoSR5_R4Negotiated negotiated values (Rel5)
         */     
        void SetProfileChanged( 
            RPacketQoS::TQoSGPRSNegotiated*  aQoSGPRSNegotiated, 
            RPacketQoS::TQoSR99_R4Negotiated* aQoSR99_R4Negotiated, 
            RPacketQoS::TQoSR5Negotiated* aQoSR5_R4Negotiated )
            {
            iQoSGPRSNegotiated = aQoSGPRSNegotiated;
            iQoSR99_R4Negotiated = aQoSR99_R4Negotiated;
            iQoSR5_Negotiated = aQoSR5_R4Negotiated;
            }   
       
        /**
         * Get profile changed
         *
         * @param aQoSGPRSNegotiated negotiated QoS values
         * @param aQoSR99_R4Negotiated negotiated values (Rel99 & Rel4)
         * @param aQoSR5_R4Negotiated negotiated values (Rel5)
         */    
        void GetProfileChanged( 
            RPacketQoS::TQoSGPRSNegotiated&  aQoSGPRSNegotiated, 
            RPacketQoS::TQoSR99_R4Negotiated& aQoSR99_R4Negotiated,
            RPacketQoS::TQoSR5Negotiated& aQoSR5_R4Negotiated  ) const
            {
            aQoSGPRSNegotiated = *iQoSGPRSNegotiated;
            aQoSR99_R4Negotiated = *iQoSR99_R4Negotiated;
            aQoSR5_R4Negotiated = *iQoSR5_Negotiated;
            }    
        
        /**
         * Destructor.
         */
        virtual ~CMmPacketDataPackage( void ){};

    private: // Data
        
        // Context configurations for GPRS
        RPacketContext::TContextConfigGPRS* iConfig;
        
        // configuration type
        TInt iConfigurationType;
        
        // negotiated QoS values
        RPacketQoS::TQoSGPRSNegotiated*  iQoSGPRSNegotiated;
        
        // negotiated values (Rel99 & Rel4)
        RPacketQoS::TQoSR99_R4Negotiated* iQoSR99_R4Negotiated;
        
        // negotiated values (Rel5)
        RPacketQoS::TQoSR5Negotiated* iQoSR5_Negotiated;
        
    };

#endif //CMmDataPackage_H

//  End of file
