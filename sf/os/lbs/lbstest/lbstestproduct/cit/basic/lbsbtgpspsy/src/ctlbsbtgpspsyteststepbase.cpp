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
// Base testexecute test step for the BT GPS PSY Integration tests
//

#include "ctlbsbtgpspsyteststepbase.h"
#include <BtGpsPsyPrivateCRKeys.h>
#include <centralrepository.h>


CT_LbsBtGpsPsyTestStepBase::CT_LbsBtGpsPsyTestStepBase()
	{
	iScheduler = new(ELeave) CActiveScheduler();
	CActiveScheduler::Install(iScheduler);
		
    // Enable he test framework to use bluetooth
    _LIT(KLDDName, "ECOMM");
    #if defined (__WINS__)
    	_LIT(KPDDName, "ECDRV");        
    #else   
    	_LIT(KPDDName, "EUART1");
    #endif
    TInt rerr = User::LoadPhysicalDevice(KPDDName);
    if(rerr != KErrNone && rerr != KErrAlreadyExists)
    	{
    	User::Leave(rerr);
    	}
    rerr = User::LoadLogicalDevice(KLDDName);
    if(rerr != KErrNone && rerr != KErrAlreadyExists)
    	{
        User::Leave(rerr);
        }
	}

CT_LbsBtGpsPsyTestStepBase::~CT_LbsBtGpsPsyTestStepBase()
	{
	delete iScheduler;
	}


TVerdict CT_LbsBtGpsPsyTestStepBase::doTestStepPreambleL()
	{
	// Construct the positioner to enable positioning
	iPositioner = CBtGpsPositioner::NewL(*this);
	
	// Construct the simulation watcher to enable observer callbacks
	iSimWatcher = CBtGpsSimWatcher::NewL(*this);
	
	//Read Test Data Config
	ReadTestDataConfig();
	
	//Enable the positioner
	EnablePositionerL();
	
	//Setup the positioner
	SetupPositionerL();

	return TestStepResult();
	}

/** Reads all the test data from the test execute config .ini file
 */
void CT_LbsBtGpsPsyTestStepBase::ReadTestDataConfig()
	{
	//Test Step ID
	iTestConfig.iTestCaseId = 0;
	if(!GetIntFromConfig(ConfigSection(), _L("TestCaseId"), iTestConfig.iTestCaseId))
		{
		GetIntFromConfig(_L("Default"), _L("TestCaseId"), iTestConfig.iTestCaseId);
		}
	
	//Number of Location Requests
	iTestConfig.iNumOfLocRequests = 1;
	if(!GetIntFromConfig(ConfigSection(), _L("NumOfLocRequests"), iTestConfig.iNumOfLocRequests))
		{
		GetIntFromConfig(_L("Default"), _L("NumOfLocRequests"), iTestConfig.iNumOfLocRequests);
		}
	
	//Location Request Pass
	iTestConfig.iLocationRequestPass = ETrue;
	if(!GetIntFromConfig(ConfigSection(), _L("LocationRequestPass"), iTestConfig.iLocationRequestPass))
		{
		GetIntFromConfig(_L("Default"), _L("LocationRequestPass"), iTestConfig.iLocationRequestPass);
		}
	
	//Max Age
	iTestConfig.iMaxAge = 0;
	if(!GetIntFromConfig(ConfigSection(), _L("MaxAge"), iTestConfig.iMaxAge))
		{
		GetIntFromConfig(_L("Default"), _L("MaxAge"), iTestConfig.iMaxAge);
		}
	
	//Timeout
	iTestConfig.iTimeout = 0;
	if(!GetIntFromConfig(ConfigSection(), _L("TimeOut"), iTestConfig.iTimeout))
		{
		GetIntFromConfig(_L("Default"), _L("TimeOut"), iTestConfig.iTimeout);
		}
	
	//Interval
	iTestConfig.iInterval = 0;
	if(!GetIntFromConfig(ConfigSection(), _L("Interval"), iTestConfig.iInterval))
		{
		GetIntFromConfig(_L("Default"), _L("Interval"), iTestConfig.iInterval);
		}
	
	//Partial Updates
	iTestConfig.iPartialUpdates = EFalse;
	if(!GetIntFromConfig(ConfigSection(), _L("PartialUpdates"), iTestConfig.iPartialUpdates))
		{
		GetIntFromConfig(_L("Default"), _L("PartialUpdates"), iTestConfig.iPartialUpdates);
		}
	}

	

void CT_LbsBtGpsPsyTestStepBase::EnablePositionerL()
	{
	TRAPD(err, iPositioner->EnableL());
	if(err != KErrNone)
		{
		INFO_PRINTF2(_L("ERROR - Failed to enable the Positioner, err=%d"), err);
		User::Leave(err);
		}
	}


