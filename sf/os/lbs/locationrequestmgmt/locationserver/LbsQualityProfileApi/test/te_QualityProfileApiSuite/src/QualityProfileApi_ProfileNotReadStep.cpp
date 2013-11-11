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
 @file QualityProfileApi_ProfileNotReadStep.cpp
 @internalTechnology
*/
#include "QualityProfileApi_ProfileNotReadStep.h"
#include "Te_QualityProfileApiSuiteSuiteDefs.h"
#include <lbs/lbsqualityprofileapi.h>

CQualityProfileApi_ProfileNotReadStep::~CQualityProfileApi_ProfileNotReadStep()
/**
 * Destructor
 */
	{
	}

CQualityProfileApi_ProfileNotReadStep::CQualityProfileApi_ProfileNotReadStep()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KQualityProfileApi_ProfileNotReadStep);
	}

TVerdict CQualityProfileApi_ProfileNotReadStep::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CQualityProfileApi_ProfileNotReadStep::doTestStepL()
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
		// This test is meant to be run when the LBS system has not
		// been started. It is meant to test that the quality profile api
		// returns the expected error code(s).
		//
		CLbsQualityProfileApi* profileApi = CLbsQualityProfileApi::NewL();
		CleanupStack::PushL(profileApi);

		TReal32 realValue;
		TTimeIntervalMicroSeconds timeValue;
		
		TInt err = profileApi->Get(1, KLbsQualityProfileHorizontalAccuracy, realValue);
		TESTL(err == KErrNotFound);

		err = profileApi->Get(1, KLbsQualityProfileVerticalAccuracy, realValue);
		TESTL(err == KErrNotFound);

		err = profileApi->Get(1, KLbsQualityProfileMaxFixTime, timeValue);
		TESTL(err == KErrNotFound);

		err = profileApi->Get(1, KLbsQualityProfileMeasurementInitialFixTime, timeValue);
		TESTL(err == KErrNotFound);

		err = profileApi->Get(1, KLbsQualityProfileMeasurementIntermediateFixTime, timeValue);
		TESTL(err == KErrNotFound);
		
		CleanupStack::PopAndDestroy(profileApi);
		
		SetTestStepResult(EPass);
		}
	  return TestStepResult();
	}



TVerdict CQualityProfileApi_ProfileNotReadStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	return TestStepResult();
	}
