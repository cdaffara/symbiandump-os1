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
 @file TDBTSisInstallStep.cpp
 @internalTechnology
*/
#include "tdbtsisinstallstep.h"

using namespace Swi;

CTDBTSisInstallStep::~CTDBTSisInstallStep()
/**
 * Destructor
 */
	{
	}

CTDBTSisInstallStep::CTDBTSisInstallStep()
/**
 * Constructor
 */
	{
	SetTestStepName(KTDBTSisInstallStep);
	}

TVerdict CTDBTSisInstallStep::doTestStepPreambleL()
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


TVerdict CTDBTSisInstallStep::doTestStepL()
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
		TPtrC sispath;
		GetStringFromConfig(ConfigSection(),KTe_LbsSisInstallSisLocation,sispath);
		INFO_PRINTF1(sispath);
		SISInstallTestL(sispath);
		}
	  return TestStepResult();
	}

void CTDBTSisInstallStep::SISInstallTestL(const TDesC& aSisPath)
	{
	CUISisAdaptor *ui = CUISisAdaptor::NewLC(Logger());
	CInstallPrefs* prefs = CInstallPrefs::NewLC();

	TInt err = Swi::Launcher::Install(*ui, aSisPath, *prefs);

	if(err != KErrNone)
		{
		INFO_PRINTF2(_L("Failed: Launcher::Install() returned %d\n"), err);
		SetTestStepResult(EFail);
		}
	
	CleanupStack::PopAndDestroy(prefs);
	CleanupStack::PopAndDestroy(ui);

	}

TVerdict CTDBTSisInstallStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	return TestStepResult();
	}

// End of file
