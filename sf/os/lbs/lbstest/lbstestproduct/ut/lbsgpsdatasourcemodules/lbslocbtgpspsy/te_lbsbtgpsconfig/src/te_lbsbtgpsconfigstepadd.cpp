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
// te_lbsbtgpsconfigstepadd.h
// lbsbtgpsconfig unit test step: Add device tests 
//

#include "te_lbsbtgpsconfigstepadd.h"


CLbsBtGpsConfigStepAdd::CLbsBtGpsConfigStepAdd()
	{
	SetTestStepName(KLbsBtGpsConfigStepAdd);
	}


CLbsBtGpsConfigStepAdd::~CLbsBtGpsConfigStepAdd()
	{
	
	}


TBool CLbsBtGpsConfigStepAdd::RunTestStage(TInt aStage)
	{
	switch(aStage)
		{

		case 1:
			// Reset updates counter
			iUpdatesRecieved = 0;
			if (TestAddDeviceBasic() == EFail)
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
			if (TestAddDeviceMany() == EFail)
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
			if (TestAddDeviceDuplicate() == EFail)
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
		
		case 7:
			if (TestAddDeviceForward() == EFail)
				{
				SetTestStepResult(EFail);
				}
			return ETrue;

		case 8:
			// Check that we got an update notification following the previous stages
			if (iUpdatesRecieved != 4)
				{
				INFO_PRINTF1(_L("No update recieved"));
				SetTestStepResult(EFail);
				}
			return ETrue;

		case 9:
			if (TestAddDeviceReverse() == EFail)
				{
				SetTestStepResult(EFail);
				}
			return ETrue;

		case 10:
			// Check that we got an update notification following the previous stages
			if (iUpdatesRecieved != 5)
				{
				INFO_PRINTF1(_L("No update recieved"));
				SetTestStepResult(EFail);
				}
			return ETrue;

		case 11:
			if (TestAddDeviceRandom() == EFail)
				{
				SetTestStepResult(EFail);
				}
			return ETrue;

		case 12:
			// Check that we got an update notification following the previous stages
			if (iUpdatesRecieved != 6)
				{
				INFO_PRINTF1(_L("No update recieved"));
				SetTestStepResult(EFail);
				}
			return ETrue;
		}
	
	return EFalse;
	}


TVerdict CLbsBtGpsConfigStepAdd::TestAddDeviceBasic()
	{
	TInt err;
	TInt numAdded = 0;

	// First empty the device list
	err = iConfig->EmptyDeviceList();
	if (err != KErrNone)
		{
		INFO_PRINTF1(_L("Failed to empty device list"));
		return EFail;
		}

	// Add a device to the list
	INFO_PRINTF2(_L("Adding single device"),numAdded);
	
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

	// Retrieve the device list using the API
	iDeviceList.ResetAndDestroy();
	TRAP(err, iConfig->GetDeviceListL(iDeviceList));

	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("GetDeviceListL left with error %d"),err);
		return EFail;
		}

	// Check there is a single device in the returned list
	if (iDeviceList.Count() != 1)
		{
		INFO_PRINTF2(_L("Returned device list contains %d entries, expected 1"),iDeviceList.Count());
		return EFail;
		}

	// Check the key is correct
	if (iDeviceList[0]->Key() != key)
		{
		INFO_PRINTF3(_L("Device entry 0 has incorrect key (%d), expected %d"),iDeviceList[0]->Key(),key);
		return EFail;
		}
	
	// Check the address is correct
	if (iDeviceList[0]->Address() != addr)
		{
		TBuf<20> readableAddr;
		iDeviceList[0]->Address().GetReadable(readableAddr);
		TBuf<20> readableExpectedAddr;
		addr.GetReadable(readableExpectedAddr);
		INFO_PRINTF3(_L("Device entry 0 has incorrect address (%S), expected %S"),&readableAddr,&readableExpectedAddr);
		return EFail;
		}
	
	// Check the type is set to default
	if (iDeviceList[0]->Type() != TLbsBtGpsDeviceInfo::ELbsBtGpsDeviceUnknown)
		{
		INFO_PRINTF3(_L("Device entry 0 has incorrect type (%d), expected %d"),iDeviceList[0]->Type(),TLbsBtGpsDeviceInfo::ELbsBtGpsDeviceUnknown);
		return EFail;
		}

	// Test has passed if its got to this point
	return EPass;
	}


