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




#ifndef BTGPSDEVICELISTENER_H
#define BTGPSDEVICELISTENER_H

//  INCLUDES

#include <e32base.h> //For CTimer

// CONSTANTS

// MACROS

// DATA TYPES
/** 
* Connected BT Device type
*/
enum TBTDeviceType
    {
    EBTDeviceTypeUnknown = 0, ///Device type unkown
    EBTDeviceTypeNonGPS,      ///Device is not a GPS device
    EBTDeviceTypeNonNokGps,   ///Device is GPS and doesn't support Nokia proprietary feature
    EBTDeviceTypeNokGps       ///Device is GPS and support Nokia proprietary feature
    };
            
/**
* BT Device connection status
*/
enum TBTDeviceConnectStatus
    {
    EBTDeviceDisconnected = 0,///Device is disconnected
    EBTDeviceConnecting,      ///Connecting to device
    EBTDeviceConnected,       ///Device is connected
    EBTDeviceConnectError     ///Device can't be connected
    };


// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  Interface class for getting BT device connection status change and
*  device type change. 
*  
*/
class MBTGPSDeviceListener
    {
	public:
        /**
        * Observer function when BT Device status changed
        * @param aConnectStatus Connection status
        * @param aDeviceType Device type
        * @param aErr The error code from last connection operation. This value
        *       is valid when aConnectStatus is EBTDeviceConnectError
        */
        virtual void BTDeviceStatusChanged(
            TInt aConnectStatus, 
            TInt aDeviceType,
            TInt aErr) = 0;

    };
#endif
// End of File

