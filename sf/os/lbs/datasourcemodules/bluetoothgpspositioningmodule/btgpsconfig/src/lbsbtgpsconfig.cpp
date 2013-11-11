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
// LBS Bluetooth GPS Configuration API
//

#include <lbs/lbsbtgpsconfig.h>
#include "lbsbtgpsconfigimpl.h"


/** Get the observer interface version
@return The current version of the observer interface
@publishedPartner
@prototype
*/
EXPORT_C TVersion MLbsBtGpsConfigObserver::Version() const
	{
	return TVersion(1, 0, 0);
	}
	

/**	This methods is reserved for future expansion and should not be used

@param aFunctionNumber Reserved for future expansion
@param aPtr1 Reserved for future expansion
@param aPtr2 Reserved for future expansion

Currently not implemented.

*/								
EXPORT_C TAny* MLbsBtGpsConfigObserver::ExtendedInterface(TInt /*aFunctionNumber*/, TAny* /*aPtr1*/, TAny* /*aPtr2*/)
	{
	return NULL;
	}


/** Static constructor (without observer)
Creates an instance of the LBS Bluetooth GPS Configuration API.
This version is for clients that do not require configuration update notifications.

@return A newly constructed CLbsBtGpsConfig object.

@leave KErrPermissionDenied Client does not have the required capabilities.

@capability LocalServices

@publishedPartner
@prototype
*/
EXPORT_C CLbsBtGpsConfig* CLbsBtGpsConfig::NewL()
	{
	CLbsBtGpsConfig* self = new (ELeave) CLbsBtGpsConfig();
    CleanupStack::PushL(self);
    self->ConstructL(NULL);
    CleanupStack::Pop(self);
    return self;
	}


/** Static constructor (with observer)
Creates an instance of the LBS Bluetooth GPS Configuration API, specifying a callback
to recieve update notifications.

@param aObserver [IN] Observer which will recieve update notifications.

@return A newly constructed CLbsBtGpsConfig object.

@leave KErrPermissionDenied Client does not have the required capabilities.

@see MLbsBtGpsConfigObserver

@capability LocalServices

@publishedPartner
@prototype
*/
EXPORT_C CLbsBtGpsConfig* CLbsBtGpsConfig::NewL(MLbsBtGpsConfigObserver& aObserver)
	{
	CLbsBtGpsConfig* self = new (ELeave) CLbsBtGpsConfig();
    CleanupStack::PushL(self);
    self->ConstructL(&aObserver);
    CleanupStack::Pop(self);
    return self;
	}


/** Destructor 
@publishedPartner
@prototype
*/
EXPORT_C CLbsBtGpsConfig::~CLbsBtGpsConfig()
	{
	delete iImpl;
	}


/** Returns the number of devices present in the list

@param aCount [Out] Revieves the number of devices in the list.

@return KErrNone if the operation succeeded.
		KErrPermissionDenied if the client does not have the required capabilities.

@capability LocalServices

@publishedPartner
@prototype
*/
EXPORT_C TInt CLbsBtGpsConfig::GetDeviceCount(TInt& aCount)
	{
	return iImpl->GetDeviceCount(aCount);
	}


/** Adds a new device at the specified position index within the list

@param aAddress [In] Address of the new device.
@param aPosition [In] Position in the list at which to add the device (where 0 indicates the start of the list).
@param aKey [Out] Recieves the unique key assigned to identify the new device.

The new device will assume the specified position in the list, shifting exsiting devices at this position or below 
one place towards the end of the list in order to achieve this.

Specifying aPosition = 0 will result in the device being added to the start of the list.
Specifying aPosition >= DeviceCount() will result in the device being added to the end of the list.

@return KErrNone if the operation succeeded.
		KErrAlreadyExists if an entry with the same address is already present in the list.
		KErrOverflow if there is no space left to add another entry.
		KErrPermissionDenied if the client does not have the required capabilities.

@capability LocalServices
@capability WriteDeviceData

@publishedPartner
@prototype
*/
EXPORT_C TInt CLbsBtGpsConfig::AddDevice(const TBTDevAddr& aAddress, TInt aPosition, TLbsBtGpsEntryKey& aKey)
	{
	return iImpl->AddDevice(aAddress, aPosition, aKey);
	}


