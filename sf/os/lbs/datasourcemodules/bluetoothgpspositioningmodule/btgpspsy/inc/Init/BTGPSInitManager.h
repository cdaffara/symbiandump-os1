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




#ifndef BTGPSINITMANAGER_H
#define BTGPSINITMANAGER_H

//  INCLUDES

#include <e32base.h> 
#include "BTGPSDeviceListener.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION  PROTOTYPES

// FORWARD DECLARATIONS
class CBTGPSConnectManager;
class CBTGPSDeviceManager;
class CBTGPSNokGpsDetector;
class CBTGPSNokGpsInitialiser;
class CBTGPSNormalGpsInitialiser;
class CBTGPSSettingManager;

// CLASS DECLARATION

/**
*  This class defines manager that controls device initialisation. When 
*  this manager get device connect status from device manager that device
*  is connected it will start initialisation procedure. If device type is 
*  unkown, it will first detect BT device type by sending PNok message and
*  detect if message received from BT device is valid NMEA sentence. After
*  the connected device is initialised, it will update device status from
*  device manager.
*
*/
class CBTGPSInitManager: public CBase, private MBTGPSDeviceListener
    {
    public: 

        /**
        * Two-phase construction.
		* @param aConnectManager Reference to connect manager
		* @param aDeviceManager Reference to device manager
		* @param aSettingManager Reference to setting manager
        */
        static CBTGPSInitManager * NewL(
            CBTGPSConnectManager& aConnectManager,
            CBTGPSDeviceManager& aDeviceManager,
            CBTGPSSettingManager& aSettingManager);

        /**
        * Destructor
        */
        virtual ~CBTGPSInitManager();

    private:
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
        CBTGPSInitManager(
            CBTGPSConnectManager& aConnectManager,
            CBTGPSDeviceManager& aDeviceManager,
            CBTGPSSettingManager& aSettingManager);

    private:
        //Connect Manager
        CBTGPSConnectManager& iConnectManager;

        //Device manager
        CBTGPSDeviceManager& iDeviceManager;
        
        //Setting manager
        CBTGPSSettingManager& iSettingManager;
        
        //Nok GPS detector
        CBTGPSNokGpsDetector* iNokGpsDetector;
        
        //Nok GPS Initialiser
        CBTGPSNokGpsInitialiser* iNokGpsInitialiser;
        
        //Normal GPS Initialiser
        CBTGPSNormalGpsInitialiser* iNormalGpsInitialiser;
        
    };
#endif
// End of File

