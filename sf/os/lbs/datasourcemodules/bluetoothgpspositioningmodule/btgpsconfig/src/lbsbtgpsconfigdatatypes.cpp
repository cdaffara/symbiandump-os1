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
// LBS Bluetooth GPS Configuration API: Data types
//

#include <lbs/lbsbtgpsconfigdatatypes.h>


/** Get the class type 
@return Value indicating the type of LBS Bluetooth GPS class
@publishedPartner
@prototype
*/
EXPORT_C TUint32 TLbsBtGpsClassTypeBase::ClassType() const
	{
	return iClassType;
	}


/** Get the class size
@return The size of the class in bytes 
@publishedPartner
@prototype
*/
EXPORT_C TUint TLbsBtGpsClassTypeBase::ClassSize() const
	{
	return iClassSize;
	}


/** Constructor 
@publishedPartner
@prototype
*/
EXPORT_C TLbsBtGpsClassTypeBase::TLbsBtGpsClassTypeBase()
: iClassType(ELbsBtGpsUnknownClass), 
  iClassSize(sizeof(TLbsBtGpsClassTypeBase))
	{

	}


/** Constructor 
@publishedPartner
@prototype
*/
EXPORT_C TLbsBtGpsDeviceInfoBase::TLbsBtGpsDeviceInfoBase()
	{
	iClassType |= ELbsBtGpsDeviceInfoBaseClass;
	iClassSize = sizeof(TLbsBtGpsDeviceInfoBase);
	}


/** Get device key
@return The device key 
@publishedPartner
@prototype
*/
EXPORT_C TLbsBtGpsEntryKey TLbsBtGpsDeviceInfoBase::Key() const
	{
	return iKey;
	}


/** Set device key
@param aKey [In] New device key value to set 
@publishedPartner
@prototype
*/
EXPORT_C void TLbsBtGpsDeviceInfoBase::SetKey(TLbsBtGpsEntryKey aKey)
	{
	iKey = aKey;
	}


/** Constructor 
@publishedPartner
@prototype
*/
EXPORT_C TLbsBtGpsDeviceInfo::TLbsBtGpsDeviceInfo()
	{
	iClassType |= ELbsBtGpsDeviceInfoClass;
	iClassSize = sizeof(TLbsBtGpsDeviceInfo);
	}


/** Get Bluetooth device address
@return The Bluetooth device address
@publishedPartner
@prototype
*/
EXPORT_C const TBTDevAddr& TLbsBtGpsDeviceInfo::Address() const
	{
	return iAddress;
	}


/** Get device type
@return The device type
@publishedPartner
@prototype
*/
EXPORT_C TLbsBtGpsDeviceInfo::TLbsBtGpsDeviceType TLbsBtGpsDeviceInfo::Type() const
	{
	return iType;
	}


/** Set Bluetooth device address
@param aAddress [In] The new Bluetooth device address to set
@publishedPartner
@prototype
*/
EXPORT_C void TLbsBtGpsDeviceInfo::SetAddress(const TBTDevAddr& aAddress)
	{
	iAddress = aAddress;
	}


/** Set device type
@param aType [In] The new device type to set
@publishedPartner
@prototype
*/
EXPORT_C void TLbsBtGpsDeviceInfo::SetType(TLbsBtGpsDeviceType aType)
	{
	iType = aType;
	}
