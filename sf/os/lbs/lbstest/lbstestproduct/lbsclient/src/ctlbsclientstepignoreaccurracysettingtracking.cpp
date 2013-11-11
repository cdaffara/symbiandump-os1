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
 
#include "ctlbsclientstepignoreaccurracysettingtracking.h"

#include <lbs.h>
#include <lbssatellite.h>

#include <lbs/test/tlbsutils.h>
#include <lbs/test/ctlbsasyncwaiter.h>

_LIT(KUpdateArraySection, "update_array_section");

/**
 * Construction.
 */
CT_LbsClientStep_IgnoreAccuracySettingTracking* CT_LbsClientStep_IgnoreAccuracySettingTracking::New(CT_LbsClientServer& aParent)
	{
	// Note the lack of ELeave.
	// This means that having insufficient memory will return NULL;
	CT_LbsClientStep_IgnoreAccuracySettingTracking* testStep = new CT_LbsClientStep_IgnoreAccuracySettingTracking(aParent);
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


CT_LbsClientStep_IgnoreAccuracySettingTracking::CT_LbsClientStep_IgnoreAccuracySettingTracking(CT_LbsClientServer& aParent) 
	: CT_LbsClientStep(aParent), iTestCaseId(0)
	{
	SetTestStepName(KLbsClientStep_IgnoreAccuracySettingTracking);
	}


void CT_LbsClientStep_IgnoreAccuracySettingTracking::ConstructL()
	{
	// Connect server.
	User::LeaveIfError(iServer.Connect());
	}


/**
 * Destructor
 */
CT_LbsClientStep_IgnoreAccuracySettingTracking::~CT_LbsClientStep_IgnoreAccuracySettingTracking()
	{
	for(TInt index = 0; index < iExpectedUpdateArray.Count(); ++index)
		{
		delete iExpectedUpdateArray[index];
		}
	iExpectedUpdateArray.Close();
	iPositioner.Close();
	iServer.Close();	
	}

TVerdict CT_LbsClientStep_IgnoreAccuracySettingTracking::doTestStepPreambleL()
	{
	// Process some common pre setting to test steps then set SetTestStepResult to EFail or Epass.
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsClientStep::doTestStepPreambleL()"));
	SetTestStepResult(EPass);

	if(GetIntFromConfig(ConfigSection(), KTestCaseId, iTestCaseId))
		{
		switch(iTestCaseId)
			{
			case 112: // LBS-IGNORE-ACCURACY-ON-TRACKING-001
			case 212: // LBS-IGNORE-ACCURACY-OFF-TRACKING-001
				{ // do nothing
				User::LeaveIfError(iPositioner.Open(iServer));

				T_LbsUtils utils;
				TPtrC configFileName;
				_LIT(KUpdateOptionsFile, "pos_infos_file");

				GetStringFromConfig(ConfigSection(), KUpdateOptionsFile, configFileName);
				
				TPositionUpdateOptions updateOpts;

				utils.GetConfigured_ModuleUpdateOptionsL(configFileName, ConfigSection(), updateOpts);
				iPositioner.SetUpdateOptions(updateOpts);
				
				TPtrC updateArraySection;
				GetStringFromConfig(ConfigSection(), KUpdateArraySection, updateArraySection);
				
				utils.GetConfigured_PosInfosL(configFileName, updateArraySection, iExpectedUpdateArray);

				break;
				}
			default:
				{
				SetTestStepResult(EFail);
				}
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
TVerdict CT_LbsClientStep_IgnoreAccuracySettingTracking::doTestStepL()
	{
	// Generic test step used to test the LBS SetUpdateOptions API.
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsClientStep_IgnoreAccuracySettingTracking::doTestStepL()"));

	if (TestStepResult()==EPass) 
		{
		T_LbsUtils utils;
		TPositionInfo* posInfo = new(ELeave) TPositionInfo();
		CleanupStack::PushL(posInfo);

		// Perfom test.
		TInt err = KErrNone;
		TPosition pos;
		TInt numOfPosInfos = 5;

		for(TInt updateIndex = 0 ; TestStepResult()==EPass && updateIndex < numOfPosInfos; 
											++updateIndex)
			{
			// Do request to position update.
			err = DoNotifyUpdateL(*posInfo);

			switch(iTestCaseId)
				{
				case 212: // LBS-IGNORE-ACCURACY-OFF-TRACKING-001
					{
					switch(updateIndex)
						{
						case 0:
						case 1:
						case 3:
							{
							if(KPositionQualityLoss != err)
								{
								SetTestStepResult(EFail);
								}
							break;
							}
						case 2:
						case 4:
							{
							if(KErrNone != err)
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
					TPositionInfoBase* expectedPosition = 
						static_cast<TPositionInfoBase*>(iExpectedUpdateArray[updateIndex]);
					if(!utils.Compare_PosInfo(*posInfo, *expectedPosition))
						{
						SetTestStepResult(EFail);
						}
					break;
					}
				case 112: // LBS-IGNORE-ACCURACY-ON-TRACKING-001
					{
					if(KErrNone != err)
						{
						SetTestStepResult(EFail);
						}
					TPositionInfoBase* expectedPosition = 
						static_cast<TPositionInfoBase*>(iExpectedUpdateArray[updateIndex]);
					if(!utils.Compare_PosInfo(*posInfo, *expectedPosition))
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
		CleanupStack::PopAndDestroy(posInfo);
		}

	INFO_PRINTF1(_L("&lt;&lt;CT_LbsClientStep_IgnoreAccuracySettingTracking::doTestStepL()"));

	return TestStepResult();
	}
