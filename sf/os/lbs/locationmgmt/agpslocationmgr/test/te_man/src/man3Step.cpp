// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @file man3Step.cpp
*/
#include "man3Step.h"
#include "Te_manSuiteDefs.h"
#include "LbsInternalInterface.h"

Cman3Step::~Cman3Step()
/**
 * Destructor
 */
	{
	}

Cman3Step::Cman3Step()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(Kman3Step);
	}

TVerdict Cman3Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	//INFO_PRINTF1(_L("Please delete this line or modify it. I am in Test Step Preamble in Class Cman3Step"));
	// uncomment the following 3 lines if you have common pre setting to all the test steps in there
	// CTe_manSuiteStepBase::doTestStepPreambleL();
	// if (TestStepResult()!=EPass)
	//    return   TestStepResult();
	// process some pre setting to this test step then set SetTestStepResult to EFail or Epass.
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict Cman3Step::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	  if (TestStepResult()==EPass)
		{
		
		// Start test version of AGPS manager and use a test version if integration module
		// The test version of AGPS manager is same as actual but does not use the
		// ECOM plugin interface.

 		// Now test power mode advice when sent within a LocRequest command
 
 		TLbsPositionUpdateRequestStatus request;
 		request.SetPowerAdvice(TLbsPositionUpdateRequestBase::EPowerAdviceOff);
 		RLbsPositionUpdateRequests posReqs;
 		posReqs.OpenL(KChannelArray[0]);
 		CleanupClosePushL(posReqs);

	    RLbsModuleStatus modStatus;
		modStatus.OpenL(KLbsGpsLocManagerUid);
		CleanupClosePushL(modStatus);
		TRequestStatus stat;
		modStatus.NotifyModuleStatusChange(stat);
		
 		User::LeaveIfError(posReqs.SetPositionUpdateRequest(request));

		User::WaitForRequest(stat);
		if(stat.Int()!=KErrNone) User::Leave(KErrGeneral);

		TPositionModuleStatus status;
		TPositionModuleStatusEventBase::TModuleEvent occurredEvents =
				TPositionModuleStatusEventBase::EEventNone;
		User::LeaveIfError(modStatus.GetModuleStatus(&status, sizeof(TPositionModuleStatus), occurredEvents));

 		if(status.DeviceStatus() != TPositionModuleStatus::EDeviceInactive)
			{
			User::Leave(KErrNotFound);
			}
 		INFO_PRINTF1(_L("Test integration module correctly responds with EDeviceStandBy"));
	
		SendNgMsg(KSessionCompleteKErrNone);
		CleanupStack::PopAndDestroy(&modStatus);
		CleanupStack::PopAndDestroy(&posReqs);
	 	
	 		//  **************   Block end ****************

		SetTestStepResult(EPass);
		}
	  return TestStepResult();
	}




TVerdict Cman3Step::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	//INFO_PRINTF1(_L("Please delete this line or modify it. I am in Test Step Postamble in Class Cman3Step"));
	// process something post setting to the test step
	// uncomment the following line if you have common post setting to all the test steps in there
	// CTe_manSuiteStepBase::doTestStepPostambleL();
	// uncomment the following line if you have post process or remove the following line if no post process
	// SetTestStepResult(EPass);		// or EFail
	return TestStepResult();
	}
