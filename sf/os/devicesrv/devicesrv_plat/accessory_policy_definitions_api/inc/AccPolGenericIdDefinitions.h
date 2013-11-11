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
* Description:  Public API definitions for Generic ID.
*
*/



#ifndef TACCPOLGENERICIDDEFINITIONS_H
#define TACCPOLGENERICIDDEFINITIONS_H

//  INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <AccPolGIDHeader.h> 

// CONSTANTS

/**
*  Common Generic ID bit mask definitions
*  These definitions define the common Generic ID features.
*  Proprietary (manufacturer specific) features are defined in 
*  AccPolPropGenericID.h -file.
*/
// Device type bit mask definitions
const TUint32 KDTHeadset      = 0x1;        // 00000 00000 00000 00001
const TUint32 KDTCarKit       = 0x2;        // 00000 00000 00000 00010
const TUint32 KDTOffice       = 0x4;        // 00000 00000 00000 00100
const TUint32 KDTMessaging    = 0x8;        // 00000 00000 00000 01000
const TUint32 KDTImaging      = 0x10;       // 00000 00000 00000 10000
const TUint32 KDTLoopset      = 0x20;       // 00000 00000 00001 00000
const TUint32 KDTDataCable    = 0x40;       // 00000 00000 00010 00000
const TUint32 KDTCharger      = 0x80;       // 00000 00000 00100 00000
const TUint32 KDTTTY          = 0x100;      // 00000 00000 01000 00000
const TUint32 KDTAVDevice     = 0x200;      // 00000 00000 10000 00000


// Physical connection bit mask definitions
const TUint32 KPCWired        = 0x1;        // 00000 00000 00000 00001
const TUint32 KPCBluetooth    = 0x2;        // 00000 00000 00000 00010
const TUint32 KPCInfraRed     = 0x4;        // 00000 00000 00000 00100
const TUint32 KPCUSB          = 0x8;        // 00000 00000 00000 01000
const TUint32 KPCHDMI         = 0x10;       // 00000 00000 00000 10000

// Application protocol bit mask definitions
const TUint32 KAPAT           = 0x1;        // 00000 00000 00000 00001
const TUint32 KAPOBEX         = 0x2;        // 00000 00000 00000 00010

// Audio capability group
const TUint64 KSBAudioSubblock        = 0x1;// 00000 00000 00000 00000 00000 00000 00000 00001

// CarKit capability group
const TUint64 KSBCarKitSubblock       = 0x2;// 00000 00000 00000 00000 00000 00000 00000 00010

// Charging capability group
const TUint64 KSBChargingSubblock     = 0x4;// 00000 00000 00000 00000 00000 00000 00000 00100

// Bluetooth capability group
const TUint64 KSBBluetoothSubblock    = 0x8;// 00000 00000 00000 00000 00000 00000 00000 01000

// Video capability group
const TUint64 KSBVideoSubblock    	  = 0x10;// 00000 00000 00000 00000 00000 00000 00000 10000


#endif      // TACCPOLGENERICIDDEFINITIONS_H  
            
// End of File
