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
 @file man1Step.cpp
*/
#include "man1Step.h"
#include "Te_manSuiteDefs.h"
#include "LbsInternalInterface.h"



Cman1Step::~Cman1Step()
/**
 * Destructor
 */
	{
	}

Cman1Step::Cman1Step()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(Kman1Step);
	}

TVerdict Cman1Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	//INFO_PRINTF1(_L("Please delete this line or modify it. I am in Test Step Preamble in Class Cman1Step"));
	// uncomment the following 3 lines if you have common pre setting to all the test steps in there
	// CTe_manSuiteStepBase::doTestStepPreambleL();
	// if (TestStepResult()!=EPass)
	//    return   TestStepResult();
	// process some pre setting to this test step then set SetTestStepResult to EFail or Epass.
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict Cman1Step::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	
 	if (TestStepResult()==EPass)
		{
		
		INFO_PRINTF1(_L("Simulate location server sending a location update cancel request"));
		INFO_PRINTF1(_L("Check that the test integration module receives the cancel and responds accordingly"));
			
		const RLbsPositionUpdateRequests::TChannelIdentifer KChannelIdentifier = 
			{
				{KLbsGpsLocManagerUidValue},{KLbsLocServerUidValue}
			};
		
		const TUint KLbsGpsLocManagerUidValue		= 0x10281D44;
		const TUid 	KLbsGpsLocManagerUid			= {KLbsGpsLocManagerUidValue};

		RLbsPositionUpdateRequests posUpdateReq;
		posUpdateReq.OpenL(KChannelIdentifier);
		CleanupClosePushL(posUpdateReq);
		
		RLbsModuleStatus modStatus;
		modStatus.OpenL(KLbsGpsLocManagerUid);
		CleanupClosePushL(modStatus);
		TRequestStatus stat;
		modStatus.NotifyModuleStatusChange(stat);
		
		// first send a status, in case a cancel was sent before since the AGPS manager doesn't handle 
		// more than one cancel if cancels follow one after the other
		TLbsPositionUpdateRequestStatus initialStatus;
		initialStatus.SetPowerAdvice(TLbsPositionUpdateRequestBase::EPowerAdviceOn);
		User::LeaveIfError(posUpdateReq.SetPositionUpdateRequest(initialStatus));
		User::WaitForRequest(stat);
		modStatus.NotifyModuleStatusChange(stat);

		// simulate what is done by Location Server when it wants to cancel
		TLbsPositionUpdateRequestCancel cancel;
		cancel.SetPowerAdvice(TLbsPositionUpdateRequestBase::EPowerAdviceStandby);
		User::LeaveIfError(posUpdateReq.SetPositionUpdateRequest(cancel));
			
		INFO_PRINTF1(_L("Send cancel location request to integration module"));

	    // the test version of the integration module is hardwired to now send
	    // a device status change EDeviceStandBy - as confirmation of getting the cancel!
		User::WaitForRequest(stat);
		if(stat.Int()!=KErrNone) User::Leave(KErrGeneral);

		TPositionModuleStatus status;
		TPositionModuleStatusEventBase::TModuleEvent occurredEvents =
				TPositionModuleStatusEventBase::EEventNone;
		User::LeaveIfError(modStatus.GetModuleStatus(&status, sizeof(status),occurredEvents));

		if(status.DeviceStatus() != TPositionModuleStatus::EDeviceStandBy)
			{
			User::Leave(KErrNotFound);
			}
		INFO_PRINTF1(_L("Test integration module correctly responds with EDeviceStandBy"));
	
		SendNgMsg(KSessionCompleteKErrNone);

		CleanupStack::PopAndDestroy(&modStatus);
		CleanupStack::PopAndDestroy(&posUpdateReq);
	

	 		//  **************   Block end ****************

		SetTestStepResult(EPass);
		}
	  return TestStepResult();
	}




TVerdict Cman1Step::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	//INFO_PRINTF1(_L("Please delete this line or modify it. I am in Test Step Postamble in Class Cman1Step"));
	// process something post setting to the test step
	// uncomment the following line if you have common post setting to all the test steps in there
	// CTe_manSuiteStepBase::doTestStepPostambleL();
	// uncomment the following line if you have post process or remove the following line if no post process
	// SetTestStepResult(EPass);		// or EFail
	return TestStepResult();
	}
