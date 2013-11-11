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




#ifndef BTGPSNOKDEVICEHANDLER_H
#define BTGPSNOKDEVICEHANDLER_H

//  INCLUDES
#include <e32base.h> 
#include "BTGPSMessageListener.h"
#include "BTGPSSettingsListener.h"
#include "BTGPSDeviceListener.h"

// CLASS DECLARATION
class CBTGPSSettingManager;
class CBTGPSConnectManager;
class CBTGPSConstantsManager;
class CBTGPSDeviceManager;
class CBTGPSNokNightModeHandler;
class TBTGPSNmeaParser;
class CBTGPSDeviceListManager;


/**
*  Handler for specific Nokia BT GPS device features. This class defines objects that 
*  listen NMEA messages and show battery, external power and external antenna status
*  information note when specific messages received. It also monitoring night mode
*  status of the connected device.
*  
*/
class CBTGPSNokDeviceHandler: 
        public CBase, 
        private MBTGPSMessageListener,
        private MBTGPSSettingsListener,
        private MBTGPSDeviceListener
    {
    public: 

        /**
        * Two-phase construction.
        * @param aConnectManager Reference to connect manager
        * @param aSettingManager Reference to setting manager
        * @param aConstantsManager Reference to constants manager
        * @param aDeviceManager Reference to device manager
        */
        static CBTGPSNokDeviceHandler * NewL(
            CBTGPSConnectManager& aConnectManager,
            CBTGPSSettingManager& aSettingManager,
            CBTGPSConstantsManager& aConstantsManager,
            CBTGPSDeviceManager& aDeviceManager,
            CBTGPSDeviceListManager& aDeviceListManager);

        /**
        * Destructor
        */
        virtual ~CBTGPSNokDeviceHandler();

    private:
        /**
        * From MBTGPSMessageListener
        */
        virtual void HandleMessage(const TBTGPSNmeaParser& aParser);
        
        /**
        * From MBTGPSSettingsListener
        */
        virtual void HandleNightModeChange( 
	        const TBTGPSSettingsApi::TNightModeState aStatus );

        /**
        * From MBTGPSDeviceListener
        */
        virtual void BTDeviceStatusChanged(
            TInt aConnectStatus, 
            TInt aDeviceType,
            TInt aErr=KErrNone);


    private:

        /**
        * Second phase of the construction
        */
        void ConstructL();

        /**
        * Private constructor
        */
        CBTGPSNokDeviceHandler(
            CBTGPSConnectManager& aConnectManager,
            CBTGPSSettingManager& aSettingManager,
            CBTGPSConstantsManager& aConstantsManager,
            CBTGPSDeviceManager& aDeviceManager,
            CBTGPSDeviceListManager& aDeviceListManager);
            
        /**
        * Update device status to setting manager
        */
        void UpdateDeviceStatus(
            TInt aBattLevel,
            TInt aBattStatus,
            TInt aExtPowStatus,
            TInt aExtAntStatus);
            
        /**
        * Handle PNOK STAT message
        */
        void HandleStatMessage(const TBTGPSNmeaParser& aParser);
        
        /**
        * Update device version
        */
        void UpdateDeviceVersion(const TBTGPSNmeaParser& aParser);
        
        /**
        * Check night mode status from PNOK nmea sentence
        */
        void CheckNightModeStatus(TInt aStatus);
        
        /**
        * Update night mode on device according to setting
        */
        void UpdateNightMode(TBTGPSSettingsApi::TNightModeState aSetting);
        
        /**
        * Update PSY state
        */
        void UpdatePsyState(
            TInt aConnectStatus, 
            TInt aDeviceType);
        
        /**
        * Update BT device info in setting
        */
        void UpdateBtDeviceInfo(
            TInt aConnectStatus, 
            TInt aDeviceType);
        
    private:
        //Connect manager
        CBTGPSConnectManager& iConnectManager;
        
        //Setting manager
        CBTGPSSettingManager& iSettingManager;
        
        //Constants manager
        CBTGPSConstantsManager& iConstantsManager;
        
        //Device manager
        CBTGPSDeviceManager& iDeviceManager;
        
        //Device List Manager
        CBTGPSDeviceListManager& iDeviceListManager;
		
		//Night mode handler
		CBTGPSNokNightModeHandler* iNightModeHandler;
        
        //Night mode status
        TInt iNightModeSetting;
        
        //Battery full or empty status
        TInt iBattStatus;
        
        //Ext power status;
        TInt iExtPowerStatus;
        
        //Ext antenna status;
        TInt iExtAntennaStatus;
        
        //Previous BT device type
        TInt iPreviousDeviceType;
    };
#endif
// End of File

