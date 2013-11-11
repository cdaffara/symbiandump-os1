/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#ifndef LOCATIONSETTINGSPRIVATECRKEYS_H
#define LOCATIONSETTINGSPRIVATECRKEYS_H

// Location Settings Storage API
const TUid KCRUidLocationSettings = { 0x101f500c };

// Key for storing the UIDs of visible PSYs.
// UIDs are stored in the PSY priority order.
// Subject to RFS
// The value is a string in format: "UID,availability,cost[,...]"
// UID is 8 hexadecimal digits
// Availability is 0 (Disabled) or 1 (Enabled)
// Cost is one digit from 1 to 4 as defined in TPositionQuality::TCostIndicator
// Example: "1234ABCD,1,2,DBCA2345,0,0"
const TUint32 KLocSettingsPriorityListFactory           = 1;

// Key for storing UID of post-installed PSYs
// UIDs are stored in installation order.
// Not subject to RFS
// The value is a string in format: "UID[,...]"
// UID is 8 hexadecimal digits
// Example: "1234ABCD,1,2,DBCA2345,0,0"
const TUint32 KLocSettingsInstalledPluginsList  		= 2;

// Key for storing the UIDs of invisible PSYs.
// Subject to RFS
// The value is a string in format: "UID,availability,cost[,...]"
// UID is 8 hexadecimal digits
// Availability is 0 (Disabled) or 1 (Enabled)
// Cost is one digit from 1 to 4 as defined in TPositionQuality::TCostIndicator
// Example: "1234ABCD,1,2,DBCA2345,0,0"
const TUint32 KLocSettingsInvisibleListFactory         	= 3;

// Working copy or priority list
const TUint32 KLocSettingsPriorityListWorking         	= 4;
// Working copy or inivisiblity list
const TUint32 KLocSettingsInvisibleListWorking         	= 5;

// Version of factory settings, arbitrary string.
// Recommended value: should contain product identifier
// and firmware version, e.g. "nokn95pr2"
const TUint32 KLocSettingsVersionFactory         		= 6;

// Working copy of settings version
// If it found to be different from factory version,
// then working settings are reset to factory settings
// (but availability and cost will be kept)
const TUint32 KLocSettingsVersionWorking         		= 7;

#endif      // LOCATIONSETTINGSPRIVATECRKEYS_H
