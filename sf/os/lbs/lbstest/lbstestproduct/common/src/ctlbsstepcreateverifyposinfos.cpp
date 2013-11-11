// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// @file ctlbsstep_createverifyposinfos.cpp
// This is the class implementation for the Notify Position Update Tests
// 
//
 
#include "ctlbsstepcreateverifyposinfos.h"

#include <lbs/test/tlbsutils.h>

/**
	"pos_infos_file" = name of the .ini file containing the position info array entries.

	If the file name is not present the array will contain a single default TPositionInfo entry.
	See tlbsutils.cpp for details of support ini file commands.
*/
_LIT(KPosInfosFile, "pos_infos_file");
_LIT(KLbsConcurrencyMode, "concurrency_mode");
_LIT(KLbsConcurrencyMode_Concurrent, "concurrent");

/**
 * Constructor
 */
CT_LbsStep_CreateVerifyPosInfos::CT_LbsStep_CreateVerifyPosInfos(CT_LbsServer& aParent) : CT_LbsStep(aParent)
	{
	SetTestStepName(KLbsStep_CreateVerifyPosInfos);
	}


/**
Static Constructor
*/
CT_LbsStep_CreateVerifyPosInfos* CT_LbsStep_CreateVerifyPosInfos::New(CT_LbsServer& aParent)
	{
	return new CT_LbsStep_CreateVerifyPosInfos(aParent);
	// Note the lack of ELeave.
	// This means that having insufficient memory will return NULL;
	}


/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
TVerdict CT_LbsStep_CreateVerifyPosInfos::doTestStepL()
	{
	// Helper test step used to Create the Verify Position Information Array.
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsStep_CreateVerifyPosInfos::doTestStepL()"));

	if (TestStepResult() == EPass)
		{
		TPtrC configFileName;
		TPtrC concurrencyMode;
		T_LbsUtils utils;

		// If this test step has been launched by a script that loads
		// the test server with -SharedData and installs an active scheduler
		// with a dedicated test step, then the scheduler is already installed
		// in the thread running this test step. If this test step is however
		// launched by a script that includes the command "CONCURRENT"
		// then the active scheduler is not present and one has to be installed
		// before running the rest of thie method as it adds AOs to the scheduler.
		if( NULL == CActiveScheduler::Current() )
			{
			iScheduler = new (ELeave) CActiveScheduler();
			CActiveScheduler::Install(iScheduler);
			}

		GetStringFromConfig(ConfigSection(), KPosInfosFile, configFileName);
		GetStringFromConfig(ConfigSection(), KLbsConcurrencyMode, concurrencyMode);
		
		if (concurrencyMode != KLbsConcurrencyMode_Concurrent)
			{
			// Only access common data (iVerifyPosInfoArray)
			// when the test is run in a script that runs test
			// steps sequentially (SharedData)

			// Access the verify pos array and pass to utils func to be populated.
			RPointerArray<TAny>& posInfoArr = iParent.iSharedData->iVerifyPosInfoArr;
			utils.GetConfigured_PosInfosL(configFileName, ConfigSection(), posInfoArr); // Note previous entries will be cleared.
			}
			
		// Notify the test module that it's required to update it's test data.
		TPtrC modType;
		GetStringFromConfig(ConfigSection(), KLbsModuleType, modType);
		if (modType != KLbsModuleType_Real)
			{
			
			TModuleDataIn modDataIn;
			
			modDataIn.iRequestType = TModuleDataIn::EModuleRequestConfig;
			modDataIn.iConfigFileName = configFileName;
			modDataIn.iConfigSection = ConfigSection();

			// Now publish the ini file and section name to the test module - will block.
			utils.NotifyModuleOfConfigChangeL(modDataIn);
			}
		else
			{
			iParent.iSharedData->iTestModuleInUse = EFalse;	// Indicate to test harness the real A-GPS integration module is being used.			
			}

		// If this step has installed its own scheduler...
		if (iScheduler)
			{
			//CActiveScheduler::Stop();
			delete iScheduler;
			}
		}

	INFO_PRINTF1(_L("&lt;&lt;CT_LbsStep_CreateVerifyPosInfos::doTestStepL()"));

	return TestStepResult();
	}


