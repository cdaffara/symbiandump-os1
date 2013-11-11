// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file Te_ConfiguratorSuiteStepBase.cpp
*/

#include "Te_ConfiguratorSuiteStepBase.h"
#include "Te_ConfiguratorSuiteDefs.h"

// Device driver constants

TVerdict CTe_ConfiguratorSuiteStepBase::doTestStepPreambleL()
/**
 * @return - TVerdict
 * Implementation of CTestStep base class virtual
 * It is used for doing all initialisation common to derived classes in here.
 * Make it being able to leave if there are any errors here as there's no point in
 * trying to run a test step if anything fails.
 * The leave will be picked up by the framework.
 */
	{
	SetTestStepResult(EPass);
	TInt err = iConfigurator.Connect();
	if(err != KErrNone)
		{
		INFO_PRINTF1(_L("Connection to configurator failed !"));
		SetTestStepResult(EFail);
		}
	return TestStepResult();
	}

TVerdict CTe_ConfiguratorSuiteStepBase::doTestStepPostambleL()
/**
 * @return - TVerdict
 * Implementation of CTestStep base class virtual
 * It is used for doing all after test treatment common to derived classes in here.
 * Make it being able to leave
 * The leave will be picked up by the framework.
 */
	{
	iConfigurator.Close();
	return TestStepResult();
	}

CTe_ConfiguratorSuiteStepBase::~CTe_ConfiguratorSuiteStepBase()
	{
	}

CTe_ConfiguratorSuiteStepBase::CTe_ConfiguratorSuiteStepBase()
	{
	}

