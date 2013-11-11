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
// Example CTestStep derived implementation
// 
//

/**
 @file LbsAdminErrorCheckStep.cpp
*/
#include "lbsadminerrorcheckstep.h"
#include "te_lbsadminsuitedefs.h"
#include <lbs/lbsadmin.h>



CLbsAdminErrorCheckStep::~CLbsAdminErrorCheckStep()
	{
	if(iLbsAdmin)
		{
		iLbsAdmin->ResetToDefault();
		delete iLbsAdmin;
		}
	}

CLbsAdminErrorCheckStep::CLbsAdminErrorCheckStep()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KLbsAdminErrorCheckStep);
	}

TVerdict CLbsAdminErrorCheckStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	if (TestStepResult()!=EPass)
	   return   TestStepResult();
	// Create an admin API 
	//
	iLbsAdmin = CLbsAdmin::NewL();
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CLbsAdminErrorCheckStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	  if (TestStepResult()==EPass)
		{
		// The aim of this test is to check for the correct error values
		// when incorrect values are passed in for settings.
		
		// Test that Uids are checked correctly
		// Note: 0x10281D61 is the Uid3 of te_LbsAdminSuite.exe, so it
		// should never show up as a valid ECom plugins
		TLbsProtocolModuleId protocolModule = TUid::Uid(0x10281D61);
		TInt err = iLbsAdmin->Set(KLbsSettingHomeProtocolModule, protocolModule);
		TESTL(err == KErrNotFound);
		err = iLbsAdmin->Set(KLbsSettingRoamingProtocolModule, protocolModule);
		TESTL(err == KErrNotFound);
		
		TLbsSetClockModuleId clockModule = TUid::Uid(0x10281D61);
		err = iLbsAdmin->Set(KLbsSettingSetClockModule, clockModule);
		TESTL(err == KErrNotFound);
		
		err = iLbsAdmin->Set(KLbsSettingPrivacyTimeoutAction, CLbsAdmin::EPrivacyTimeoutIgnore);
		TESTL(err == KErrNotSupported);
		
		/* as expected - won't link
		CLbsAdmin::TSpecialFeature ignoreAccuracy = CLbsAdmin::ESpecialFeatureOff;
		err = iLbsAdmin->Set(KLbsSpecialFeatureIgnoreAccuracy, ignoreAccuracy);
		TESTL(err == KErrArgument);
		*/
		
		INFO_PRINTF1(KErrorCheckRunBeforeLeaving);
		SetTestStepResult(EPass);
		}
		
	  return TestStepResult();
	}



TVerdict CLbsAdminErrorCheckStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	return TestStepResult();
	}
