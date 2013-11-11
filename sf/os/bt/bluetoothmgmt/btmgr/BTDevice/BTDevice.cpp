// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

#include <btdevice.h>
#include <utf.h>

inline void __BTDEBUGGER()
// needed to call __DEBUGGER inside an __ASSERT
	{
	__DEBUGGER()
	}

#define __BT_BITMASK__(length) ((1 << (length)) - 1)

enum TBTDeviceClassV1
	{
	EMajorServiceLength = 11,
	EMajorDeviceLength = 5,
	EMinorDeviceLength = 6,
	EFormatTypeLength = 2,
	EMajorServiceMask = __BT_BITMASK__(EMajorServiceLength),
	EMajorDeviceMask = __BT_BITMASK__(EMajorDeviceLength),
	EMinorDeviceMask = __BT_BITMASK__(EMinorDeviceLength),
	EFormatTypeMask = __BT_BITMASK__(EFormatTypeLength)
	};
/** Push an object onto the cleanupstack.

@param aBTDeviceArray "The CBTDeviceArray object to be pushed onto the cleanupstack.
@leave This method will leave if an error occurs.
**/
EXPORT_C void BTDeviceArrayCleanupStack::PushL(CBTDeviceArray* aBTDeviceArray)
	{
	CleanupStack::PushL(TCleanupItem(&BTDeviceArrayCleanupStack::ResetAndDestroy, (TAny*)aBTDeviceArray));
	}

/** Method used by the cleanup stack to reset and destroy the array.

@param aBTDeviceArray "The array to be reset and destroyed"
**/
void BTDeviceArrayCleanupStack::ResetAndDestroy(TAny* aBTDeviceArray)
	{
	CBTDeviceArray* array = static_cast<CBTDeviceArray*>(aBTDeviceArray);
	array->ResetAndDestroy();
	delete array;
	}
	
/**
Convert a narrow TBTDeviceName8 to a wide TBTDeviceName.  Uses utf8->unicode conversion.
@param aName "The name to be converted"
@return "The converted name"
@leave "This method will leave if an error occurs with the utf8->unicode conversion"
**/
EXPORT_C TBTDeviceName BTDeviceNameConverter::ToUnicodeL(const TBTDeviceName8& aName)

	{
	TBTDeviceName ret;
	User::LeaveIfError(CnvUtfConverter::ConvertToUnicodeFromUtf8(ret, aName));
	return ret;
	}

/**
Convert a wide TBTDeviceName to a narrow TBTDeviceName8.  Uses unicode->utf8 conversion.
@param aName "The name to be converted"
@return "The converted name"
@leave "This method will leave if an error occurs with the unicode->utf8 conversion"
**/
EXPORT_C TBTDeviceName8 BTDeviceNameConverter::ToUTF8L(const TBTDeviceName& aName)
	{
	TBTDeviceName8 ret;
	User::LeaveIfError(CnvUtfConverter::ConvertFromUnicodeToUtf8(ret, aName));
	return ret;
	}

/**
Constructor.
Initialises the device class to 0.
**/
EXPORT_C TBTDeviceClass::TBTDeviceClass() :
	iDeviceClass(0), iPadding1(0), iPadding2(0)
	{
	}

/**
Constructor
@param	aDeviceClass The initial device class
**/
EXPORT_C TBTDeviceClass::TBTDeviceClass(TUint32 aDeviceClass) :
	iDeviceClass(aDeviceClass), iPadding1(0), iPadding2(0)
	{
	}

/** 
Constructor taking separate fields for combination into device class

Construct a Class of Device using format #1 as per the
<a href="https://www.bluetooth.org/foundry/assignnumb/document/baseband">baseband
assigned numbers</a>.

Note that this is a 24-bit field which we store in a 32-bit integer.
@param aMajorServiceClass The major service class for the device.
@param aMajorDeviceClass The major device class.
@param aMinorDevice The minor device class.
*/
EXPORT_C TBTDeviceClass::TBTDeviceClass(TUint16 aMajorServiceClass, TUint8 aMajorDeviceClass,
										TUint8 aMinorDeviceClass) :
	iDeviceClass(0), iPadding1(0), iPadding2(0)
	{
	iDeviceClass = (aMajorServiceClass & EMajorServiceMask);	// [00000000]0000000000000xxxxxxxxxxx
	iDeviceClass <<= EMajorDeviceLength;	// [00000000]00000000xxxxxxxxxxx00000

	iDeviceClass |= (aMajorDeviceClass & EMajorDeviceMask);	// [00000000]00000000xxxxxxxxxxxyyyyy
	iDeviceClass <<= EMinorDeviceLength;	// [00000000]00xxxxxxxxxxxyyyyy000000

	iDeviceClass |= (aMinorDeviceClass & EMinorDeviceMask);// [00000000]00xxxxxxxxxxxyyyyyzzzzzz
	iDeviceClass <<= EFormatTypeLength;	// [00000000]xxxxxxxxxxxyyyyyzzzzzz00
	}

/**
Getter for major service class
@return The MajorServerClass

Currently we only support CoD format #1.
*/
EXPORT_C TUint16 TBTDeviceClass::MajorServiceClass() const
	{
	if ((iDeviceClass & EFormatTypeMask) != 0x00)
		{
		return 0;
		}
	
	TUint32 servClass =	iDeviceClass >> (EMajorDeviceLength +
										 EMinorDeviceLength +
										 EFormatTypeLength);

	return static_cast<TUint16>(servClass & EMajorServiceMask);
	}

/**
Getter for major device class
@return The MajorDeviceClass

Currently we only support CoD format #1.
*/
EXPORT_C TUint8 TBTDeviceClass::MajorDeviceClass() const
	{
	if ((iDeviceClass & EFormatTypeMask) != 0x00)
		{
		return 0;
		}
	
	TUint32 majClass = iDeviceClass >> (EMinorDeviceLength + EFormatTypeLength);

	return static_cast<TUint8>(majClass & EMajorDeviceMask);
	}

/**	
Getter for minor device class
@return The MinorDeviceClass

Currently we only support CoD format #1.
*/
EXPORT_C TUint8 TBTDeviceClass::MinorDeviceClass() const
	{
	if ((iDeviceClass & EFormatTypeMask) != 0x00)
		{
		return 0;
		}
	
	TUint32 minClass = iDeviceClass >> (EFormatTypeLength);

	return static_cast<TUint8>(minClass & EMinorDeviceMask);
	}

/**
Getter for whole device class
@return The DeviceClass
*/
EXPORT_C TUint32 TBTDeviceClass::DeviceClass() const
	{
	return iDeviceClass;
	}

/**
Comparison operator
@param aDeviceClass the instance with which this is compared
@return ETrue if device classes are equal, otherwise false
*/
EXPORT_C TBool TBTDeviceClass::operator==(const TBTDeviceClass& aDeviceClass) const
	{
	TBool retval = (iDeviceClass==aDeviceClass.DeviceClass());
	return retval;
	}

/**
Assignment operator
@param aDeviceClass the instance from which this is assigned
@return this Device Class
*/
EXPORT_C TBTDeviceClass& TBTDeviceClass::operator=(const TBTDeviceClass& aDeviceClass)
	{
	if (this != &aDeviceClass)
		{
		iDeviceClass = aDeviceClass.DeviceClass();
		}
	return *this;
	}

/**
Extenalize this object to a stream.
@param aStream The stream object to which the class data will be written.
@leave This method will leave if an error occurs.
*/
EXPORT_C void TBTDeviceClass::ExternalizeL(RWriteStream& aStream) const
	{
	aStream.WriteUint32L(KStreamVersion);
	aStream.WriteUint32L(iDeviceClass);
	}

/**
Internalize this object from a stream.
@param aStream The stream object containing the data with which to internalize this object.
@leave This method will leave if an error occurs.
*/
EXPORT_C void TBTDeviceClass::InternalizeL(RReadStream& aStream)
	{
	TUint32 version = aStream.ReadUint32L();
	static_cast<void>(version); // Currently only one valid version.
	iDeviceClass = aStream.ReadUint32L();
	}


//====================================================================

/**
Constructor.
*/
EXPORT_C TBTNamelessDevice::TBTNamelessDevice()
	: iSetMask(0)
	, iLinkKeyType(ELinkKeyCombination)
	{
	}

/**
Constructor.  Use NewL() or NewLC() to create an object of this type instead of this constructor.
*/
CBTDevice::CBTDevice()	
	{
	}

