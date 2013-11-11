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
// te_lbsbtgpsconfigstepremove.h
// lbsbtgpsconfig unit test step: Remove device tests 
//

#include "te_lbsbtgpsconfigstepremove.h"


CLbsBtGpsConfigStepRemove::CLbsBtGpsConfigStepRemove()
	{
	SetTestStepName(KLbsBtGpsConfigStepRemove);
	}


CLbsBtGpsConfigStepRemove::~CLbsBtGpsConfigStepRemove()
	{
	
	}


TBool CLbsBtGpsConfigStepRemove::RunTestStage(TInt aStage)
	{
	switch(aStage)
		{
	
		case 1:
			// Reset updates counter
			iUpdatesRecieved = 0;
			if (TestRemoveDeviceBasic() == EFail)
				{
				SetTestStepResult(EFail);
				}
			return ETrue;

		case 2:
			// Check that we got an update notification following the previous stages
			if (iUpdatesRecieved != 1)
				{
				INFO_PRINTF1(_L("No update recieved"));
				SetTestStepResult(EFail);
				}
			return ETrue;
			
		case 3:
			if (TestRemoveDeviceMany() == EFail)
				{
				SetTestStepResult(EFail);
				}
			return ETrue;

		case 4:
			// Check that we got an update notification following the previous stages
			if (iUpdatesRecieved != 2)
				{
				INFO_PRINTF1(_L("No update recieved"));
				SetTestStepResult(EFail);
				}
			return ETrue;

		case 5:
			if (TestRemoveDeviceNonExistant() == EFail)
				{
				SetTestStepResult(EFail);
				}
			return ETrue;

		case 6:
			// Check that we got an update notification following the previous stages
			if (iUpdatesRecieved != 3)
				{
				INFO_PRINTF1(_L("No update recieved"));
				SetTestStepResult(EFail);
				}
			return ETrue;
		}
	
	return EFalse;
	}


TVerdict CLbsBtGpsConfigStepRemove::TestRemoveDeviceBasic()
	{
	TInt err;
	TInt numAdded = 0;

	INFO_PRINTF1(_L("Test removing a single device from the list"));
	
	// First empty the device list
	err = iConfig->EmptyDeviceList();
	if (err != KErrNone)
		{
		INFO_PRINTF1(_L("Failed to empty device list"));
		return EFail;
		}

	// Add a device to the list
	INFO_PRINTF2(_L("Adding device"),numAdded);
	
	TLbsBtGpsEntryKey key;
	TBTDevAddr addr = iServer->GenerateUniqueBTDevAddr();
	
	err = iConfig->AddDevice(addr, numAdded, key);
	
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("AddDevice returned error %d"),err);
		return EFail;
		}
	
	// Check the allocated key is valid
	if (key == KLbsBtGpsEntryKeyNull)
		{
		INFO_PRINTF1(_L("Returned key is invalid"));
		return EFail;
		}

	++numAdded;
	INFO_PRINTF2(_L("Added device, key=%d"),key);
	
	// Get the number of devices and check it is correct
	TInt deviceCount;
	err = iConfig->GetDeviceCount(deviceCount);

	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("GetDeviceCount returned error %d"),err);
		return EFail;
		}

	if (deviceCount != numAdded)
		{
		INFO_PRINTF3(_L("GetDeviceCount returned incorrect count %d, expected %d"), deviceCount, numAdded);
		return EFail;
		}

	// Now attempt to remove the device from the list
	err = iConfig->RemoveDevice(key);

	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("RemoveDevice failed with error %d"),err);
		return EFail;
		}
	
	--numAdded;
	INFO_PRINTF2(_L("Removed device (key=%d)"),key);

	err = iConfig->GetDeviceCount(deviceCount);

	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("GetDeviceCount returned error %d"),err);
		return EFail;
		}

	if (deviceCount != numAdded)
		{
		INFO_PRINTF3(_L("GetDeviceCount returned incorrect count %d, expected %d"), deviceCount, numAdded);
		return EFail;
		}

	// Retrieve the device list using the API
	iDeviceList.ResetAndDestroy();
	TRAP(err, iConfig->GetDeviceListL(iDeviceList));

	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("GetDeviceListL left with error %d"),err);
		return EFail;
		}

	// Check there are no devices in the returned list
	if (iDeviceList.Count() != 0)
		{
		INFO_PRINTF2(_L("Returned device list contains %d entries, expected 0"),iDeviceList.Count());
		return EFail;
		}
	
	INFO_PRINTF1(_L("Item succesfully removed"));
	return EPass;
	}


