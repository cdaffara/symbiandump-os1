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
// te_lbsbtgpsconfigstepreorder.h
// lbsbtgpsconfig unit test step: Reorder device tests 
//

#include "te_lbsbtgpsconfigstepreorder.h"


CLbsBtGpsConfigStepReorder::CLbsBtGpsConfigStepReorder()
	{
	SetTestStepName(KLbsBtGpsConfigStepReorder);
	}


CLbsBtGpsConfigStepReorder::~CLbsBtGpsConfigStepReorder()
	{
	
	}


TBool CLbsBtGpsConfigStepReorder::RunTestStage(TInt aStage)
	{
	switch(aStage)
		{
	
		case 1:
			// Reset updates counter
			iUpdatesRecieved = 0;
			if (TestReorderDeviceRandom() == EFail)
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
			if (TestReorderDeviceNonExistant() == EFail)
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
		}
	
	return EFalse;
	}


TVerdict CLbsBtGpsConfigStepReorder::TestReorderDeviceRandom()
	{
	TInt err;
	TInt numAdded = 0;

	INFO_PRINTF1(_L("Test reordering devices within the list"));
	
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
			case 0: position = 1; break; // 0
			case 1: position = 2; break; // 0,1
			case 2: position = 0; break; // 2,0,1
			case 3: position = 4; break; // 2,0,1,3
			case 4: position = 2; break; // 2,0,4,1,3
			case 5: position = 1; break; // 2,5,0,4,1,3
			case 6: position = 0; break; // 6,2,5,0,4,1,3
			case 7: position = 5; break; // 6,2,5,0,4,7,1,3
			case 8: position = 6; break; // 6,2,5,0,4,7,8,1,3
			case 9: position = 3; break; // 6,2,5,9,0,4,7,8,1,3
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

	// Now reorder some devices at random
	TInt index;
	for (index=0; index<5; ++index)
		{
		// Decide which entry to reorder
		TInt keyIndex=0;
		TInt newPos=0;
		switch (index)
			{
			case 0: keyIndex = 2; newPos = 7; break; // 6,5,9,0,4,7,8,2,1,3
			case 1: keyIndex = 1; newPos = 0; break; // 1,6,5,9,0,4,7,8,2,3
			case 2: keyIndex = 5; newPos = 9; break; // 1,6,9,0,4,7,8,2,3,5
			case 3: keyIndex = 4; newPos = 99; break;// 1,6,9,0,7,8,2,3,5,4
			case 4: keyIndex = 0; newPos = 1; break; // 1,0,6,9,7,8,2,3,5,4
			}

		INFO_PRINTF3(_L("Reordering device %d to position %d"),keyIndex,newPos);
		
		TLbsBtGpsEntryKey key = iKeys[keyIndex];
		
		err = iConfig->ReorderDevice(key,newPos);

		if (err != KErrNone)
			{
			INFO_PRINTF2(_L("ReorderDevice returned error %d"),err);
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
	if (iDeviceList.Count() != numAdded)
		{
		INFO_PRINTF3(_L("List contains %d items, expected %d"),iDeviceList.Count(),numAdded);
		return EFail;
		}
	
	// Check the devices are in the correct order following the reorder operations above
	for (index=0; index<iDeviceList.Count(); ++index)
		{
		TLbsBtGpsEntryKey key1=0;
		switch (index)
			{
			case 0: key1 = iKeys[1]; break;
			case 1: key1 = iKeys[0]; break;
			case 2: key1 = iKeys[6]; break;
			case 3: key1 = iKeys[9]; break;
			case 4: key1 = iKeys[7]; break;
			case 5: key1 = iKeys[8]; break;
			case 6: key1 = iKeys[2]; break;
			case 7: key1 = iKeys[3]; break;
			case 8: key1 = iKeys[5]; break;
			case 9: key1 = iKeys[4]; break;
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


TVerdict CLbsBtGpsConfigStepReorder::TestReorderDeviceNonExistant()
	{
	TInt err;
	TInt numAdded = 0;

	INFO_PRINTF1(_L("Test reordering a non-existant device within the list"));
	
	// First empty the device list
	err = iConfig->EmptyDeviceList();
	if (err != KErrNone)
		{
		INFO_PRINTF1(_L("Failed to empty device list"));
		return EFail;
		}

	// Try and reorder a device with a NULL key
	err = iConfig->ReorderDevice(KLbsBtGpsEntryKeyNull,7);
	
	if (err != KErrNotFound)
		{
		INFO_PRINTF2(_L("ReorderDevice returned error %d, expected KErrNotFound"),err);
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

	// Try and reorder a device with a NULL key again
	err = iConfig->ReorderDevice(KLbsBtGpsEntryKeyNull,0);
	
	if (err != KErrNotFound)
		{
		INFO_PRINTF2(_L("ReorderDevice returned error %d, expected KErrNotFound"),err);
		return EFail;
		}
	
	// Try and reorder a device with a similar key (-1)
	err = iConfig->ReorderDevice(key-1,2);
	
	if (err != KErrNotFound)
		{
		INFO_PRINTF2(_L("ReorderDevice returned error %d, expected KErrNotFound"),err);
		return EFail;
		}

	// Try and reorder a device with a similar key (+1)
	err = iConfig->ReorderDevice(key+1,4);
	
	if (err != KErrNotFound)
		{
		INFO_PRINTF2(_L("ReorderDevice returned error %d, expected KErrNotFound"),err);
		return EFail;
		}

	// Now really reorder the device, can't do much as its the only device in the list
	err = iConfig->ReorderDevice(key,7);

	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("ReorderDevice failed with error %d"),err);
		return EFail;
		}

	// Reorder the device using its existing position, again this shouldn't do anything
	err = iConfig->ReorderDevice(key,0);

	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("ReorderDevice failed with error %d"),err);
		return EFail;
		}
	
	// Check the number of devices
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
	
	INFO_PRINTF1(_L("Reordering non existant devices handled correctly"));
	return EPass;
	}
