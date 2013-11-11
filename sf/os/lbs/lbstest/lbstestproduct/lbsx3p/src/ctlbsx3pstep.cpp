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
// @file ctlbsx3pstep.cpp
// This is the class implementation for the LBS Client Test Step Base
// 
//

#include "ctlbsx3pstep.h"

#include <lbs/test/tlbsutils.h>
#include <lbs/test/ctlbsasyncwaiter.h>


/**
 * @return - TVerdict
 * Implementation of CTestStep base class virtual
 * It is used for doing all initialisation common to derived classes in here.
 * Make it being able to leave if there are any errors here as there's no point in
 * trying to run a test step if anything fails.
 * The leave will be picked up by the framework.
 */
TVerdict CT_LbsX3PStep::doTestStepPreambleL()
	{
	// Process some common pre setting to test steps then set SetTestStepResult to EFail or Epass.
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsX3PStep::doTestStepPreambleL()"));
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
TVerdict CT_LbsX3PStep::doTestStepPostambleL()
	{
	// Process some common post setting to test steps then set SetTestStepResult to EFail or Epass.
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsX3PStep::doTestStepPostabmleL()"));

	//SetTestStepResult(EPass);  // or EFail
	return TestStepResult();
	}


CT_LbsX3PStep::~CT_LbsX3PStep()
	{
	}


CT_LbsX3PStep::CT_LbsX3PStep(CT_LbsX3PServer& aParent) : iParent(aParent)
	{
	}

void CT_LbsX3PStep::ConstructL()
	{
	// Create the base class objects.
	CT_LbsNetSimStep::ConstructL();
	}
