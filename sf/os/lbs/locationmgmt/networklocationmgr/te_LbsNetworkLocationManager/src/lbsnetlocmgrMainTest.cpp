
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
 @file LbsnetlocmgrMainTest.cpp
*/
#include "lbsnetlocmgrMainTest.h"
#include "Te_LbsNetLocMgrTestSuiteDefs.h"
#include "cprocesslaunch.h"
#include "tprocessstartparams.h"

#include "lbsprocesssupervisor.h"

#include "lbspositioninfo.h"
ClbsnetlocmgrMainTest::~ClbsnetlocmgrMainTest()
/**
 * Destructor
 */
	{
	}

ClbsnetlocmgrMainTest::ClbsnetlocmgrMainTest()
/**
 * Constructor
 */
	{
	SetTestStepName(KLbsnetlocmgrMainTest);
	}

TVerdict ClbsnetlocmgrMainTest::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CTe_LbsNetLocMgrTestSuiteStepBase::doTestStepPreambleL();
	if (TestStepResult()!=EPass)
	    return TestStepResult();
	
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict ClbsnetlocmgrMainTest::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	INFO_PRINTF1(_L("Simulate location server sending location update request"));
	INFO_PRINTF1(_L("Check that location published by manager is correct"));	  
	
	if (TestStepResult()==EPass)
		{
		//Initialize the process supervisor
		RLbsProcessSupervisor lbsSupervisor;
		lbsSupervisor.InitializeL();
		lbsSupervisor.OpenL(KLbsNetLocManagerUid);

		TLbsPositionUpdateRequest request;
		TTime targetTime;
		targetTime.UniversalTime(); // target time is now and try for 5 seconds!
		targetTime += (TTimeIntervalSeconds)5;
		TLbsLocRequestQualityInt requestQuality;				
		requestQuality.SetMaxFixTime(5000000);
		requestQuality.SetMinHorizontalAccuracy(0.1);
		requestQuality.SetMinVerticalAccuracy(0.1);
	
		request.TargetTime() = targetTime;
		request.RequestQuality() = requestQuality;
		
		TInt err= KErrNone;
		TRAP(err, iPositionReq.SetPositionUpdateRequest(request);)
		if (err!=KErrNone)
			{
			INFO_PRINTF1(_L("\t Send request failed.")); 
			SetTestStepResult(EFail);	
			return TestStepResult();			
			}			
		INFO_PRINTF1(_L("Send location request to netlocmanager."));
			
		// wait for response
		User::WaitForRequest(iStatusUpd);		
		
		if(iStatusUpd.Int()!=KErrNone)
			{
			INFO_PRINTF1(_L("\t response failed.")); 

			SetTestStepResult(EFail);	
			return TestStepResult();			
			}	
		
		TInt 	error = 1; // something other than KErrNone
		TTime 	actualTime = 0;
		TTime   newTime = 0;
		TPositionCourseInfo posInfo;
		TPosition pos;
		TBool cControl;//Not used
		TCourse course;
		
		error = iPositionUpd.GetPositionInfo(cControl, &posInfo, sizeof(TPositionCourseInfo),targetTime, actualTime);
		posInfo.GetPosition(pos);
        posInfo.GetCourse(course);

        // Check the TCourse values set in the dummy netgateway.cpp
        INFO_PRINTF2(_L("TCourse::Speed, Expected:10.0 Got:%2.1f"), course.Speed());
        TESTL(course.Speed() == 10.0);

        INFO_PRINTF2(_L("TCourse::VerticalSpeed, Expected:20.0 Got:%2.1f"), course.VerticalSpeed());
        TESTL(course.VerticalSpeed() == 20.0);

        INFO_PRINTF2(_L("TCourse::Heading, Expected:30.0 Got:%2.1f"), course.Heading());
        TESTL(course.Heading() == 30.0);

        INFO_PRINTF2(_L("TCourse::Course, Expected:40.0 Got:%2.1f"), course.Course());
        TESTL(course.Course() == 40.0);
        
        INFO_PRINTF2(_L("TCourse::SpeedAccuracy, Expected:1.0 Got:%2.1f"), course.SpeedAccuracy());
        TESTL(course.SpeedAccuracy() == 1.0);

        INFO_PRINTF2(_L("TCourse::VerticalSpeedAccuracy, Expected:2.0 Got:%2.1f"), course.VerticalSpeedAccuracy());
        TESTL(course.VerticalSpeedAccuracy() == 2.0);

        INFO_PRINTF2(_L("TCourse::HeadingAccuracy, Expected:3.0 Got:%2.1f"), course.HeadingAccuracy());
        TESTL(course.HeadingAccuracy() == 3.0);

        INFO_PRINTF2(_L("TCourse::CourseAccuracy, Expected:4.0 Got:%2.1f"), course.CourseAccuracy());
        TESTL(course.CourseAccuracy() == 4.0);

        if(error != KErrNone || pos.Latitude() != 50)
			{
			INFO_PRINTF1(_L("\t response posinfo/time wrong.")); 
			SetTestStepResult(EFail);	
			return TestStepResult();			
			}
		
		
		lbsSupervisor.CloseDownProcess();
			
		lbsSupervisor.Close();
		

		SetTestStepResult(EPass);
		}

	  return TestStepResult();
	}



TVerdict ClbsnetlocmgrMainTest::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CTe_LbsNetLocMgrTestSuiteStepBase::doTestStepPostambleL();
	
	return TestStepResult();
	}
