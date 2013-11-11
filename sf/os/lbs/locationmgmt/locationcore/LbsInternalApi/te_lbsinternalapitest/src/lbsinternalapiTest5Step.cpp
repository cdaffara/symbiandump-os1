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
 @file lbsinternalapiTest5Step.cpp
*/
#include "lbsinternalapitest5step.h"
#include "te_lbsinternalapiTestsuitedefs.h"

#include "LbsInternalInterface.h"

ClbsinternalapiTest5Step::~ClbsinternalapiTest5Step()
/**
 * Destructor
 */
	{
	}

ClbsinternalapiTest5Step::ClbsinternalapiTest5Step()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KlbsinternalapiTest5Step);
	}

TVerdict ClbsinternalapiTest5Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("Please delete this line or modify it. I am in Test Step Preamble in Class ClbsinternalapiTest5Step"));
	// uncomment the following 3 lines if you have common pre setting to all the test steps in there
	// CTe_lbsinternalapiTestSuiteStepBase::doTestStepPreambleL();
	// if (TestStepResult()!=EPass)
	//    return   TestStepResult();
	// process some pre setting to this test step then set SetTestStepResult to EFail or Epass.
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict ClbsinternalapiTest5Step::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	  if (TestStepResult()==EPass)
		{
		RLbsSystemModuleEvent::InitializeL();
		
		RLbsSystemModuleEvent sysModEvent;
		sysModEvent.OpenL();
		CleanupClosePushL(sysModEvent);
		CleanupStack::PopAndDestroy(&sysModEvent);
		
		sysModEvent.OpenL();
		CleanupClosePushL(sysModEvent);
		
		//Check default value is ESystemUnknown...
		TPositionModuleStatusEventBase::TSystemModuleEvent event = 
					TPositionModuleStatusEventBase::ESystemModuleRemoved;
		User::LeaveIfError(sysModEvent.GetSystemModuleEvent(event));
		if(event!=TPositionModuleStatusEventBase::ESystemUnknown)
			User::Leave(KErrGeneral);
		
		//Now check Set works...
		User::LeaveIfError(sysModEvent.SetSystemModuleEvent(TPositionModuleStatusEventBase::ESystemModuleInstalled));
		User::LeaveIfError(sysModEvent.GetSystemModuleEvent(event));
		if(event!=TPositionModuleStatusEventBase::ESystemModuleInstalled)
			User::Leave(KErrGeneral);
		
		//Now check notifications...
		//Use another handle
		RLbsSystemModuleEvent sysModEvent2;
		sysModEvent2.OpenL();
		CleanupClosePushL(sysModEvent2);
		
		TRequestStatus stat;
		sysModEvent2.NotifySystemModuleEvent(stat);
		User::LeaveIfError(sysModEvent.SetSystemModuleEvent(TPositionModuleStatusEventBase::ESystemModuleInstalled));
		User::WaitForRequest(stat);
		if(stat.Int()!=KErrNone)
			User::Leave(KErrGeneral);
		
		//Now check cancel...
		sysModEvent2.NotifySystemModuleEvent(stat);
		sysModEvent2.CancelNotifySystemModuleEvent();
		User::WaitForRequest(stat);
		if(stat.Int()!=KErrCancel)
			User::Leave(KErrGeneral);
		
		CleanupStack::PopAndDestroy(2, &sysModEvent);
		
		SetTestStepResult(EPass);
		}
	  return TestStepResult();
	}



TVerdict ClbsinternalapiTest5Step::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("Please delete this line or modify it. I am in Test Step Postamble in Class ClbsinternalapiTest5Step"));
	// process something post setting to the test step
	// uncomment the following line if you have common post setting to all the test steps in there
	// CTe_lbsinternalapiTestSuiteStepBase::doTestStepPostambleL();
	// uncomment the following line if you have post process or remove the following line if no post process
	// SetTestStepResult(EPass);		// or EFail
	return TestStepResult();
	}