void CT_LbsBtGpsPsyTestStepBase::SetupPositionerL()
	{
	TPositionUpdateOptions options;
	TInt err = KErrNone;

	//Open the positioner
	TRAP(err, iPositioner->OpenL());
	
	if(err != KErrNone)
		{
		INFO_PRINTF2(_L("ERROR - Failed to open the Positioner, err=%d"), err);
		User::Leave(err);
		}
	
	//Set the update options
	options.SetMaxUpdateAge(TTimeIntervalMicroSeconds(iTestConfig.iMaxAge));
	options.SetUpdateTimeOut(TTimeIntervalMicroSeconds(iTestConfig.iTimeout));
	options.SetUpdateInterval(TTimeIntervalMicroSeconds(iTestConfig.iInterval));
	options.SetAcceptPartialUpdates(iTestConfig.iPartialUpdates);
	iPositioner->Positioner().SetUpdateOptions(options);
	}

/** Makes a standard location request to the BT PSY
 */
TInt CT_LbsBtGpsPsyTestStepBase::LocationRequest()
	{
	ResetEventCounters();

	INFO_PRINTF1(_L("Requesting Location"));
	iPositioner->StartLocationRequest();
	
	CActiveScheduler::Start();
	
	//Check to see if the location request completed as expected
	if((iGotPositionUpdate == 0) && (iTestConfig.iLocationRequestPass))
		{
		INFO_PRINTF1(_L("ERROR - Position Update not received"));
		return KErrGeneral;
		}
	else if((iGotPositionUpdate != 0) && (!iTestConfig.iLocationRequestPass))
		{
		INFO_PRINTF1(_L("ERROR - Position Update received, but not expected"));
		return KErrGeneral;
		}
	return KErrNone;
	}

TVerdict CT_LbsBtGpsPsyTestStepBase::doTestStepPostambleL()
	{
	delete iPositioner;
	iPositioner = NULL;
	delete iSimWatcher;
	iSimWatcher = NULL;
	return TestStepResult();
	}


void CT_LbsBtGpsPsyTestStepBase::OnPositionUpdate(const TPosition& aPosition)
	{
	++iGotPositionUpdate;
	INFO_PRINTF3(_L("Position update received: lat=%3.4f, long=%3.4f"), 
			aPosition.Latitude(), aPosition.Longitude());
	CActiveScheduler::Stop();
	}


void CT_LbsBtGpsPsyTestStepBase::OnPositionError(TInt aError)
	{
	++iGotPositionError;
	INFO_PRINTF2(_L("Positioning error: %d"), aError);
	CActiveScheduler::Stop();
	}


void CT_LbsBtGpsPsyTestStepBase::OnDeviceConnect(const TBTSockAddr& aAddr)
	{
	++iGotDeviceConnect;
	TBuf<20> readableAddr;
	aAddr.BTAddr().GetReadable(readableAddr);
	INFO_PRINTF2(_L("Device [%S] connected"), &readableAddr);
	}


void CT_LbsBtGpsPsyTestStepBase::OnDeviceDisconnect(const TBTSockAddr& aAddr)
	{
	++iGotDeviceDisconnect;
	TBuf<20> readableAddr;
	aAddr.BTAddr().GetReadable(readableAddr);
	INFO_PRINTF2(_L("Device [%S] disconnected"), &readableAddr);
	}


void CT_LbsBtGpsPsyTestStepBase::OnDeviceError(const TBTSockAddr& aAddr)
	{
	++iGotDeviceError;
	TBuf<20> readableAddr;
	aAddr.BTAddr().GetReadable(readableAddr);
	INFO_PRINTF2(_L("Device [%S] error"), &readableAddr);
	}


void CT_LbsBtGpsPsyTestStepBase::ResetEventCounters()
	{
	iGotPositionUpdate = 0;
	iGotPositionError = 0;
	iGotDeviceConnect = 0;
	iGotDeviceDisconnect = 0;
	iGotDeviceError = 0;
	}


/** Used to set the night mode of the PSY (0=Off, 1=ON)
 */
TInt CT_LbsBtGpsPsyTestStepBase::SetupNightModeL(TBool aNightMode)
	{	
	TInt err = KErrNone;
	
	//Open the BT PSY SettingsRepository
	CRepository* repos = CRepository::NewLC(KCRUidBtGpsPsy);
	
	//Setup the night mode
	err =  repos->Set(KBluetoothGpsPsyNightModeState, aNightMode);
	
	CleanupStack::PopAndDestroy(repos);
	
	return err;
	}
