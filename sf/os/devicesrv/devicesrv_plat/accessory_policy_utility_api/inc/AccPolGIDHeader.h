/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This is the static part of the Generic ID class. Every 
*                     Generic ID has values defined for these basic capabilities.
*
*/


#ifndef TACCPOLGIDHEADER_H
#define TACCPOLGIDHEADER_H

// INCLUDES

#include <e32std.h>

// CONSTANTS
const TInt KHWModelIDMaxLength( 12 );
const TInt KHWDeviceIDMaxLength( 12 );
const TInt KGIDIntMaxLength( 1024 );

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  Generic ID data structure. 
*  This is strictly for Accessory Server internal use , and it is used by 
*  TAccPolGenericID -class.
*
*  @lib AccPolicy.lib
*  @since S60 3.1
*/
struct TAccPolGIDHeader
    {
    // Accessory device type
    // Common part      : 0x00000001-0x0000FFFF
    // Proprietary part : 0x00010000-0xFFFFFFFF
    TUint32 iAccessoryDeviceType;

    // Physical connection type
    // Common part      : 0x00000001-0x0000FFFF
    // Proprietary part : 0x00010000-0xFFFFFFFF
    TUint32 iPhysicalConnection;

    // Application protocols    
    // Common part      : 0x00000001-0x0000FFFF
    // Proprietary part : 0x00010000-0xFFFFFFFF
    TUint32 iApplicationProtocol;

    // Capabilities subblock    
    // Common part      : 0x0000000000000001-0x00000000FFFFFFFF
    // Proprietary part : 0x0000000100000000-0xFFFFFFFFFFFFFFFF
    TUint64 iCapabilitiesSubblocks;
    
    // Manufacturer specific model ID of the device.
    TBuf<KHWModelIDMaxLength> iHWModelID;

    // Device specific HW identifier.
    TUint64 iHWDeviceID;

    // Device address. For example BT devices,
    // this is the unique Bluetooth device address (TBTDevAddr).
    TUint64 iDeviceAddress;
    
    // Unique identifier for identifying just this GID specific data in DB.
    // Unique amongst all run time Generic ID instances.
    TInt iDBID;
    };

#endif      // TACCPOLGIDHEADER_H

// End of File
