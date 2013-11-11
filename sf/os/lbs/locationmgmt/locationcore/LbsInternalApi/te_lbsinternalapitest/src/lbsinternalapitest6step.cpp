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
 @file lbsinternalapiTest6Step.cpp
*/
#include "lbsinternalapitest6step.h"
#include "te_lbsinternalapiTestsuitedefs.h"

#include "LbsInternalInterface.h"
#include "lbsqualityprofile.h"

ClbsinternalapiTest6Step::~ClbsinternalapiTest6Step()
/**
 * Destructor
 */
	{
	}

ClbsinternalapiTest6Step::ClbsinternalapiTest6Step()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KlbsinternalapiTest6Step);
	}

TVerdict ClbsinternalapiTest6Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("Please delete this line or modify it. I am in Test Step Preamble in Class ClbsinternalapiTest6Step"));
	// uncomment the following 3 lines if you have common pre setting to all the test steps in there
	// CTe_lbsinternalapiTestSuiteStepBase::doTestStepPreambleL();
	// if (TestStepResult()!=EPass)
	//    return   TestStepResult();
	// process some pre setting to this test step then set SetTestStepResult to EFail or Epass.
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict ClbsinternalapiTest6Step::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	  if (TestStepResult()==EPass)
		{
		//
		// Test the Quality Profile interface
		//
		
		TQualityProfile profile1;
		TQualityProfile profile2;
		RArray<TQualityProfile> profileArray;
		CleanupClosePushL(profileArray);
		
		profile1.SetQualityProfileId(1);
		profile1.SetMinHorizontalAccuracy(TReal32(50.0f));
		profile1.SetMinVerticalAccuracy(TReal32(100.0f));
		profile1.SetMaxFixTime(10000000);

		profile2.SetQualityProfileId(2);
		profile2.SetMinHorizontalAccuracy(TReal32(250.0f));
		profile2.SetMinVerticalAccuracy(TReal32(1250.0f));
		profile2.SetMaxFixTime(5000000);
		
		// Store profile2 first, so that we can check the
		// 'GetById' function is working correctly
		profileArray.Append(profile2);
		profileArray.Append(profile1);
		LbsQualityProfile::InitializeL(profileArray);
		
		TUint numProfiles;
		TInt err = LbsQualityProfile::GetNumQualityProfiles(numProfiles);
		TESTL(err == KErrNone);
		TESTL(numProfiles == 2);
		
		// Test the GetQualityProfileById function..
		TQualityProfile profile;
		err = LbsQualityProfile::GetQualityProfileById(1, profile);
		TESTL(err == KErrNone);
		TESTL(profile.QualityProfileId() == profile1.QualityProfileId());
		TESTL(profile.MaxFixTime() == profile1.MaxFixTime());
		TESTL(profile.MinHorizontalAccuracy() == profile1.MinHorizontalAccuracy());
		TESTL(profile.MinVerticalAccuracy() == profile1.MinVerticalAccuracy());

		err = LbsQualityProfile::GetQualityProfileById(2, profile);
		TESTL(err == KErrNone);
		TESTL(profile.QualityProfileId() == profile2.QualityProfileId());
		TESTL(profile.MaxFixTime() == profile2.MaxFixTime());
		TESTL(profile.MinHorizontalAccuracy() == profile2.MinHorizontalAccuracy());
		TESTL(profile.MinVerticalAccuracy() == profile2.MinVerticalAccuracy());

		err = LbsQualityProfile::GetQualityProfileById(6, profile);
		TESTL(err == KErrNotFound);
		
		// Test the GetQualityProfileByIndex function..
		err = LbsQualityProfile::GetQualityProfileByIndex(0, profile);
		TESTL(err == KErrNone);
		TESTL(profile.QualityProfileId() == profile2.QualityProfileId());
		TESTL(profile.MaxFixTime() == profile2.MaxFixTime());
		TESTL(profile.MinHorizontalAccuracy() == profile2.MinHorizontalAccuracy());
		TESTL(profile.MinVerticalAccuracy() == profile2.MinVerticalAccuracy());

		err = LbsQualityProfile::GetQualityProfileByIndex(1, profile);
		TESTL(err == KErrNone);
		TESTL(profile.QualityProfileId() == profile1.QualityProfileId());
		TESTL(profile.MaxFixTime() == profile1.MaxFixTime());
		TESTL(profile.MinHorizontalAccuracy() == profile1.MinHorizontalAccuracy());
		TESTL(profile.MinVerticalAccuracy() == profile1.MinVerticalAccuracy());
		
		err = LbsQualityProfile::GetQualityProfileByIndex(2, profile);
		TESTL(err == KErrArgument);
				
		CleanupStack::PopAndDestroy(&profileArray);
		}
	  return TestStepResult();
	}



TVerdict ClbsinternalapiTest6Step::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("Please delete this line or modify it. I am in Test Step Postamble in Class ClbsinternalapiTest6Step"));
	// process something post setting to the test step
	// uncomment the following line if you have common post setting to all the test steps in there
	// CTe_lbsinternalapiTestSuiteStepBase::doTestStepPostambleL();
	// uncomment the following line if you have post process or remove the following line if no post process
	// SetTestStepResult(EPass);		// or EFail
	return TestStepResult();
	}
