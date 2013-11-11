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
// LBS Bluetooth GPS Configuration API implementation
//

#include "lbsbtgpsconfigimpl.h"
#include <centralrepository.h>

/** LBS Bluetooth GPS Configuration central repository UID
@internalComponent
*/
const TUint32 KLbsBtGpsConfigRepositoryUid = 0x102869C1;

//-------------------------------------------------------
// CENREP FORMAT
//
// key format: 0xAAAABBBB
// where:
//   AAAA = Device index
//   BBBB = Setting index

const TUint32 KDeviceIndexMask			= 0xFFFF0000;
const TUint32 KDeviceIndexShift			= 16;

// Maximum number of devices allowed in the list
const TUint32 KMaxDevices 				= 20;

// Index of first device in the list
const TUint32 KFirstDeviceIndex 		= 0x0001;

// GENERAL SETTINGS (stored under device index 0000)

// Centrep data version
//const TUint32 KSettingVersion 			= 0x00000000;

// Stores the last allocated unique device key
const TUint32 KSettingLastKey 			= 0x00000001;

// Stores the number of devices in the list
const TUint32 KSettingNumDevices 		= 0x00000002;

// PER-DEVICE SETTINGS (BBBB)

// Unique key identifying the device
const TUint32 KDeviceSettingKey			= 0x0000;

// Device type
const TUint32 KDeviceSettingType		= 0x0001;

// Device bluetooth socket address
const TUint32 KDeviceSettingSockAddr	= 0x0002;
//-------------------------------------------------------

// Partial key and mask for centrep notifications - notify on all changes
const TUint32 KNotifyPartialKey = 0x00000000;
const TUint32 KNotifyMask = 0x00000000;


// Number of times to retry if the repository is locked
const TInt KLockedRetries = 10000;

// Time to wait before retrying if the repository is locked
const TInt KLockedRetryDelay = 100;


/** Static constructor (with observer)
Creates an instance of the LBS Bluetooth GPS Configuration API implementation, 
specifying a callback to recieve update notifications.

@param aObserver [IN] Observer which will recieve update notifications.

@return A newly constructed CLbsBtGpsConfigImpl object.

@capability LocalServices

@internalComponent
*/
CLbsBtGpsConfigImpl* CLbsBtGpsConfigImpl::NewL(MLbsBtGpsConfigObserver* aObserver)
	{
	// Check capabilities first
	RProcess process;
	if (!process.HasCapability(ECapabilityLocalServices))
		{
		User::Leave(KErrPermissionDenied);
		}
	
	CLbsBtGpsConfigImpl* self = new (ELeave) CLbsBtGpsConfigImpl(aObserver);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
	}


/** Destructor 
@internalComponent
*/
CLbsBtGpsConfigImpl::~CLbsBtGpsConfigImpl()
	{
	Cancel();
	delete iRepos;
	}


/** Returns the number of devices present in the list

@return Number of devices.

@leave KErrPermissionDenied if the client does not have the required capabilities.

@capability LocalServices

@internalComponent
*/
TInt CLbsBtGpsConfigImpl::GetDeviceCount(TInt& aCount)
	{
	// Check capabilities first
	RProcess process;
	if (!process.HasCapability(ECapabilityLocalServices))
		{
		return KErrPermissionDenied;
		}

	TInt err = iRepos->Get(KSettingNumDevices, aCount); 
	if (err == KErrNotFound)
		{
		// If the entry is not found, the number of devices is zero.
		// Don't treat this as an error.
		aCount = 0;
		err = KErrNone;
		}
	
	return err;
	}


/** Adds a new device at the specified position index within the list

@param aAddress [In] Address of the new device.
@param aPosition [In] Position in the list at which to add the device, note that any existing entries below this position will be shifted down.
@param aKey [Out] Recieves the unique key assigned to identify the new device.

Specifying aPosition >= DeviceCount() will result in the device being added to the end of the list.

@return KErrNone if the operation succeeded.
		KErrAlreadyExists if an entry with the same address is already present in the list.
		KErrOverflow if there is no space left to add another entry.
		KErrPermissionDenied if the client does not have the required capabilities.

@capability LocalServices
@capability WriteDeviceData

@internalComponent
*/
TInt CLbsBtGpsConfigImpl::AddDevice(const TBTDevAddr& aAddress, TInt aPosition, TLbsBtGpsEntryKey& aKey)
	{
	// Check capabilities first
	RProcess process;
	if (!process.HasCapability(ECapabilityLocalServices))
		{
		return KErrPermissionDenied;
		}
	if (!process.HasCapability(ECapabilityWriteDeviceData))
		{
		return KErrPermissionDenied;
		}
	 
	TRAPD(err, AddDeviceL(aAddress, aPosition, aKey));
	return err;
	}


