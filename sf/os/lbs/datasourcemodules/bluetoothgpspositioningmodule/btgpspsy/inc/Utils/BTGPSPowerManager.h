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




#ifndef BTGPSPOWERMANAGER_H
#define BTGPSPOWERMANAGER_H

//  INCLUDES

#include <e32base.h>
#include "BTGPSPsyConnectionListener.h"
#include "BTGPSDeviceListener.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CBTGPSConnectManager;
class CBTGPSPsyConnectionManager;
class CBTGPSDeviceManager;
class CBTGPSConstantsManager;

// CLASS DECLARATION

/**
*  This class manages power related features. It shurts down BT connection
*  when there is not location request received within pre-defined timeout
*  value.
*  
*/
class CBTGPSPowerManager: 
        public CBase, 
        private MBTGPSPsyConnectionListener,
        private MBTGPSDeviceListener
    {
    public: 

        /**
        * Two-phase construction.
        * @param aConnectManager Reference to connect manager
        * @param aPsyConnectionManager Reference to connection manager
        * @param aDeviceManager Reference to device manager.
        * @param aConstantsManager Reference to contants manager.
        */
        static CBTGPSPowerManager * NewL(
            CBTGPSConnectManager& aConnectManager,
            CBTGPSPsyConnectionManager& aPsyConnectionManager,
            CBTGPSDeviceManager& aDeviceManager,
            CBTGPSConstantsManager& aConstantsManager);

        /**
        * Destructor
        */
        virtual ~CBTGPSPowerManager();

    private:
    
        /**
        * From MBTGPSPsyConnectionListener
        */
        virtual void HandlePsyConnectionChange();
        
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
        CBTGPSPowerManager(
            CBTGPSConnectManager& aConnectManager,
            CBTGPSPsyConnectionManager& aPsyConnectionManager,
            CBTGPSDeviceManager& aDeviceManager,
            CBTGPSConstantsManager& aConstantsManager);
        
        /**
        * Start or cancel timer if necessary.
        */
        void CheckTimer();    
        
        /**
        * Timer callback function
        */
        void TimerTick();
        
        /**
        * Static timer callback function
        */
        static TInt TimerCallback(TAny* aAny);
        
    private:
        //Connect manager
        CBTGPSConnectManager& iConnectManager;
        
        //Psy connection manager
        CBTGPSPsyConnectionManager& iPsyConnectionManager;
        
        //Device manager
        CBTGPSDeviceManager& iDeviceManager;
        
        //Constants manager
        CBTGPSConstantsManager& iConstantsManager;
        
        //Timer
        CPeriodic* iTimer;
    };
#endif
// End of File

