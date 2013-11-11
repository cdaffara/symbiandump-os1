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

#include "ctlbsprivfwstep.h"

//#include "tlbsutils.h"
//#include "ctlbsasyncwaiter.h"


/**
 * @return - TVerdict
 * Implementation of CTestStep base class virtual
 * It is used for doing all initialisation common to derived classes in here.
 * Make it being able to leave if there are any errors here as there's no point in
 * trying to run a test step if anything fails.
 * The leave will be picked up by the framework.
 */
TVerdict CT_LbsPrivFwStep::doTestStepPreambleL()
	{
	// Process some common pre setting to test steps then set SetTestStepResult to EFail or Epass.
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsPrivFwStep::doTestStepPreambleL()"));
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
TVerdict CT_LbsPrivFwStep::doTestStepPostambleL()
	{
	// Process some common post setting to test steps then set SetTestStepResult to EFail or Epass.
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsPrivFwStep::doTestStepPostabmleL()"));

	//SetTestStepResult(EPass);  // or EFail
	return TestStepResult();
	}


CT_LbsPrivFwStep::~CT_LbsPrivFwStep()
	{
	}


CT_LbsPrivFwStep::CT_LbsPrivFwStep(CT_LbsPrivFwServer& aParent) : iParent(aParent)
	{
	}

void CT_LbsPrivFwStep::ConstructL()
	{
	// Create the base class objects.
	//CTestStep::ConstructL();
	}
