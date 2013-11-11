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
// Test step which makes a standard Location Request whilst issuing PNOK commands
// Contains multiple test steps:
// 1. Tests sending Night Mode down to the PNOK device
// 2. Tests receiving PNOK STAT messages from the PNOK device
//



#include "ctlbsbtgpspsypnokstep.h"
#include <bt_sock.h>
#include "btgpssim.h"

//PNOK STAT Message.  Defines Battery Level as 50
_LIT8(KPnokStatMsg, "$PNOK,STAT,50,0,1,0");


CT_LbsBtGpsPsyPnokStep::CT_LbsBtGpsPsyPnokStep()
	{
	SetTestStepName(KBTGPSPsyPnokStep);
	}


CT_LbsBtGpsPsyPnokStep::~CT_LbsBtGpsPsyPnokStep()
	{
	if(iTestConfig.iTestCaseId == 2)
		delete iEventsWatcher;
	}


TVerdict CT_LbsBtGpsPsyPnokStep::doTestStepPreambleL()
	{
	// Call base class version first
	CT_LbsBtGpsPsyTestStepBase::doTestStepPreambleL();
	
	iPnokBatteryLevel = 0;
	
	return TestStepResult();
	}


TVerdict CT_LbsBtGpsPsyPnokStep::doTestStepL()
	{
	if(TestStepResult()==EPass)
		{
		TInt err = KErrNone;
		RBtGpsSim sim;
		
		//Connect to Sim framework
		err = sim.Connect();
		if(err != KErrNone)
			{
			INFO_PRINTF2(_L("Error, Failed to open Sim Framework with error %d"), err);
			SetTestStepResult(EFail);
			//End the test here, no point in continuing, problem with the simulation device framework
			return TestStepResult();
			}
		
		//Make a location request to get the device connected
		err = LocationRequest();
		if(err != KErrNone)
			{
			SetTestStepResult(EFail);
			}
		
		//Setup the correct PNOK event
		if(iTestConfig.iTestCaseId == 1)
			{
			//Enable night mode
			SetupNightModeL(ETrue);
			}
		else if(iTestConfig.iTestCaseId == 2)
			{
			//Subscribe to the PNOK event property
			iEventsWatcher = CBtGpsEventsWatcher::NewL(*this, KBluetoothGpsPsyBatteryLevel);
			
			//Open the first PNOK device in the list and update its PNOK value
			RBtGpsSimDevice simPnokDevice;
			err = simPnokDevice.Open(sim, 0);
			if(err == KErrNone)
				{
				simPnokDevice.Command(EBtGpsSimSendSTATMessage, KPnokStatMsg);
				}
			else
				{
				INFO_PRINTF2(_L("Error %d, unable to open SIM device to send PNOK message"), err);
				SetTestStepResult(EFail);
				}
			simPnokDevice.Close();
			}
		
		//Make the location requests
		for(TInt requestNum=0; requestNum < iTestConfig.iNumOfLocRequests; ++requestNum)
			{
			err = LocationRequest();
			if(err != KErrNone)
				{
				//Fail the test, but keep on trying to get the remaining location requests
				SetTestStepResult(EFail);
				}
			}

		//Open up the SIM device
		RBtGpsSimDevice simDevice;
		err = simDevice.OpenConnected(sim);
		if(err != KErrNone)
			{
			INFO_PRINTF2(_L("Error, Failed to open Sim Device with error %d"), err);
			SetTestStepResult(EFail);
			return TestStepResult();	
			}
		
		//Check the PNOK event occurred correctly
		if(iTestConfig.iTestCaseId == 1)
			{
			//Check to see if the night mode was enabled correctly
			TBool nightMode;
			simDevice.GetParameter(EBtGpsSimNightMode, nightMode);
			
			if(nightMode)
				{
				INFO_PRINTF1(_L("Night Mode correctly set"));
				}
			else
				{
				INFO_PRINTF1(_L("ERROR - Night Mode not setup correctly"));
				SetTestStepResult(EFail);
				}
			}
		else if(iTestConfig.iTestCaseId == 2)
			{
			if(iPnokBatteryLevel != 50)
				{
				//Err
				INFO_PRINTF2(_L("Error - PNOK battery level not correct: %d"), iPnokBatteryLevel);
				SetTestStepResult(EFail);
				}
			else
				{
				INFO_PRINTF1(_L("PNOK Battery Level updated correctly"));
				}
			}
		
		simDevice.Close();
		sim.Close();
		}
	return TestStepResult();
	}


TVerdict CT_LbsBtGpsPsyPnokStep::doTestStepPostambleL()
	{
	// Call base class version first
	CT_LbsBtGpsPsyTestStepBase::doTestStepPostambleL();
	
	return TestStepResult();
	}

/* Called when a PNOK property event is updated/changed
 */
void CT_LbsBtGpsPsyPnokStep::OnEventsUpdated(const TUint aMask, TInt aValue)
	{
	switch(aMask)
		{
		case KBluetoothGpsPsyBatteryLevel:
			iPnokBatteryLevel = aValue;
			break;
		
		default:
			INFO_PRINTF3(_L("PNOK message with mask id: %d sent with value: %d"), aMask, aValue);
			break;
		}
	}
