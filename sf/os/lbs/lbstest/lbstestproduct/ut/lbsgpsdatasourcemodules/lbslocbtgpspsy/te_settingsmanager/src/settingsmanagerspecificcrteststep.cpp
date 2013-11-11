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
 @file settingsmanagerspecificcrteststep.cpp
 @internalTechnology
*/

#include "settingsmanagerspecificcrteststep.h"
#include "te_settingsmanagerdefs.h"

CSettingsManagerSpecificCRTestStep::~CSettingsManagerSpecificCRTestStep()
/**
 * Destructor
 */
	{
	delete iCRChecker;
	}

CSettingsManagerSpecificCRTestStep::CSettingsManagerSpecificCRTestStep()
/**
 * Constructor
 */
	{
	SetTestStepName(KSettingsManagerSpecificCRTestStep);
	}

CSettingsManagerSpecificCRTestStep* CSettingsManagerSpecificCRTestStep::NewLC()
	{
	CSettingsManagerSpecificCRTestStep* self = new (ELeave)CSettingsManagerSpecificCRTestStep();
	CleanupStack::PushL (self);
	self->ConstructL ();
	return self;
	}

CSettingsManagerSpecificCRTestStep* CSettingsManagerSpecificCRTestStep::NewL()
	{
	CSettingsManagerSpecificCRTestStep* self=CSettingsManagerSpecificCRTestStep::NewLC ();
	CleanupStack::Pop (self);
	return self;
	}

void CSettingsManagerSpecificCRTestStep::ConstructL()
	{
	iCRChecker = CCRChecker::NewL();
	}

TVerdict CSettingsManagerSpecificCRTestStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CTe_settingsmanagerStepBase::doTestStepPreambleL();
	return TestStepResult();
	}


TVerdict CSettingsManagerSpecificCRTestStep::doTestStepL()
/**
 * @return - TVerdict code
 * 
 * This test step check the specific central repository keys used by the BTPSY.
 * 
 */
	{
	_LIT(KSpecificCRStepBeginning,"Start to check the central repository keys specific to all psy.");
	_LIT(KSpecificCRStepEnd,"Central repository keys specific to all psy are all correctly defined.");
	_LIT(KSpecificCRError,"A Central repository keys wasn't valid: %d.");
	
	if (TestStepResult()==EPass)
		{
		TInt err = KErrNone;
		INFO_PRINTF1(KSpecificCRStepBeginning);
		err = iCRChecker->CheckSpecificRepositorykeys();
		if (err != KErrNone)
			{
			ERR_PRINTF2(KSpecificCRError, err);
			SetTestStepResult(EFail);
			}
		else
			{
			INFO_PRINTF1(KSpecificCRStepEnd);
			}
		}
	return TestStepResult();
	}



TVerdict CSettingsManagerSpecificCRTestStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CTe_settingsmanagerStepBase::doTestStepPostambleL();
	return TestStepResult();
	}
