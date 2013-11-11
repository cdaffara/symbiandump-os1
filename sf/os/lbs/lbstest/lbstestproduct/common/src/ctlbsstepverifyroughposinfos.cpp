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
// @file ctlbsstep_verifyposinfos.cpp
// This is the class implementation for the Notify Position Update Tests
// 
//
 
#include "ctlbsstepverifyroughposinfos.h"

#include <lbs/test/tlbsutils.h>


/**
 * Constructor
 */
CT_LbsStep_VerifyRoughPosInfos::CT_LbsStep_VerifyRoughPosInfos(CT_LbsServer& aParent) : CT_LbsStep(aParent)
	{
	SetTestStepName(KLbsStep_VerifyRoughPosInfos);
	}


/**
Static Constructor
*/
CT_LbsStep_VerifyRoughPosInfos* CT_LbsStep_VerifyRoughPosInfos::New(CT_LbsServer& aParent)
	{
	return new CT_LbsStep_VerifyRoughPosInfos(aParent);
	// Note the lack of ELeave.
	// This means that having insufficient memory will return NULL;
	}


/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
TVerdict CT_LbsStep_VerifyRoughPosInfos::doTestStepL()
	{
	// Helper test step used to Verify the Position Information Array.
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsStep_VerifyRoughPosInfos::doTestStepL()"));

	if (TestStepResult() == EPass)
		{
		// Access the arrays to be compared.
		RPointerArray<TAny>& verifyPosInfoArr = iParent.iSharedData->iVerifyPosInfoArr;
		RPointerArray<TAny>& currentPosInfoArr = iParent.iSharedData->iCurrentPosInfoArr;
		T_LbsUtils utils;
		
		// Verify arrays are equal.
		TBool res = utils.Compare_PosInfoArr(verifyPosInfoArr, currentPosInfoArr, T_LbsUtils::ERoughAccuracy);
		
		// Fail test if arrays do not match.
		if (!res)
			SetTestStepResult(EFail);
		}

	INFO_PRINTF1(_L("&lt;&lt;CT_LbsStep_VerifyRoughPosInfos::doTestStepL()"));

	return TestStepResult();
	}

