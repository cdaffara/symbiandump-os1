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
// te_lbsbtgpsconfigstepMultipleClient.h
// lbsbtgpsconfig unit test step: Multiple client tests
//

#include "te_lbsbtgpsconfigstepmultipleclient.h"
#include "lbsbtgpsdevicerecord.h"

// Number of threads to use in testing
const int KTestNumThreads = 5;


CLbsBtGpsConfigStepMultipleClient::CLbsBtGpsConfigStepMultipleClient()
	{
	SetTestStepName(KLbsBtGpsConfigStepMultipleClient);
	}


CLbsBtGpsConfigStepMultipleClient::~CLbsBtGpsConfigStepMultipleClient()
	{
	iThreads.ResetAndDestroy();
	iThreadDeviceList.ResetAndDestroy();
	}


TBool CLbsBtGpsConfigStepMultipleClient::RunTestStage(TInt aStage)
	{
	switch(aStage)
		{

		case 1:
			// Create the threads
			for (TInt index=0; index<KTestNumThreads; ++index)
				{
				CTestBtGpsConfigThread* thread = CTestBtGpsConfigThread::NewL();
				iThreads.Append(thread);
				thread->Start();
				}
			return ETrue;
		
		case 2:
			if (TestUpdates() == EFail)
				{
				SetTestStepResult(EFail);
				}
			return ETrue;

		case 3:
			// Check that we got an update notification following the previous stages
			if (iUpdatesRecieved != 1)
				{
				INFO_PRINTF1(_L("No update recieved"));
				SetTestStepResult(EFail);
				}
			return ETrue;

		case 4:
			if (TestThreadAdd() == EFail)
				{
				SetTestStepResult(EFail);
				}
			return ETrue;

		case 5:
			// Check that we got an update notification following the previous stages
			if (iUpdatesRecieved != 2)
				{
				INFO_PRINTF1(_L("No update recieved"));
				SetTestStepResult(EFail);
				}
			return ETrue;

		case 6:
			if (TestThreadReorder() == EFail)
				{
				SetTestStepResult(EFail);
				}
			return ETrue;

		case 7:
			// Check that we got an update notification following the previous stages
			if (iUpdatesRecieved != 3)
				{
				INFO_PRINTF1(_L("No update recieved"));
				SetTestStepResult(EFail);
				}
			return ETrue;

		case 8:
			if (TestThreadRemove() == EFail)
				{
				SetTestStepResult(EFail);
				}
			return ETrue;

		case 9:
			// Check that we got an update notification following the previous stages
			if (iUpdatesRecieved != 4)
				{
				INFO_PRINTF1(_L("No update recieved"));
				SetTestStepResult(EFail);
				}
			return ETrue;
			
		case 10:
			// Kill the threads
			iThreads.ResetAndDestroy();
			return ETrue;
		}
	
	return EFalse;
	}


TVerdict CLbsBtGpsConfigStepMultipleClient::TestUpdates()
	{
	TInt err;
	TInt numAdded = 0;

	INFO_PRINTF1(_L("Test threads recieve update notifications"));
	
	// First empty the device list
	err = iConfigInternal->EmptyDeviceList();
	if (err != KErrNone)
		{
		INFO_PRINTF1(_L("Failed to empty device list"));
		return EFail;
		}

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
		
		iKeys.Append(key);
		
		for (TInt index=0; index<iThreads.Count(); ++index)
			{
			iThreadDeviceList.ResetAndDestroy();
			TInt threadErr = iThreads[index]->GetDeviceList(iThreadDeviceList);
			
			if (threadErr != KErrNone)
				{
				INFO_PRINTF3(_L("Thread %d reported error %d accessing device list"),index,err);
				return EFail;
				}

			if (iThreadDeviceList.Count() != numAdded+1)
				{
				INFO_PRINTF3(_L("Thread %d device list not correct (%d)"),index,iThreadDeviceList.Count());
				return EFail;
				}
			}
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

	
	INFO_PRINTF1(_L("Threads recieved updates"));
	return EPass;
	}


