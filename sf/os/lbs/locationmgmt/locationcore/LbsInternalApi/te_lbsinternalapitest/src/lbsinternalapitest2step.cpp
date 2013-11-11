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
#include "lbsinternalapitest2step.h"
#include "te_lbsinternalapiTestsuitedefs.h"

#include "LbsInternalInterface.h"

ClbsinternalapiTest2Step::~ClbsinternalapiTest2Step()
/**
 * Destructor
 */
	{
	}

ClbsinternalapiTest2Step::ClbsinternalapiTest2Step()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KlbsinternalapiTest2Step);
	}

TVerdict ClbsinternalapiTest2Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("Please delete this line or modify it. I am in Test Step Preamble in Class ClbsinternalapiTest2Step"));
	// uncomment the following 3 lines if you have common pre setting to all the test steps in there
	// CTe_lbsinternalapiTestSuiteStepBase::doTestStepPreambleL();
	// if (TestStepResult()!=EPass)
	//    return   TestStepResult();
	// process some pre setting to this test step then set SetTestStepResult to EFail or Epass.
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict ClbsinternalapiTest2Step::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	  if (TestStepResult()==EPass)
		{			
		RLbsPositionUpdateRequests::InitializeL();
		
		//OK, open a RLbsPositionUpdateRequests, close it, then re-open. This checks for leaked
		//handles
		
		const RLbsPositionUpdateRequests::TChannelIdentifer KChannelIdentifier = 
			{
				{KLbsGpsLocManagerUidValue},{KLbsLocServerUidValue}
			};
		
		RLbsPositionUpdateRequests posUpdateReq;
		posUpdateReq.OpenL(KChannelIdentifier);
		CleanupClosePushL(posUpdateReq);
		CleanupStack::PopAndDestroy(&posUpdateReq);
		
		posUpdateReq.OpenL(KChannelIdentifier);
		CleanupClosePushL(posUpdateReq);
		
		
		//Now check the default value - it should be of KUnknown type.
		//In order to do this we need to put a value other than EUnknown into the the iType.
		//The code below does this with a hack.
		TLbsPositionUpdateRequestBase updateRequest = TLbsPositionUpdateRequestCancel();
				
		User::LeaveIfError(posUpdateReq.GetPositionUpdateRequest(updateRequest));
		if(updateRequest.Type()!=TLbsPositionUpdateRequestBase::EUnknown)
			{
			INFO_PRINTF2(_L("KERRGeneral A: request type = %d should have been EUnknown"),updateRequest.Type());
			User::Leave(KErrGeneral);
			}
		
		//Now do a set and check it works.
		User::LeaveIfError(posUpdateReq.SetPositionUpdateRequest(TLbsPositionUpdateRequestCancel()));
		User::LeaveIfError(posUpdateReq.GetPositionUpdateRequest(updateRequest));
		if(updateRequest.Type()!=TLbsPositionUpdateRequestBase::ECancel)
			{
			INFO_PRINTF2(_L("KERRGeneral B: request type = %d should have been ECancel"),updateRequest.Type());
			User::Leave(KErrGeneral);
			}
		
		//Now we need to check notifications.
		//To be ultra sure everything's OK, we'll use another handle for the notification.
		RLbsPositionUpdateRequests posUpdateReq2;
		posUpdateReq2.OpenL(KChannelIdentifier);
		CleanupClosePushL(posUpdateReq);
		
		TRequestStatus stat;
		posUpdateReq2.NotifyPositionUpdateRequestChange(stat);
		
		User::LeaveIfError(posUpdateReq.SetPositionUpdateRequest(TLbsPositionUpdateRequestCancel()));
		User::WaitForRequest(stat);
		if(stat.Int()!=KErrNone)
			{
			INFO_PRINTF2(_L("KERRGeneral C: stat.Int() = %d should have been KErrNone"),stat.Int());
			User::Leave(KErrGeneral);
			}
		//Next test that cancel works OK
		posUpdateReq2.NotifyPositionUpdateRequestChange(stat);
		posUpdateReq2.CancelNotifyPositionUpdateRequestChange();
		User::WaitForRequest(stat);
		
		if(stat.Int()!=KErrCancel)
			{
			INFO_PRINTF2(_L("KERRGeneral D: stat.Int() = %d should have been KErrCancel"),stat.Int());
			User::Leave(KErrCancel);
			}
		
		CleanupStack::PopAndDestroy(2, &posUpdateReq);
		
		SetTestStepResult(EPass);
		}
	  return TestStepResult();
	}



TVerdict ClbsinternalapiTest2Step::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("Please delete this line or modify it. I am in Test Step Postamble in Class ClbsinternalapiTest2Step"));
	// process something post setting to the test step
	// uncomment the following line if you have common post setting to all the test steps in there
	// CTe_lbsinternalapiTestSuiteStepBase::doTestStepPostambleL();
	// uncomment the following line if you have post process or remove the following line if no post process
	// SetTestStepResult(EPass);		// or EFail
	return TestStepResult();
	}
