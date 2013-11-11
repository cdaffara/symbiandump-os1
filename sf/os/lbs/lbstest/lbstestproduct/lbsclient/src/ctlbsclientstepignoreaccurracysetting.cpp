// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// @file ctlbsclientstepignoreaccurracysetting.cpp
// 
//
 
#include "ctlbsclientstepignoreaccurracysetting.h"

#include <lbs.h>
#include <lbssatellite.h>
#include <lbs/lbsadmin.h>

#include <lbs/test/tlbsutils.h>
#include <lbs/test/ctlbsasyncwaiter.h>

#define EXPECTED_GPS_ACCURACY_HORIZONTAL 10
#define EXPECTED_GPS_ACCURACY_VERTICAL 10

const TTimeIntervalMicroSeconds KModuleTimeout = 5000000;//makes test module time out(which should be more than the Maxtime in lbsprofile.ini)

/**
 * Construction.
 */
CT_LbsClientStep_IgnoreAccuracySetting* CT_LbsClientStep_IgnoreAccuracySetting::New(CT_LbsClientServer& aParent)
	{
	// Note the lack of ELeave.
	// This means that having insufficient memory will return NULL;
	CT_LbsClientStep_IgnoreAccuracySetting* testStep = new CT_LbsClientStep_IgnoreAccuracySetting(aParent);
	if (testStep)
		{
		TInt err = KErrNone;

		TRAP(err, testStep->ConstructL());
		if (err)
			{
			delete testStep;
			testStep = NULL;
			}
		}
		
	return testStep;
	}


CT_LbsClientStep_IgnoreAccuracySetting::CT_LbsClientStep_IgnoreAccuracySetting(CT_LbsClientServer& aParent) 
	: CT_LbsClientStep(aParent), iTestCaseId(0)
	{
	SetTestStepName(KLbsClientStep_IgnoreAccuracySetting);
	}


void CT_LbsClientStep_IgnoreAccuracySetting::ConstructL()
	{
	// Connect server.
	User::LeaveIfError(iServer.Connect());
	}


/**
 * Destructor
 */
CT_LbsClientStep_IgnoreAccuracySetting::~CT_LbsClientStep_IgnoreAccuracySetting()
	{
	iPositioner.Close();
	CloseNetSim();
	iServer.Close();	
	}
/**
 * The set-up for the tests open the net sim, where required (the terminal based 
 * tests) and open the connection to the server. Also it sets the update options 
 * for the requests with those in the ini file. RPositioner is open through one 
 * of the three APIs, depending on the tests
 */
