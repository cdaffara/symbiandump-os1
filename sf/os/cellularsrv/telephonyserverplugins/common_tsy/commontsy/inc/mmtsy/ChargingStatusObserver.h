/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef CHARGING_STATUS_OBSERVER
#define CHARGING_STATUS_OBSERVER

// INCLUDES
#include <e32property.h>
#include <e32base.h>
#include <hwrmpowerstatesdkpskeys.h>
#include <etelmm.h>

// CLASS DECLARATION

/**
* MChargingStatusNotifier
* Interface class for NotifyChargingStatus()
*/
class MChargingStatusNotifier
    {
    public:
    
        /**
        * Notifier for charging status changes
        *
        * @param aError error code
        * @param aBatteryInfo data structure of contents of the battery status
        */
        virtual void NotifyChargingStatus( TInt aErrorCode,
            RMobilePhone::TMobilePhoneBatteryInfoV1 aBatteryInfo ) = 0;
    };

// CLASS DECLARATION

/**
* CChargingStatusObserver is used to provide battery charging status from HWRM
* to CTSY
*/
NONSHARABLE_CLASS( CChargingStatusObserver ) : public CActive
	{
    public:
    
        /**
        * Two-phased constructor
        *
        * @param aChargingStatusNotifier reference to interface class
        * MChargingStatusNotifier
        * @return Pointer to the created object
        */
    	static CChargingStatusObserver* NewL( 
    	    MChargingStatusNotifier& aChargingStatusNotifier );

    	/**
        * Two-phased constructor
        *
        * @param aChargingStatusNotifier reference to interface class
        * MChargingStatusNotifier
        * @return Pointer to the created object
        */
    	static CChargingStatusObserver* NewLC( 
    	    MChargingStatusNotifier& aChargingStatusNotifier );
    	
    	/**
        * Destructor
        */
    	~CChargingStatusObserver();
    	
    	/**
    	* Handles GetChargingStatus request completion event
    	*/
    	void RunL();
    	
    	/**
    	* Cancellation of an outstanding request
    	*/
    	void DoCancel();
    		
    	/**
    	* Get charging status
    	*/
    	void GetChargingStatus();

    private:
        
        /**
        * By default Symbian 2nd phase constructor is private
        */
       	void ConstructL();
       	
       	/**
        * Constructor
        *
        * @param aChargingStatusNotifier reference to interface class
        * MChargingStatusNotifier
        */
    	CChargingStatusObserver( 
    	    MChargingStatusNotifier& aChargingStatusNotifier );
    	
    private:
    
        /**
        * Property for charging status
        */
        RProperty iChargingStatus;
        
        /**
        * Reference to ChargingStatusNotifier object
        */
        MChargingStatusNotifier& iChargingStatusNotifier;
    };
	

#endif /*CHARGING_STATUS_OBSERVER*/

//  End of File