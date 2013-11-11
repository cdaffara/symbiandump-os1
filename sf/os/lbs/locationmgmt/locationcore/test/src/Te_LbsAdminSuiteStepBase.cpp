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
//

/**
 @file Te_LbsAdminSuiteStepBase.cpp
*/

#include "te_lbsadminsuitestepbase.h"
#include "te_lbsadminsuitedefs.h"
#include "tprocessstartparams.h"
#include "cprocesslaunch.h"


_LIT(KModifierSemName, "LBSADMIN_MODIFIER_SEM");
_LIT(KObserverSemName, "LBSADMIN_OBSERVER_SEM");

TVerdict CTe_LbsAdminSuiteStepBase::doTestStepPreambleL()
/**
 * @return - TVerdict
 * Implementation of CTestStep base class virtual
 * It is used for doing all initialisation common to derived classes in here.
 * Make it being able to leave if there are any errors here as there's no point in
 * trying to run a test step if anything fails.
 * The leave will be picked up by the framework.
 */
	{
	// Create/open semaphores
	TName semNameModifier;
	semNameModifier.Append(KModifierSemName);

	TInt err = iModifierSem.CreateGlobal(semNameModifier, 0);
	if (err == KErrAlreadyExists)
		{
		iModifierSem.OpenGlobal(semNameModifier);
		}

	TName semNameObserver;
	semNameObserver.Append(KObserverSemName);

	err = iObserverSem.CreateGlobal(semNameObserver, 0);
	if (err == KErrAlreadyExists)
		{
		iObserverSem.OpenGlobal(semNameObserver);
		}
		
	TProcessStartParams params;
	_LIT(KDummyFileName, "\\sys\\bin\\LbsRoot.exe");
	_LIT(KDummyProcessName, "LbsRoot");
	params.SetProcessFileName(KDummyFileName);
	params.SetProcessName(KDummyProcessName);
	params.SetRendezvousRequired(ETrue);
	TInt r = CProcessLaunch::ProcessLaunch(params);
	if(r != KErrNone)
		{
		INFO_PRINTF1(_L("Launch LbsRoot failed."));
		SetTestStepResult(EFail);
		}
	INFO_PRINTF1(_L("Launch LbsRoot succeeded."));

		
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CTe_LbsAdminSuiteStepBase::doTestStepPostambleL()
/**
 * @return - TVerdict
 * Implementation of CTestStep base class virtual
 * It is used for doing all after test treatment common to derived classes in here.
 * Make it being able to leave
 * The leave will be picked up by the framework.
 */
	{
	iModifierSem.Close();
	iObserverSem.Close();
	return TestStepResult();
	}

CTe_LbsAdminSuiteStepBase::~CTe_LbsAdminSuiteStepBase()
	{
	}

CTe_LbsAdminSuiteStepBase::CTe_LbsAdminSuiteStepBase()
	{

	}
