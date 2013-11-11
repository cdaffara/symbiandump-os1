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
// LBS Bluetooth GPS Configuration API: Device record
//



/**
 @file
 @internalTechnology
*/

#ifndef LBS_BTGPSDEVICERECORD_H
#define LBS_BTGPSDEVICERECORD_H

#include <lbs/lbsbtgpsconfigdatatypes.h>
#include <bt_sock.h>

/** Bluetooth GPS device record
@internalTechnology
@prototype
*/
class TLbsBtGpsDeviceRecord
	{
public:

	IMPORT_C TLbsBtGpsDeviceRecord();

public:

	IMPORT_C TLbsBtGpsEntryKey Key() const;
	IMPORT_C const TBTSockAddr& SocketAddress() const;
	IMPORT_C TLbsBtGpsDeviceInfo::TLbsBtGpsDeviceType Type() const;

	IMPORT_C void SetKey(TLbsBtGpsEntryKey aKey);
	IMPORT_C void SetSocketAddress(const TBTSockAddr& aSockAddr);
	IMPORT_C void SetType(TLbsBtGpsDeviceInfo::TLbsBtGpsDeviceType aType);

private:
	
	/** The key identifying the device */
	TLbsBtGpsEntryKey iKey;

	/** The Bluetooth socket address */
	TBTSockAddr iSocketAddress;

	/** The device type */
	TLbsBtGpsDeviceInfo::TLbsBtGpsDeviceType iType;
	};

#endif //LBS_BTGPSDEVICERECORD_H
