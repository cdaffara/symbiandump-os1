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
 @file lbsinternalapiTest3Step.cpp
*/
#include "lbsinternalapitest3step.h"
#include "te_lbsinternalapiTestsuitedefs.h"


#include <e32math.h>
#include "LbsInternalInterface.h"
#include <lbs/lbsextendedsatellite.h>

ClbsinternalapiTest3Step::~ClbsinternalapiTest3Step()
/**
 * Destructor
 */
	{
	}

ClbsinternalapiTest3Step::ClbsinternalapiTest3Step()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KlbsinternalapiTest3Step);
	}

TVerdict ClbsinternalapiTest3Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("Please delete this line or modify it. I am in Test Step Preamble in Class ClbsinternalapiTest3Step"));
	// uncomment the following 3 lines if you have common pre setting to all the test steps in there
	// CTe_lbsinternalapiTestSuiteStepBase::doTestStepPreambleL();
	// if (TestStepResult()!=EPass)
	//    return   TestStepResult();
	// process some pre setting to this test step then set SetTestStepResult to EFail or Epass.
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict ClbsinternalapiTest3Step::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	  if (TestStepResult()==EPass)
		{
		//Let's test!

	    TPositionExtendedSatelliteInfo posSatInfo;
		RLbsPositionUpdates::InitializeL(posSatInfo);
		
		RLbsPositionUpdates posUpdates;
		
		//First Open, Close, re-Open to check for handle leaks etc.
		posUpdates.OpenL(KLbsGpsLocManagerUid);
		CleanupClosePushL(posUpdates);
		CleanupStack::PopAndDestroy(&posUpdates);
		
		posUpdates.OpenL(KLbsGpsLocManagerUid);
		CleanupClosePushL(posUpdates);
		
		//First, check the default data is OK.
		//Should be a default TPositionExtendedSatelliteInfo.
		TPositionExtendedSatelliteInfo satInfo;
		//Put some non-default data in it, then check we get the default data back.
		satInfo.SetSatelliteTime(TTime(500));
		
		TInt error=1;
		TTime targetTime(100);
		TTime actualTime(200);
		TBool conflictControl;
		error = posUpdates.GetPositionInfo(conflictControl, &satInfo, sizeof(satInfo),targetTime, actualTime);
		//Now check we got deviuce not ready
		
		if(error!=KErrNotReady)
			User::Leave(KErrGeneral);
			
		//Now check that the same for TPositionInfo and TPositionCourseInfo
		
		TPositionInfo posInfo;
		posInfo.SetModuleId(KLbsGpsLocManagerUid);
		error = posUpdates.GetPositionInfo(conflictControl, &posInfo, sizeof(posInfo),targetTime, actualTime);

		if(error!=KErrNotReady)
			User::Leave(KErrGeneral);
	
				
		TPositionCourseInfo courseInfo;
		TCourse course;
		course.SetSpeed(TReal32(100));
		courseInfo.SetCourse(course);
		
		error = posUpdates.GetPositionInfo(conflictControl, &courseInfo, sizeof(courseInfo),targetTime, actualTime);
		if(error!=KErrNotReady)
			User::Leave(KErrGeneral);

	
		
		//OK - we are now reasonably happy with Getting - it donsn't mash up the class size/type data
		//and it seems to be doing the data copying OK.
		
		//Next move onto the set method...
		//Choose some dummy values.
		error = KErrGeneral;
		targetTime = 1000;
		actualTime = 1001;
		satInfo.SetSatelliteTime(TTime(4020));
		
		User::LeaveIfError(posUpdates.SetPositionInfo(error, EFalse, &satInfo, sizeof(satInfo),targetTime, actualTime));
		error = posUpdates.GetPositionInfo(conflictControl, &satInfo, sizeof(satInfo),targetTime, actualTime);
		if(error!=KErrGeneral)
			User::Leave(KErrGeneral);
		if(conflictControl!=EFalse)
			User::Leave(KErrGeneral);
		if(targetTime!=1000)
			User::Leave(KErrGeneral);
		if(actualTime!=1001)
			User::Leave(KErrGeneral);
		if(satInfo.SatelliteTime()!=TTime(4020))
			User::Leave(KErrGeneral);
		
		
		//Go on to check out notifications.
		//Open another handle.
		
		RLbsPositionUpdates posUpdates2;
		
		//First Open, Close, re-Open to check for handle leaks etc.
		posUpdates2.OpenL(KLbsGpsLocManagerUid);
		CleanupClosePushL(posUpdates2);
		
		TRequestStatus stat;
		posUpdates2.NotifyPositionUpdate(stat);
		User::LeaveIfError(posUpdates.SetPositionInfo(error, EFalse, &satInfo, sizeof(satInfo),targetTime, actualTime));
		User::WaitForRequest(stat);
		
		if(stat.Int()!=KErrNone)
			User::Leave(KErrGeneral);
		
		//Now check the cancel.
		posUpdates2.NotifyPositionUpdate(stat);
		posUpdates2.CancelNotifyPositionUpdate();
		User::WaitForRequest(stat);
		
		if(stat.Int()!=KErrCancel)
			User::Leave(KErrGeneral);

		CleanupStack::PopAndDestroy(2, &posUpdates);
		SetTestStepResult(EPass);
		}
	  return TestStepResult();
	}




TVerdict ClbsinternalapiTest3Step::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("Please delete this line or modify it. I am in Test Step Postamble in Class ClbsinternalapiTest3Step"));
	// process something post setting to the test step
	// uncomment the following line if you have common post setting to all the test steps in there
	// CTe_lbsinternalapiTestSuiteStepBase::doTestStepPostambleL();
	// uncomment the following line if you have post process or remove the following line if no post process
	// SetTestStepResult(EPass);		// or EFail
	return TestStepResult();
	}