TVerdict CLbsBtGpsConfigStepMultipleClient::TestThreadAdd()
	{
	TInt err;
	TInt numAdded = 0;

	INFO_PRINTF1(_L("Test adding devices simultaneously from multiple threads"));
	
	// First empty the device list
	err = iConfigInternal->EmptyDeviceList();
	if (err != KErrNone)
		{
		INFO_PRINTF1(_L("Failed to empty device list"));
		return EFail;
		}

	// Command each thread to add a device to the list
	for (numAdded=0; numAdded<iThreads.Count(); ++numAdded)
		{
		INFO_PRINTF2(_L("Commanding thread %d to add a device"),numAdded);
		
		TBTDevAddr addr = iServer->GenerateUniqueBTDevAddr();

		iThreads[numAdded]->SetTaskAddDevice(addr,0);
		}

	// Check the devices were added
	for (TInt index=0; index<numAdded; ++index)
		{
		do
			{
			err = iThreads[index]->GetTaskError();
			}
		while (err > 0); // Wait for the task to actually run
		
		if (err != KErrNone)
			{
			INFO_PRINTF3(_L("Thread %d AddDevice returned error %d"),index,err);
			return EFail;
			}

		INFO_PRINTF2(_L("Device added by thread %d"),index);
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
	
	INFO_PRINTF1(_L("Added devices simultaneously via threads"));
	return EPass;
	}


TVerdict CLbsBtGpsConfigStepMultipleClient::TestThreadReorder()
	{
	TInt err;
	TInt index;

	INFO_PRINTF1(_L("Test reordering devices simultaneously from multiple threads"));

	// Get the number of devices
	TInt deviceCount;
	err = iConfigInternal->GetDeviceCount(deviceCount);

	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("GetDeviceCount returned error %d"),err);
		return EFail;
		}
	
	// Get the device list
	iConfigInternal->GetDeviceRecordListL(iDeviceRecordList);

	iKeys.Reset();

	for (index=0; index<deviceCount; ++index)
		{
		iKeys.Append(iDeviceRecordList[index]->Key());
		}
	
	index = 0;
	TInt newPosition = deviceCount - index - 1;
	INFO_PRINTF3(_L("Commanding thread 0 to reorder device key %d to position %d"),iDeviceRecordList[index]->Key(),newPosition);
	iThreads[0]->SetTaskReorderDevice(iDeviceRecordList[index]->Key(),newPosition);
	
	index = deviceCount - 1;
	newPosition = deviceCount - index - 1;
	INFO_PRINTF3(_L("Commanding thread 1 to reorder device key %d to position %d"),iDeviceRecordList[index]->Key(),newPosition);
	iThreads[1]->SetTaskReorderDevice(iDeviceRecordList[index]->Key(),newPosition);
	
	// Check the reorder commands completed
	for (index=0; index<2; ++index)
		{
		do
			{
			err = iThreads[index]->GetTaskError();
			}
		while (err > 0); // Wait for the task to actually run
		
		if (err != KErrNone)
			{
			INFO_PRINTF3(_L("Thread %d ReorderDevice returned error %d"),index,err);
			return EFail;
			}

		INFO_PRINTF2(_L("Device reordered by thread %d"),index);
		}

	// Get the device list
	iDeviceRecordList.ResetAndDestroy();
	iConfigInternal->GetDeviceRecordListL(iDeviceRecordList);
	
	// First and last devices in the list should be reversed
	for (index=0; index<deviceCount; ++index)
		{
		TLbsBtGpsEntryKey key1 = iKeys[index];
		if (index == 0 || index == deviceCount-1)
			{
			key1 = iKeys[deviceCount-index-1];
			}
		TLbsBtGpsEntryKey key2 = iDeviceRecordList[index]->Key();
		if (key1 != key2)
			{
			INFO_PRINTF4(_L("Device entry %d key should be %d, not %d"),index,key1,key2);
			return EFail;
			}
		}
	
	INFO_PRINTF1(_L("Reordered devices simultaneously via threads"));
	return EPass;
	}


TVerdict CLbsBtGpsConfigStepMultipleClient::TestThreadRemove()
	{
	TInt err;
	TInt index;

	INFO_PRINTF1(_L("Test removing devices simultaneously from multiple threads"));

	// Get the device list
	iConfigInternal->GetDeviceRecordListL(iDeviceRecordList);

	// Command each thread to remove a device in the list
	for (index=0; index<iThreads.Count(); ++index)
		{
		INFO_PRINTF3(_L("Commanding thread %d to remove device %d"),index,index);
		
		iThreads[index]->SetTaskRemoveDevice(iDeviceRecordList[index]->Key());
		}

	// Check the remove commands completed
	for (index=0; index<iThreads.Count(); ++index)
		{
		do
			{
			err = iThreads[index]->GetTaskError();
			}
		while (err > 0); // Wait for the task to actually run
		
		if (err != KErrNone)
			{
			INFO_PRINTF3(_L("Thread %d RemoveDevice returned error %d"),index,err);
			return EFail;
			}

		INFO_PRINTF2(_L("Device removed by thread %d"),index);
		}

	// Get the number of devices and check it is correct
	TInt deviceCount;
	err = iConfigInternal->GetDeviceCount(deviceCount);

	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("GetDeviceCount returned error %d"),err);
		return EFail;
		}

	if (deviceCount != 0)
		{
		INFO_PRINTF3(_L("GetDeviceCount returned incorrect count %d, expected %d"), deviceCount, 0);
		return EFail;
		}
	
	INFO_PRINTF1(_L("Removed devices simultaneously via threads"));
	return EPass;
	}
