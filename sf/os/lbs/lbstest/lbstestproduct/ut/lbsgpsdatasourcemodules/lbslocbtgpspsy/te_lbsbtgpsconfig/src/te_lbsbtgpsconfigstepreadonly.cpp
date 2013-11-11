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
// te_lbsbtgpsconfigstepreadonly.h
// lbsbtgpsconfig unit test step: Read only API access tests 
//

#include "te_lbsbtgpsconfigstepreadonly.h"


CLbsBtGpsConfigStepReadOnly::CLbsBtGpsConfigStepReadOnly()
	{
	SetTestStepName(KLbsBtGpsConfigStepReadOnly);
	}


CLbsBtGpsConfigStepReadOnly::~CLbsBtGpsConfigStepReadOnly()
	{
	
	}


TBool CLbsBtGpsConfigStepReadOnly::RunTestStage(TInt aStage)
	{
	switch(aStage)
		{

		case 1:
			// Reset updates counter
			if (TestAllowed() == EFail)
				{
				SetTestStepResult(EFail);
				}
			return ETrue;

		case 2:
			// Check that we DID NOT get an update notification following the previous stage
			if (iUpdatesRecieved != 0)
				{
				INFO_PRINTF1(_L("No update recieved"));
				SetTestStepResult(EFail);
				}
			return ETrue;

		case 3:
			if (TestNotAllowed() == EFail)
				{
				SetTestStepResult(EFail);
				}
			return ETrue;

		case 4:
			// Check that we DID NOT get an update notification following the previous stage
			if (iUpdatesRecieved != 0)
				{
				INFO_PRINTF1(_L("Unexpected update recieved"));
				SetTestStepResult(EFail);
				}
			return ETrue;
		}
	
	return EFalse;
	}


TVerdict CLbsBtGpsConfigStepReadOnly::TestAllowed()
	{
	TInt err;

	// Get the number of devices
	TInt deviceCount;
	err = iConfig->GetDeviceCount(deviceCount);

	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("GetDeviceCount returned error %d"),err);
		return EFail;
		}

	// Check there are some devices in the list, as the previous test should have left some
	if (deviceCount == 0)
		{
		INFO_PRINTF2(_L("GetDeviceCount returned incorrect count %d, expected > 0"), deviceCount);
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

	// Check that the list size matches the device count as read previously
	if (iDeviceList.Count() != deviceCount)
		{
		INFO_PRINTF3(_L("Returned device list contains %d entries, expected %d"),iDeviceList.Count(),deviceCount);
		return EFail;
		}

	// Test has passed if its got to this point
	INFO_PRINTF1(_L("Read-only access to BT GPS Config API was permitted"));
	return EPass;
	}


TVerdict CLbsBtGpsConfigStepReadOnly::TestNotAllowed()
	{
	TInt err;
	TInt deviceCount;
	
	// Get the number of devices
	err = iConfig->GetDeviceCount(deviceCount);

	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("GetDeviceCount returned error %d"),err);
		return EFail;
		}

	if (deviceCount == 0)
		{
		INFO_PRINTF2(_L("GetDeviceCount returned incorrect count %d, expected > 0"), deviceCount);
		return EFail;
		}

	
	INFO_PRINTF1(_L("Test AddDevice is not allowed"));

	TLbsBtGpsEntryKey key;
	TBTDevAddr addr = iServer->GenerateUniqueBTDevAddr();

	err = iConfig->AddDevice(addr, 0, key);

	// We expect to get KErrPermissionDenied
	if (err != KErrPermissionDenied)
		{
		INFO_PRINTF2(_L("AddDevice returned error %d, expected KErrPermissionDenied"),err);
		return EFail;
		}

	
	INFO_PRINTF1(_L("Test RemoveDevice is not allowed"));
	
	// (using iDeviceList from the previous stage)
	err = iConfig->RemoveDevice(iDeviceList[0]->Key());
	
	if (err != KErrPermissionDenied)
		{
		INFO_PRINTF2(_L("RemoveDevice returned error %d, expected KErrPermissionDenied"),err);
		return EFail;
		}

	// Attempting to remove a non-existant device should not give anything away
	err = iConfig->RemoveDevice(KLbsBtGpsEntryKeyNull);
	
	if (err != KErrPermissionDenied)
		{
		INFO_PRINTF2(_L("RemoveDevice returned error %d, expected KErrPermissionDenied"),err);
		return EFail;
		}
	

	INFO_PRINTF1(_L("Test ReorderDevice is not allowed"));
	
	// (using iDeviceList from the previous stage)
	err = iConfig->ReorderDevice(iDeviceList[0]->Key(),0);
	
	if (err != KErrPermissionDenied)
		{
		INFO_PRINTF2(_L("ReorderDevice returned error %d, expected KErrPermissionDenied"),err);
		return EFail;
		}

	// Attempting to reorder a device to its current position should not give anything away
	err = iConfig->ReorderDevice(iDeviceList[0]->Key(),0);
	
	if (err != KErrPermissionDenied)
		{
		INFO_PRINTF2(_L("ReorderDevice returned error %d, expected KErrPermissionDenied"),err);
		return EFail;
		}
	
	// Attempting to reorder a non-existant device should not give anything away
	err = iConfig->ReorderDevice(KLbsBtGpsEntryKeyNull,0);
	
	if (err != KErrPermissionDenied)
		{
		INFO_PRINTF2(_L("ReorderDevice returned error %d, expected KErrPermissionDenied"),err);
		return EFail;
		}

	
	INFO_PRINTF1(_L("Test EmptyDeviceList is not allowed"));
	
	err = iConfig->EmptyDeviceList();

	if (err != KErrPermissionDenied)
		{
		INFO_PRINTF2(_L("EmptyDeviceList returned error %d, expected KErrPermissionDenied"),err);
		return EFail;
		}


	// Get the number of devices and check it is the same as at the start of the test
	TInt deviceCountEnd;
	err = iConfig->GetDeviceCount(deviceCountEnd);

	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("GetDeviceCount returned error %d"),err);
		return EFail;
		}

	if (deviceCount != deviceCountEnd)
		{
		INFO_PRINTF3(_L("GetDeviceCount returned incorrect count %d, expected %d"), deviceCountEnd, deviceCount);
		return EFail;
		}

	// Test has passed if its got to this point
	INFO_PRINTF1(_L("Write access to BT GPS Config API was not permitted"));
	return EPass;
	}
