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
 @file lbsrootapiTest31Step.cpp
*/
#include "lbscommon.h" 
#include "LbsInternalInterface.h" 
#include "lbsrootapitest31step.h"
#include "te_lbsrootapiTestSuiteDefs.h"
#include "lbsrootapi.h"
#include "lbsqualityprofile.h"



CLbsRootApiTest31Step::~CLbsRootApiTest31Step()
/**
 * Destructor
 */
	{
	}

CLbsRootApiTest31Step::CLbsRootApiTest31Step()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KLbsRootApiTest31Step);
	}

TVerdict CLbsRootApiTest31Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("I am in Test Step Preamble in Class CLbsRootApiTest31Step"));

	// Set test-specific values for the settings in the LbsRoot cenrep.
	SetTestRepositorySettingsL();
    
 	SetTestStepResult(EPass);
	return TestStepResult();
	}

void CLbsRootApiTest31Step::DoMainL()
{
}

TVerdict CLbsRootApiTest31Step::doTestStepL()
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

		INFO_PRINTF1(_L("CLbsRootApiTest31Step : open RLbsSystemController"));

		RLbsSystemController lbsRootProcessApi;
		lbsRootProcessApi.OpenL(KLbsTestProcessUid);
		CleanupClosePushL(lbsRootProcessApi);
		lbsRootProcessApi.RequestCompleteCloseDown();

		INFO_PRINTF1(_L("CLbsRootApiTest31Step : Request SystemStartup "));
		retVal = lbsRootProcessApi.RequestSystemStartup();
		if(retVal != ELbsStartupSuccess)
			{
			SetTestStepResult(EFail);
			}

		// Expected quality profile data
		
		// First quality profile
		const TInt KExpectedProfileId1 = 1;
		const TReal32 KExpectedHorizontalAccuracy1 = 23.2;
		const TReal32 KExpectedVerticalAccuracy1 = 87.9;
		const TTimeIntervalMicroSeconds KExpectedMaxFixTime1 = 5000000;
		const TTimeIntervalMicroSeconds KExpectedMeasurementInitialTime1 = 0;
		const TTimeIntervalMicroSeconds KExpectedMeasurementIntermediateTimeTime1 = 0;

		// Second qquality profile
		const TInt KExpectedProfileId2 = 2;
		const TReal32 KExpectedHorizontalAccuracy2 = 5.0;
		const TReal32 KExpectedVerticalAccuracy2 = 250.0;
		const TTimeIntervalMicroSeconds KExpectedMaxFixTime2 = 60000000;
		const TTimeIntervalMicroSeconds KExpectedMeasurementInitialTime2= 10000000;
		const TTimeIntervalMicroSeconds KExpectedMeasurementIntermediateTimeTime2 = 0;
		
		// Third quality profile
		const TInt KExpectedProfileId3 = 3;
		const TReal32 KExpectedHorizontalAccuracy3 = 100.0;
		const TReal32 KExpectedVerticalAccuracy3 = 500.0;
		const TTimeIntervalMicroSeconds KExpectedMaxFixTime3 = 120000000;
		const TTimeIntervalMicroSeconds KExpectedMeasurementInitialTime3 = 0;
		const TTimeIntervalMicroSeconds KExpectedMeasurementIntermediateTimeTime3 = 4000000;

		// Check the quality profile info was read in correctly.
		//
		// This test is mainly checking that the sections of the .ini file
		// are read properly, and that the parsing code doesn't try to read
		// values from another section other than the current one.
		//
		// In this tes, that means that the 'MeasurementInitialTime' and
		// 'MeasurementIntermediateTime' should default to 0 if they are not
		// specified in the section in the .ini file.

		TUint numProfiles(0);
		TInt err = LbsQualityProfile::GetNumQualityProfiles(numProfiles);
		TESTL(err == KErrNone);
		TESTL(numProfiles == 3);
		
		TQualityProfile profile;
		err = LbsQualityProfile::GetQualityProfileById(KExpectedProfileId1, profile);
		TESTL(err == KErrNone);
		TESTL(profile.MaxFixTime() == KExpectedMaxFixTime1);
		TESTL(profile.MinHorizontalAccuracy() == KExpectedHorizontalAccuracy1);
		TESTL(profile.MinVerticalAccuracy() == KExpectedVerticalAccuracy1);
		TESTL(profile.MeasurementInitialFixTime() == KExpectedMeasurementInitialTime1);
		TESTL(profile.MeasurementIntermediateFixTime() == KExpectedMeasurementIntermediateTimeTime1);

		err = LbsQualityProfile::GetQualityProfileById(KExpectedProfileId2, profile);
		TESTL(err == KErrNone);
		TESTL(profile.MaxFixTime() == KExpectedMaxFixTime2);
		TESTL(profile.MinHorizontalAccuracy() == KExpectedHorizontalAccuracy2);
		TESTL(profile.MinVerticalAccuracy() == KExpectedVerticalAccuracy2);
		TESTL(profile.MeasurementInitialFixTime() == KExpectedMeasurementInitialTime2);
		TESTL(profile.MeasurementIntermediateFixTime() == KExpectedMeasurementIntermediateTimeTime2);

		err = LbsQualityProfile::GetQualityProfileById(KExpectedProfileId3, profile);
		TESTL(err == KErrNone);
		TESTL(profile.MaxFixTime() == KExpectedMaxFixTime3);
		TESTL(profile.MinHorizontalAccuracy() == KExpectedHorizontalAccuracy3);
		TESTL(profile.MinVerticalAccuracy() == KExpectedVerticalAccuracy3);
		TESTL(profile.MeasurementInitialFixTime() == KExpectedMeasurementInitialTime3);
		TESTL(profile.MeasurementIntermediateFixTime() == KExpectedMeasurementIntermediateTimeTime3);
			
		INFO_PRINTF1(_L("CLbsRootApiTest31Step : close system down"));
		//lbsRootProcessApi.RequestSystemCloseDown();

		CleanupStack::PopAndDestroy(&lbsRootProcessApi);
		}		
		
	return TestStepResult();
	}

TVerdict CLbsRootApiTest31Step::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("I am in Test Step Postamble in Class CLbsRootApiTest31Step"));
	// process something post setting to the test step
	// uncomment the following line if you have post process or remove the following line if no post process
	return TestStepResult();
	}
