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
 @file CenrepTestStep.cpp
 @internalTechnology
*/
#include "cenrepteststep.h"
#include "te_defproxysuitedefs.h"

CCenrepTestStep::~CCenrepTestStep()
/**
 * Destructor
 */
	{
	}

CCenrepTestStep::CCenrepTestStep()
/**
 * Constructor
 */
	{
	SetTestStepName(KCenrepTestStep);
	}

TVerdict CCenrepTestStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CTe_defproxySuiteStepBase::doTestStepPreambleL();
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CCenrepTestStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
    TUid uid; uid.iUid = 0x101F7A80;
    TUid dtorIdKey;

    TAny* ptr = NULL;
    TInt err;
    TRAP(err, ptr = REComSession::CreateImplementationL(uid, dtorIdKey));
    CheckExpectedResult(err, KErrNotFound, KBadResults);
    delete ptr;

    TPositionModuleInfo info;
    iModulesSettings = CPosModules::OpenL();

    TRAP(err, iModulesSettings->GetModuleInfoL(uid, info));
    CheckExpectedResult(err, KErrNotFound, KBadResults);

    CRepository* repository = NULL;
    TRAP(err, repository = CRepository::NewL(uid));
    CheckExpectedResult(err, KErrNotFound, KBadResults);

    TRAP(err, repository = CRepository::NewL(KNullUid));
    CheckExpectedResult(err, KErrNotFound, KBadResults);
	if(repository)
		{
		SetTestStepResult(EFail);
		}
	return TestStepResult();
	}



TVerdict CCenrepTestStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CTe_defproxySuiteStepBase::doTestStepPostambleL();
	return TestStepResult();
	}
