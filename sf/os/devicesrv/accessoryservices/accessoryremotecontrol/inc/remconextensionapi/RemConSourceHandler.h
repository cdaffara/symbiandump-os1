/*
* Copyright (c) 2002-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This class provides utility functions to find the 
*               bearer source with the UID 
*
*/

#ifndef TREMCONSOURCEHANDLER_H
#define TREMCONSOURCEHANDLER_H

#include <e32base.h>

// CONSTANTS
// Bluetooth AVRCP bearer
const TUint32 KRemConExtBTAVRCPBearerID  = 0x1020685F;  //BT

// Bluetooth Mono audio bearer
const TUint32 KRemConExtBTBearerID       = 0x101FBAE9;  //BT

// USB HID bearer
const TUint32 KRemConExtUSBBearerID      = 0x10282c18;  //Wired

// Wired bearer
const TUint32 KRemConExtWiredBearerID    = 0x102069AA;  //Wired

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

#endif // TREMCONSOURCEHANDLER_H 
            
// End of File

