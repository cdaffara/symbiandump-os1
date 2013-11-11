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
// te_lbsbtgpsconfigstepinternalapi.h
// lbsbtgpsconfig unit test step: Internal API tests
//

#include "te_lbsbtgpsconfigstepinternalapi.h"
#include "lbsbtgpsdevicerecord.h"


CLbsBtGpsConfigStepInternalApi::CLbsBtGpsConfigStepInternalApi()
	{
	SetTestStepName(KLbsBtGpsConfigStepInternalApi);
	}


CLbsBtGpsConfigStepInternalApi::~CLbsBtGpsConfigStepInternalApi()
	{
	
	}


TBool CLbsBtGpsConfigStepInternalApi::RunTestStage(TInt aStage)
	{
	switch(aStage)
		{
	
		case 1:
			if (TestUpdateDevice() == EFail)
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
		}
	
	return EFalse;
	}


TVerdict CLbsBtGpsConfigStepInternalApi::TestUpdateDevice()
	{
	TInt err;
	TInt numAdded = 0;

	INFO_PRINTF1(_L("Test internal API to update device record in the list"));
	
	// First empty the device list
	err = iConfigInternal->EmptyDeviceList();
	if (err != KErrNone)
		{
		INFO_PRINTF1(_L("Failed to empty device list"));
		return EFail;
		}

	// This will store all the keys for devices that are added
	iKeys.Reset();

	// Add some devices to the list
	for (numAdded=0; numAdded<10; ++numAdded)
		{
		INFO_PRINTF3(_L("Adding device %d at position %d"),numAdded,numAdded);
		
		TLbsBtGpsEntryKey key;
		TBTDevAddr addr = iServer->GenerateUniqueBTDevAddr();
		
		err = iConfigInternal->AddDevice(addr, numAdded, key);

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
	err = iConfigInternal->GetDeviceCount(deviceCount);

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

	// Now update some devices at random
	TInt index;
	for (index=0; index<5; ++index)
		{
		// Decide which entry to reorder
		TInt keyIndex=0;
		TInt newType=0;
		switch (index)
			{
			case 0: keyIndex = 2; newType = 1; break;
			case 1: keyIndex = 1; newType = 2; break;
			case 2: keyIndex = 5; newType = 3; break;
			case 3: keyIndex = 4; newType = 1; break;
			case 4: keyIndex = 0; newType = 0; break;
			}

		INFO_PRINTF3(_L("Updating device %d to type %d"),keyIndex,newType);
		
		TLbsBtGpsDeviceRecord record;
		record.SetKey(iKeys[keyIndex]);
		TBTSockAddr sockAddr;
		sockAddr.SetBTAddr(iServer->GenerateUniqueBTDevAddr());
		record.SetSocketAddress(sockAddr);
		record.SetType(newType);
		
		err = iConfigInternal->UpdateDevice(record);

		if (err != KErrNone)
			{
			INFO_PRINTF2(_L("UpdateDevice returned error %d"),err);
			return EFail;
			}
		}

	// Attempt to update a non-existant device
	
	TLbsBtGpsDeviceRecord crudRecord;
	crudRecord.SetKey(KLbsBtGpsEntryKeyNull);
	TBTSockAddr sockAddr;
	sockAddr.SetBTAddr(iServer->GenerateUniqueBTDevAddr());
	crudRecord.SetSocketAddress(sockAddr);
	
	err = iConfigInternal->UpdateDevice(crudRecord);
	
	if (err != KErrNotFound)
			{
			INFO_PRINTF2(_L("UpdateDevice returned error %d, expected KErrNotFound"),err);
			return EFail;
			}
	
	// Retrieve the device list using the API
	iDeviceRecordList.ResetAndDestroy();
	TRAP(err, iConfigInternal->GetDeviceRecordListL(iDeviceRecordList));

	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("GetDeviceRecordListL left with error %d"),err);
		return EFail;
		}

	// Check the returned list contains the expected number of items
	if (iDeviceRecordList.Count() != numAdded)
		{
		INFO_PRINTF3(_L("List contains %d items, expected %d"),iDeviceRecordList.Count(),numAdded);
		return EFail;
		}
	
	// Check the devices are of the correct type following the updates
	for (index=0; index<iDeviceRecordList.Count(); ++index)
		{
		TInt expType=0;
		switch (index)
			{
			case 0: expType = 0; break;
			case 1: expType = 2; break;
			case 2: expType = 1; break;
			case 3: expType = 0; break;
			case 4: expType = 1; break;
			case 5: expType = 3; break;
			case 6: expType = 0; break;
			case 7: expType = 0; break;
			case 8: expType = 0; break;
			case 9: expType = 0; break;
			}

		TInt actType = iDeviceRecordList[index]->Type();
		if (actType != expType)
			{
			INFO_PRINTF4(_L("Device entry %d type should be %d, not %d"),index,expType,actType);
			return EFail;
			}
		}
	
	INFO_PRINTF1(_L("Items in the list were updated as requested"));
	return EPass;
	}
