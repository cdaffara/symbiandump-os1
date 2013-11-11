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
 @file lbsrootapiTest27Step.cpp
*/
#include "lbscommon.h" 
#include "LbsInternalInterface.h" 
#include "lbsrootapitest27step.h"
#include "te_lbsrootapiTestSuiteDefs.h"
#include "lbsrootapi.h"
#include "lbsqualityprofile.h"



CLbsRootApiTest27Step::~CLbsRootApiTest27Step()
/**
 * Destructor
 */
	{
	}

CLbsRootApiTest27Step::CLbsRootApiTest27Step()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KLbsRootApiTest27Step);
	}

TVerdict CLbsRootApiTest27Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("I am in Test Step Preamble in Class CLbsRootApiTest27Step"));

	// Set test-specific values for the settings in the LbsRoot cenrep.
	SetTestRepositorySettingsL();
    
 	SetTestStepResult(EPass);
	return TestStepResult();
	}

void CLbsRootApiTest27Step::DoMainL()
{
}

//
// Expected quality profile data
//
// This should be the same as the values in the LbsProfile.ini
// that is stored in LbsRoot\group, i.e. the 'default' settings
const TReal32 KExpectedHorizontalAccuracy1 = 50.0;
const TReal32 KExpectedVerticalAccuracy1 = 1000.0;
const TTimeIntervalMicroSeconds KExpectedMaxFixTime1 = 130000000;

const TReal32 KExpectedHorizontalAccuracy2 = 20.0;
const TReal32 KExpectedVerticalAccuracy2 = 1000.0;
const TTimeIntervalMicroSeconds KExpectedMaxFixTime2 = 120000000;


TVerdict CLbsRootApiTest27Step::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	TInt retVal;
	if (TestStepResult()==EPass)
		{

		INFO_PRINTF1(_L("CLbsRootApiTest27Step : open RLbsSystemController"));

		RLbsSystemController lbsRootProcessApi;
		lbsRootProcessApi.OpenL(KLbsTestProcessUid);
		CleanupClosePushL(lbsRootProcessApi);
		lbsRootProcessApi.RequestCompleteCloseDown();

		INFO_PRINTF1(_L("CLbsRootApiTest27Step : Request SystemStartup "));
		retVal = lbsRootProcessApi.RequestSystemStartup();
		if(retVal != ELbsStartupSuccess)
			{
			SetTestStepResult(EFail);
			}

		// Check the quality profile info was read in correctly.

		TUint numProfiles(0);
		TInt err = LbsQualityProfile::GetNumQualityProfiles(numProfiles);
		TESTL(err == KErrNone);
		TESTL(numProfiles == 2);
		
		TQualityProfile profile;
		err = LbsQualityProfile::GetQualityProfileById(0, profile);
		TESTL(err == KErrNone);
		TESTL(profile.MaxFixTime() == KExpectedMaxFixTime1);
		TESTL(profile.MinHorizontalAccuracy() == KExpectedHorizontalAccuracy1);
		TESTL(profile.MinVerticalAccuracy() == KExpectedVerticalAccuracy1);

		err = LbsQualityProfile::GetQualityProfileById(1, profile);
		TESTL(err == KErrNone);
		TESTL(profile.MaxFixTime() == KExpectedMaxFixTime2);
		TESTL(profile.MinHorizontalAccuracy() == KExpectedHorizontalAccuracy2);
		TESTL(profile.MinVerticalAccuracy() == KExpectedVerticalAccuracy2);
			
		INFO_PRINTF1(_L("CLbsRootApiTest27Step : close system down"));
		lbsRootProcessApi.RequestSystemCloseDown();

		CleanupStack::PopAndDestroy(&lbsRootProcessApi);
		}		
		
	return TestStepResult();
	}

TVerdict CLbsRootApiTest27Step::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("I am in Test Step Postamble in Class CLbsRootApiTest27Step"));
	// process something post setting to the test step
	// uncomment the following line if you have post process or remove the following line if no post process
	return TestStepResult();
	}
