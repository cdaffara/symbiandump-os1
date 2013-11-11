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


#ifndef BATTERY_INFO_OBSERVER
#define BATTERY_INFO_OBSERVER

// INCLUDES
#include <e32base.h>
#include <hwrmpower.h>

// CONSTANTS
const TInt KHalfSecond = 500000;
const TInt KTwoMins = 120000000;

// CLASS DECLARATION

/**
* MBatteryInfoNotifier
* Interface class for NotifyBatteryInfo()
*/
class MBatteryInfoNotifier
    {
    public:
    
        /**
        * Notifier for battery info changes
        *
        * @param aError error code
        * @param aBatteryData data structure to battery data
        */
        virtual void NotifyBatteryInfo( TInt aError,
            CHWRMPower::TBatteryConsumptionData aBatteryData ) = 0;
    };

// CLASS DECLARATION

/**
* CBatteryInfoObserver is used to provide battery information from HWRM to CTSY
*/
NONSHARABLE_CLASS( CBatteryInfoObserver ) : public CActive
	{
    public:

        /**
        * Two-phased constructor
        *
        * @param aPower reference to CHWRMPower instance
        * @param aBatteryInfoNotifier reference to interface class
        *  MBatteryInfoNotifier
        * @return Pointer to the created object
        */
    	static CBatteryInfoObserver* NewL( CHWRMPower& aPower,
    	    MBatteryInfoNotifier& aBatteryInfoNotifier );
    	
    	/**
    	* Two-phased constructor
    	*
    	* @param aPower reference to CHWRMPower instance
    	* @param aBatteryInfoNotifier reference to interface class
    	*  MBatteryInfoNotifier
    	* @return Pointer to the created object
    	*/
    	static CBatteryInfoObserver* NewLC( CHWRMPower& aPower,
    	    MBatteryInfoNotifier& aBatteryInfoNotifier );
    	
    	/**
        * Destructor
        */
    	~CBatteryInfoObserver();
    	
    	/**
    	* Handles GetBatteryInfo request completion event
    	*/
    	void RunL();
        
        /**
    	* Cancellation of an outstanding request
    	*/
    	void DoCancel();
    	
    	/**
    	* Get battery info
    	*/
    	void GetBatteryInfo();
    	
    	/**
    	* Run timer 
    	*
    	* @param aObject pointer to object which start timer
        * @return TInt KErrNone because callback function is intended to be
        * called again
    	*/
    	static TInt RunTimer(TAny* aObject);
    	
    private:

        /**
        * By default Symbian 2nd phase constructor is private
        */
    	void ConstructL();
    	
    	/**
        * Constructor
        *
        * @param aPower reference to CHWRMPower instance
        * @param aBatteryInfoNotifier reference to interface class 
        * MBatteryInfoNotifier
        */
    	CBatteryInfoObserver( CHWRMPower& aPower,
    	    MBatteryInfoNotifier& aBatteryInfoNotifier );
    	
    private:

    	/**
    	* Reference to CHWRMPower instance
    	*/
        CHWRMPower& iPower;
        
        /**
        * Data structure used to retrieve data from a call to GetBatteryInfo
        */
        CHWRMPower::TBatteryConsumptionData iBatteryData;
        
        /**
        * Reference to BatteryInfoNotifier object
        */
        MBatteryInfoNotifier& iBatteryInfoNotifier;
        
        /**
        * Periodic timer
        */
        CPeriodic* iPeriodic;
	};
	
	
#endif /*BATTERY_INFO_OBSERVER*/

//  End of File
