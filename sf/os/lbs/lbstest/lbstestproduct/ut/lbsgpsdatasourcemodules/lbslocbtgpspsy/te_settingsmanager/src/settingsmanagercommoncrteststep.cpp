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
// SettingsManagerFunction implementation
//



/**
 @file settingsmanagercommoncrteststep.cpp
 @internalTechnology
*/

#include "settingsmanagercommoncrteststep.h"
#include "te_settingsmanagerdefs.h"

CSettingsManagerCommonCRTestStep::~CSettingsManagerCommonCRTestStep()
/**
 * Destructor
 */
	{
	delete iCRChecker;
	}

CSettingsManagerCommonCRTestStep::CSettingsManagerCommonCRTestStep()
/**
 * Constructor
 */
	{
	SetTestStepName(KSettingsManagerCommonCRTestStep);
	}

CSettingsManagerCommonCRTestStep* CSettingsManagerCommonCRTestStep::NewLC()
	{
	CSettingsManagerCommonCRTestStep* self = new (ELeave)CSettingsManagerCommonCRTestStep();
	CleanupStack::PushL (self);
	self->ConstructL ();
	return self;
	}

CSettingsManagerCommonCRTestStep* CSettingsManagerCommonCRTestStep::NewL()
	{
	CSettingsManagerCommonCRTestStep* self=CSettingsManagerCommonCRTestStep::NewLC ();
	CleanupStack::Pop (self);
	return self;
	}

void CSettingsManagerCommonCRTestStep::ConstructL()
	{
	iCRChecker = CCRChecker::NewL();
	}

TVerdict CSettingsManagerCommonCRTestStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CTe_settingsmanagerStepBase::doTestStepPreambleL();
	return TestStepResult();
	}


TVerdict CSettingsManagerCommonCRTestStep::doTestStepL()
/**
 * @return - TVerdict code
 * 
 * This test step check the common central repository keys used by the BTPSY.
 * 
 */
	{
	_LIT(KCommonCRStepBeginning,"Start to check the central repository keys common to all psy.");
	_LIT(KCommonCRStepEnd,"Central repository keys common to all psy are all correctly defined.");
	_LIT(KCommonCRError,"A Central repository keys wasn't valid: %d.");
	
	if (TestStepResult()==EPass)
		{
		TInt err = KErrNone;
		INFO_PRINTF1(KCommonCRStepBeginning);
		err = iCRChecker->CheckCommonRepositorykeys();
		if (err != KErrNone)
			{
			ERR_PRINTF2(KCommonCRError, err);
			SetTestStepResult(EFail);
			}
		else
			{
			INFO_PRINTF1(KCommonCRStepEnd);
			}
		}
	return TestStepResult();
	}



TVerdict CSettingsManagerCommonCRTestStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CTe_settingsmanagerStepBase::doTestStepPostambleL();
	return TestStepResult();
	}
