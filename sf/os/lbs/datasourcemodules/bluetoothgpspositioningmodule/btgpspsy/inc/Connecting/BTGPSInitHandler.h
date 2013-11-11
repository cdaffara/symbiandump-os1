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




#ifndef BTGPSINITHANDLER_H
#define BTGPSINITHANDLER_H

//  INCLUDES
#include <e32base.h> 
#include "BTGPSDeviceListener.h"

// FORWARD DECLARATIONS
class MBTGPSConnectManagerExt;

// CLASS DECLARATION

/**
*  Connection handler to get initialisation result. Init manager is 
*  responsible for device intialize which include BT device type detection.
*  If the device is not a GPS device(either support Nokia propriatery feature 
*  or not), then connect manager shall inform end user to connect to anoter 
*  BT device.
*
*/
class CBTGPSInitHandler: public CBase, private MBTGPSDeviceListener
    {
    public: 

        /**
        * Two-phase construction.
		* @param aManagerExt Reference to connect manager.
        */
        static CBTGPSInitHandler * NewL(
            MBTGPSConnectManagerExt& aManagerExt);

        /**
        * Destructor
        */
        virtual ~CBTGPSInitHandler();

    private:

        /**
        * Second phase of the construction
        */
        void ConstructL();

        /**
        * Private constructor
        */
        CBTGPSInitHandler(
            MBTGPSConnectManagerExt& aManagerExt);
        
        /**
        * From MBTGPSDeviceListener
        */
        virtual void BTDeviceStatusChanged(
            TInt aConnectStatus, 
            TInt aDeviceType,
            TInt aErr=KErrNone);
        
    private:
        //Manager
        MBTGPSConnectManagerExt& iManagerExt;
        
    };
#endif
// End of File

