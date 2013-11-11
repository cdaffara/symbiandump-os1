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

#include "lbsbtgpsdevicerecord.h"


/** Constructor 
@internalTechnology
@prototype
*/
EXPORT_C TLbsBtGpsDeviceRecord::TLbsBtGpsDeviceRecord()
	{

	}


/** Get device key
@return The device key 
@internalTechnology
@prototype
*/
EXPORT_C TLbsBtGpsEntryKey TLbsBtGpsDeviceRecord::Key() const
	{
	return iKey;
	}


/** Get Bluetooth device address
@return The Bluetooth device address
@internalTechnology
@prototype
*/
EXPORT_C const TBTSockAddr& TLbsBtGpsDeviceRecord::SocketAddress() const
	{
	return iSocketAddress;
	}


/** Get device type
@return The device type
@internalTechnology
@prototype
*/
EXPORT_C TLbsBtGpsDeviceInfo::TLbsBtGpsDeviceType TLbsBtGpsDeviceRecord::Type() const
	{
	return iType;
	}


/** Set device key
@param aKey [In] New device key value to set 
@internalTechnology
@prototype
*/
EXPORT_C void TLbsBtGpsDeviceRecord::SetKey(TLbsBtGpsEntryKey aKey)
	{
	iKey = aKey;
	}


/** Set Bluetooth device address
@param aAddress [In] The new Bluetooth device address to set
@internalTechnology
@prototype
*/
EXPORT_C void TLbsBtGpsDeviceRecord::SetSocketAddress(const TBTSockAddr& aSocketAddress)
	{
	iSocketAddress = aSocketAddress;
	}


/** Set device type
@param aType [In] The new device type to set
@internalTechnology
@prototype
*/
EXPORT_C void TLbsBtGpsDeviceRecord::SetType(TLbsBtGpsDeviceInfo::TLbsBtGpsDeviceType aType)
	{
	iType = aType;
	}