TVerdict CLbsBtGpsConfigStepAdd::TestAddDeviceMany()
	{
	TInt err;
	TInt numAdded = 0;

	// First empty the device list
	err = iConfig->EmptyDeviceList();
	if (err != KErrNone)
		{
		INFO_PRINTF1(_L("Failed to empty device list"));
		return EFail;
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
	
	// This will store all the keys for devices that are added, so we can check they are unique
	iKeys.Reset();

	while (ETrue)
		{
		INFO_PRINTF2(_L("Adding device %d"),numAdded);
		
		TLbsBtGpsEntryKey key;
		TBTDevAddr addr = iServer->GenerateUniqueBTDevAddr();
		
		err = iConfig->AddDevice(addr, 0, key);

		// We expect to get KErrOverflow once the list is full, but check that the limit is acceptable
		if (err == KErrOverflow)
			{
			if (numAdded < KMinDeviceLimit)
				{
				INFO_PRINTF1(_L("Failed to add the required minimum number of devices"));
				return EFail;
				}
			else
				{
				// Reached the limit on the number of items, but its more than required, so its ok, stop here
				INFO_PRINTF1(_L("Device not added, acceptable limit reached, ending test"));
				break;
				}
			}

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
		++numAdded;
		
		// Get the number of devices and check it is correct
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
		}
	
	// Get the number of devices and check it is correct
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

	// Test has passed if its got to this point
	return EPass;
	}


TVerdict CLbsBtGpsConfigStepAdd::TestAddDeviceDuplicate()
	{
	TInt err;
	TInt numAdded = 0;
	
	// First empty the device list
	err = iConfig->EmptyDeviceList();
	if (err != KErrNone)
		{
		INFO_PRINTF1(_L("Failed to empty device list"));
		return EFail;
		}
	
	// This will store all the keys for devices that are added, so we can check they are unique
	iKeys.Reset();
	
	// Add some devices, with unique addresses
	for (numAdded=0; numAdded<8; ++numAdded)
		{
		INFO_PRINTF2(_L("Adding device %d"),numAdded);
		
		TLbsBtGpsEntryKey key;
		TBTDevAddr addr = iServer->GenerateUniqueBTDevAddr();
		
		err = iConfig->AddDevice(addr, 0, key);
	
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
	
		// Retrieve the device list using the API
		iDeviceList.ResetAndDestroy();
		TRAP(err, iConfig->GetDeviceListL(iDeviceList));
	
		if (err != KErrNone)
			{
			INFO_PRINTF2(_L("GetDeviceListL left with error %d"),err);
			return EFail;
			}
		
		TInt index;
		for (index=0; index<iDeviceList.Count(); ++index)
			{
			INFO_PRINTF2(_L("Attempting to add duplicate of device %d"),index);
			
			// Try and add a device with an address that already exists
			TLbsBtGpsEntryKey key2;
			
			err = iConfig->AddDevice(iDeviceList[index]->Address(), 0, key2);
	
			// Should get KErrAlreadyExists because a device with that address is already present
			if (err != KErrAlreadyExists)
				{
				INFO_PRINTF2(_L("AddDevice returned error %d, expected KErrAlreadyExists"),err);
				return EFail;
				}
	
			// Key2 should have been set to the key of the existing device
			if (key2 != iDeviceList[index]->Key())
				{
				INFO_PRINTF3(_L("AddDevice should return existing device key (%d), got %d"),key,key2);
				return EFail;
				}
			}
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
	
	// Test has passed if its got to this point
	return EPass;
	}


TVerdict CLbsBtGpsConfigStepAdd::TestAddDeviceForward()
	{
	TInt err;
	TInt numAdded = 0;

	INFO_PRINTF1(_L("Test adding devices to end of list"));
	
	// First empty the device list
	err = iConfig->EmptyDeviceList();
	if (err != KErrNone)
		{
		INFO_PRINTF1(_L("Failed to empty device list"));
		return EFail;
		}

	// This will store all the keys for devices that are added, so we can check they are unique and correctly ordered
	iKeys.Reset();

	// Add some devices, always adding to the end of the list
	for (numAdded=0; numAdded<8; ++numAdded)
		{
		INFO_PRINTF3(_L("Adding device %d at position %d"),numAdded,numAdded);
		
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

	// Retrieve the device list using the API
	iDeviceList.ResetAndDestroy();
	TRAP(err, iConfig->GetDeviceListL(iDeviceList));

	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("GetDeviceListL left with error %d"),err);
		return EFail;
		}
	
	// Device list should correspond directly to the iKeys array
	TInt index;
	for (index=0; index<iDeviceList.Count(); ++index)
		{
		TLbsBtGpsEntryKey key1 = iKeys[index];
		TLbsBtGpsEntryKey key2 = iDeviceList[index]->Key();
		if (key1 != key2)
			{
			INFO_PRINTF4(_L("Device entry %d key should be %d, not %d"),index,key1,key2);
			return EFail;
			}
		}
	
	INFO_PRINTF1(_L("List is correctly ordered"));
	return EPass;
	}


TVerdict CLbsBtGpsConfigStepAdd::TestAddDeviceReverse()
	{
	TInt err;
	TInt numAdded = 0;

	INFO_PRINTF1(_L("Test adding devices to start of list"));
	
	// First empty the device list
	err = iConfig->EmptyDeviceList();
	if (err != KErrNone)
		{
		INFO_PRINTF1(_L("Failed to empty device list"));
		return EFail;
		}

	// This will store all the keys for devices that are added, so we can check they are unique and correctly ordered
	iKeys.Reset();

	// Add some devices, always adding to the start of the list
	for (numAdded=0; numAdded<8; ++numAdded)
		{
		INFO_PRINTF2(_L("Adding device %d at position 0"),numAdded);
		
		TLbsBtGpsEntryKey key;
		TBTDevAddr addr = iServer->GenerateUniqueBTDevAddr();
		
		err = iConfig->AddDevice(addr, 0, key);

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

	// Retrieve the device list using the API
	iDeviceList.ResetAndDestroy();
	TRAP(err, iConfig->GetDeviceListL(iDeviceList));

	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("GetDeviceListL left with error %d"),err);
		return EFail;
		}
	
	// Devices in the list should be in the reverse order of the iKeys array
	TInt index;
	for (index=0; index<iDeviceList.Count(); ++index)
		{
		TLbsBtGpsEntryKey key1 = iKeys[iKeys.Count()-index-1];
		TLbsBtGpsEntryKey key2 = iDeviceList[index]->Key();
		if (key1 != key2)
			{
			INFO_PRINTF4(_L("Device entry %d key should be %d, not %d"),index,key1,key2);
			return EFail;
			}
		}
	
	INFO_PRINTF1(_L("List is correctly ordered"));
	return EPass;
	}


