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




#ifndef BTGPSMODULESTATUSHANDLER_H
#define BTGPSMODULESTATUSHANDLER_H

//  INCLUDES

#include <e32base.h> 
#include "BTGPSDeviceListener.h"
#include "BTGPSLocationFixListener.h"
#include "BTGPSPsyConnectionListener.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CBTGPSPsyConnectionManager;
class CBTGPSDeviceManager;
class CBTGPSRequestManager;

// CLASS DECLARATION

/**
*  This class manages module status report according to device
*  connection status and fix information status.
*  
*/
class CBTGPSModuleStatusHandler: 
        public CBase, 
        private MBTGPSDeviceListener,
        private MBTGPSLocationFixListener,
        private MBTGPSPsyConnectionListener
    {
    public: 

        /**
        * Two-phase construction.
        * @param aPsyConnectionManager Reference to PSY connection manager
        * @param aDeviceManager Reference to device manager
        * @param aRequestManager Reference to request manager
        */
        static CBTGPSModuleStatusHandler * NewL(
            CBTGPSPsyConnectionManager& aPsyConnectionManager,
            CBTGPSDeviceManager& aDeviceManager,
            CBTGPSRequestManager& aRequestManager);

        /**
        * Destructor
        */
        virtual ~CBTGPSModuleStatusHandler();

    private:

        /**
        * From MBTGPSDeviceListener
        */
        virtual void BTDeviceStatusChanged(
            TInt aConnectStatus, 
            TInt aDeviceType,
            TInt aErr=KErrNone);

        /**
        * From MBTGPSLocationFixListener
        */
        virtual void HandleLocationFixUpdate(
            const CBTGPSFix& aFix);

        /**
        * From MBTGPSPsyConnectionListener
        */
        virtual void HandlePsyConnectionChange();
            
    private:

        /**
        * Second phase of the construction
        */
        void ConstructL();

        /**
        * Private constructor
        */
        CBTGPSModuleStatusHandler(
            CBTGPSPsyConnectionManager& aPsyConnectionManager,
            CBTGPSDeviceManager& aDeviceManager,
            CBTGPSRequestManager& aRequestManager);
        
        /**
        * Check device status
        */
        TPositionModuleStatus::TDeviceStatus GetDeviceStatus();
        
    private:
        //Positioner ext
        CBTGPSPsyConnectionManager& iPsyConnectionManager;
        
        //Device manager
        CBTGPSDeviceManager& iDeviceManager;
        
        //Request manager
        CBTGPSRequestManager& iRequestManager;
        
        //Module status
        TPositionModuleStatus iModuleStatus;
        
        //Indicate the device has been initialised
        TBool iInitialized;
    };
#endif
// End of File