/**
Two-phase default constructor
Create an empty device.  The object will be placed on the cleanupstack and must be removed by the caller.
@return CBTDevice
@leave This method will leave if an error occurs.
*/
EXPORT_C CBTDevice* CBTDevice::NewLC()
	{
	CBTDevice* self = new(ELeave) CBTDevice();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

/**
Two-phase default constructor
Create an empty device.
@return The new CBTDevice object.
@leave This method will leave if an error occurs.
*/
EXPORT_C CBTDevice* CBTDevice::NewL()
	{
	CBTDevice* self = CBTDevice::NewLC();
	CleanupStack::Pop();	//self
	return self;
	}

/**
Two-phase constructor
@param aBDAddr the address of the device to which this class refers
@return The new CBTDevice object.  The object will be left on the cleanupstack.
@leave This method will leave if an error occurs.
*/
EXPORT_C CBTDevice* CBTDevice::NewLC(const TBTDevAddr& aBDAddr)
	{
	CBTDevice* self = new(ELeave) CBTDevice;
	CleanupStack::PushL(self);
	self->ConstructL(aBDAddr);
	return self;
	}

/**
Two-phase constructor
Create a device and set its address.
@param aBDAddr the address of the device to which this class refers
@return The new CBTDevice object.
@leave This method will leave if an error occurs.
*/
EXPORT_C CBTDevice* CBTDevice::NewL(const TBTDevAddr& aBDAddr)
	{
	CBTDevice* self = CBTDevice::NewLC(aBDAddr);
	CleanupStack::Pop();	//self
	return self;
	}

/**
Two-phase copy constructor
Create a device based on the smaller nameless device
@param aNamelessDevice the device to copy
@return this Device
@leave This method will leave if an error occurs.
*/
EXPORT_C CBTDevice* CBTDevice::NewL(const TBTNamelessDevice& aNamelessDevice)
	{
	CBTDevice* self = CBTDevice::NewLC(aNamelessDevice);
	CleanupStack::Pop();	//self
	return self;
	}

/**
Two-phase copy constructor
Create a device based on the smaller nameless device
@param aNamelessDevice the device to copy
@return this Device, placed on the cleanupstack.
@leave This method will leave if an error occurs.
*/
EXPORT_C CBTDevice* CBTDevice::NewLC(const TBTNamelessDevice& aNamelessDevice)
	{
	CBTDevice* self = new(ELeave) CBTDevice();
	CleanupStack::PushL(self);
	self->ConstructL(aNamelessDevice);
	return self;
	}

void CBTDevice::ConstructL()
	{
	CommonConstructL();
	}
	
void CBTDevice::ConstructL(const TBTDevAddr& aDevAddr)
	{
	CommonConstructL();
	iDevice.SetAddress(aDevAddr);
	}
	
void CBTDevice::ConstructL(const TBTNamelessDevice& aNamelessDevice)
	{
	CommonConstructL();
	iDevice.Update(aNamelessDevice);
	}
	
void CBTDevice::CommonConstructL()
	{
	}


/**
Destructor.
*/
EXPORT_C CBTDevice::~CBTDevice()
	{
	delete iDeviceName;
	delete iFriendlyName;
	}

/**
Copies the contents of this into a new CBTDevice object.
@leave This method will leave if an error occurs.
*/
EXPORT_C CBTDevice* CBTDevice::CopyL() const
	{
	CBTDevice* newDevice = CBTDevice::NewLC();

	if (IsValidBDAddr())
		newDevice->SetDeviceAddress(BDAddr());
	if (IsValidDeviceName())
		newDevice->SetDeviceNameL(DeviceName());
	if (IsValidFriendlyName())
		newDevice->SetFriendlyNameL(FriendlyName());
	if (IsValidDeviceClass())
		newDevice->SetDeviceClass(DeviceClass());
	 if(IsValidLinkKey())
		newDevice->SetLinkKey(LinkKey(),LinkKeyType());
	if (IsValidGlobalSecurity())
		newDevice->SetGlobalSecurity(GlobalSecurity());
	if (iDevice.IsValidPageScanRepMode())
		newDevice->AsNamelessDevice().SetPageScanRepMode(iDevice.PageScanRepMode());
	if (iDevice.IsValidPageScanMode())
		newDevice->AsNamelessDevice().SetPageScanMode(iDevice.PageScanMode());
	if (iDevice.IsValidPageScanPeriodMode())
		newDevice->AsNamelessDevice().SetPageScanPeriodMode(iDevice.PageScanPeriodMode());
	if (iDevice.IsValidClockOffset())
		newDevice->AsNamelessDevice().SetClockOffset(iDevice.ClockOffset());
	if (IsValidSeen())
		newDevice->SetSeen(Seen());
	if (IsValidUsed())
		newDevice->SetUsed(Used());
	if (IsValidPassKey())
		newDevice->SetPassKey(PassKey());
	if (IsValidPaired())
		{
		if (!IsPaired())
			{
			newDevice->SetPaired(EFalse);
			}
		else if (!IsValidLinkKey())
			{
			newDevice->SetPaired(LinkKeyType()); // If link key is valid then "paired" status is already set.
			}
		}
	if (IsValidUiCookie())
		newDevice->SetUiCookie(UiCookie());
	
	CleanupStack::Pop(newDevice);

	return newDevice;
	}

/**
Update 'this' with valid details of aDevice, leaving already present settings
@param aDevice The device details to be used to update this device.
@publishedPartner
@released
*/
EXPORT_C void TBTNamelessDevice::Update(const TBTNamelessDevice& aDevice)
	{
	if (aDevice.IsValidAddress())
		SetAddress(aDevice.Address());

	if (aDevice.IsValidDeviceClass())
		SetDeviceClass(aDevice.DeviceClass());

	if (aDevice.IsValidLinkKey())
		SetLinkKey(aDevice.LinkKey(), aDevice.LinkKeyType());

	if (aDevice.IsValidGlobalSecurity())
		SetGlobalSecurity(aDevice.GlobalSecurity());
	
	if (aDevice.IsValidPageScanRepMode())
		SetPageScanRepMode(aDevice.PageScanRepMode());

	if (aDevice.IsValidPageScanMode())
		SetPageScanMode(aDevice.PageScanMode());

	if (aDevice.IsValidPageScanPeriodMode())
		SetPageScanPeriodMode(aDevice.PageScanPeriodMode());

	if (aDevice.IsValidClockOffset())
		SetClockOffset(aDevice.ClockOffset());

	if (aDevice.IsValidSeen())
		SetSeen(aDevice.Seen());

	if (aDevice.IsValidUsed())
		SetUsed(aDevice.Used());
		
	if (aDevice.IsValidPassKey())
		SetPassKey(aDevice.PassKey());
	
	if(aDevice.IsValidPaired())
		{		
		if (!aDevice.IsPaired())
			{
			SetPaired(EFalse);
			}
		else if (!aDevice.IsValidLinkKey())
			{
			SetPaired(aDevice.LinkKeyType()); // If link key is valid then "paired" status is already set.
			}		
		}
	
	if(aDevice.IsValidUiCookie())
		SetUiCookie(aDevice.UiCookie());
		
	}


/**
Compare the attributes of aDevice to *this and return a bitfield of matching attributes.
@see TBTDeviceSet
@publishedPartner
@released
@return bit field indicating what attributes of aDevice are equal to self
*/
EXPORT_C TUint TBTNamelessDevice::CompareTo(const TBTNamelessDevice& aDevice) const
	{
	TUint retval = EAllNamelessProperties;
	
	//Check that data is valid before comparing values:
	//if both valid, values must equate
	//if both invalid, treat as equal regardless of values
	if (!((IsValidAddress() && aDevice.IsValidAddress() && (Address()==aDevice.Address()))
		|| (!IsValidAddress() && !aDevice.IsValidAddress())))
		{
		retval &= ~EAddress;
		}
	if (!((IsValidDeviceClass() && aDevice.IsValidDeviceClass() && (DeviceClass()==aDevice.DeviceClass()))
		|| (!IsValidDeviceClass() && !aDevice.IsValidDeviceClass())))
		{
		retval &= ~EDeviceClass;
		}
	if (!((IsValidLinkKey() && aDevice.IsValidLinkKey() && (LinkKey()==aDevice.LinkKey()) && (LinkKeyType() == aDevice.LinkKeyType()))
		|| (!IsValidLinkKey() && !aDevice.IsValidLinkKey())))
		{
		retval &= ~ELinkKey;
		}

	if (!((IsValidPageScanRepMode() && aDevice.IsValidPageScanRepMode() 
		&& (PageScanRepMode()==aDevice.PageScanRepMode()))
		|| (!IsValidPageScanRepMode()  && !aDevice.IsValidPageScanRepMode())))
		{
		retval &= ~EPageScanRepMode;
		}

	if (!((IsValidPageScanMode() && aDevice.IsValidPageScanMode()
		&& (PageScanMode() == aDevice.PageScanMode()))
		|| (!IsValidPageScanMode()  && !aDevice.IsValidPageScanMode())))
		{
		retval &= ~EPageScanMode;
		}

	if (!((IsValidPageScanPeriodMode() && aDevice.IsValidPageScanPeriodMode()
		&& (PageScanPeriodMode() == aDevice.PageScanPeriodMode()))
		|| (!IsValidPageScanPeriodMode()  && !aDevice.IsValidPageScanPeriodMode())))
		{
		retval &= ~EPageScanPeriodMode;
		}
	
	if (!((IsValidClockOffset() && aDevice.IsValidClockOffset()
		&& (ClockOffset() == aDevice.ClockOffset()))
		|| (!IsValidClockOffset()  && !aDevice.IsValidClockOffset())))
		{
		retval &= ~EClockOffset;
		}

	if (!((IsValidSeen() && aDevice.IsValidSeen()
		&& (static_cast<TTime>(Seen()) == static_cast<TTime>(aDevice.Seen())))
		||(!IsValidSeen() && !aDevice.IsValidSeen())))
		{
		retval &= ~ESeen;
		}

	if (!((IsValidUsed() && aDevice.IsValidUsed()
		&& (static_cast<TTime>(Used()) == static_cast<TTime>(aDevice.Used())))
		||(!IsValidUsed() && !aDevice.IsValidUsed())))
		{
		retval &= ~EUsed;
		}

	if (!((IsValidGlobalSecurity() && aDevice.IsValidGlobalSecurity()
		&& (GlobalSecurity() == aDevice.GlobalSecurity()))
		||(!IsValidGlobalSecurity() && !aDevice.IsValidGlobalSecurity())))
		{
		retval &= ~EGlobalSecurity;
		}
	
	if (!((IsValidPassKey() && aDevice.IsValidPassKey() && (PassKey()() == aDevice.PassKey()()))
		|| (!IsValidPassKey() && !aDevice.IsValidPassKey())))
		{
		retval &= ~EPassKey;
		}
	
	if(!((IsValidPaired() && aDevice.IsValidPaired() && (IsPaired() == aDevice.IsPaired()) 
		&& (!IsPaired() || (LinkKeyType() == aDevice.LinkKeyType()))) 
		||(!IsValidPaired() && !aDevice.IsValidPaired())))
		{
		// This is slightly more complex than other cases because of the way link key type
		// and "paired" are bound together.
		retval &= ~EIsPaired;
		}
	
	if (!((IsValidUiCookie() && aDevice.IsValidUiCookie() 
		&& (UiCookie()==aDevice.UiCookie()))
		|| (!IsValidUiCookie() && !aDevice.IsValidUiCookie())))
		{
		retval &= ~EUiCookie;
		}

	return retval;
	}

/**
@publishedPartner
@released

Compare aDevice with this.
@param aDevice The device to compare with this.
@return ETrue if they match, EFalse if not.
*/
EXPORT_C TBool TBTNamelessDevice::operator==(const TBTNamelessDevice& aDevice) const
	{
	return CompareTo(aDevice)==EAllNamelessProperties ? ETrue : EFalse;
	}

/**
@publishedPartner
@released

Compare aDevice with this.
@param aDevice The device to compare with this.
@return EFalse if they match, ETrue if not.
*/
EXPORT_C TBool TBTNamelessDevice::operator!=(const TBTNamelessDevice& aDevice) const
	{
	return !(*this==aDevice);
	}

/**
Compiler generated operator= is ok for this class, but ARM compiler warns
@publishedPartner
@released
@param aDevice The device to copy.
@return this Nameless Device
*/
EXPORT_C TBTNamelessDevice& TBTNamelessDevice::operator=(const TBTNamelessDevice& aDevice)
	{
	Update(aDevice);
	return *this;
	}

/**
Get the device address.
@return The device address this refers to
*/
EXPORT_C const TBTDevAddr& TBTNamelessDevice::Address() const
	{
	return iBDAddr;
	}

/**
Returns the name of the device
@return The device name.  If the device has no name, a zero-length descriptor will be returned.
*/
EXPORT_C const TDesC8& CBTDevice::DeviceName() const
	{
	if (iDeviceName)
		{
		return *iDeviceName;
		}
	else
		{
		return KNullDesC8;
		}
	}

/**
Returns the friendly name of the device
@return The friendly name.  If the device has no friendly name, a zero-length descriptor will be returned.
*/
EXPORT_C const TDesC& CBTDevice::FriendlyName() const
	{
	if (iFriendlyName)
		{
		return *iFriendlyName;
		}
	else
		{
		return KNullDesC;
		}
	}

/**
Returns the class of the device
@return The device class.
*/
EXPORT_C const TBTDeviceClass& TBTNamelessDevice::DeviceClass() const
	{
	return iDeviceClass;
	}

/**
@internalAll
@released
Return the link key.
@return the link key
*/
EXPORT_C const TBTLinkKey& TBTNamelessDevice::LinkKey() const
	{
	return iLinkKey;
	}

/**
@publishedPartner
@released
Return the link key type.
@return the link key type
*/
EXPORT_C TBTLinkKeyType TBTNamelessDevice::LinkKeyType() const
	{
	return iLinkKeyType;
	}


/**
Retrieve the global security settings.
@return The Global security settings for all services used by this device
*/
EXPORT_C const TBTDeviceSecurity& TBTNamelessDevice::GlobalSecurity() const
	{
	return iGlobalSecurity;
	}

/**
@internalAll
@released
@return The Page Scan Repitition Mode of the device
*/
EXPORT_C TUint8 TBTNamelessDevice::PageScanRepMode() const
	{
	return iBasebandParams.iPageScanRepetitionMode;
	}

/**
@internalAll
@released
@return The Page Scan Mode of the device
*/
EXPORT_C TUint8 TBTNamelessDevice::PageScanMode() const
	{
	return iBasebandParams.iPageScanMode;
	}

/**
@return the Page Scan Period Mode of the device
@internalAll
@released
*/
EXPORT_C TUint8 TBTNamelessDevice::PageScanPeriodMode() const
	{
	return iBasebandParams.iPageScanPeriodMode;
	}

/**
@return the ClockOffSet of the device
@internalAll
@released
*/
EXPORT_C TUint16 TBTNamelessDevice::ClockOffset() const
	{
	return iBasebandParams.iClockOffset;
	}

/**
Return the time when this device was last seen.
Note that this is currently not support by the bluetooth stack.  
Only the time when the device was last _used_ can is stored.
@return TTime of when device last seen
*/
EXPORT_C const TTime& TBTNamelessDevice::Seen() const
	{
	return iSeen;
	}

/**
Return the time when the device was last used.
@return TTime of when device last connected to
*/
EXPORT_C const TTime& TBTNamelessDevice::Used() const
	{
	return iUsed;
	}

/**
Return the PIN code.
@return TBTPinCode
*/
EXPORT_C const TBTPinCode& TBTNamelessDevice::PassKey() const
	{
	return iPassKey;
	}
	
/**
Check whether the device is paired
@return ETrue = Paired
*/
EXPORT_C TBool TBTNamelessDevice::IsPaired() const
	{
	return iPaired;
	}

/**
Set the bluetooth device address.
@param aBDAddr the address of the device
*/
EXPORT_C void TBTNamelessDevice::SetAddress(const TBTDevAddr& aBDAddr)
	{
	iBDAddr = aBDAddr;
	iSetMask |= EAddress;
	}

/**
Set the device class.
@param aDeviceClass the deviceclass of the device
*/
EXPORT_C void TBTNamelessDevice::SetDeviceClass(TBTDeviceClass aDeviceClass)
	{
	iDeviceClass = aDeviceClass;
	iSetMask |= EDeviceClass;
	}

/**
@publishedPartner
@released
Set the link key.
@param aLinkkey the link key of the device
*/
EXPORT_C void TBTNamelessDevice::SetLinkKey(const TBTLinkKey& aLinkKey)
	{
	SetLinkKey(aLinkKey, ELinkKeyCombination);
	}

/**
@publishedPartner
@released
Set the link key.
@param aLinkkey the link key of the device
@param aLinkKeyType the link key type of the device
*/
EXPORT_C void TBTNamelessDevice::SetLinkKey(const TBTLinkKey& aLinkKey, TBTLinkKeyType aLinkKeyType)
	{
	iLinkKey.Copy(aLinkKey);
	iSetMask |= ELinkKey;
	SetPaired(aLinkKeyType);
	}


/**
Ensure that the previously known link key is discarded
@publishedPartner
@released
*/
EXPORT_C void TBTNamelessDevice::DeleteLinkKey() 
	{
	iSetMask &= ~ELinkKey;
	iSetMask &= ~EIsPaired;
	}

/**
Set the page scan repetition mode.
@param aPageScanRepMode the PSRM of the device
@internalAll
@released
*/
EXPORT_C void TBTNamelessDevice::SetPageScanRepMode(TUint8 aPageScanRepMode) 
	{
	iBasebandParams.iPageScanRepetitionMode = aPageScanRepMode;
	iSetMask |= EPageScanRepMode;
	}

/**
Set the page scan mode.
@param aPageScanMode the PSM of the device
@internalAll
@released
*/
EXPORT_C void TBTNamelessDevice::SetPageScanMode(TUint8 aPageScanMode) 
	{
	iBasebandParams.iPageScanMode = aPageScanMode;
	iSetMask |= EPageScanMode;
	}

/**
Set the page scan period mode.
@param aPageScanPeriodMode the PSPM of the device
@internalAll
@released
*/
EXPORT_C void TBTNamelessDevice::SetPageScanPeriodMode(TUint8 aPageScanPeriodMode) 
	{
	iBasebandParams.iPageScanPeriodMode = aPageScanPeriodMode;
	iSetMask |= EPageScanPeriodMode;
	}

/**
Set the clock offset.
@param aClockOffset the clockoffset for the device
@internalAll
@released
*/
EXPORT_C void TBTNamelessDevice::SetClockOffset(TUint16 aClockOffset) 
	{
	iBasebandParams.iClockOffset = aClockOffset;
	iSetMask |= EClockOffset;
	}


/**
Set the Global security settings for all services used by this device
@param aSetting the security override of the device
*/
EXPORT_C void TBTNamelessDevice::SetGlobalSecurity(const TBTDeviceSecurity& aSetting)
	{
	iGlobalSecurity = aSetting;
	iSetMask |= EGlobalSecurity;
	}

/**
Set when last seen in inquiry (not supported by stack at present)
@param aDateTime The date/time stamp.
*/
EXPORT_C void TBTNamelessDevice::SetSeen(const TTime& aDateTime)
	{
	iSeen = aDateTime;
	iSetMask |= ESeen;
	}

/**
Set when the device last connected from or to.
@param aDateTime The date/time stamp.
*/
EXPORT_C void TBTNamelessDevice::SetUsed(const TTime& aDateTime)
	{
	iUsed = aDateTime;
	iSetMask |= EUsed;
	}
	
/**
Set whether the device is paired.
This function should not be used any more, instead the overload with a link key type
should be used.
@see TBTNamelessDevice::SetPaired(TBTLinkKeyType aLinkKeyType)
@param aPaired whether the device is paired
*/
EXPORT_C void TBTNamelessDevice::SetPaired(TBool aPaired)
	{
	SetPaired(ELinkKeyCombination);	
	iPaired = aPaired;	
	}

/**
Set the link key type because the device is paired
@param aLinkKeyType the link key type
*/
EXPORT_C void TBTNamelessDevice::SetPaired(TBTLinkKeyType aLinkKeyType)
	{
	iSetMask |= EIsPaired;
	iLinkKeyType = aLinkKeyType;
	iPaired = ETrue;
	}
	
/**
Set PIN code for device
@param aPassKey used for pairing
*/
EXPORT_C void TBTNamelessDevice::SetPassKey(const TBTPinCode& aPassKey)
	{
	iPassKey = aPassKey;
	iSetMask |= EPassKey;
	}
	
	
/**
Denotes whether the device address has been set.
@return ETrue = The address of the device been set
*/
EXPORT_C TBool TBTNamelessDevice::IsValidAddress() const
	{
	return (iSetMask & EAddress);
	}

/**
Denotes whether the device class has been set.
@return ETrue = The class of the device been set
*/
EXPORT_C TBool TBTNamelessDevice::IsValidDeviceClass() const
	{
	return (iSetMask & EDeviceClass);
	}

/**
Denotes whether the link key has been set.
@return ETrue = The link key of the device been set
*/
EXPORT_C TBool TBTNamelessDevice::IsValidLinkKey() const
	{
	return (iSetMask & ELinkKey);
	}

/**
Denotes whether the global security settings have been set.
@return ETrue = The global security settings of the device been set
*/
EXPORT_C TBool TBTNamelessDevice::IsValidGlobalSecurity() const
	{
	return (iSetMask & EGlobalSecurity);
	}

/**
Denotes whether the page scan repition mode has been set.
@return ETrue = The pagescanrepetitionmode of the device been set
*/
EXPORT_C TBool TBTNamelessDevice::IsValidPageScanRepMode() const
	{
	return (iSetMask & EPageScanRepMode);
	}

/**
Denotes whether the page scan mode has been set.
@return ETrue = The pagescanmode of the device been set
*/
EXPORT_C TBool TBTNamelessDevice::IsValidPageScanMode() const
	{
	return (iSetMask & EPageScanMode);
	}

/**
Denotes whether the page scan period mode has been set.
@return ETrue = The pagescanperiodmode of the device been set
*/
EXPORT_C TBool TBTNamelessDevice::IsValidPageScanPeriodMode() const
	{
	return (iSetMask & EPageScanPeriodMode);
	}

/**
Denotes whether the clock offset has been set.
@return ETrue = The clockoffset of the device been set
*/
EXPORT_C TBool TBTNamelessDevice::IsValidClockOffset() const
	{
	return (iSetMask & EClockOffset);
	}

/**
Denotes whether the time last seen has been set.
@return ETrue = The time last seen has been set
*/
EXPORT_C TBool TBTNamelessDevice::IsValidSeen() const
	{
	return (iSetMask & ESeen);
	}

/**
Denotes whether the time last used has been set.
@return ETrue = The time last used has been set
*/
EXPORT_C TBool TBTNamelessDevice::IsValidUsed() const
	{
	return (iSetMask & EUsed);
	}

/**
Denote whether is paired has been set
@return Etrue = whether is paired has been set
*/
EXPORT_C TBool TBTNamelessDevice::IsValidPaired() const
	{
	return iSetMask & EIsPaired;
	}
	
/**
Denotes whether the PassKey has been set.
@return ETrue = The PassKey of the device been set
*/
EXPORT_C TBool TBTNamelessDevice::IsValidPassKey() const
	{
	return iSetMask & EPassKey;
	}

/**
@return the PassKey length
@released
*/
EXPORT_C TUint TBTNamelessDevice::PassKeyLength() const
	{
	return iPassKey().iLength;
	}

/**
Denotes whether the UI Cookie has been set.
@return ETrue = The UI Cookie for the device been set
@publishedPartner
@released
*/
EXPORT_C TBool TBTNamelessDevice::IsValidUiCookie() const
	{
	return iSetMask & EUiCookie;
	}

/**
Sets the UI Cookie for the device.  The format of the cookie is
specific to the UI using the field.
@param aUiCookie The 32bit cookie value.
@publishedPartner
@released
*/
EXPORT_C void TBTNamelessDevice::SetUiCookie(TUint32 aUiCookie)
	{
	iUiCookie = aUiCookie;
	iSetMask |= EUiCookie;
	}

/**
Returns the UI Cookie value associated with the device.  The format
of the cookie is specific to the UI using the field.
@return The 32bit cookie value.
@publishedPartner
@released
*/
EXPORT_C TUint32 TBTNamelessDevice::UiCookie() const
	{
	return iUiCookie;
	}


//

/** Set the device name.
@param aName device name
@leave This method will leave if an error occurs.
@internalAll
@released
*/
EXPORT_C void CBTDevice::SetDeviceNameL(const TDesC8& aName)
	{
	if (iDeviceName)
		{
		// scrub the present one
		delete iDeviceName;
		iDeviceName = NULL;
		}
	iDeviceName = HBufC8::NewL(aName.Length());	//device names are always 8bit	
	*iDeviceName = aName;
	iDevice.iSetMask |= EDeviceName;
	
	}

/** Set the friendly name.
@param aName friendly name of the device
@leave This method will leave if an error occurs.
*/
EXPORT_C void CBTDevice::SetFriendlyNameL(const TDesC& aName)	
	{
	if (iFriendlyName)
		{
		// scrub the present one
		delete iFriendlyName;
		iFriendlyName = NULL;
		}
	iFriendlyName = HBufC::NewL(aName.Length());
	*iFriendlyName = aName;
	iDevice.iSetMask |= static_cast<TUint>(EFriendlyName);
	}

/** Has the name of the device been set?
@return ETrue=>device name has been set, otherwise EFalse
*/
EXPORT_C TBool CBTDevice::IsValidDeviceName() const
	{
	return (iDevice.iSetMask & static_cast<TUint>(EDeviceName));
	}

/** Has the friendly name of the device been set?
@return ETrue=>device name has been set, otherwise EFalse
*/
EXPORT_C TBool CBTDevice::IsValidFriendlyName() const
	{
	return (iDevice.iSetMask & static_cast<TUint>(EFriendlyName));
	}

/**
Equality operator
@param aDevice instance against which this is compared
@return ETrue if aDevice==this
*/
EXPORT_C TBool CBTDevice::operator==(const CBTDevice& aDevice) const
	{
	return (CompareTo(aDevice) == (EAllNameProperties | TBTNamelessDevice::EAllNamelessProperties)) ? ETrue: EFalse;
	}

/**
Inequality operator
@param aDevice instance against which this is compared
@return ETrue if aDevice!=this
*/
EXPORT_C TBool CBTDevice::operator!=(const CBTDevice& aDevice) const
	{
	return !(*this==aDevice);
	}
	
/**
Externalise this object to aStream.
@param aStream The stream to which this object will be written.
@leave This method will leave if an error occurs.
*/
EXPORT_C void CBTDevice::ExternalizeL(RWriteStream& aStream) const
	{
	aStream.WriteUint32L(KStreamVersion);
	aStream << BDAddr().Des();
	aStream << DeviceName();
	aStream << FriendlyName();

	DeviceClass().ExternalizeL(aStream);
	aStream << LinkKey();
	aStream << PassKey();
	GlobalSecurity().ExternalizeL(aStream);
	iDevice.iBasebandParams.ExternalizeL(aStream);

	TPckg<TTime> seenPtr(Seen());
	aStream << seenPtr;
	
	TPckg<TTime> usedPtr(Used());
	aStream << usedPtr;
	
	aStream.WriteUint32L(iDevice.iSetMask);
	
	aStream.WriteUint32L(static_cast<TUint32>(IsPaired()));
	aStream.WriteUint32L(static_cast<TUint32>(LinkKeyType()));
	aStream.WriteUint32L(UiCookie());
	}

/**
Internalize this object from aStream.
@param aStream The stream from which this object shall be internalized.
@leave This method will leave if an error occurs.
*/
EXPORT_C void CBTDevice::InternalizeL(RReadStream& aStream)
	{
	TUint32 version = aStream.ReadUint32L();
	static_cast<void>(version); // Currently only one valid version.
	TPtr8 addr (iDevice.iBDAddr.Des());
	aStream >> addr;
	
	iDeviceName = HBufC8::NewL(aStream, KMaxBluetoothNameLen);
	iFriendlyName = HBufC16::NewL(aStream, KMaxFriendlyNameLen);

	iDevice.iDeviceClass.InternalizeL(aStream);
	aStream >> iDevice.iLinkKey;
	aStream >> iDevice.iPassKey;
	iDevice.iGlobalSecurity.InternalizeL(aStream);

	iDevice.iBasebandParams.InternalizeL(aStream);

	TPckg<TTime> seenPtr(iDevice.iSeen);
	aStream >> seenPtr;
	
	TPckg<TTime> usedPtr(iDevice.iUsed);
	aStream >> usedPtr;
	
	iDevice.iSetMask = aStream.ReadUint32L();
	
	iDevice.iPaired = static_cast<TBool>(aStream.ReadUint32L());
	iDevice.iLinkKeyType = static_cast<TBTLinkKeyType>(aStream.ReadUint32L());
	iDevice.iUiCookie = aStream.ReadUint32L();
	}

/**
Compares two devices.
@param aDevice instance against which this is compared
@return bit field (values of TBTDeviceComparisonResult) describing the similarities.
*/
EXPORT_C TUint CBTDevice::CompareTo(const CBTDevice& aDevice) const
	{
	TUint retval = (iDevice.CompareTo(aDevice.AsNamelessDevice()) | static_cast<TUint>(EAllNameProperties));

	if (!((IsValidDeviceName() && aDevice.IsValidDeviceName() && (*iDeviceName==aDevice.DeviceName()))
		|| (!IsValidDeviceName() && !aDevice.IsValidDeviceName())))
		{
		retval &= ~EDeviceName;
		}
	if (!((IsValidFriendlyName() && aDevice.IsValidFriendlyName() && (*iFriendlyName==aDevice.FriendlyName()))
		|| (!IsValidFriendlyName() && !aDevice.IsValidFriendlyName())))
		{
		retval &= ~EFriendlyName;
		}
	return retval;
	}

/**
Effectively operator=, but can leave.
@param aDevice  The device details to assign to this object.
@leave This method will leave if an error occurs.
*/
EXPORT_C void CBTDevice::AssignL(CBTDevice& aDevice)
	{
	// set the super class bits
	aDevice.iDevice.Update(AsNamelessDevice());

	// now the derived class
	if (IsValidDeviceName())
		aDevice.SetDeviceNameL(DeviceName());
	if (IsValidFriendlyName())
		aDevice.SetFriendlyNameL(FriendlyName());
	}

/**
Copies the contents of aDevice into *this
@param aDevice copied into *this
@leave This method will leave if an error occurs.
*/
EXPORT_C void CBTDevice::UpdateL(const CBTDevice& aDevice)
	{
	iDevice.Update(aDevice.AsNamelessDevice());

	if (aDevice.IsValidDeviceName())
		SetDeviceNameL(aDevice.DeviceName());
	else
		iDevice.iSetMask &= ~EDeviceName;

	if (aDevice.IsValidFriendlyName())
		SetFriendlyNameL(aDevice.FriendlyName());
	else
		iDevice.iSetMask &= ~EFriendlyName;
	}


/**
Get the TBTNamelessDevice portion of this.
@return the TBTNamelessDevice portion of this
*/
EXPORT_C TBTNamelessDevice& CBTDevice::AsNamelessDevice()
	{
	return iDevice;
	}
	
/**
Get the TBTNamelessDevice portion of this.
@return the TBTNamelessDevice portion of this
*/
EXPORT_C const TBTNamelessDevice& CBTDevice::AsNamelessDevice() const
	{
	return iDevice;
	}

//

//TBTNamelessDevice facades
//in 7.0s

/**
Get the device address.
@return the address of the device.
*/
EXPORT_C const TBTDevAddr& CBTDevice::BDAddr() const
	{
	return iDevice.Address();
	}
	
/**
Get the device class.
@return the device class
*/
EXPORT_C TBTDeviceClass CBTDevice::DeviceClass() const
	{
	return iDevice.DeviceClass();
	}
	
/**
Get the link key.
@return the link key
*/
EXPORT_C const TBTLinkKey& CBTDevice::LinkKey() const
	{
	return iDevice.LinkKey();
	}

/**
@publishedPartner
Get the link key type.
@return the link key type
*/
EXPORT_C TBTLinkKeyType CBTDevice::LinkKeyType() const
	{
	return iDevice.LinkKeyType();
	}


/**
Get the global security settings.
@return the global security overrides of this device
*/
EXPORT_C const TBTDeviceSecurity CBTDevice::GlobalSecurity() const
	{
	return iDevice.iGlobalSecurity;
	}
	
/**
Set the device address.
@param aBDAddr the address of the device
*/
EXPORT_C void CBTDevice::SetDeviceAddress(const TBTDevAddr& aBDAddr)
	{
	iDevice.SetAddress(aBDAddr);
	}
	
/**
Set the device class.
@param aDeviceClass the device class of the device
*/
EXPORT_C void CBTDevice::SetDeviceClass(TBTDeviceClass aDeviceClass)
	{
	iDevice.SetDeviceClass(aDeviceClass);
	}
	
/**
Set the link key.
@param aLinkKey the link key of the device
@internalAll
@released
*/
EXPORT_C void CBTDevice::SetLinkKey(const TBTLinkKey& aLinkKey)
	{
	iDevice.SetLinkKey(aLinkKey);
	}

/**
Set the link key type of the device.
@param aLinkKey the link key of the device
@param aLinkKeyType the link key type of the device
@internalAll
@released
*/
EXPORT_C void CBTDevice::SetLinkKey(const TBTLinkKey& aLinkKey, TBTLinkKeyType aLinkKeyType)
	{
	iDevice.SetLinkKey(aLinkKey,aLinkKeyType);
	}

/**
Set the global security settings.
@param aSetting the security override of the device
*/
EXPORT_C void CBTDevice::SetGlobalSecurity(const TBTDeviceSecurity& aSetting)
	{
	iDevice.SetGlobalSecurity(aSetting);
	}

/**
Ensure that the previously known link key is discarded
*/	
EXPORT_C void CBTDevice::DeleteLinkKey()
	{
	iDevice.DeleteLinkKey();
	}
	
/**
Denotes whether the device address has been set.
@return ETrue if address has been set
*/
EXPORT_C TBool CBTDevice::IsValidBDAddr() const
	{
	return iDevice.IsValidAddress();
	}
	
/**
Denotes whether the device class has been set.
@return ETrue if DeviceClass has been set
*/
EXPORT_C TBool CBTDevice::IsValidDeviceClass() const
	{
	return iDevice.IsValidDeviceClass();
	}
	
/**
Denotes whether the link key has been set, and hence whether this is a paired device.
@return ETrue if link key has been set
*/
EXPORT_C TBool CBTDevice::IsValidLinkKey() const
	{
	return iDevice.IsValidLinkKey();
	}
	
/**
Denotes whether the global security settings have been set.
@return ETrue if security override has been set
*/
EXPORT_C TBool CBTDevice::IsValidGlobalSecurity() const
	{
	return iDevice.IsValidGlobalSecurity();
	}
				
/**	
Get the time when the device was last seen.  Note that this feature is not yet implemented in the
bluetooth stack, and the method CBTDevice::Used() should be used instead.
@return The time when the device was last seen.
*/
EXPORT_C const TTime& CBTDevice::Seen() const
	{
	return iDevice.Seen();
	}
		
/**	
Get the time when the device was last connected to.
@return The time when the device was last used.
*/
EXPORT_C const TTime& CBTDevice::Used() const
	{
	return iDevice.Used();
	}
	
/**	
Set the time when the device was last used.
@param aDateTime The time when the device was last used.
*/
EXPORT_C void CBTDevice::SetUsed(const TTime& aDateTime)
	{
	iDevice.SetUsed(aDateTime);
	}
	
/**	
Set the time when the device was last seen.
@param aDateTime The time when the device was last seen.
*/
EXPORT_C void CBTDevice::SetSeen(const TTime& aDateTime)
	{
	iDevice.SetSeen(aDateTime);
	}

		
/**
Denotes whether the time last used has been set.
@return ETrue if time last used has been set
*/
EXPORT_C TBool CBTDevice::IsValidUsed() const
	{
	return iDevice.IsValidUsed();
	}
	
/**
Denotes whether the time last seen has been set.
@return ETrue if time last seen has been set
*/
EXPORT_C TBool CBTDevice::IsValidSeen() const
	{
	return iDevice.IsValidSeen();
	}
/**
Check whether the device is paired.
@return ETrue if the device is paired.
*/
EXPORT_C TBool CBTDevice::IsPaired() const
	{
	return iDevice.IsPaired();
	}

/**
Set whether the device is paired;
This function should no longer be used, the overloaded SetPaired function should be used.
@see CBTDevice::SetPaired(TBTLinkKeyType aLinkKeyType)
@param aPaired Whether the device is paired
*/
EXPORT_C void CBTDevice::SetPaired(TBool aPaired) 
	{
	iDevice.SetPaired(aPaired);
	}

/**
Set whether the link key type when the device is paired;
@param aLinkKeyType the link key type of the paired device
*/
EXPORT_C void CBTDevice::SetPaired(TBTLinkKeyType aLinkKeyType)
	{
	iDevice.SetPaired(aLinkKeyType);
	}


/**
Denotes whether the device is paired has been set.
@return ETrue if whether the device is paired has been set.
*/
EXPORT_C TBool CBTDevice::IsValidPaired() const
	{
	return iDevice.IsValidPaired();
	}

/**
Set the passkey.
@param aPassKey the PIN code of the device
@internalAll
@released
*/
EXPORT_C void CBTDevice::SetPassKey(const TBTPinCode& aPassKey)
	{
	iDevice.SetPassKey(aPassKey);
	}
	
/**	
Get the PIN code
@return The PIN code
*/
EXPORT_C const TBTPinCode& CBTDevice::PassKey() const
	{
	return iDevice.PassKey();
	}

/**
Denotes whether the PIN code has been set.
@return ETrue if whether the PIN code has been set.
*/
EXPORT_C TBool CBTDevice::IsValidPassKey() const
	{
	return iDevice.IsValidPassKey();
	}
	
/**	
Get the PIN code length
@return The PIN code length
*/
EXPORT_C TUint CBTDevice::PassKeyLength() const
	{
	return iDevice.PassKeyLength();
	}

/**
Denotes whether the UI Cookie has been set.
@return ETrue = The UI Cookie for the device been set
@publishedPartner
@released
*/
EXPORT_C TBool CBTDevice::IsValidUiCookie() const
	{
	return iDevice.IsValidUiCookie();
	}

/**
Sets the UI Cookie for the device.  The format of the cookie is
specific to the UI using the field.
@param aUiCookie The 32bit cookie value.
@publishedPartner
@released
*/
EXPORT_C void CBTDevice::SetUiCookie(TUint32 aUiCookie)
	{
	iDevice.SetUiCookie(aUiCookie);
	}

/**
Returns the UI Cookie value associated with the device.  The format
of the cookie is specific to the UI using the field.
@return The 32bit cookie value.
@publishedPartner
@released
*/
EXPORT_C TUint32 CBTDevice::UiCookie() const
	{
	return iDevice.UiCookie();
	}



//



/**
Constructor
*/
EXPORT_C TBTDeviceSecurity::TBTDeviceSecurity()
	: iSecurity(0)
	, iPasskeyMinLength(0)
	{
	}

/**
Constructor with userdefined security
@param aSecurity bitfield describing security settings
@see SecurityValue()
*/
EXPORT_C TBTDeviceSecurity::TBTDeviceSecurity(TUint8 aSecurity)
	: iSecurity(aSecurity)
	, iPasskeyMinLength(0)
	{
	}

/**
Constructor with userdefined security
@param aSecurity bitfield describing security settings
@param aPasskeyMinLength 0..16
@see SecurityValue()
*/
EXPORT_C TBTDeviceSecurity::TBTDeviceSecurity(TUint8 aSecurity, TUint aPasskeyMinLength)
	: iSecurity(aSecurity)
	, iPasskeyMinLength(aPasskeyMinLength)
	{
	}

/**
Constructor with user-defined security
@param aNoAuthenticate Use ETrue if connections with this device should not be authenticated.
@param aNoAuthorise Use ETrue if connections with this device should not be authorised - ie the device is trusted.
@param aEncrypt Use ETrue if connections with this device should be encrypted
@param aBanned Use ETrue if connections with this device should not be allowed
*/
EXPORT_C TBTDeviceSecurity::TBTDeviceSecurity(TBool aNoAuthenticate, TBool aNoAuthorise, TBool aEncrypt, TBool aBanned)
	{
	iSecurity = 0;
	if ( aNoAuthenticate )
		iSecurity |= ENoAuthenticate;
	if ( aNoAuthorise )
		iSecurity |= ENoAuthorise;
	if (aEncrypt)
		iSecurity |= EEncrypt;
	if (aBanned)
		iSecurity |= EBanned;
	}

/**
Constructor with user-defined security
@param aMitmRequirements Use TBTDeviceSecurity::EMitmRequired if connections with this device should require MITM protection.
@param aNoAuthorise Use ETrue if connections with this device should not be authorised - ie the device is trusted.
@param aEncrypt Use ETrue if connections with this device should be encrypted
@param aBanned Use ETrue if connections with this device should not be allowed
*/
EXPORT_C TBTDeviceSecurity::TBTDeviceSecurity(TBTDeviceSecurity::TMitmRequired aMitmRequirements, TBool aNoAuthorise, TBool aEncrypt, TBool aBanned)
	{
	iSecurity = 0;
	if ( aMitmRequirements == EMitmRequired )
		iSecurity |= EMitmProtectionRequired;
	if ( aNoAuthorise )
		iSecurity |= ENoAuthorise;
	if (aEncrypt)
		iSecurity |= EEncrypt;
	if (aBanned)
		iSecurity |= EBanned;
	}

/**
Set (no) authentication
@param aDecision ETrue=>don't authenticate, EFalse=>authenticate
*/
EXPORT_C void TBTDeviceSecurity::SetNoAuthenticate(TBool aDecision)
	{
	if (aDecision)
		iSecurity |= ENoAuthenticate;
	else 
		iSecurity &= ~ENoAuthenticate;
	}

/**
Specifies the man-in-the-middle requirements on a security service.
@param aDecision EMitmRequired if MITM protection is required for connections for a particular
	device, EMitmUnspecified otherwise to use the default service requirements.
*/
EXPORT_C void TBTDeviceSecurity::SetMitmRequirements(TBTDeviceSecurity::TMitmRequired aDecision)
	{
	if (aDecision == EMitmRequired)
		iSecurity |= EMitmProtectionRequired;
	else 
		iSecurity &= ~EMitmProtectionRequired;
	}

/**
Set (no) authorisation (=make trusted)
@param aDecision ETrue=>don't authorise, EFalse=>authorise
*/
EXPORT_C void TBTDeviceSecurity::SetNoAuthorise(TBool aDecision)
	{
	if (aDecision)
		iSecurity |= ENoAuthorise;
	else 
		iSecurity &= ~ENoAuthorise;
	}

/**
Set encryption
@param aDecision ETrue=>encrypt, EFalse=>don't encrypt
*/
EXPORT_C void TBTDeviceSecurity::SetEncrypt(TBool aDecision)
	{
	if (aDecision)
		iSecurity |= EEncrypt;
	else 
		iSecurity &= ~EEncrypt;
	}

/**
Set banned
@param aDecision ETrue=>device is banned, EFalse=>device allowed
*/
EXPORT_C void TBTDeviceSecurity::SetBanned(TBool aDecision)
	{
	if (aDecision)
		iSecurity |= EBanned;
	else 
		iSecurity &= ~EBanned;
	}

/**
Determine whether connections to this device should not be authenticated.
@return ETrue=>don't authenticate, EFalse=>authenticate
*/
EXPORT_C TBool TBTDeviceSecurity::NoAuthenticate() const
	{
	if ( iSecurity & ENoAuthenticate )
		return ETrue;
	else
		return EFalse;
	}

/**
Determine whether connections to this device should have MITM protection.
@return TBTDeviceSecurity::EMitmRequired=>require MITM protection, TBTDeviceSecurity::EMitmUnspecified=>unspecified MITM protection requirement.
*/
EXPORT_C TBTDeviceSecurity::TMitmRequired TBTDeviceSecurity::MitmRequirements() const
	{
	if ( iSecurity & EMitmProtectionRequired )
		return EMitmRequired;
	else
		return EMitmUnspecified;
	}

/**
Determine whether connections to this device should not be authorised.
@return ETrue=>don't authorise, EFalse=>authorisation
*/
EXPORT_C TBool TBTDeviceSecurity::NoAuthorise() const
	{
	if ( iSecurity & ENoAuthorise )
		return ETrue;
	else
		return EFalse;
	}

/**
Determine whether connections to this device should be encrypted.
@return ETrue=>encrypt, EFalse=>don't encrypt
*/
EXPORT_C TBool TBTDeviceSecurity::Encrypt() const
	{
	if ( iSecurity & EEncrypt )
		return ETrue;
	else
		return EFalse;
	}

/**
Determine whether connections to this device should be banned.
@return ETrue=>banned, EFalse=>not banned
*/
EXPORT_C TBool TBTDeviceSecurity::Banned() const
	{
	if ( iSecurity & EBanned )
		return ETrue;
	else
		return EFalse;
	}

/**
Get the raw security settings.
@return 8 bit unsigned value of security settings
*/
EXPORT_C TUint8 TBTDeviceSecurity::SecurityValue() const
	{
	return iSecurity;
	}

/**
Set the raw security settings.
@param aDeviceSecurity 8 bit settings of security
*/
EXPORT_C void TBTDeviceSecurity::SetSecurityValue(TUint8 aValue)
	{
	iSecurity = aValue;
	}

/**
Set passkey min length.
@param aPasskeyMinLength:0..16
*/
EXPORT_C TInt TBTDeviceSecurity::SetPasskeyMinLength(TUint aPasskeyMinLength)
	{
	if (1<=aPasskeyMinLength && aPasskeyMinLength<=KHCIPINCodeSize)
		{
		iPasskeyMinLength = aPasskeyMinLength;
		return KErrNone;
		}
	else
		{	
		iPasskeyMinLength = 0;
		return KErrOverflow;
		}
	}

/**
Get passkey min length.
@return TUint passkey min length
*/
EXPORT_C TUint TBTDeviceSecurity::PasskeyMinLength() const
	{
	return iPasskeyMinLength;
	}

/**
Comparison operator
@param aDeviceSecurity instance of class with which to compare
@return ETrue if instances equal
*/
EXPORT_C TBool TBTDeviceSecurity::operator==(const TBTDeviceSecurity& aSetting) const
	{
	if ( iSecurity == aSetting.iSecurity && iPasskeyMinLength == aSetting.iPasskeyMinLength )
		return ETrue;
	else
		return EFalse;
	}

/**
Inequality operator
@param aDeviceSecurity instance of class with which to compare
@return ETrue if instances not equal
*/
EXPORT_C TBool TBTDeviceSecurity::operator!=(const TBTDeviceSecurity& aSetting) const
	{
	return !(*this==aSetting);
	}


/**
Externalise object.
@param aStream The stream to which the object shall be externalised.
@leave This method will leave if an error occurs.
*/
EXPORT_C void TBTDeviceSecurity::ExternalizeL(RWriteStream& aStream) const
	{
	aStream.WriteUint32L(KStreamVersion);
	aStream.WriteUint8L(iSecurity);
	aStream.WriteUint32L(iPasskeyMinLength);
	}

/**
Internalise object.
@param aStream The stream from which the object shall be internalised.
@leave This method will leave if an error occurs.
*/
EXPORT_C void TBTDeviceSecurity::InternalizeL(RReadStream& aStream)
	{
	TUint32 version		= aStream.ReadUint32L();
	static_cast<void>(version); // Currently only one valid version.
	iSecurity			= aStream.ReadUint8L();
	iPasskeyMinLength	= aStream.ReadUint32L();
	}


/**
Constructor
@param aDevice	The device to which the security applies
@param aSecuritySettings The settings for the device
@see TBTDeviceSecurity
*/
EXPORT_C TBTServiceSecurityPerDevice::TBTServiceSecurityPerDevice(const TBTDevAddr& aAddress, const TBTDeviceSecurity& aSecSettings)
 : iDeviceAddress(aAddress), iDeviceSecurity(aSecSettings)
	{
	}

/**
Constructor
*/
EXPORT_C TBTServiceSecurityPerDevice::TBTServiceSecurityPerDevice()
	{
	}


/**
Copy constructor.
@param aData The object to be copied.
*/
EXPORT_C TBTServiceSecurityPerDevice::TBTServiceSecurityPerDevice(const TBTServiceSecurityPerDevice& aData) :
	iDeviceAddress(aData.iDeviceAddress), iDeviceSecurity(aData.iDeviceSecurity)	
	{
	}


/**
Reset the device security
@param aSecuritySettings the new security settings
*/
EXPORT_C void TBTServiceSecurityPerDevice::SetDeviceSecurity(const TBTDeviceSecurity& aSecuritySettings)
	{
	iDeviceSecurity = aSecuritySettings;
	}

/**
Reset device address 
@param aAddress The new address
*/
EXPORT_C void TBTServiceSecurityPerDevice::SetAddress(const TBTDevAddr& aAddress)
	{
	iDeviceAddress = aAddress;
	}

/**
Get the device security.
@return The device security
*/
EXPORT_C const TBTDeviceSecurity& TBTServiceSecurityPerDevice::DeviceSecurity() const
	{
	return iDeviceSecurity;
	}

/**
Get the device address.
@return The device address
*/
EXPORT_C const TBTDevAddr& TBTServiceSecurityPerDevice::DeviceAddress() const
	{
	return iDeviceAddress;
	}

/**
Assignment operator
@param aServiceSecurityPerDevice The instance to which this will be assigned
*/
EXPORT_C void TBTServiceSecurityPerDevice::operator=(const TBTServiceSecurityPerDevice& aData)
	{
	iDeviceAddress = aData.iDeviceAddress;
	iDeviceSecurity = aData.iDeviceSecurity;
	}

/**
Equality operator
@param aServiceSecurityPerDevice The instance to which this will be compared
@return ETrue if this equals aServiceSecurityPerDevice
*/
EXPORT_C TBool TBTServiceSecurityPerDevice::operator==(const TBTServiceSecurityPerDevice& aService) const
	{
	if (( iDeviceAddress == aService.iDeviceAddress) && (iDeviceSecurity == aService.iDeviceSecurity ))
		return ETrue;
	else
		return EFalse;
	}

/**
Inequality operator
@param aServiceSecurityPerDevice The instance to which this will be compared
@return ETrue if this does not equal aServiceSecurityPerDevice
*/
EXPORT_C TBool TBTServiceSecurityPerDevice::operator!=(const TBTServiceSecurityPerDevice& aService) const
	{
	return !(*this==aService);
	}

/**
Externalise object.
@param aStream The stream to which the object shall be externalized.
@leave This method will leave if an error occurs.
*/
EXPORT_C void TBTServiceSecurityPerDevice::ExternalizeL(RWriteStream& aStream) const
	{
	aStream.WriteUint32L(KStreamVersion);
	iDeviceSecurity.ExternalizeL(aStream);
	aStream.WriteL(iDeviceAddress.Des());
	}

/**
Internalise object.
@param aStream The stream from which the object shall be internalized.
@leave This method will leave if an error occurs.
*/
EXPORT_C void TBTServiceSecurityPerDevice::InternalizeL(RReadStream& aStream)
	{
	TUint32 version = aStream.ReadUint32L();
	static_cast<void>(version); // Currently only one valid version.
	iDeviceSecurity.InternalizeL(aStream);

	TPtr8 addr (iDeviceAddress.Des());
	aStream >> addr;
	}

/**
Externalise object.
@param aStream The stream to which the object shall be externalized.
@leave This method will leave if an error occurs.
*/
void TBTNamelessDevice::TBTBasebandParameters::ExternalizeL(RWriteStream& aStream) const
	{
	aStream.WriteUint32L(KStreamVersion);
	aStream.WriteUint8L(iPageScanRepetitionMode);
	aStream.WriteUint8L(iPageScanMode);
	aStream.WriteUint8L(iPageScanPeriodMode);
	aStream.WriteUint16L(iClockOffset);
	}

/**
Internalise object.
@param aStream The stream from which the object shall be internalized.
@leave This method will leave if an error occurs.
*/
void TBTNamelessDevice::TBTBasebandParameters::InternalizeL(RReadStream& aStream)		
	/**
	Internalise class
	*/
	{
	TUint32 version = aStream.ReadUint32L();
	static_cast<void>(version); // Currently only one version is available
	iPageScanRepetitionMode = aStream.ReadUint8L();
	iPageScanMode = aStream.ReadUint8L();
	iPageScanPeriodMode = aStream.ReadUint8L();
	iClockOffset = aStream.ReadUint16L();
	}


// AFH Host Channel Classification

const TUint8 KNumBitIndexBits=3;
const TUint8 KBitsInByte=1<<KNumBitIndexBits;
const TUint8 KBitIndexBitsMask=KBitsInByte-1;


/**	Constructor
	
@see TBuf8
*/
EXPORT_C TBTAFHHostChannelClassification::TBTAFHHostChannelClassification()
: TBuf8<KHCIAFHHostChannelClassificationSize>()
	{
	Reset();
	}


/**Resets descriptor to default value 

The default value is that in which there are no bad channels in 79 channel set.
Note: most significant bit is reserved and set to zero.
*/
EXPORT_C void TBTAFHHostChannelClassification::Reset()
	{
	Fill(0xff,KHCIAFHHostChannelClassificationSize);
	(*this)[0] &= ~(1<<(KBitsInByte-1)); //set most significant reserved bit to 0
	}


/**Check that this AFH host channel classification conforms to the spec.

Checks that at least 20 channels are NOT set to bad, as
required in Bluetooth specification.
Also checks length is ten bytes.

@return an error (KErrNone if this is valid)
*/
EXPORT_C TInt TBTAFHHostChannelClassification::Validate() const
	{ 
	if(Length()<KHCIAFHHostChannelClassificationSize)
		{
		return KErrNotSupported;
		}
	if(Length()>KHCIAFHHostChannelClassificationSize)
		{
		return KErrNotSupported;
		}

	TUint8 bitCounter = NumUnknownsWithinRange(0, KAFHNumChannels-1);
	if(bitCounter<KAFHMinUnknownChannels)
		{
		return KErrUnderflow;
		}
		
	return KErrNone;
	}


/**Set a channel in this TBTAFHHostChannelClassification as 'bad' or busy.
	
Channels are represented by the bits in the TBTAFHHostChannelClassification descriptor.
The most significant leftmost bit is always set to 0.
The next most significant bit represents channel 78
The least significant rightmost bit represents channel 0
SetChannelBad unsets (i.e. sets to 0) the bit specified by 
the parameter 'aChannel'.

@param aChannel The number (in the range [0,78]) of the channel to be set as bad
@return an error (KErrNone implies success)
*/
EXPORT_C TInt TBTAFHHostChannelClassification::SetChannelBad(TUint8 aChannel)
	{
	return SetChannelRangeBad(aChannel, aChannel);
	}


/**Set a range of channels in this TBTAFHHostChannelClassification as 'bad' or busy.
	
Channels are represented by the bits in the TBTAFHHostChannelClassification descriptor.
The most significant leftmost bit is always set to 0.
The next most significant bit represents channel 78
The least significant rightmost bit represents channel 0
SetChannelRangeBad unsets (i.e. sets to 0) all bits in the range 
['aChannelRangeLowest','aChannelRangeHighest'], that is all bits 
between and including the bit repesenting 'aChannelRangeLowest' and
the bit representing 'aChannelRangeHighest'.

@param aChannelRangeLowest The lowest channel number representing a channel to be set as bad
@param aChannelRangeHighest The highest channel number representing a channel to be set as bad
@return an error (KErrNone implies success)
*/
EXPORT_C TInt TBTAFHHostChannelClassification::SetChannelRangeBad(const TUint8 aChannelRangeLowest, const TUint8 aChannelRangeHighest)
	{
	__ASSERT_DEBUG(aChannelRangeLowest <= aChannelRangeHighest, User::Panic(_L("AFH Bad Channel Range"), KErrArgument));
	if(aChannelRangeLowest > aChannelRangeHighest)
		{
		return KErrArgument;
		}
	//count how many 'unknowns' exist outside range to be set as 'bad'
	TUint8 bitCounter = static_cast<TUint8>(NumUnknownsWithinRange(0, KAFHNumChannels-1)
							- NumUnknownsWithinRange(aChannelRangeLowest, aChannelRangeHighest)); //cast because msvc6 complains otherwise
	if(bitCounter<KAFHMinUnknownChannels) 
		{
		return KErrUnderflow;
		}
	TInt ret = KErrNone;
	for(TUint8 i=aChannelRangeLowest;i<=aChannelRangeHighest; ++i)
		{
		ret = DoSetChannelBad(i);
		if(ret!=KErrNone)
			{
			return ret;
			}
		}
	return KErrNone;
	}

TInt TBTAFHHostChannelClassification::BitByteIndices(TUint8& aByteIndex, TUint8& aBitIndex, const TUint8 aChannel) const
	//Actually set 'aChannel' as bad.
	{ 
	__ASSERT_DEBUG(aChannel<KAFHNumChannels, User::Panic(_L("AFH Bad Channel Number"), KErrArgument));
	//Numbering bytes from left of descriptor, bits from right of byte, we have:-
	//aChannel = 79  -> byte 0,  bit 7 most significant bit (reserved 0)
	//aChannel = 78  -> byte 0,  bit 6
	//aChannel = 77  -> byte 0,  bit 5
	//aChannel = 72  -> byte 0,  bit 0
	//aChannel = 71  -> byte 1,  bit 7
	//aChannel = 0   -> byte 9,  bit 0 least significant bit 
	//                                 referred in spec to bit 0
	//Extract byte index from 'aChannel' (right to left)
	aByteIndex = static_cast<TUint8>(aChannel >> KNumBitIndexBits); //cast because msvc6 complains otherwise
	//Change to go left to right
	aByteIndex = static_cast<TUint8>(KHCIAFHHostChannelClassificationSize - 1 - aByteIndex); //cast because msvc6 complains otherwise
	//Extract bit index from 'aChannel' 
	aBitIndex = static_cast<TUint8>(aChannel & KBitIndexBitsMask); //cast because msvc6 complains otherwise
	if(aByteIndex>=KHCIAFHHostChannelClassificationSize||aBitIndex>=KBitsInByte)
		{
		#ifdef __DEBUG
	    __BTDEBUGGER();
	    #endif
	    return KErrArgument;
		}
	return KErrNone;
	}
	
TInt TBTAFHHostChannelClassification::DoSetChannelBad(const TUint8 aChannel)
	//Actually set 'aChannel' as bad.
	{ 
	__ASSERT_DEBUG(aChannel<KAFHNumChannels, User::Panic(_L("AFH Bad Channel Number"), KErrArgument));
	TUint8 byteIndex;
	TUint8 bitIndex;
	TInt ret = BitByteIndices(byteIndex, bitIndex, aChannel);
	if(ret==KErrNone)
		{
		(*this)[byteIndex] &= ~(1<<bitIndex); //Set bad channel bit to zero
		}
	return ret;
	}

TUint8 TBTAFHHostChannelClassification::NumUnknownsWithinRange(const TUint8 aChannelRangeLowest, const TUint8 aChannelRangeHighest) const
	//Count bits set to 1 within range
	{
	TUint8 bitCounter = 0;
	for(TUint8 i = aChannelRangeLowest;i<=aChannelRangeHighest;++i)
		{
		TUint8 byteIndex;
		TUint8 bitIndex;
		TInt ret = BitByteIndices(byteIndex, bitIndex, i);
		if(ret==KErrNone)
			{
			if((*this)[byteIndex] & 1<<bitIndex)
				{
				bitCounter++;
				}
			}
		}
	return bitCounter;
	}



// local device

/**
Constructor
*/	
EXPORT_C TBTLocalDevice::TBTLocalDevice()
:iSetMask(0), iSimpleSettings(0)
//NB Zeroing iSimpleSettings is not strictly necessary but helps with debugging.
	{
	}

/**
Get the address of the local device.
@return Address of local device
*/
EXPORT_C const TBTDevAddr& TBTLocalDevice::Address() const
	{
	return iAddress;
	}

/**
Get the device class.
@return DeviceClass (as uint32)
@see TBTDeviceClass constructor
*/
EXPORT_C TUint32 TBTLocalDevice::DeviceClass() const
	{
	return iCod;
	}

/**
Get the device name.
@return name of local device
*/
EXPORT_C const TDesC8& TBTLocalDevice::DeviceName() const
	{
	return iLocalName;
	}

/**
Get the scanning mode.
@return the scanning of the local device
*/
EXPORT_C THCIScanEnable TBTLocalDevice::ScanEnable() const
	{
	return iScanEnable;
	}

/**
Get the limited discoverable mode.
@return ETrue if local device is limited discoverable, otherwise EFalse
*/
EXPORT_C TBool TBTLocalDevice::LimitedDiscoverable() const
	{
	return iLimitedDiscoverable;
	}

/**
Get the power setting.
@return The power setting of the local device
@released
*/
EXPORT_C TUint8 TBTLocalDevice::PowerSetting() const
	{
	return iPowerSetting;
	}

/**
Get the AFH channel assessment mode.
@return ETrue if local device has AFHChannelModeAssessment switched on (i.e. available for use), otherwise EFalse
*/
EXPORT_C TBool TBTLocalDevice::AFHChannelAssessmentMode() const
	{
	return (iSimpleSettings & EAFHChannelAssessmentModeValue)?ETrue:EFalse;
	}

/**
Get the setting for whether to accept all incoming connections or 
just those from paired devices. Note this is in fact a setting in the stack not the controller.
@return ETrue if local device is set only to accept incoming connections from paired devices, otherwise EFalse
*/
EXPORT_C TBool TBTLocalDevice::AcceptPairedOnlyMode() const
	{
	return (iSimpleSettings & EAcceptPairedOnlyModeValue)?ETrue:EFalse;
	}

/**
Set the device address.
@param aAddr the local device address
*/
EXPORT_C void TBTLocalDevice::SetAddress(const TBTDevAddr& aAddr)
	{
	iAddress = aAddr;
	iSetMask |= EAddress;
	}

/**
Set the device class.
@param aCod the local device class
*/
EXPORT_C void TBTLocalDevice::SetDeviceClass(TUint32 aCod)
	{
	iCod = aCod;
	iSetMask |= ECoD;
	}

/**
Set the device name.
@param aName the local device name
*/
EXPORT_C void TBTLocalDevice::SetDeviceName(const TDesC8& aName)
	{
	iLocalName = aName;
	iSetMask |= EDeviceName;
	}

/**
Enable local device scanning.
@param aEnable the local device scanning
*/
EXPORT_C void TBTLocalDevice::SetScanEnable(THCIScanEnable aEnable)
	{
	iScanEnable = aEnable;
	iSetMask |= EScanEnable;
	}

/**
Set limited discoverable mode.
@param aOn the local device limited discoverable-ness
*/
EXPORT_C void TBTLocalDevice::SetLimitedDiscoverable(TBool aOn)
	{
	iLimitedDiscoverable = aOn;
	iSetMask |= ELimitedDiscoverable;
	}

/**
Set the power setting.
@param aPowerSetting the local device power setting
*/
EXPORT_C void  TBTLocalDevice::SetPowerSetting(TUint8 aPowerSetting)
	{
	iPowerSetting = aPowerSetting;
	iSetMask |= EPowerSetting;
	}

/**
Set the AFH channel assessment mode.
@param aOn the local device channel assessment mode is switched on (available for use)
*/
EXPORT_C void TBTLocalDevice::SetAFHChannelAssessmentMode(TBool aOn)
	{
	if(aOn)
		{
		iSimpleSettings |= EAFHChannelAssessmentModeValue;
		}
	else
		{
		iSimpleSettings &= ~EAFHChannelAssessmentModeValue;
		}
			
	iSetMask |= EAFHChannelAssessmentMode;
	}

/**
Set the paired device only mode. This is a stack setting which makes
the decision as to whether to accept connection requests ONLY from
paired devices (rejecting connection requests from unpaired devices).
@param aOn the local stack will ONLY accept connection requests from paired devices
*/
EXPORT_C void TBTLocalDevice::SetAcceptPairedOnlyMode(TBool aOn)
	{
	if(aOn)
		{
		iSimpleSettings |= EAcceptPairedOnlyModeValue;
		}
	else
		{
		iSimpleSettings &= ~EAcceptPairedOnlyModeValue;
		}
			
	iSetMask |= EAcceptPairedOnlyMode;
	}

/**
Denotes whether the address has been set.
@return ETrue if Address has been set
*/
EXPORT_C TBool TBTLocalDevice::IsValidAddress() const
	{
	return (iSetMask & EAddress);
	}

/**
Denotes whether the device class has been set.
@return ETrue if DeviceClass has been set
*/
EXPORT_C TBool TBTLocalDevice::IsValidDeviceClass() const
	{
	return (iSetMask & ECoD);
	}

/**
Denotes whether the device name has been set.
@return ETrue if DeviceName has been set
*/
EXPORT_C TBool TBTLocalDevice::IsValidDeviceName() const
	{
	return (iSetMask & EDeviceName);
	}

/**
Denotes whether the scan enable has been set.
@return ETrue if ScanEnable has been set
*/
EXPORT_C TBool TBTLocalDevice::IsValidScanEnable() const
	{
	return (iSetMask & EScanEnable);
	}

/**
Denotes whether the limited discoverable mode has been set.
@return ETrue if LimitedDiscoverble has been set
*/
EXPORT_C TBool TBTLocalDevice::IsValidLimitedDiscoverable() const
	{
	return (iSetMask & ELimitedDiscoverable);
	}

/**
Denotes whether the power setting has been set.
@return ETrue if Power setting has been set
*/
EXPORT_C TBool TBTLocalDevice::IsValidPowerSetting() const
	{
	return (iSetMask & EPowerSetting);
	}

/**
Denotes whether the AFH channel assessment mode has been set.
@return ETrue if AFH channel assessment mode has been set
*/
EXPORT_C TBool TBTLocalDevice::IsValidAFHChannelAssessmentMode() const
	{
	return (iSetMask & EAFHChannelAssessmentMode);
	}

/**
Denotes whether "Accept Paired Only Mode" has been set.
@return ETrue if "Accept Paired Only Mode" has been set
*/
EXPORT_C TBool TBTLocalDevice::IsValidAcceptPairedOnlyMode() const
	{
	return (iSetMask & EAcceptPairedOnlyMode);
	}