TVerdict CT_LbsClientStep_IgnoreAccuracySetting::doTestStepPreambleL()
	{
	// Process some common pre setting to test steps then set SetTestStepResult to EFail or Epass.
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsClientStep::doTestStepPreambleL()"));
	SetTestStepResult(EPass);
	T_LbsUtils utils;


	_LIT(KExpectedApiBehaviour, "expected_api_behaviour");
	TInt expectedApiBehaviour;
	if(GetIntFromConfig(ConfigSection(), KExpectedApiBehaviour, expectedApiBehaviour))
		{
		iExpectedApiBehaviour = static_cast<TExpectedApiBehaviour>(expectedApiBehaviour);;
		}
	else
		{
		iExpectedApiBehaviour = EApiVariant1;
		}	
	
	if(GetIntFromConfig(ConfigSection(), KTestCaseId, iTestCaseId))
		{
		switch(iTestCaseId)
			{ // open the network simulator for the tests that run in terminal based
			case 151: // LBS-IGNORE-ACCURACY-ON-TERMINAL-BASED-001
			case 152: // LBS-IGNORE-ACCURACY-ON-TERMINAL-BASED-002
			case 153: // LBS-IGNORE-ACCURACY-ON-TERMINAL-BASED-003
				{
				CloseNetSim();
				User::LeaveIfError(OpenNetSim());
				break;
				}
			default:
				{
				}
			}

		switch(iTestCaseId)
			{ // open the positioner for the tests that are doing a position update
			  // the positioner is opened through the required API for each test
			case 1: // LBS-IGNORE-ACCURACY-ENABLED-001
			case 2: // LBS-IGNORE-ACCURACY-DISABLED-001
				{ // do nothing, these tests don't ask for a position update
				break;
				}
			case 3: // LBS-IGNORE-ACCURACY-ABSENT-001
			case 101: // LBS-MOLR-IGNORE-ACCURACY-ON-ACCURATE-001
			case 104: // LBS-MOLR-IGNORE-ACCURACY-ON-INACCURATE-001
			case 107: // LBS-MOLR-IGNORE-ACCURACY-ON-TIMEOUT-001
			case 110: // LBS-IGNORE-ACCURACY-ON-PARTL-UPD-001
			case 111: // LBS-IGNORE-ACCURACY-ON-PARTIAL-UPDATE-002
			case 114: // LBS-IGNORE-ACCURACY-ON-EARLY-COMPLETE-001
			case 151: // LBS-IGNORE-ACCURACY-ON-TERMINAL-BASED-001
			case 201: // LBS-MOLR-IGNORE-ACCURACY-OFF-ACCURATE-001
			case 204: // LBS-MOLR-IGNORE-ACCURACY-OFF-INACCURATE-001
			case 207: // LBS-MOLR-IGNORE-ACCURACY-OFF-TIMEOUT-001
			case 210: // LBS-IGNORE-ACCURACY-OFF-PARTIAL-UPDATE-001 
			case 211: // LBS-IGNORE-ACCURACY-OFF-PARTIAL-UPDATE-002
			case 214: // LBS-IGNORE-ACCURACY-OFF-EARLY-COMPLETE-001
				{
				User::LeaveIfError(iPositioner.Open(iServer));
				break;
				}
			case 102: // LBS-MOLR-IGNORE-ACCURACY-ON-ACCURATE-002
			case 105: // LBS-MOLR-IGNORE-ACCURACY-ON-INACCURATE-002
			case 108: // LBS-IGNORE-ACCURACY-ON-TIMEOUT-002
			case 152: // LBS-IGNORE-ACCURACY-ON-TERMINAL-BASED-002
			case 202: // LBS-MOLR-IGNORE-ACCURACY-OFF-ACCURATE-002
			case 205: // LBS-MOLR-IGNORE-ACCURACY-OFF-INACCURATE-002
			case 208: // LBS-MOLR-IGNORE-ACCURACY-OFF-TIMEOUT-002
				{
				TPositionModuleId gpsModuleId = utils.GetAGpsModuleIdL(iServer);
				User::LeaveIfError(iPositioner.Open(iServer, gpsModuleId));
				break;
				}
			case 103: // LBS-MOLR-IGNORE-ACCURACY-ON-ACCURATE-003
			case 106: // LBS-MOLR-IGNORE-ACCURACY-ON-INACCURATE-003
			case 109: // LBS-MOLR-IGNORE-ACCURACY-ON-TIMEOUT-003
			case 153: // LBS-IGNORE-ACCURACY-ON-TERMINAL-BASED-003
			case 203: // LBS-MOLR-IGNORE-ACCURACY-OFF-ACCURATE-003
			case 206: // LBS-MOLR-IGNORE-ACCURACY-OFF-INACCURATE-003
			case 209: // LBS-MOLR-IGNORE-ACCURACY-OFF-TIMEOUT-003
				{
				TPositionCriteria criteria;
				TPositionQuality    quality;
				// set up the required accuracy
				quality.SetHorizontalAccuracy(EXPECTED_GPS_ACCURACY_HORIZONTAL);
				quality.SetVerticalAccuracy(EXPECTED_GPS_ACCURACY_VERTICAL);
				criteria.SetRequiredQuality(quality);
				User::LeaveIfError(iPositioner.Open(iServer, criteria));
				break;
				}
			default:
				{
				SetTestStepResult(EFail);
				}
			}
		if(iTestCaseId != 1 && iTestCaseId !=2)
			{ // Loading the update options for all tests, except those that 
			  // don't request a position update
			TPtrC configFileName;
			_LIT(KUpdateOptionsFile, "pos_infos_file");

			GetStringFromConfig(ConfigSection(), KUpdateOptionsFile, configFileName);
			
			TPositionUpdateOptions updateOpts;

			utils.GetConfigured_ModuleUpdateOptionsL(configFileName, ConfigSection(), updateOpts);
			iPositioner.SetUpdateOptions(updateOpts);
			}
		}
	else
		{
		SetTestStepResult(EFail);
		}

	return TestStepResult();
	}
	
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
TVerdict CT_LbsClientStep_IgnoreAccuracySetting::doTestStepL()
	{
	// Generic test step used to test the LBS SetUpdateOptions API.
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsClientStep_IgnoreAccuracySetting::doTestStepL()"));

	if(iTestCaseId == 1 || iTestCaseId == 2)
		{ // Testing the ignore accuracy setting is as required
		CLbsAdmin* lbsAdmin = CLbsAdmin::NewL();
		CleanupStack::PushL(lbsAdmin);
		CLbsAdmin::TSpecialFeature ignoreAccuracySetting;
		TInt err = lbsAdmin->Get(KLbsSpecialFeatureIgnoreAccuracy, ignoreAccuracySetting);
		if(err != KErrNone)
			{
			SetTestStepResult(EFail);
			}
		else
			{
			switch(iTestCaseId)
				{
				case 1:  // LBS-IGNORE-ACCURACY-ENABLED-001
					{
					if(ignoreAccuracySetting != CLbsAdmin::ESpecialFeatureOn)
						{
						SetTestStepResult(EFail);
						}
					break;
					}
				case 2:  // LBS-IGNORE-ACCURACY-DISABLED-001
					{
					if(ignoreAccuracySetting != CLbsAdmin::ESpecialFeatureOff)
						{
						SetTestStepResult(EFail);
						}					
					break;
					}
				default:
					{
					SetTestStepResult(EFail);
					}
				}
			}
		
		CleanupStack::PopAndDestroy(lbsAdmin);
		}
	else
		{
		// Testing the ignore accuracy setting is absent
		if(iTestCaseId == 3)
			{ // Testing the ignore accuracy setting is absent
			CLbsAdmin* lbsAdmin = CLbsAdmin::NewL();
			CleanupStack::PushL(lbsAdmin);
			TInt err(KErrNone);
			CLbsAdmin::TSpecialFeature ignoreAccuracySetting;
			err = lbsAdmin->Get(KLbsSpecialFeatureIgnoreAccuracy, ignoreAccuracySetting);
			if(err != KErrNone || ignoreAccuracySetting != CLbsAdmin::ESpecialFeatureOff)
				{
				SetTestStepResult(EFail);
				}
			CleanupStack::PopAndDestroy(lbsAdmin);
			}

		switch(iTestCaseId)
			{ // Requesting the module to delay it's response so we get a time-out
			case 107: // LBS-MOLR-IGNORE-ACCURACY-ON-TIMEOUT-001
			case 108: // LBS-IGNORE-ACCURACY-ON-TIMEOUT-002
			case 109: // LBS-MOLR-IGNORE-ACCURACY-ON-TIMEOUT-003
			case 207: // LBS-MOLR-IGNORE-ACCURACY-OFF-TIMEOUT-001
			case 208: // LBS-MOLR-IGNORE-ACCURACY-OFF-TIMEOUT-002
			case 209: // LBS-MOLR-IGNORE-ACCURACY-OFF-TIMEOUT-003
				{
				if (iParent.iSharedData->iTestModuleInUse)
					{
					T_LbsUtils utils;
					TModuleDataIn modDataIn;
	
					modDataIn.iRequestType = TModuleDataIn::EModuleRequestTimeOut;
					modDataIn.iTimeOut = KModuleTimeout;
				
					utils.NotifyModuleOfConfigChangeL(modDataIn);	// This will block.
					}
				else
					{
					SetTestStepResult(EFail);
					}
				break;
				}
			default:
				{
				break;
				}
			}
	
		if (TestStepResult()==EPass) 
			{
			T_LbsUtils utils;

			// Create a posinfo and store in our shared array for later verification.
			RPointerArray<TAny>& posInfoArr = iParent.iSharedData->iCurrentPosInfoArr;
			// The posInfo shouldn't be put on the cleanup stack because ownership is passed to the parent
			TPositionInfo* posInfo = new(ELeave) TPositionInfo();
	
			utils.ResetAndDestroy_PosInfoArr(posInfoArr);	// Clear previous entries before new entry is appended.
	
			posInfoArr.Append(posInfo);
			TInt err;
			
			// Request a position update
			switch(iTestCaseId)
				{
				case 114: // LBS-IGNORE-ACCURACY-ON-EARLY-COMPLETE-001
				case 214: // LBS-IGNORE-ACCURACY-OFF-EARLY-COMPLETE-001
					{ // In the early complete tests we request for a position
					  // update in a different way so the client can complete 
					  // the request early
					err = DoNotifyUpdateL(*posInfo);
					if(err != KPositionPartialUpdate)
						{
						SetTestStepResult(EFail);
						break;
						}
					
					TRequestStatus status;
					iPositioner.NotifyPositionUpdate(*posInfo, status);
					
					err = iPositioner.CompleteRequest(EPositionerNotifyPositionUpdate);
					if(err != KErrNone)
						{
						SetTestStepResult(EFail);
						}
					// this should complete with either KErrNone or KErrCancel
					User::WaitForRequest(status);
					
					err = status.Int();
					break;
					}
				default:
					{
					// Get the actual position
					err = DoNotifyUpdateL(*posInfo);
					}
				}

			if(iTestCaseId == 151 || iTestCaseId == 153)
				{ // expect reference position in the appropiate terminal based tests
				if(KErrNone != err)
					{
					SetTestStepResult(EFail);
					}
				else
					{
					// Verify reference position.
					TPositionInfo verifyRefPosInfo;
					verifyRefPosInfo.SetPosition(iRefPos);

					if (!utils.Compare_PosInfo(verifyRefPosInfo, *posInfo))
						{
						if(!iParent.iSharedData->iTestModuleInUse)
							{
							WARN_PRINTF1(_L("WARNING: position received was not reference position"));	
							}
						else
							{	
							INFO_PRINTF1(_L("Failed test, reference position incorrect."));
							SetTestStepResult(EFail);
							}
						}
					}
				err = DoNotifyUpdateL(*posInfo);
				}

			// check we receive the appropiate error code from the request we made
			switch(iTestCaseId)
				{
				case 101: // LBS-MOLR-IGNORE-ACCURACY-ON-ACCURATE-001
				case 104: // LBS-MOLR-IGNORE-ACCURACY-ON-INACCURATE-001
				case 201: // LBS-MOLR-IGNORE-ACCURACY-OFF-ACCURATE-001
				case 102: // LBS-MOLR-IGNORE-ACCURACY-ON-ACCURATE-002
				case 105: // LBS-MOLR-IGNORE-ACCURACY-ON-INACCURATE-002
				case 151: // LBS-IGNORE-ACCURACY-ON-TERMINAL-BASED-001
				case 152: // LBS-IGNORE-ACCURACY-ON-TERMINAL-BASED-002
				case 153: // LBS-IGNORE-ACCURACY-ON-TERMINAL-BASED-003
				case 202: // LBS-MOLR-IGNORE-ACCURACY-OFF-ACCURATE-002
				case 103: // LBS-MOLR-IGNORE-ACCURACY-ON-ACCURATE-003
				case 106: // LBS-MOLR-IGNORE-ACCURACY-ON-INACCURATE-003
				case 203: // LBS-MOLR-IGNORE-ACCURACY-OFF-ACCURATE-003
					{
					if (KErrNone != err)
						{
						INFO_PRINTF2(_L("Failed test, error = %d."), err);
						SetTestStepResult(EFail);
						}
					if(iTestCaseId ==152 && iExpectedApiBehaviour == EApiVariant2)// on variant2 api we expect a reference position first
						{
						// Verify reference position.
						TPositionInfo verifyRefPosInfo;
						verifyRefPosInfo.SetPosition(iRefPos);

						if (!utils.Compare_PosInfo(verifyRefPosInfo, *posInfo))
							{
							if(!iParent.iSharedData->iTestModuleInUse)
								{
								WARN_PRINTF1(_L("WARNING: position received was not reference position"));	
								}
							else
								{	
								INFO_PRINTF1(_L("Failed test, reference position incorrect."));
								SetTestStepResult(EFail);
								}
							}	
						// now get the gps position:
						err = DoNotifyUpdateL(*posInfo);
						if (KErrNone != err)
							{
							INFO_PRINTF2(_L("Failed test, error = %d."), err);
							SetTestStepResult(EFail);
							}						
						}
					break;
					}
				case 107: // LBS-MOLR-IGNORE-ACCURACY-ON-TIMEOUT-001
				case 108: // LBS-IGNORE-ACCURACY-ON-TIMEOUT-002
				case 109: // LBS-MOLR-IGNORE-ACCURACY-ON-TIMEOUT-003
				case 207: // LBS-MOLR-IGNORE-ACCURACY-OFF-TIMEOUT-001
				case 208: // LBS-MOLR-IGNORE-ACCURACY-OFF-TIMEOUT-002
				case 209: // LBS-MOLR-IGNORE-ACCURACY-OFF-TIMEOUT-003
					{
					if (KErrTimedOut != err)
						{
						INFO_PRINTF2(_L("Failed test, expected KErrTimedOut, got error = %d."), err);
						SetTestStepResult(EFail);
						}
					break;
					}
				case 3: // LBS-IGNORE-ACCURACY-ABSENT-001
				case 110: // LBS-IGNORE-ACCURACY-ON-PARTIAL-UPDATE-001
				case 204: // LBS-MOLR-IGNORE-ACCURACY-OFF-INACCURATE-001
				case 205: // LBS-MOLR-IGNORE-ACCURACY-OFF-INACCURATE-002
				case 206: // LBS-MOLR-IGNORE-ACCURACY-OFF-INACCURATE-003
				case 210: // LBS-IGNORE-ACCURACY-OFF-PARTIAL-UPDATE-001
					{
					TInt expected_error = KPositionQualityLoss;
					if((iExpectedApiBehaviour == EApiVariant2) && (iTestCaseId == 110))
						{
						expected_error = KErrTimedOut;
						}
					if(expected_error != err)
						{
						INFO_PRINTF3(_L("Failed test, expected %d, got error = %d."), expected_error, err);
						SetTestStepResult(EFail);
						}
					break;
					}
				case 111: // LBS-IGNORE-ACCURACY-ON-PARTIAL-UPDATE-002
				case 211: // LBS-IGNORE-ACCURACY-OFF-PARTIAL-UPDATE-002
					{
					if (KPositionPartialUpdate != err)
						{
						INFO_PRINTF2(_L("Failed test, expected KPositionPartialUpdate, got error = %d."), err);
						SetTestStepResult(EFail);
						}
					break;
					}
				case 114: // LBS-IGNORE-ACCURACY-ON-EARLY-COMPLETE-001
				case 214: // LBS-IGNORE-ACCURACY-OFF-EARLY-COMPLETE-001
					{
					if (KPositionEarlyComplete != err)
						{
						INFO_PRINTF2(_L("Failed test, expected KPositionEarlyComplete, got error = %d."), err);
						SetTestStepResult(EFail);
						}
					break;
					}
				default:
					{
					INFO_PRINTF2(_L("Failed test, unexpected Test Case Id %d"), iTestCaseId);
					SetTestStepResult(EFail);
					}
				}
			}
		}

	INFO_PRINTF1(_L("&lt;&lt;CT_LbsClientStep_IgnoreAccuracySetting::doTestStepL()"));

	return TestStepResult();
	}