TVerdict CLbsBtGpsConfigStepRemove::TestRemoveDeviceMany()
	{
	TInt err;
	TInt numAdded = 0;

	INFO_PRINTF1(_L("Test removing many devices from the list"));
	
	// First empty the device list
	err = iConfig->EmptyDeviceList();
	if (err != KErrNone)
		{
		INFO_PRINTF1(_L("Failed to empty device list"));
		return EFail;
		}

	// This will store all the keys for devices that are added
	iKeys.Reset();

	// Add some devices in a "random" manner
	for (numAdded=0; numAdded<10; ++numAdded)
		{
		// Decide where to add this entry
		TInt position=0;
		switch (numAdded)
			{
			case 0: position = 3; break; // 0
			case 1: position = 1; break; // 0,1
			case 2: position = 0; break; // 2,0,1
			case 3: position = 2; break; // 2,0,3,1
			case 4: position = 1; break; // 2,4,0,3,1
			case 5: position = 5; break; // 2,4,0,3,1,5
			case 6: position = 2; break; // 2,4,6,0,3,1,5
			case 7: position = 4; break; // 2,4,6,0,7,3,1,5
			case 8: position = 0; break; // 8,2,4,6,0,7,3,1,5
			case 9: position = 3; break; // 8,2,4,9,6,0,7,3,1,5
			}

		INFO_PRINTF3(_L("Adding device %d at position %d"),numAdded,position);
		
		TLbsBtGpsEntryKey key;
		TBTDevAddr addr = iServer->GenerateUniqueBTDevAddr();
		
		err = iConfig->AddDevice(addr, position, key);

		if (err != KErrNone)
			{
			INFO_PRINTF2(_L("AddDevice returned error %d"),err);
			return EFail;
			}
		
		// Check the allocated key is valid
		if (key == KLbsBtGpsEntryKeyNull)
			{
			INFO_PRINTF1(_L("Returned key is invalid"));
			return EFail;
			}

		// Check the allocated key is unique
		if (iKeys.Find(key) != KErrNotFound)
			{
			INFO_PRINTF1(_L("Returned key is not unique"));
			return EFail;
			}

		// Append the new key to the list for checking
		iKeys.Append(key);
		}

	// Get the number of devices and check it is correct
	TInt deviceCount;
	err = iConfig->GetDeviceCount(deviceCount);

	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("GetDeviceCount returned error %d"),err);
		return EFail;
		}

	if (deviceCount != numAdded)
		{
		INFO_PRINTF3(_L("GetDeviceCount returned incorrect count %d, expected %d"), deviceCount, numAdded);
		return EFail;
		}

	// Now remove some devices at random
	TInt numRemoved;
	for (numRemoved=0; numRemoved<5; ++numRemoved)
		{
		// Decide which entry to remove
		TInt keyIndex=0;
		switch (numRemoved)
			{
			case 0: keyIndex = 6; break; // 8,2,4,9,0,7,3,1,5
			case 1: keyIndex = 1; break; // 8,2,4,9,0,7,3,5
			case 2: keyIndex = 8; break; // 2,4,9,0,7,3,5
			case 3: keyIndex = 4; break; // 2,9,0,7,3,5
			case 4: keyIndex = 0; break; // 2,9,7,3,5
			}

		INFO_PRINTF2(_L("Removing device %d"),keyIndex);
		
		TLbsBtGpsEntryKey key = iKeys[keyIndex];
		
		err = iConfig->RemoveDevice(key);

		if (err != KErrNone)
			{
			INFO_PRINTF2(_L("RemoveDevice returned error %d"),err);
			return EFail;
			}
		}
	
	// Retrieve the device list using the API
	iDeviceList.ResetAndDestroy();
	TRAP(err, iConfig->GetDeviceListL(iDeviceList));

	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("GetDeviceListL left with error %d"),err);
		return EFail;
		}

	// Check the returned list contains the expected number of items
	if (iDeviceList.Count() != (numAdded - numRemoved))
		{
		INFO_PRINTF3(_L("List contains %d items, expected %d"),iDeviceList.Count(),numAdded - numRemoved);
		return EFail;
		}
	
	// Check the devices are in the correct order following the add/remove operations above
	TInt index;
	for (index=0; index<iDeviceList.Count(); ++index)
		{
		TLbsBtGpsEntryKey key1=0;
		switch (index)
			{
			case 0: key1 = iKeys[2]; break;
			case 1: key1 = iKeys[9]; break;
			case 2: key1 = iKeys[7]; break;
			case 3: key1 = iKeys[3]; break;
			case 4: key1 = iKeys[5]; break;
			}
		TLbsBtGpsEntryKey key2 = iDeviceList[index]->Key();
		if (key1 != key2)
			{
			INFO_PRINTF4(_L("Device entry %d key should be %d, not %d"),index,key1,key2);
			return EFail;
			}
		}
	
	INFO_PRINTF1(_L("List is correctly ordered following remove operations"));
	return EPass;
	}


