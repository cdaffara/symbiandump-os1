// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// LBS Bluetooth GPS Configuration API: Types
//



/**
 @file
 @publishedPartner
 @prototype
*/

#ifndef LBS_BTGPSCONFIGTYPES_H
#define LBS_BTGPSCONFIGTYPES_H

/** Used to uniquely identify an LBS Bluetooth GPS device in the configuration
@publishedPartner
@prototype
*/
typedef TUint32 TLbsBtGpsEntryKey;


/** Special key value to indicate a non-existant device
@publishedPartner
@prototype
*/
const TLbsBtGpsEntryKey KLbsBtGpsEntryKeyNull = 0;


/** Class types for TLbsBtGpsClassTypeBase and derived classes
@publishedPartner
@prototype
*/
enum TLbsBtGpsClassType
	{
	/** Unknown class */
	ELbsBtGpsUnknownClass = 0x00,
	
	/** Class TLbsBtGpsDeviceInfoBase */
	ELbsBtGpsDeviceInfoBaseClass = 0x01,
	
	/** Class TLbsBtGpsDeviceInfo */
	ELbsBtGpsDeviceInfoClass = 0x02,
	
	/** Last class type (should be last in the list - add new values above) */
	ELbsBtGpsLastClass = 0xFFFFFFFF
	};

#endif //LBS_BTGPSCONFIGTYPES_H
