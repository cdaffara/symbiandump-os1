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
// @file ctlbsstep_createverifymoduledata.cpp
// This is the class implementation for the Notify Position Update Tests
// 
//
 
#include "ctlbsstepcreateverifymoduledata.h"

#include <lbs/test/tlbsutils.h>

/**
	"module_data_file" = name of the .ini file containing the module data items including
	position info array entries, module stauts, module update options.

	If the file name is not present the module data will contain default items.
	See tlbsutils.cpp for details of support ini file commands.
*/
_LIT(KModuleDataFile, "module_data_file");

/**
 * Constructor
 */
CT_LbsStep_CreateVerifyModuleData::CT_LbsStep_CreateVerifyModuleData(CT_LbsServer& aParent) : CT_LbsStep(aParent)
	{
	SetTestStepName(KLbsStep_CreateVerifyModuleData);
	}


/**
Static Constructor
*/
CT_LbsStep_CreateVerifyModuleData* CT_LbsStep_CreateVerifyModuleData::New(CT_LbsServer& aParent)
	{
	return new CT_LbsStep_CreateVerifyModuleData(aParent);
	// Note the lack of ELeave.
	// This means that having insufficient memory will return NULL;
	}


/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
TVerdict CT_LbsStep_CreateVerifyModuleData::doTestStepL()
	{
	// Helper test step used to Create the Verify Position Information Array.
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsStep_CreateVerifyModuleData::doTestStepL()"));

	if (TestStepResult() == EPass)
		{
		TPtrC configFileName;
		GetStringFromConfig(ConfigSection(), KModuleDataFile, configFileName);

		// Access the verify pos array and pass to utils func to be populated.
		RPointerArray<TAny>& posInfoArr = iParent.iSharedData->iVerifyPosInfoArr;
		TPositionUpdateOptions& updateOpts = iParent.iSharedData->iVerifyUpdateOpts;
		TPositionModuleStatus& modStatus = iParent.iSharedData->iVerifyModuleStatus;
		T_LbsUtils utils;
		TBool delaystatusupdate;
		
		// Populate the shared data items.
		utils.GetConfigured_PosInfosL(configFileName, ConfigSection(), posInfoArr); // Note previous entries will be cleared.
		utils.GetConfigured_ModuleStatusL(configFileName, ConfigSection(), modStatus, delaystatusupdate);
		utils.GetConfigured_ModuleUpdateOptionsL(configFileName, ConfigSection(), updateOpts);
// remove aove module does not know abot update options...

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
		}

	INFO_PRINTF1(_L("&lt;&lt;CT_LbsStep_CreateVerifyModuleData::doTestStepL()"));

	return TestStepResult();
	}


