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
// @file ctlbsstepverifymodinfo.cpp
// This is the class implementation for the Verify Module Status test step
// 
//
 
#include "ctlbsstepverifymodstatus.h"
#include <lbs/test/tlbsutils.h>


/**
 * Constructor
 */
CT_LbsStep_VerifyModStatus::CT_LbsStep_VerifyModStatus(CT_LbsServer& aParent) : CT_LbsStep(aParent)
	{
	SetTestStepName(KLbsStep_VerifyModuleStatus);
	}


/**
Static Constructor
*/
CT_LbsStep_VerifyModStatus* CT_LbsStep_VerifyModStatus::New(CT_LbsServer& aParent)
	{
	return new CT_LbsStep_VerifyModStatus(aParent);
	// Note the lack of ELeave.
	// This means that having insufficient memory will return NULL;
	}


/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
TVerdict CT_LbsStep_VerifyModStatus::doTestStepL()
	{
	// Helper test step used to verify module information.
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsStep_VerifyModStatus::doTestStepL()"));

	if (TestStepResult() == EPass)
		{
		// Access the module infos to be compared.
		TPositionModuleStatus&	verifyModStatus = iParent.iSharedData->iVerifyModuleStatus;
		TPositionModuleStatus&	currentModStatus = iParent.iSharedData->iCurrentModuleStatus;
		T_LbsUtils utils;
		
		// Verify module informations are equal.
		TBool res;
		
		INFO_PRINTF3(_L("comparing expected and received status %d %d"), verifyModStatus.DeviceStatus(), currentModStatus.DeviceStatus());
		res = utils.Compare_ModuleStatus(verifyModStatus, currentModStatus);
		
		// Fail test if they do not match.
		if (!res)
			{
			SetTestStepResult(EFail);
			}
		}

	INFO_PRINTF1(_L("&lt;&lt;CT_LbsStep_VerifyModStatus::doTestStepL()"));

	return TestStepResult();
	}

