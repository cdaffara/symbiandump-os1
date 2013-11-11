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
#ifndef MLTSYFACTORY_H
#define MLTSYFACTORY_H

//  INCLUDES
#include <e32base.h>
#include <ctsy/mltsyfactorybase.h>

// FORWARD DECLARATIONS

// CLASS DECLARATION
class MMessageRouter;
class MmMessageManagerCallback;
class MCtsySatService;
class CTsySatMessagingBase;
class MTelephonyAudioControl;
class MTelephonyAudioControlCallback;

// CONSTANTS
// Constant used to recieve packet service static caps.
const TUint32 KStaticCapsPacketService = 1;

/**
*  Version 1 of LicenseeTsy factory interface. 
*/
class MLtsyFactoryV1 : public MLtsyFactoryBase
    {
    public:
    
        /**
         * For getting messagerouter from LicenseeTsy.         
         *
         *
         * @param aMessageManager reference to MessageManager
         * @return pointer to LTSY messagerouter object
         */
        virtual MMessageRouter* GetMessageRouter( 
            MmMessageManagerCallback &aMessageManager ) = 0;
 
#ifdef USING_CTSY_DISPATCHER
        /**
         * For getting SIM ATK TSY messaging service from LicenseeTsy.           
         *
         *
         * @param aSatMessagingService reference to CTSY side SAT services
         * @return pointer to SIM ATK TSY service object
         */
        virtual CTsySatMessagingBase* GetSatMessagingService( 
            MCtsySatService &aSatMessagingService ) = 0;       
#else
        /**
         * For getting SIM ATK TSY messaging service from LicenseeTsy.           
         *
         *
         * @return pointer to SIM ATK TSY service object
         */
        virtual CTsySatMessagingBase* GetSatMessagingService( ) = 0;  
#endif
    };

/**
*  Version 2 of LicenseeTsy factory interface. 
*/
class MLtsyFactoryV2 : public MLtsyFactoryV1
    {
    public:
    
        /**
         * For getting telephony audio control.
         *
         * @param aTelephonyAudioControlCallback reference to callback
         * inteface to inform errors in TelephonyAudioControl
         * @return pointer to telephony audio control service object
         */
        virtual MTelephonyAudioControl* GetTelephonyAudioControl( 
	        MTelephonyAudioControlCallback* aTelephonyAudioControlCallback ) = 0;        
    };

/**
*  Version 3 of LicenseeTsy factory interface. 
*/
class MLtsyFactoryV3 : public MLtsyFactoryV2
    {
    public:
        
        /**
         * For getting static capabilities from LicenseeTsy.           
         *
         * @param aRequestedCapabilitySet defines which capabilities are requested from
         * LTSY, E.g. packet service caps, which are currently supported. This can be extended
         * with other caps.  
         * @return requested static capabilities, e.g packet service caps.
         */
    	virtual TUint32 GetLTSYStaticCapabilities( TUint32 aRequestedCapabilitySet ) = 0;
    };
#endif // MLTSYFACTORY_H

//  End of file
