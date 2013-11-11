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
// SettingsManagerInitialisation implementation
//



/**
 @file settingsmanagerinitialisationstep.cpp
 @internalTechnology
*/
#include "settingsmanagerinitialisationstep.h"
#include "te_settingsmanagerdefs.h"

CSettingsManagerInitialisationStep::~CSettingsManagerInitialisationStep()
/**
 * Destructor
 */
	{
	delete iPNSChecker;
	if(iLBSCustomer)
		{
		delete iLBSCustomer;
		}
	}

CSettingsManagerInitialisationStep::CSettingsManagerInitialisationStep()
/**
 * Constructor
 */
	{
	SetTestStepName(KSettingsManagerInitialisationStep);
	}

CSettingsManagerInitialisationStep* CSettingsManagerInitialisationStep::NewLC()
	{
	CSettingsManagerInitialisationStep* self = new (ELeave)CSettingsManagerInitialisationStep();
	CleanupStack::PushL (self);
	self->ConstructL ();
	return self;
	}

CSettingsManagerInitialisationStep* CSettingsManagerInitialisationStep::NewL()
	{
	CSettingsManagerInitialisationStep* self=CSettingsManagerInitialisationStep::NewLC ();
	CleanupStack::Pop (self);
	return self;
	}

void CSettingsManagerInitialisationStep::ConstructL()
	{
	iPNSChecker = CPNSChecker::NewL();
	iLBSCustomer = CLBSCustomer::NewL();
	}

TVerdict CSettingsManagerInitialisationStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CTe_settingsmanagerStepBase::doTestStepPreambleL();
	return TestStepResult();
	}


TVerdict CSettingsManagerInitialisationStep::doTestStepL()
/**
 * @return - TVerdict code
 * 
 * This test step look for the existence of P&S keys.
 * It also start the location server and load the Bluetooth PSY, as it update automatically the P&S keys.
 * At the end, the test control the value of each P&S keys.
 * If some keys are missing or some keys value are outside their expected range, the test will fail.
 * 
 */
	{
	if (TestStepResult()==EPass)
		{
		TBool areAvailableKeys = ETrue;
		TBool isBTPSYLoaded = ETrue;
		TBool areValidKeys = ETrue;
		iPNSChecker->CheckPNSKeys(areAvailableKeys, isBTPSYLoaded, areValidKeys);
		PrintResults(areAvailableKeys, isBTPSYLoaded, areValidKeys);
		
		TInt err = iLBSCustomer->ConnectToPositionServer();
		_LIT(KConnectInfo, "Connect to position server: %d");
		INFO_PRINTF2(KConnectInfo, err);
		
		if(KErrNone == err || KErrAlreadyExists == err)
			{
			if(KErrNone == err)
				{
				err = iLBSCustomer->OpenBTPSYPositioner();
				_LIT(KOpenModule, "Open BT module: %d");
				INFO_PRINTF2(KOpenModule, err);
				}
			}
		if(KErrNone == err || KErrAlreadyExists == err)
			{
			iPNSChecker->CheckPNSKeys(areAvailableKeys, isBTPSYLoaded, areValidKeys);
			PrintResults(areAvailableKeys, isBTPSYLoaded, areValidKeys);
			}
		else
			{
			_LIT(KUnexpectedError, "Failed to Connect to the Bt module = %d");
			ERR_PRINTF2(KUnexpectedError, err);
			SetTestStepResult(EFail);
			}

		delete iLBSCustomer;
		iLBSCustomer = NULL;
		
		// If some keys are missing or some keys value are outside their expected range,
		// the test will fail.
		if ((!areAvailableKeys)||(!areValidKeys))
			{
			SetTestStepResult(EFail);
			}
		}
	return TestStepResult();
	}

TVerdict CSettingsManagerInitialisationStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CTe_settingsmanagerStepBase::doTestStepPostambleL();
	return TestStepResult();
	}

void CSettingsManagerInitialisationStep::PrintResults(TBool areAvailableKeys,TBool isBTPSYLoaded,TBool areValidKeys)
/**
 * This utility class prints tests output in the testexecute logs.
 */
	{
	_LIT(KKeysAvailable, "Keys available");
	_LIT(KKeysNotAvailable, "Keys not available");
	_LIT(KPSYNotLoaded, "psy not loaded");
	_LIT(KPSYLoaded, "psy loaded");
	_LIT(KKeysValid, "Keys valid");
	_LIT(KKeysNotValid, "Keys not valid");
	
	if(areAvailableKeys)
		{
		INFO_PRINTF1(KKeysAvailable);
		}
	else
		{
		INFO_PRINTF1(KKeysNotAvailable);
		}
	if(isBTPSYLoaded)
		{
		INFO_PRINTF1(KPSYNotLoaded);
		}
	else
		{
		INFO_PRINTF1(KPSYLoaded);
		}
	if(areValidKeys)
		{
		INFO_PRINTF1(KKeysValid);
		}
	else
		{
		INFO_PRINTF1(KKeysNotValid);
		}
	}

