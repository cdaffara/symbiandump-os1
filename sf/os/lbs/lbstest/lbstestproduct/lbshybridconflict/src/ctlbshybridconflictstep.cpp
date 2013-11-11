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
// @file ctlbshybridconflictstep.cpp
// This is the class implementation for the LBS HYBRID Conflict Test Step Base
// 
//

#include "ctlbshybridconflictstep.h"


CT_LbsHybridConflictStep::~CT_LbsHybridConflictStep()
	{
	}


CT_LbsHybridConflictStep::CT_LbsHybridConflictStep(CT_LbsHybridConflictServer& aParent) :iParent(aParent)
	{
	}

void CT_LbsHybridConflictStep::ConstructL()
	{
	}

/**
 * @return - TVerdict
 * Implementation of CTestStep base class virtual
 * It is used for doing all initialisation common to derived classes in here.
 * Make it being able to leave if there are any errors here as there's no point in
 * trying to run a test step if anything fails.
 * The leave will be picked up by the framework.
 */
TVerdict CT_LbsHybridConflictStep::doTestStepPreambleL()
	{
	// Process some common pre setting to test steps then set SetTestStepResult to EFail or Epass.
	INFO_PRINTF1(_L("doTestStepPreambleL()"));

	SetTestStepResult(EPass);

	return TestStepResult();
	}


/**
 * @return - TVerdict
 * Implementation of CTestStep base class virtual
 * It is used for doing all after test treatment common to derived classes in here.
 * Make it being able to leave
 * The leave will be picked up by the framework.
 */
TVerdict CT_LbsHybridConflictStep::doTestStepPostambleL()
	{
	// Process some common post setting to test steps then set SetTestStepResult to EFail or Epass.
	INFO_PRINTF1(_L("doTestStepPostambleL()"));

	//SetTestStepResult(EPass);  // or EFail
	return TestStepResult();
	}
