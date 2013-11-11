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
// Test step which tests device list updates with the BT PSY
// Contains multiple test steps:
// 1. Device is added to the list during the connection process; PSY should not see/use this device and should get the
// location update from the different user entered device.
// 2. Device is removed during the connection process; PSY should not see this and should still connect to the device.
// 3. Device is added to the list during the connection process; PSY should not see this change.  The test enters the
// same device via the device selection dialog and should get a location update from this device.
//



#include "ctlbsbtgpspsylistupdatesstep.h"
#include <bt_sock.h>
#include "btgpssim.h"


CT_LbsBtGpsPsyListUpdatesStep::CT_LbsBtGpsPsyListUpdatesStep()
	{
	SetTestStepName(KBTGPSPsyListUpdatesStep);
	}


CT_LbsBtGpsPsyListUpdatesStep::~CT_LbsBtGpsPsyListUpdatesStep()
	{
	}


TVerdict CT_LbsBtGpsPsyListUpdatesStep::doTestStepPreambleL()
	{
	// Call base class version first
	CT_LbsBtGpsPsyTestStepBase::doTestStepPreambleL();
	
	iConfig = CLbsBtGpsConfig::NewL();
	
	return TestStepResult();
	}


TVerdict CT_LbsBtGpsPsyListUpdatesStep::doTestStepL()
	{
	if(TestStepResult()==EPass)
		{
		RPointerArray<TLbsBtGpsDeviceInfo> list;
		TLbsBtGpsEntryKey key;
		TInt numOfDevices = 0;
		
		ResetEventCounters();
		
		//Make the first location request
		INFO_PRINTF1(_L("Requesting Location"));
		iPositioner->StartLocationRequest();
		
		//Unavailable devices at the beginning of the list will cause a delay before a it reaches the
		// end of the list/valid device.  Wait for a few seconds to ensure we are in the connecting process
		User::After(1 * 1000 * 1000);
		
		iConfig->GetDeviceCount(numOfDevices);
		
		if((iTestConfig.iTestCaseId == 1) || (iTestConfig.iTestCaseId == 3))
			{
			//Add the new device to the end of the list
			CIniData* btListConfigFile = CIniData::NewL(_L("c:\\testdata\\configs\\btgpsdevicelist.ini"));
			
			TPtrC btAddress;
			if(!btListConfigFile->FindVar(_L("BT1"), _L("Address"), btAddress))
				{
				INFO_PRINTF1(_L("Error, cannot find new BT device to add"));
				SetTestStepResult(EFail);
				return TestStepResult();
				}
			
			TInt64 hwAddrInt(0);
			TLex lex(btAddress);
			User::LeaveIfError(lex.Val(hwAddrInt, EHex));
			iConfig->AddDevice(hwAddrInt, numOfDevices, key);
			}
		else if(iTestConfig.iTestCaseId == 2)
			{
			//Delete the device from the list (but not the actual device)
			iConfig->GetDeviceListL(list);
			
			//Available device should be the last device in the list
			key = list[(numOfDevices - 1)]->Key();
			iConfig->RemoveDevice(key);
			}
		
		//Wait for the location request to complete
		CActiveScheduler::Start();
			
		//Check to see if the location request completed as expected
		if((iGotPositionUpdate == 0) && (iTestConfig.iLocationRequestPass))
			{
			INFO_PRINTF1(_L("ERROR - Position Update not received"));
			SetTestStepResult(EFail);
			}
		else if((iGotPositionUpdate != 0) && (!iTestConfig.iLocationRequestPass))
			{
			INFO_PRINTF1(_L("ERROR - Position Update received, but not expected"));
			SetTestStepResult(EFail);
			}
		}
	return TestStepResult();
	}


TVerdict CT_LbsBtGpsPsyListUpdatesStep::doTestStepPostambleL()
	{
	// Call base class version first
	CT_LbsBtGpsPsyTestStepBase::doTestStepPostambleL();
	
	return TestStepResult();
	}