/** Removes a device from the list

@param aKey [In] The key identifying the device.

@return KErrNone if the operation succeeded.
		KErrNotFound if the specified device is not present in the list.
		KErrPermissionDenied if the client does not have the required capabilities.

@capability LocalServices
@capability WriteDeviceData

@internalComponent
*/
TInt CLbsBtGpsConfigImpl::RemoveDevice(TLbsBtGpsEntryKey aKey)
	{
	// Check capabilities first
	RProcess process;
	if (!process.HasCapability(ECapabilityLocalServices))
		{
		return KErrPermissionDenied;
		}
	if (!process.HasCapability(ECapabilityWriteDeviceData))
		{
		return KErrPermissionDenied;
		}

	TRAPD(err, RemoveDeviceL(aKey));
	return err;
	}


/** Empties the device list, removing all device entries

@return KErrNone if the operation succeeded.
		KErrPermissionDenied if the client does not have the required capabilities.

@capability LocalServices
@capability WriteDeviceData

@internalComponent
*/
TInt CLbsBtGpsConfigImpl::EmptyDeviceList()
	{
	// Check capabilities first
	RProcess process;
	if (!process.HasCapability(ECapabilityLocalServices))
		{
		return KErrPermissionDenied;
		}
	if (!process.HasCapability(ECapabilityWriteDeviceData))
		{
		return KErrPermissionDenied;
		}

	TRAPD(err, EmptyDeviceListL());
	return err;
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

@internalComponent
*/
TInt CLbsBtGpsConfigImpl::ReorderDevice(TLbsBtGpsEntryKey aKey, TInt aNewPosition)
	{
	// Check capabilities first
	RProcess process;
	if (!process.HasCapability(ECapabilityLocalServices))
		{
		return KErrPermissionDenied;
		}
	if (!process.HasCapability(ECapabilityWriteDeviceData))
		{
		return KErrPermissionDenied;
		}

	TRAPD(err, ReorderDeviceL(aKey, aNewPosition));
	return err;
	}


/** Retrieves the list of devices

@param aList [Out] Reference to an empty RPointerArray to populate with device entries from the list.

@leave KErrPermissionDenied if the client does not have the required capabilities.

@capability LocalServices

@internalComponent
*/
void CLbsBtGpsConfigImpl::GetDeviceListL(RPointerArray<TLbsBtGpsDeviceInfo>& aList)
	{
	// Check capabilities first
	RProcess process;
	if (!process.HasCapability(ECapabilityLocalServices))
		{
		User::Leave(KErrPermissionDenied);
		}

	// Attempt to start a transaction
	User::LeaveIfError(StartTransactionL(CRepository::EReadTransaction));

	TInt deviceCount;
	User::LeaveIfError(GetDeviceCount(deviceCount));

	for (TInt index=0; index<deviceCount; ++index)
		{
		
		TLbsBtGpsDeviceRecord devRecord;
		
		if (ReadDeviceRecord(index, devRecord) == KErrNone)
			{
			TLbsBtGpsDeviceInfo* devInfo = new (ELeave) TLbsBtGpsDeviceInfo();
			CleanupStack::PushL(devInfo);
			devInfo->SetKey(devRecord.Key());
			devInfo->SetAddress(devRecord.SocketAddress().BTAddr());
			devInfo->SetType(devRecord.Type());
			aList.AppendL(devInfo);
			CleanupStack::Pop(devInfo);
			}
		
		}

	// End central repository transaction
	User::LeaveIfError(EndTransaction());
	}
	

/**	This methods is reserved for future expansion and should not be used

@param aFunctionNumber Reserved for future expansion
@param aPtr1 Reserved for future expansion
@param aPtr2 Reserved for future expansion

@capability LocalServices

@internalComponent
*/								
TAny* CLbsBtGpsConfigImpl::ExtendedInterface(TInt /*aFunctionNumber*/, TAny* /*aPtr1*/, TAny* /*aPtr2*/)
	{
	return NULL;
	}


// ------------------ Internal API methods ------------------


/** Updates the device information for a device in the list

@param aDeviceInfo [In] The updated device info for the specified device. The key should identify an existing device in the list.

@return KErrNone if the operation succeeded.
		KErrNotFound if the specified device is not present in the list.
		KErrPermissionDenied if the client does not have the required capabilities.

@capability LocalServices
@capability WriteDeviceData

@internalComponent
*/
TInt CLbsBtGpsConfigImpl::UpdateDevice(const TLbsBtGpsDeviceRecord& aDeviceRecord)
	{
	// Check capabilities first
	RProcess process;
	if (!process.HasCapability(ECapabilityLocalServices))
		{
		return KErrPermissionDenied;
		}
	if (!process.HasCapability(ECapabilityWriteDeviceData))
		{
		return KErrPermissionDenied;
		}

	TRAPD(err, UpdateDeviceL(aDeviceRecord));
	return err;
	}


/** Retrieves the list of device records

@param aList [Out] Reference to an empty RPointerArray to populate with device records from the list.

@leave KErrPermissionDenied if the client does not have the required capabilities.

@capability LocalServices

@internalComponent
*/
void CLbsBtGpsConfigImpl::GetDeviceRecordListL(RPointerArray<TLbsBtGpsDeviceRecord>& aRecordList)
	{
	// Check capabilities first
	RProcess process;
	if (!process.HasCapability(ECapabilityLocalServices))
		{
		User::Leave(KErrPermissionDenied);
		}
	
	// Attempt to start a transaction
	User::LeaveIfError(StartTransactionL(CRepository::EReadTransaction));

	// Get the number of devices in the list
	TInt deviceCount;
	User::LeaveIfError(GetDeviceCount(deviceCount));

	for (TInt index=0; index<deviceCount; ++index)
		{
		// Create a new record container
		TLbsBtGpsDeviceRecord* devRecord = new (ELeave) TLbsBtGpsDeviceRecord();
		CleanupStack::PushL(devRecord);

		// Read the record into the container
		User::LeaveIfError(ReadDeviceRecord(index, *devRecord));

		// Add to the passes list
		aRecordList.AppendL(devRecord);
		
		CleanupStack::Pop(devRecord);
		}

	// End central repository transaction
	User::LeaveIfError(EndTransaction());
	}


// ------------------ Protected methods ------------------


/**	Active object DoCancel method
@internalComponent
*/								
void CLbsBtGpsConfigImpl::DoCancel()
	{
	iRepos->NotifyCancel(KNotifyPartialKey, KNotifyMask);
	}


/**	Active object RunL method
@internalComponent
*/								
void CLbsBtGpsConfigImpl::RunL()
	{
	if (iObserver)
		{
		iObserver->OnDeviceListUpdate();
		iRepos->NotifyRequest(KNotifyPartialKey, KNotifyMask, iStatus);
		SetActive();
		}
	}


/** Adds a new device at the specified position index within the list - leaving version

@see AddDevice

@internalComponent
*/
void CLbsBtGpsConfigImpl::AddDeviceL(const TBTDevAddr& aAddress, TInt aPosition, TLbsBtGpsEntryKey& aKey)
	{
	TInt err;
	TInt index;
		
	aKey = KLbsBtGpsEntryKeyNull;

	// Attempt to start a transaction
	User::LeaveIfError(StartTransactionL(CRepository::EReadWriteTransaction));

	// Retrieve the current number of devices in the list
	TInt deviceCount;
	User::LeaveIfError(GetDeviceCount(deviceCount));

	// Check that the desired position is in range, adjust if required
	if (aPosition < 0 || aPosition > deviceCount)
		{
		aPosition = deviceCount;
		}

	// Leave if we've reached the maximum number of devices allowed in the list
	if (deviceCount >= KMaxDevices)
		{
		User::Leave(KErrOverflow);
		}

	// Device record container
	TLbsBtGpsDeviceRecord devRecord;
	
	// Leave if a device with this address is already present in the list
	for (index=0; index<deviceCount; ++index)
		{
		User::LeaveIfError(ReadDeviceRecord(index, devRecord));

		if (devRecord.SocketAddress().BTAddr() == aAddress)
			{
			// Return the key identifying the existing device
			aKey = devRecord.Key();
			User::Leave(KErrAlreadyExists);
			}
		}
	
	// Read the last allocated device key
	TInt key;
	err = iRepos->Get(KSettingLastKey, key);
	if (err == KErrNotFound)
		{
		// Value not present in centrep, initialise sequence to NULL key
		key = KLbsBtGpsEntryKeyNull;
		}
	else
		{
		User::LeaveIfError(err);
		}

	// Increment to get next available key to assign to the new device 
	++key;
	
	// Write current key
	User::LeaveIfError(iRepos->Set(KSettingLastKey, key));

	// Move all entries following the desired position down one place, starting from the end
	for (index=deviceCount-1; index>=aPosition; --index)
		{
		User::LeaveIfError(MoveDeviceRecord(index, index+1));
		}
		
	// Build a device record to add to the list
	devRecord.SetKey(key);

	TBTSockAddr sockAddr;
	sockAddr.SetBTAddr(aAddress);
	devRecord.SetSocketAddress(sockAddr);

	// Set unknown type for new devices
	devRecord.SetType(TLbsBtGpsDeviceInfo::ELbsBtGpsDeviceUnknown);

	// Write the new device record at the desired position
	User::LeaveIfError(WriteDeviceRecord(aPosition, devRecord));

	// Increment device counter and write to cenrep
	++deviceCount;
	User::LeaveIfError(iRepos->Set(KSettingNumDevices, deviceCount));
	
	// End central repository transaction
	User::LeaveIfError(EndTransaction());

	// Set key to return to client now the transaction has succeeded
	aKey = key;
	}


/** Removes a device from the list - leaving version

@see RemoveDevice

@internalComponent
*/
void CLbsBtGpsConfigImpl::RemoveDeviceL(TLbsBtGpsEntryKey aKey)
	{
	// Attempt to start a transaction
	User::LeaveIfError(StartTransactionL(CRepository::EReadWriteTransaction));

	// Get the number of devices in the list
	TInt deviceCount;
	User::LeaveIfError(GetDeviceCount(deviceCount));

	// Flag to indicate if we have found the device record yet
	TBool foundDevice = EFalse;

	// Device record container
	TLbsBtGpsDeviceRecord devRecord;

	// Look for the device record
	for (TInt index=0; index<deviceCount; ++index)
		{
		User::LeaveIfError(ReadDeviceRecord(index, devRecord));

		if (foundDevice)
			{
			// Move following records up one place
			User::LeaveIfError(MoveDeviceRecord(index, index-1));
			}
		else if (devRecord.Key() == aKey)
			{
			// Found the device record, delete it from cenrep
			foundDevice = ETrue;
			
			TUint32 keyBase = (KFirstDeviceIndex + index) << KDeviceIndexShift;
			TUint32 errKey;
				
			User::LeaveIfError(iRepos->Delete(keyBase, KDeviceIndexMask, errKey));
			}
		}

	if (!foundDevice)
		{
		User::Leave(KErrNotFound);
		}

	// Decrement device counter and write to cenrep
	--deviceCount;
	User::LeaveIfError(iRepos->Set(KSettingNumDevices, deviceCount));
	
	// End central repository transaction
	User::LeaveIfError(EndTransaction());
	}


/** Empties the device list, removing all device entries - leaving version

@see EmptyDeviceList

@internalComponent
*/
void CLbsBtGpsConfigImpl::EmptyDeviceListL()
	{
	// Read last allocated device key as we want to save this
	TInt key;
	TInt err = iRepos->Get(KSettingLastKey, key);
	if (err == KErrNotFound)
		{
		// Value not present in centrep, initialise sequence to NULL key
		key = KLbsBtGpsEntryKeyNull;
		}
	else if (err != KErrNone)
		{
		User::Leave(err);
		}

	// Reset the repository
	User::LeaveIfError(iRepos->Reset());

	// Write back last key value
	User::LeaveIfError(iRepos->Set(KSettingLastKey, key));
	}


/** Reorders a device in the list by specifying a new position index - leaving version

@see ReorderDevice

@internalComponent
*/
void CLbsBtGpsConfigImpl::ReorderDeviceL(TLbsBtGpsEntryKey aKey, TInt aNewPosition)
	{
	// Attempt to start a transaction
	User::LeaveIfError(StartTransactionL(CRepository::EReadWriteTransaction));

	// Get the number of devices in the list
	TInt deviceCount;
	User::LeaveIfError(GetDeviceCount(deviceCount));

	// Check that the desired position is in range, adjust if required
	if (aNewPosition < 0 || aNewPosition >= deviceCount)
		{
		aNewPosition = deviceCount-1;
		}
	
	// This will store the current position of the device, if found
	TInt oldPosition = KErrNotFound;

	// This will store the device record while we move it
	TLbsBtGpsDeviceRecord devRecord;
	
	// Look for the device entry to move
	TInt index;
	for (index=0; index<deviceCount; ++index)
		{
		User::LeaveIfError(ReadDeviceRecord(index, devRecord));

		if (devRecord.Key() == aKey)
			{
			// Found the device record
			oldPosition = index;
			break;
			}
		}

	// Leave if the device entry to move cannot be found in the list
	if (oldPosition == KErrNotFound)
		{
		User::Leave(KErrNotFound);
		}

	// Check if device record needs to be moved
	if (oldPosition != aNewPosition)
		{
		// Delete the record from cenrep
		TUint32 keyBase = (KFirstDeviceIndex + oldPosition) << KDeviceIndexShift;
		TUint32 errKey;
		User::LeaveIfError(iRepos->Delete(keyBase, KDeviceIndexMask, errKey));
	
		if (oldPosition < aNewPosition)
			{
			// Moving towards the end of the list - shift intermediate entries up one place
			for (index = oldPosition + 1; index <= aNewPosition; ++index)
				{
				User::LeaveIfError(MoveDeviceRecord(index,index-1));
				}
			}
		else if (oldPosition > aNewPosition)
			{
			// Moving towards the start of the list - shift intermediate entries down one place
			for (index = oldPosition - 1; index >= aNewPosition; --index)
				{
				User::LeaveIfError(MoveDeviceRecord(index,index+1));
				}
			}
	
		// Write the record at the new position 
		User::LeaveIfError(WriteDeviceRecord(aNewPosition, devRecord));
		}
	
	// End central repository transaction
	User::LeaveIfError(EndTransaction());
	}


/** Updates the device information for a device in the list - leaving version

@see UpdateDevice

@internalComponent
*/
void CLbsBtGpsConfigImpl::UpdateDeviceL(const TLbsBtGpsDeviceRecord& aDeviceRecord)
	{
	// Attempt to start a transaction
	User::LeaveIfError(StartTransactionL(CRepository::EReadWriteTransaction));

	// Get the number of devices in the list
	TInt deviceCount;
	User::LeaveIfError(GetDeviceCount(deviceCount));

	// Flag to indicate if we have found the device record yet
	TBool foundDevice = EFalse;

	// Device record container
	TLbsBtGpsDeviceRecord devRecord;

	// Look for the device record
	for (TInt index=0; index<deviceCount; ++index)
		{
		User::LeaveIfError(ReadDeviceRecord(index, devRecord));

		if (devRecord.Key() == aDeviceRecord.Key())
			{
			// Found the device record, update it with the new one passed in
			foundDevice = ETrue;
			User::LeaveIfError(WriteDeviceRecord(index, aDeviceRecord));
			break;
			}
		}

	if (!foundDevice)
		{
		User::Leave(KErrNotFound);
		}
	
	// End central repository transaction
	User::LeaveIfError(EndTransaction());
	}


/** Attempt to start a central repository transaction 

@return KErrNone if the operation succeeded.
 
@internalComponent
*/
TInt CLbsBtGpsConfigImpl::StartTransactionL(CRepository::TTransactionMode aMode)
	{
	TInt retries = 0;

	while (ETrue)
		{
		// Attempt to start the transaction
		TInt err = iRepos->StartTransaction(aMode);
		
		if (err == KErrNone)
			{
			// Transaction started succesfully
			break;
			}
		else if (err == KErrLocked && (retries++ < KLockedRetries)) 
			{
			// Repository is locked by another client, wait before retrying
			User::After(TTimeIntervalMicroSeconds32(KLockedRetryDelay));
			}
		else
			{
			// Another error occurred, or maximum number of retries reached
			return err;
			}
		}

	// Make sure the transaction is cancelled if something goes wrong
	iRepos->CleanupCancelTransactionPushL();
	
	return KErrNone;
	}


/** End (commit) a central repository transaction 

@return KErrNone if the operation succeeded.
 
@internalComponent
*/
TInt CLbsBtGpsConfigImpl::EndTransaction()
	{
	CleanupStack::Pop(); // CleanupCancelTransaction

	TUint32 errKey;
	return iRepos->CommitTransaction(errKey);
	}


/** Read a device record from cenrep by index 

@param aIndex [In] The index of the device entry to read
@param aDeviceInfo [Out] Device info to fill from cenrep  

StartTransaction should have been called.

@internalComponent
*/
TInt CLbsBtGpsConfigImpl::ReadDeviceRecord(TInt aIndex, TLbsBtGpsDeviceRecord& aDeviceRecord)
	{
	TInt err;
	TUint32 keyBase = (KFirstDeviceIndex + aIndex) << KDeviceIndexShift;

	TInt deviceKey;
	err = iRepos->Get(keyBase | KDeviceSettingKey, deviceKey);
	if (err != KErrNone)
		{
		return err;
		}
	aDeviceRecord.SetKey(deviceKey);

	TInt deviceType;
	err = iRepos->Get(keyBase | KDeviceSettingType, deviceType);
	if (err != KErrNone)
		{
		return err;
		}
	aDeviceRecord.SetType(deviceType);

	TBuf<KMaxSockAddrSize> addressBuffer;
	err = iRepos->Get(keyBase | KDeviceSettingSockAddr, addressBuffer);
	if (err != KErrNone)
		{
		return err;
		}
	TBTSockAddr address; 
	address.Copy(addressBuffer);
	aDeviceRecord.SetSocketAddress(address);
	
	return KErrNone;
	}


/** Write a device record to cenrep by index

@param aIndex [In] The index of the device entry to write
@param aDeviceInfo [Out] Device info to write to cenrep  

StartTransaction should have been called.

@internalComponent
*/
TInt CLbsBtGpsConfigImpl::WriteDeviceRecord(TInt aIndex, const TLbsBtGpsDeviceRecord& aDeviceRecord)
	{
	TInt err;
	TUint32 keyBase = (KFirstDeviceIndex + aIndex) << KDeviceIndexShift;
	
	TInt deviceKey = aDeviceRecord.Key();
	err = iRepos->Set(keyBase | KDeviceSettingKey, deviceKey);
	if (err != KErrNone)
		{
		return err;
		}
	
	TInt deviceType = aDeviceRecord.Type();
	err = iRepos->Set(keyBase | KDeviceSettingType, deviceType);
	if (err != KErrNone)
		{
		return err;
		}

	TBuf<KMaxSockAddrSize> addressBuffer;
	addressBuffer.Copy(aDeviceRecord.SocketAddress());
	err = iRepos->Set(keyBase | KDeviceSettingSockAddr, addressBuffer);
	if (err != KErrNone)
		{
		return err;
		}
	
	return KErrNone;
	}


/** Move a device record from old to new position

@param aOldIndex [In] The current index of the device record in the list
@param aOldIndex [In] The new index of the device record in the list  

StartTransaction should have been called.

@internalComponent
*/
TInt CLbsBtGpsConfigImpl::MoveDeviceRecord(TInt aOldIndex, TInt aNewIndex)
	{
	TUint32 oldKeyBase = (KFirstDeviceIndex + aOldIndex) << KDeviceIndexShift;
	TUint32 newKeyBase = (KFirstDeviceIndex + aNewIndex) << KDeviceIndexShift;
	TUint32 errKey;
		
	return iRepos->Move(oldKeyBase, newKeyBase, KDeviceIndexMask, errKey);
	}


/** Class constructor 

@param aObserver [IN] Optional observer, which will recieve update notifications.

@internalComponent
*/
CLbsBtGpsConfigImpl::CLbsBtGpsConfigImpl(MLbsBtGpsConfigObserver* aObserver)
: CActive(CActive::EPriorityStandard),
  iObserver(aObserver)
	{
	CActiveScheduler::Add(this);
	}


/** Second phase constructor 
@internalComponent
*/
void CLbsBtGpsConfigImpl::ConstructL()
	{
	iRepos = CRepository::NewL(TUid::Uid(KLbsBtGpsConfigRepositoryUid));
	iRepos->NotifyRequest(KNotifyPartialKey, KNotifyMask, iStatus);
	SetActive();
	}
