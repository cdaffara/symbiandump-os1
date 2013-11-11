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




#ifndef BTGPSNOKGPSDETECTOR_H
#define BTGPSNOKGPSDETECTOR_H

//  INCLUDES

#include <e32base.h> 
#include "BTGPSMessageListener.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION  PROTOTYPES

// FORWARD DECLARATIONS
class CBTGPSConnectManager;
class CBTGPSDeviceManager;

// CLASS DECLARATION

/**
*  This class defines a object that detect if the connect BT device support
*  Nokia proprietary sentence by sending PNok message. If the device replies 
*  the questing correctly, then the device is considered to be BT GPS device 
*  that support Nokia proprietary feature.
*
*/
class CBTGPSNokGpsDetector: public CBase, private MBTGPSMessageListener
    {
    public: 

        /**
        * Two-phase construction.
		* @param aConnectManager Reference to connect manager.
		* @param aDeviceManager Reference to device manager
        */
        static CBTGPSNokGpsDetector * NewL(
            CBTGPSConnectManager& aConnectManager,
            CBTGPSDeviceManager& aDeviceManager);

        /**
        * Destructor
        */
        virtual ~CBTGPSNokGpsDetector();
        
        /**
        * Detect
        */
        TInt Detect();
        
        /**
        * Cancel detecting
        */
        void Cancel();

    private:
        /**
        * From MBTGPSMessageListener
        */
        virtual void HandleMessage(const TBTGPSNmeaParser& aParser);

    private:

        /**
        * Second phase of the construction
        */
        void ConstructL();

        /**
        * Private constructor
        */
        CBTGPSNokGpsDetector(
            CBTGPSConnectManager& aConnectManager,
            CBTGPSDeviceManager& aDeviceManager);
            
        /**
        * Timer callback
        */
        void TimerTick();
        
        /**
        * Start detecting
        */
        void DetectL();
        
        /**
        * Static timer callback
        */
        static TInt TimerCallBack(TAny* aAny);

    private:
        //Connect Manager
        CBTGPSConnectManager& iConnectManager;

        //Device manager
        CBTGPSDeviceManager& iDeviceManager;
        
        //Timer
        CPeriodic* iTimer;
        
        //Number of valide nmea messages received
        TInt iValidNmeaMsg;
        
        //Number of PNOK polling message sent
        TInt iNumPnokPolling;
        
        //Switch to NMEA mode count
        TInt iSwitchCount;

    };
#endif
// End of File