TVerdict CLbsBtGpsConfigStepAdd::TestAddDeviceRandom()
	{
	TInt err;
	TInt numAdded = 0;

	INFO_PRINTF1(_L("Test adding devices at random positions"));
	
	// First empty the device list
	err = iConfig->EmptyDeviceList();
	if (err != KErrNone)
		{
		INFO_PRINTF1(_L("Failed to empty device list"));
		return EFail;
		}

	// This will store all the keys for devices that are added, so we can check they are unique and correctly ordered
	iKeys.Reset();

	// Add some devices in a "random" manner
	for (numAdded=0; numAdded<10; ++numAdded)
		{
		// Decide where to add this entry
		TInt position=0;
		switch (numAdded)
			{
			case 0: position = 4; break; // 0
			case 1: position = 0; break; // 1,0
			case 2: position = 1; break; // 1,2,0
			case 3: position = 1; break; // 1,3,2,0
			case 4: position = 7; break; // 1,3,2,0,4
			case 5: position = 0; break; // 5,1,3,2,0,4
			case 6: position = 3; break; // 5,1,3,6,2,0,4
			case 7: position = 5; break; // 5,1,3,6,2,7,0,4
			case 8: position = 1; break; // 5,8,1,3,6,2,7,0,4
			case 9: position = 8; break; // 5,8,1,3,6,2,7,0,9,4
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

	// Retrieve the device list using the API
	iDeviceList.ResetAndDestroy();
	TRAP(err, iConfig->GetDeviceListL(iDeviceList));

	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("GetDeviceListL left with error %d"),err);
		return EFail;
		}

	// Check the devices are in the random order assigned above
	TInt index;
	for (index=0; index<iDeviceList.Count(); ++index)
		{
		TLbsBtGpsEntryKey key1=0;
		switch (index)
			{
			case 0: key1 = iKeys[5]; break;
			case 1: key1 = iKeys[8]; break;
			case 2: key1 = iKeys[1]; break;
			case 3: key1 = iKeys[3]; break;
			case 4: key1 = iKeys[6]; break;
			case 5: key1 = iKeys[2]; break;
			case 6: key1 = iKeys[7]; break;
			case 7: key1 = iKeys[0]; break;
			case 8: key1 = iKeys[9]; break;
			case 9: key1 = iKeys[4]; break;
			}
		TLbsBtGpsEntryKey key2 = iDeviceList[index]->Key();
		if (key1 != key2)
			{
			INFO_PRINTF4(_L("Device entry %d key should be %d, not %d"),index,key1,key2);
			return EFail;
			}
		}
	
	INFO_PRINTF1(_L("List is correctly ordered"));
	return EPass;
	}
