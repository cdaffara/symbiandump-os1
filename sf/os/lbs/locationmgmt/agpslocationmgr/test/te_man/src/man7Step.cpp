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
 @file man7Step.cpp
*/
#include "man7Step.h"
#include "Te_manSuiteDefs.h"
#include "LbsInternalInterface.h"
#include <lbs/lbslocdatasourcegpsbase.h>
#include <lbs/lbsextendedsatellite.h>

Cman7Step::~Cman7Step()
/**
 * Destructor
 */
	{
	}

Cman7Step::Cman7Step()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(Kman7Step);
	}

TVerdict Cman7Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("Please delete this line or modify it. I am in Test Step Preamble in Class Cman7Step"));
	// uncomment the following 3 lines if you have common pre setting to all the test steps in there
	// CTe_manSuiteStepBase::doTestStepPreambleL();
	// if (TestStepResult()!=EPass)
	//    return   TestStepResult();
	// process some pre setting to this test step then set SetTestStepResult to EFail or Epass.
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict Cman7Step::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	if (TestStepResult()==EPass)
		{
		

		INFO_PRINTF1(_L("Simulate NRH sending location update request"));
		INFO_PRINTF1(_L("Note that test version of integration module provides a test location"));
		INFO_PRINTF1(_L("Check that location published by manager is correct"));
		User::After(10*1000000); // wait for any previous requests to finish (timeout)
	
		const RLbsPositionUpdateRequests::TChannelIdentifer KChannelIdentifierNRH = 
			{
				{KLbsGpsLocManagerUidValue},{KLbsNetRequestHandlerUidValue}
			};

		const TUint KLbsGpsLocManagerUidValue		= 0x10281D44;
		const TUid 	KLbsGpsLocManagerUid			= {KLbsGpsLocManagerUidValue};

		RLbsPositionUpdateRequests posUpdateReq;
		posUpdateReq.OpenL(KChannelIdentifierNRH);
		CleanupClosePushL(posUpdateReq);
		
		// simulate what is done by NRH when it wants a location update
		TLbsPositionUpdateRequest locRequest;
		TLbsLocRequestQualityInt quality;
				
		quality.SetMaxFixTime(/*TTimeIntervalMicroSeconds*/ 5000000);
		quality.SetMinHorizontalAccuracy(/* units? TReal32*/ 0.1);
		quality.SetMinVerticalAccuracy(0.1);
		
		TTime targetTime;
		targetTime.UniversalTime(); // target time is now and try for 5 seconds!
    
		locRequest.RequestQuality() = quality;
		locRequest.TargetTime() = targetTime;

		RLbsPositionUpdates posUpdates;
		posUpdates.OpenL(KLbsGpsLocManagerUid);
		CleanupClosePushL(posUpdates);
		TRequestStatus stat;
		posUpdates.NotifyPositionUpdate(stat);

		// Make sure the request is a Terminal Based one
		TLbsNetPosRequestMethodInt methods;
		TLbsNetPosMethodInt posMethods[1];
		posMethods[0].SetPosMethod(KLbsRootUid, (TPositionModuleInfo::ETechnologyTerminal|TPositionModuleInfo::ETechnologyAssisted));
		methods.SetPosMethods(posMethods, 1);
		locRequest.RequestMethod() = methods;

		INFO_PRINTF1(_L("Send location request to integration module"));
		User::LeaveIfError(posUpdateReq.SetPositionUpdateRequest(locRequest));

	    // the test version of the integration module is hardwired to now send
	    // a test TPositionExtendedSatelliteInfo!

		User::WaitForRequest(stat);
		
		if(stat.Int()!=KErrNone)
			User::Leave(KErrGeneral);
		
		TInt 	error = 1; // something other than KErrNone
		targetTime = 0;
		TTime 	actualTime = 0;
		TPositionExtendedSatelliteInfo satInfo;
		TBool conflictControl;
		
		error = posUpdates.GetPositionInfo(conflictControl, &satInfo, sizeof(satInfo),targetTime, actualTime);

		if(error!=KErrNone)
			User::Leave(KErrGeneral);
			
		if(conflictControl!=EFalse)
			User::Leave(KErrGeneral);

		if(targetTime!=locRequest.TargetTime())
			User::Leave(KErrGeneral);

		if(satInfo.SatelliteTime()!=locRequest.TargetTime())
			User::Leave(KErrGeneral);

		User::After(5*1000000);
		
		CleanupStack::PopAndDestroy(&posUpdates);

		WaitAndValidateNgMsg(TLbsNetInternalMsgBase::EAssistanceDataRequest);
	
		SendNgMsg(KSessionCompleteKErrNone);
		INFO_PRINTF1(_L("Test integration module correctly responds with location"));

		CleanupStack::PopAndDestroy(&posUpdateReq);
	

	 		//  **************   Block end ****************

		SetTestStepResult(EPass);
		}
	  return TestStepResult();
	}




TVerdict Cman7Step::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("Please delete this line or modify it. I am in Test Step Postamble in Class Cman7Step"));
	// process something post setting to the test step
	// uncomment the following line if you have common post setting to all the test steps in there
	// CTe_manSuiteStepBase::doTestStepPostambleL();
	// uncomment the following line if you have post process or remove the following line if no post process
	// SetTestStepResult(EPass);		// or EFail
	return TestStepResult();
	}
