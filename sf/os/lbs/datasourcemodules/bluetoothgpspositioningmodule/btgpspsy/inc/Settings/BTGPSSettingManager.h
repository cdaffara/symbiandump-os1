/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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




#ifndef BTGPSSETTINGMANAGER_H
#define BTGPSSETTINGMANAGER_H

//  INCLUDES

#include <e32base.h> 
#include <e32property.h>
#include <centralrepository.h>
#include "BTGPSSettingsApi.h"
#include "BtGpsPsyPrivatePSKeys.h"

class TBTSockAddr;
class MBTGPSSettingsListener;

/**
*  This class manage settings stored in Central Repositary and Publish&Subsciption.
*  It reads UI settings from CR and inform listeners if there is UI settings
*  change. It also updates BT GPS PSY events to P&S if needed.
*  
*/ 
    
class CBTGPSSettingManager :
    public CActive
    {
    public: 

        /**
        * Two phase constructor.
        */
        static CBTGPSSettingManager* NewL();

        /**
        * Destructor
        */
        virtual ~CBTGPSSettingManager();
        
	    /**
        * Updates the BT GPS PSY status
        * @param aStatus Status of the BT GPS PSY
        * @return The error code
        */
        TInt UpdatePsyState(const TBTGPSPSYState aStatus);
        
	    /**
        * Updates the status of the BT GPS device
        * @param aBatteryLevel The battery level of BT GPS device 
        * @param aExtPowerState The ext. antenna state of BT GPS device 
        * @param aExtAntennaState The ext. power state of BT GPS device
        * @param aBatteryState The battery state of BT GPS device 
        * @return The error code
        */   
  		TInt UpdateDeviceStatus(const TInt aBatteryLevel, 
  		                  const TBTGPSExtPowerState aExtPowerState, 
  						  const TBTGPSExtAntennaState aExtAntennaState,
  						  const TBTGPSBatteryState aBatteryState);
        
	    /**
        * Updates the versions of the BT GPS device
        * @param aID The BT GPS device name
        * @param aBTHW The BT HW versio of BT GPS device
        * @param aBTSW The BT SW versio of BT GPS device
        * @param aGPSHW The GPS HW versio of BT GPS device
        * @param aGPSSW The GPS SW versio of BT GPS device  
        * @return The error code
        */   
        TInt UpdateVersions(const TDesC8& aID, 
				            const TDesC8& aBTHW, 
						    const TDesC8& aBTSW, 
						    const TDesC8& aGPSHW, 
						    const TDesC8& aGPSSW);
        
	    /**
        * Get the night mode status
        * @param aStatus Status of the night mode
        * @return The error code
        */
        TInt GetNightModeSetting(TBTGPSSettingsApi::TNightModeState& aStatus);
		
		/**
        * Gets the BT address information
        * @param aValid Address validity information
        * @param aAddress The address of the BT device 
        * @return The completion code
        */ 							  
		TInt GetBTDeviceInfo( TInt& aValid, TBTSockAddr& aAddress );
		
	    /**
        * Sets the BT address information
        * @param aValid Address validity information.
        * @param aAddress The address of the BT device 
        * @return The completion code
        */ 			
		TInt UpdateBTDeviceInfo(TInt aValid, const TBTSockAddr& aAddress );

        /**
        * Add listener
        * @param aListener Reference to listener
        */
        void AddListenerL(MBTGPSSettingsListener& aListener);
        
        /**
        * Remove listener
        * @param aListener Reference to listener
        */
        void RemoveListener(MBTGPSSettingsListener& aListener);

    protected:
        /**
        * Derived from CActive
        */
        void RunL();
        
        /**
        * Derived from CActive
        */
        void DoCancel();

    private:

        /**
        * Second phase of the construction
        */
        void ConstructL();

        /**
        * Private constructor
        */
        CBTGPSSettingManager();
        
		/**
		* Handle Settings change
		*/
		void HandleSettingsChange();

		/**
        * Reset the published statuses of the PSY properties trought P&S API
        */							  
		void ResetEventStatuses();

    private:

        //UI event listener
        RArray<MBTGPSSettingsListener*> iListenerArray;
        
        //CenRep Settings   
        CRepository* iSettings;
        
        //UI Information
        RProperty iEvents;
        
    };
    
#endif

// End of File

