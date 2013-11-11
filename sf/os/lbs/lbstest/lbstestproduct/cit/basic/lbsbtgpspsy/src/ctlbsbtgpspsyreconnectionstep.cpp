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
// Test step which tests reconnection with the BT PSY
// Contains multiple test steps:
// 1. Tests the PSY attempts to reconnect to the previously connection device after being disconnected from it
// 2. Test to ensure the BT PSY attempts to reconnect to devices in the list after it has been disconnected 
// from one of the devices.  Next available device is after the current device in the list.
// 3. Test to ensure the BT PSY attempts to reconnect to devices in the list after it has been disconnected 
// from one of the devices.  Next available device is before the current device in the list (so the first
// device is off at the beginning of the test, but is then turned on).
// 4. Test to ensure the BT PSY is able to deal with an available valid device in the list (with device id 2 and
// 3), but which are not paired with the handset.  Even though it is listed as available, it should go back
// through the pairing process.  Test ensures the connecting process completes correctly, by ensuring it can
// reconnect to a device in the list above it (i.e. it should refresh the device list)
//



#include "ctlbsbtgpspsyreconnectionstep.h"
#include <bt_sock.h>
#include "btgpssim.h"


CT_LbsBtGpsPsyReconnectionStep::CT_LbsBtGpsPsyReconnectionStep()
	{
	SetTestStepName(KBTGPSPsyReconnectionStep);
	}


CT_LbsBtGpsPsyReconnectionStep::~CT_LbsBtGpsPsyReconnectionStep()
	{
	}


TVerdict CT_LbsBtGpsPsyReconnectionStep::doTestStepPreambleL()
	{
	// Call base class version first
	CT_LbsBtGpsPsyTestStepBase::doTestStepPreambleL();
	
	return TestStepResult();
	}


TVerdict CT_LbsBtGpsPsyReconnectionStep::doTestStepL()
	{
	if(TestStepResult()==EPass)
		{
		TInt err = KErrNone;
		RBtGpsSim sim;
		RBtGpsSimDevice firstDevice;
		RBtGpsSimDevice connectedDevice;
		TBTSockAddr sockAddr;
		TBtGpsSimDeviceType type;
		
		//Connect to Sim framework
		err = sim.Connect();
		if(err != KErrNone)
			{
			INFO_PRINTF2(_L("Error, Failed to open Sim Framework with error %d"), err);
			SetTestStepResult(EFail);
			//End the test here, no point in continuing, problem with the simulation device framework
			return TestStepResult();
			}
		
		if((iTestConfig.iTestCaseId == 3) || (iTestConfig.iTestCaseId == 4))
			{
			//Turn of the first device
			err = firstDevice.Open(sim, 0);
			if(err == KErrNone)
				{
				firstDevice.SetParameter(EBtGpsSimActive, EFalse);
				}
			else
				{
				INFO_PRINTF2(_L("Error, Failed to open first Sim Device with error %d"), err);
				SetTestStepResult(EFail);
				return TestStepResult();
				}
			}
		if(iTestConfig.iTestCaseId == 4)
			{
			//Turn device pairing off for the second device (the device we are connecting to first)
			RBtGpsSimDevice secondDevice;
			err = secondDevice.Open(sim, 1);
			if(err == KErrNone)
				{
				secondDevice.SetParameter(EBtGpsSimPaired, EFalse);
				}
			else
				{
				INFO_PRINTF2(_L("Error, Failed to open second Sim Device with error %d"), err);
				SetTestStepResult(EFail);
				return TestStepResult();
				}
			}
		
		//Make the first location request
		err = LocationRequest();
		if(err != KErrNone)
			{
			SetTestStepResult(EFail);
			}
		
		//Remove the currently connected device
		err = connectedDevice.OpenConnected(sim);
		if(err != KErrNone)
			{
			INFO_PRINTF2(_L("Error, Failed to open Sim Device with error %d"), err);
			SetTestStepResult(EFail);
			return TestStepResult();
			}
		else
			{
			//Store the information to allow it to be created again
			connectedDevice.GetParameter(EBtGpsSimBTAddress, sockAddr);
			TInt tempType;
			connectedDevice.GetParameter(EBtGpsSimType, tempType);
			type = static_cast<TBtGpsSimDeviceType>(tempType);
			
			//Remove the device
			connectedDevice.Remove();
			}
		
		if(iTestConfig.iTestCaseId == 1)
			{
			//Re-create the deivce with the same details, to allow it to be reconnected to
			RBtGpsSimDevice reOpenDevice;
			err = reOpenDevice.Create(sim, type, sockAddr);
			if(err == KErrNone)
				{
				reOpenDevice.SetParameter(EBtGpsSimAuthorised, ETrue);
				reOpenDevice.SetParameter(EBtGpsSimPin, _L8("0000"));
				}
			else
				{
				INFO_PRINTF2(_L("Error, Failed to re-create the Sim Device with error %d"), err);
				SetTestStepResult(EFail);
				return TestStepResult();
				}
			}
		else if((iTestConfig.iTestCaseId == 3) || (iTestConfig.iTestCaseId == 4))
			{
			firstDevice.SetParameter(EBtGpsSimActive, ETrue);
			firstDevice.Close();
			}
		 
		// It can take a while for the simulation device to become available again, as such the
		// first location request may fail. However, as long as we re-connect to the device
		// and get a position returned, pass the test.
		// So set the test to fail here, if we get a location update it will be set back to pass.
		SetTestStepResult(EFail);
		
		//Make the remaining location requests
		for(TInt requestNum=0; requestNum < iTestConfig.iNumOfLocRequests; ++requestNum)
			{
			err = LocationRequest();
			if(err == KErrNone)
				{
				//Got a position correctly, pass the test.
				SetTestStepResult(EPass);
				}
			}
		
		connectedDevice.Close();
		sim.Close();
		}
	return TestStepResult();
	}


TVerdict CT_LbsBtGpsPsyReconnectionStep::doTestStepPostambleL()
	{
	// Call base class version first
	CT_LbsBtGpsPsyTestStepBase::doTestStepPostambleL();
	
	return TestStepResult();
	}
