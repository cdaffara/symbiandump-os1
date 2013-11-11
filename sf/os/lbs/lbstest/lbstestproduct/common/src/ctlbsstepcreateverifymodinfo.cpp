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
// @file ctlbsstep_createverifymodinfo.cpp
// This is the class implementation for the module info tests 
// 
//
 
#include "ctlbsstepcreateverifymodinfo.h"

#include <lbs/test/tlbsutils.h>

//module_id in ini file
_LIT(KLbsModuleId, "module_id");

/**
 * Constructor
 */
CT_LbsStep_CreateVerifyModInfo::CT_LbsStep_CreateVerifyModInfo(CT_LbsServer& aParent) : CT_LbsStep(aParent)
	{
	SetTestStepName(KLbsStep_CreateVerifyModInfo);
	}


/**
Static Constructor
*/
CT_LbsStep_CreateVerifyModInfo* CT_LbsStep_CreateVerifyModInfo::New(CT_LbsServer& aParent)
	{
	return new CT_LbsStep_CreateVerifyModInfo(aParent);
	// Note the lack of ELeave.
	// This means that having insufficient memory will return NULL;
	}

/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
TVerdict CT_LbsStep_CreateVerifyModInfo::doTestStepL()
	{
	// Helper test step used to Create the Verify Mod Information 
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsStep_CreateVerifyModInfo::doTestStepL()"));

	if (TestStepResult() == EPass)
		{
		TInt moduleid;
		TBool moduleid_exists;
		moduleid_exists = GetIntFromConfig(ConfigSection(), KLbsModuleId, moduleid);
	    if(!moduleid_exists)
			{
			INFO_PRINTF1(_L("Could not get module Id from section"));
    		SetTestStepResult(EFail);
    		}
	
	    else
		    {
		    // Access the verify mod info and pass to utils func to be populated.
			TPositionModuleInfo& modinfo = iParent.iSharedData->iVerifyModuleInfo;
			T_LbsUtils utils;
	        TPositionModuleId moduleUid = TUid::Uid(moduleid);
	        utils.Create_ModuleInfoL(moduleUid,modinfo);		
		    }
	    }

	INFO_PRINTF1(_L("&lt;&lt;CT_LbsStep_CreateVerifyModInfo::doTestStepL()"));

	return TestStepResult();
	}


