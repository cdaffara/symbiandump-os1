// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Example CTestStep derived implementation
// 
//

/**
 @file tdbtsisuninstallStep.cpp
 @internalTechnology
*/
#include "tdbtsisuninstallstep.h"

using namespace Swi;

CTDBTSisUninstallStep::~CTDBTSisUninstallStep()
/**
 * Destructor
 */
	{
	}

CTDBTSisUninstallStep::CTDBTSisUninstallStep()
/**
 * Constructor
 */
	{
	SetTestStepName(KTDBTSisUninstallStep);
	}

TVerdict CTDBTSisUninstallStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CTe_LbsSisInstallStepBase::doTestStepPreambleL();
	if (TestStepResult()!=EPass)
	    return   TestStepResult();
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CTDBTSisUninstallStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	  if (TestStepResult()==EPass)
		{
		SetTestStepResult(EPass);
		TInt aAppId;
		GetIntFromConfig(ConfigSection(),KTe_LbsSisInstallAppId,aAppId);
		SISUninstallTestL(TUid::Uid(aAppId));
		}
	  return TestStepResult();
	}

void CTDBTSisUninstallStep::SISUninstallTestL(const TUid& aAppId)
	{
	CUISisAdaptor* ui = CUISisAdaptor::NewLC(Logger());

	TInt err = Swi::Launcher::Uninstall(*ui, aAppId);

	CleanupStack::PopAndDestroy(ui);

	INFO_PRINTF2(_L("Launcher::Uninstall returned %d\n"), err);
	}

TVerdict CTDBTSisUninstallStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	return TestStepResult();
	}

// End of file
