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
// @file ctlbsstep_createupdateinfo.cpp
// This is the class implementation for the Notify Position Update Tests
// 
//
 
#include "ctlbsstepcreateupdateinfo.h"

#include <lbs/test/tlbsutils.h>


_LIT(KUpdateInfoFile, "update_info_file");

/**
 * Constructor
 */
CT_LbsStep_CreateUpdateInfo::CT_LbsStep_CreateUpdateInfo(CT_LbsServer& aParent) : CT_LbsStep(aParent)
	{
	SetTestStepName(KLbsStep_CreateUpdateInfo);
	}


/**
Static Constructor
*/
CT_LbsStep_CreateUpdateInfo* CT_LbsStep_CreateUpdateInfo::New(CT_LbsServer& aParent)
	{
	return new CT_LbsStep_CreateUpdateInfo(aParent);
	// Note the lack of ELeave.
	// This means that having insufficient memory will return NULL;
	}


/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
TVerdict CT_LbsStep_CreateUpdateInfo::doTestStepL()
	{
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsStep_CreateUpdateInfo::doTestStepL()"));

	if (TestStepResult() == EPass)
		{
		TPtrC configFileName;
		GetStringFromConfig(ConfigSection(), KUpdateInfoFile, configFileName);
		RPointerArray<TAny>& updateInfoArr = iParent.iSharedData->iUpdateArr;
		//Populate the array by extracting the data from the ini file	
		}

	INFO_PRINTF1(_L("&lt;&lt;CT_LbsStep_CreateUpdateInfo::doTestStepL()"));

	return TestStepResult();
	}


