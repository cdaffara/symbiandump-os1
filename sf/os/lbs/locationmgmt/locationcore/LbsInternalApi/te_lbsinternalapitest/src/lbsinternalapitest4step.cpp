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
 @file lbsinternalapiTest2Step.cpp
*/
#include "lbsinternalapitest4step.h"
#include "te_lbsinternalapiTestsuitedefs.h"

#include "LbsInternalInterface.h"

ClbsinternalapiTest4Step::~ClbsinternalapiTest4Step()
/**
 * Destructor
 */
	{
	}

ClbsinternalapiTest4Step::ClbsinternalapiTest4Step()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KlbsinternalapiTest4Step);
	}

TVerdict ClbsinternalapiTest4Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("Please delete this line or modify it. I am in Test Step Preamble in Class ClbsinternalapiTest4Step"));
	// uncomment the following 3 lines if you have common pre setting to all the test steps in there
	// CTe_lbsinternalapiTestSuiteStepBase::doTestStepPreambleL();
	// if (TestStepResult()!=EPass)
	//    return   TestStepResult();
	// process some pre setting to this test step then set SetTestStepResult to EFail or Epass.
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict ClbsinternalapiTest4Step::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	  if (TestStepResult()==EPass)
		{
		TPositionModuleStatus defaultPositionModuleStatus;
    	RLbsModuleStatus::InitializeL(defaultPositionModuleStatus);
		
		RLbsModuleStatus modStatus;
		modStatus.OpenL(KLbsGpsLocManagerUid);
		CleanupClosePushL(modStatus);
		CleanupStack::PopAndDestroy(&modStatus);
		
		modStatus.OpenL(KLbsGpsLocManagerUid);
		CleanupClosePushL(modStatus);
		
		//Check the defaults are OK...
		TPositionModuleStatus status;
		status.SetDeviceStatus(TPositionModuleStatus::EDeviceError);
		User::LeaveIfError(RLbsModuleStatus::GetModuleStatus(&status, sizeof(TPositionModuleStatus),KLbsGpsLocManagerUid));

		if(status.DeviceStatus()!=TPositionModuleStatus::EDeviceUnknown)
			User::Leave(KErrGeneral);
		
		status.SetDeviceStatus(TPositionModuleStatus::EDeviceError);
		TPositionModuleStatusEventBase::TModuleEvent occurredEvents =
				TPositionModuleStatusEventBase::EEventDeviceStatus;
		User::LeaveIfError(modStatus.GetModuleStatus(&status, sizeof(TPositionModuleStatus), occurredEvents));

		if(status.DeviceStatus()!=TPositionModuleStatus::EDeviceUnknown)
			User::Leave(KErrGeneral);
		if(occurredEvents!=TPositionModuleStatusEventBase::ESystemUnknown)
			User::Leave(KErrGeneral);
		
		//OK - get seems to work OK. Check Set works OK...
		//Need another RLbsModuleStatus to check passing is done correctly.
		RLbsModuleStatus modStatus2;
		modStatus2.OpenL(KLbsGpsLocManagerUid);
		CleanupClosePushL(modStatus2);
		
		//Check the two valid event types...
		//Device Status...
		status.SetDeviceStatus(TPositionModuleStatus::EDeviceError);
		//User::LeaveIfError(modStatus.SetModuleStatus(status, TPositionModuleStatusEventBase::EEventDeviceStatus));
		User::LeaveIfError(modStatus.SetModuleStatus(&status, sizeof(TPositionModuleStatus),TPositionModuleStatusEventBase::EEventDeviceStatus));
		
		User::LeaveIfError(modStatus2.GetModuleStatus(&status, sizeof(TPositionModuleStatus),occurredEvents));

		if(occurredEvents!=TPositionModuleStatusEventBase::EEventDeviceStatus)
			User::Leave(KErrGeneral);
		
		//Data Quality Status...
		status.SetDeviceStatus(TPositionModuleStatus::EDataQualityLoss);
		User::LeaveIfError(modStatus.SetModuleStatus(&status,sizeof(TPositionModuleStatus), TPositionModuleStatusEventBase::EEventDataQualityStatus));

		
		User::LeaveIfError(modStatus2.GetModuleStatus(&status, sizeof(TPositionModuleStatus),occurredEvents));
		if(occurredEvents!=TPositionModuleStatusEventBase::EEventDataQualityStatus)
			User::Leave(KErrGeneral);
		
		//Now check that a system module event returns not supported...
		if(modStatus.SetModuleStatus(&status, sizeof(TPositionModuleStatus), TPositionModuleStatusEventBase::EEventSystemModuleEvent)!=KErrNotSupported)
			User::Leave(KErrGeneral);

		//Now check the notifications...
		TRequestStatus stat;
		modStatus2.NotifyModuleStatusChange(stat);
		User::LeaveIfError(modStatus.SetModuleStatus(&status,sizeof(TPositionModuleStatus), TPositionModuleStatusEventBase::EEventDeviceStatus));
		User::WaitForRequest(stat);
		if(stat.Int()!=KErrNone)
			User::Leave(KErrGeneral);
		
		//now check the cancel...
		modStatus2.NotifyModuleStatusChange(stat);
		modStatus2.CancelNotifyModuleStatusChange();
		User::WaitForRequest(stat);
		if(stat.Int()!=KErrCancel)
			User::Leave(KErrGeneral);
		
		CleanupStack::PopAndDestroy(2, &modStatus);		
		
		SetTestStepResult(EPass);
		}
	  return TestStepResult();
	}



TVerdict ClbsinternalapiTest4Step::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("Please delete this line or modify it. I am in Test Step Postamble in Class ClbsinternalapiTest4Step"));
	// process something post setting to the test step
	// uncomment the following line if you have common post setting to all the test steps in there
	// CTe_lbsinternalapiTestSuiteStepBase::doTestStepPostambleL();
	// uncomment the following line if you have post process or remove the following line if no post process
	// SetTestStepResult(EPass);		// or EFail
	return TestStepResult();
	}
