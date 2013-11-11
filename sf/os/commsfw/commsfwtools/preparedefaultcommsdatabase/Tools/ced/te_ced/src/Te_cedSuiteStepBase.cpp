// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

/**
 @file Te_cedSuiteStepBase.cpp
 @internalTechnology
*/

#include "Te_cedSuiteStepBase.h"
#include "Te_cedSuiteDefs.h"

// Device driver constants

TVerdict CTe_cedSuiteStepBase::doTestStepPreambleL()
/**
 * @return - TVerdict
 * Implementation of CTestStep base class virtual
 * It is used for doing all initialisation common to derived classes in here.
 * Make it being able to leave if there are any errors here as there's no point in
 * trying to run a test step if anything fails.
 * The leave will be picked up by the framework.
 */
	{

	// process some common pre setting to test steps then set SetTestStepResult to EFail or Epass.
	INFO_PRINTF1(_L("Please delete this line or modify me!! I am in doTestStepPreambleL() of the class CTe_cedSuiteStepBase!"));
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CTe_cedSuiteStepBase::doTestStepPostambleL()
/**
 * @return - TVerdict
 * Implementation of CTestStep base class virtual
 * It is used for doing all after test treatment common to derived classes in here.
 * Make it being able to leave
 * The leave will be picked up by the framework.
 */
	{

	// process some common post setting to test steps then set SetTestStepResult to EFail or Epass.
	INFO_PRINTF1(_L("Please delete this line or modify me!! I am in doTestStepPostambleL() of the class CTe_cedSuiteStepBase!"));
	//SetTestStepResult(EPass);  // or EFail
	return TestStepResult();
	}

CTe_cedSuiteStepBase::~CTe_cedSuiteStepBase()
	{
	}

CTe_cedSuiteStepBase::CTe_cedSuiteStepBase()
	{
	}