/** Removes a device from the list

@param aKey [In] The key identifying the device.

@return KErrNone if the operation succeeded.
		KErrNotFound if the specified device is not present in the list.
		KErrPermissionDenied if the client does not have the required capabilities.

@capability LocalServices
@capability WriteDeviceData

@publishedPartner
@prototype
*/
EXPORT_C TInt CLbsBtGpsConfig::RemoveDevice(TLbsBtGpsEntryKey aKey)
	{
	return iImpl->RemoveDevice(aKey);
	}


/** Empties the device list, removing all device entries

@return KErrNone if the operation succeeded.
		KErrPermissionDenied if the client does not have the required capabilities.

@capability LocalServices
@capability WriteDeviceData

@publishedPartner
@prototype
*/
EXPORT_C TInt CLbsBtGpsConfig::EmptyDeviceList()
	{
	return iImpl->EmptyDeviceList();
	}


/** Reorders a device in the list by specifying a new position index

@param aKey [In] The key identifying the device.
@param aNewPosition [In] New position index for the device in the list (where 0 indicates the start of the list).

The device identified by the key will assume the specified position in the list, shifting exsiting devices as
required in order to achieve this.

Specifying aNewPosition = 0 will result in the device being moved to the start of the list.
Specifying aNewPosition >= DeviceCount() will result in the device being moved to the end of the list.

When a device is moved towards the start of the list, other devices located between the old and new positions 
will be shifted towards the end of the list by one place. For example, starting with a list {A,B,C,D,E}, moving 
device D to position 1 would result in {A,D,B,C,E}. 

Similarly, when a device is moved towards the end of the list, other devices located between the old and new
positions will be shifted towards the start of the list by one place. For example, starting with a list {A,B,C,D,E},
moving device B to position 3 would result in {A,C,D,B,E}

@return KErrNone if the operation succeeded.
		ErrNotFound if the specified device is not present in the list.
		KErrPermissionDenied if the client does not have the required capabilities.

@capability LocalServices
@capability WriteDeviceData

@publishedPartner
@prototype
*/
EXPORT_C TInt CLbsBtGpsConfig::ReorderDevice(TLbsBtGpsEntryKey aKey, TInt aNewPosition)
	{
	return iImpl->ReorderDevice(aKey, aNewPosition);
	}


/** Retrieves the list of devices

@param aList [Out] Reference to an empty RPointerArray to populate with device entries from the list.

The passed list is populated with TLbsBtGpsDeviceInfo objects containing information for each device
in the list. Ownership of these objects is returned, so they should be deleted by the client when they
are no longer needed (this can be done easily using RPointerArray::ResetAndDestroy).

@leave KErrPermissionDenied if the client does not have the required capabilities.

@capability LocalServices

@publishedPartner
@prototype
*/
EXPORT_C void CLbsBtGpsConfig::GetDeviceListL(RPointerArray<TLbsBtGpsDeviceInfo>& aList)
	{
	iImpl->GetDeviceListL(aList);
	}
	

/** Class constructor 
*/
CLbsBtGpsConfig::CLbsBtGpsConfig()
	{
	
	}


/** Second phase constructor 

@param aObserver [IN] Optional observer, which will recieve update notifications.

@leave KErrPermissionDenied Client does not have the required capabilities.

*/
void CLbsBtGpsConfig::ConstructL(MLbsBtGpsConfigObserver* aObserver)
	{
	iImpl = CLbsBtGpsConfigImpl::NewL(aObserver);
	}


/**	This methods is reserved for future expansion and should not be used

@param aFunctionNumber Reserved for future expansion
@param aPtr1 Reserved for future expansion
@param aPtr2 Reserved for future expansion

@capability LocalServices

*/								
EXPORT_C TAny* CLbsBtGpsConfig::ExtendedInterface(TInt aFunctionNumber, TAny* aPtr1, TAny* aPtr2)
	{
	return iImpl->ExtendedInterface(aFunctionNumber, aPtr1, aPtr2);
	}