TVerdict CLbsBtGpsConfigStepRemove::TestRemoveDeviceNonExistant()
	{
	TInt err;
	TInt numAdded = 0;

	INFO_PRINTF1(_L("Test removing a non-existant device from the list"));
	
	// First empty the device list
	err = iConfig->EmptyDeviceList();
	if (err != KErrNone)
		{
		INFO_PRINTF1(_L("Failed to empty device list"));
		return EFail;
		}

	// Try and remove a device with a NULL key
	err = iConfig->RemoveDevice(KLbsBtGpsEntryKeyNull);
	
	if (err != KErrNotFound)
		{
		INFO_PRINTF2(_L("Remove returned error %d, expected KErrNotFound"),err);
		return EFail;
		}
	
	// Add a device to the list
	INFO_PRINTF2(_L("Adding device"),numAdded);
	
	TLbsBtGpsEntryKey key;
	TBTDevAddr addr = iServer->GenerateUniqueBTDevAddr();
	
	err = iConfig->AddDevice(addr, numAdded, key);
	
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("AddDevice returned error %d"),err);
		return EFail;
		}
	
	// Check the allocated key is valid
	if (key == KLbsBtGpsEntryKeyNull)
		{
		INFO_PRINTF1(_L("Returned key is invalid"));
		return EFail;
		}

	++numAdded;
	INFO_PRINTF2(_L("Added device, key=%d"),key);
	
	// Get the number of devices and check it is correct
	TInt deviceCount;
	err = iConfig->GetDeviceCount(deviceCount);

	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("GetDeviceCount returned error %d"),err);
		return EFail;
		}

	if (deviceCount != numAdded)
		{
		INFO_PRINTF3(_L("GetDeviceCount returned incorrect count %d, expected %d"), deviceCount, numAdded);
		return EFail;
		}

	// Try and remove a device with a NULL key again
	err = iConfig->RemoveDevice(KLbsBtGpsEntryKeyNull);
	
	if (err != KErrNotFound)
		{
		INFO_PRINTF2(_L("Remove returned error %d, expected KErrNotFound"),err);
		return EFail;
		}
	
	// Try and remove a device with a similar key (-1)
	err = iConfig->RemoveDevice(key-1);
	
	if (err != KErrNotFound)
		{
		INFO_PRINTF2(_L("Remove returned error %d, expected KErrNotFound"),err);
		return EFail;
		}

	// Try and remove a device with a similar key (+1)
	err = iConfig->RemoveDevice(key+1);
	
	if (err != KErrNotFound)
		{
		INFO_PRINTF2(_L("Remove returned error %d, expected KErrNotFound"),err);
		return EFail;
		}

	// Get the device count, to check the device hasn't been removed
	err = iConfig->GetDeviceCount(deviceCount);

	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("GetDeviceCount returned error %d"),err);
		return EFail;
		}

	if (deviceCount != numAdded)
		{
		INFO_PRINTF3(_L("GetDeviceCount returned incorrect count %d, expected %d"), deviceCount, numAdded);
		return EFail;
		}
	
	// Now really remove the device from the list
	err = iConfig->RemoveDevice(key);

	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("RemoveDevice failed with error %d"),err);
		return EFail;
		}
	
	--numAdded;
	INFO_PRINTF2(_L("Removed device (key=%d)"),key);

	err = iConfig->GetDeviceCount(deviceCount);

	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("GetDeviceCount returned error %d"),err);
		return EFail;
		}

	if (deviceCount != numAdded)
		{
		INFO_PRINTF3(_L("GetDeviceCount returned incorrect count %d, expected %d"), deviceCount, numAdded);
		return EFail;
		}
	
	INFO_PRINTF1(_L("Removing non existant devices handled correctly"));
	return EPass;
	}
