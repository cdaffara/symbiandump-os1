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



/**
 @file
 @publishedPartner
 @prototype
*/

#ifndef LBS_BTGPSCONFIGDATATYPES_H
#define LBS_BTGPSCONFIGDATATYPES_H

#include <e32base.h> 
#include <bttypes.h>

#include <lbs/lbsbtgpsconfigtypes.h>


/** Generic base for classes that store Bluetooth GPS related information 
@publishedPartner
@prototype
 */
class TLbsBtGpsClassTypeBase
	{
public:
	
	IMPORT_C TUint32 ClassType() const;
	IMPORT_C TUint ClassSize() const;

protected:
	
	IMPORT_C TLbsBtGpsClassTypeBase();

protected:

	/** The type of the derived class */
	TUint32 iClassType;
	
	/** The size of the derived class */
	TUint iClassSize;

	/** Reserved for future expansion. */
	TUint8 iReserved[16];
	};


/** Generic base for classes that store Bluetooth GPS device information

Objects of this class (or a derived class) are returned by the API when
requesting information about a Bluetooth GPS device. The data contained is
a copy of the device information, and thus cannot be used to update the
actual configuration data.   

@publishedPartner
@prototype
*/
class TLbsBtGpsDeviceInfoBase : public TLbsBtGpsClassTypeBase
	{
public:

	IMPORT_C TLbsBtGpsDeviceInfoBase();
	
public:

	IMPORT_C TLbsBtGpsEntryKey Key() const;
	IMPORT_C void SetKey(TLbsBtGpsEntryKey aKey);
	
protected:

	/** The key identifying the device */
	TLbsBtGpsEntryKey iKey;
	
	/** Reserved for future expansion. */
	TUint8 iReserved[16];
	};


/** Bluetooth GPS device information
@publishedPartner
@prototype
*/
class TLbsBtGpsDeviceInfo : public TLbsBtGpsDeviceInfoBase
	{
public:

	/** Type of Bluetooth GPS device
	@publishedPartner
	@prototype
	*/
	typedef TUint32 TLbsBtGpsDeviceType;
	
	/** Type of Bluetooth GPS device
	@publishedPartner
	@prototype
	*/
	enum _TLbsBtGpsDeviceType
		{
		/** Unknown device - type should be determined when connected */
		ELbsBtGpsDeviceUnknown = 0,

		/** Not a GPS device */
		ELbsBtGpsDeviceNonGps = 1,
		
		/** GPS device: Non-Nok type */
		ELbsBtGpsDeviceNonNokGps = 2,
		
		/** GPS device: Nok type */
		ELbsBtGpsDeviceNokGps = 3	
		};

public:

	IMPORT_C TLbsBtGpsDeviceInfo();

public:

	IMPORT_C const TBTDevAddr& Address() const;
	IMPORT_C TLbsBtGpsDeviceType Type() const;
	IMPORT_C void SetAddress(const TBTDevAddr& aAddress);
	IMPORT_C void SetType(TLbsBtGpsDeviceType aType);

protected:
	
	/** The Bluetooth device address */
	TBTDevAddr iAddress;

	/** The device type */
	TLbsBtGpsDeviceType iType;

	/** Reserved for future expansion. */
	TUint8 iReserved[16];
	};


#endif //LBS_BTGPSCONFIGDATATYPES_H
