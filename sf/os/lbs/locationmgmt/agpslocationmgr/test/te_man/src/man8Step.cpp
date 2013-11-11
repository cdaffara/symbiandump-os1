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
 @file man8Step.cpp
*/
#include "man8Step.h"
#include "Te_manSuiteDefs.h"
#include "LbsInternalInterface.h"
#include <lbs/lbslocdatasourcegpsbase.h>
#include <lbs/lbsextendedsatellite.h>

Cman8Step::~Cman8Step()
/**
 * Destructor
 */
	{
	}

Cman8Step::Cman8Step()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(Kman8Step);
	}

TVerdict Cman8Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("Please delete this line or modify it. I am in Test Step Preamble in Class Cman8Step"));
	// uncomment the following 3 lines if you have common pre setting to all the test steps in there
	// CTe_manSuiteStepBase::doTestStepPreambleL();
	// if (TestStepResult()!=EPass)
	//    return   TestStepResult();
	// process some pre setting to this test step then set SetTestStepResult to EFail or Epass.
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict Cman8Step::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	if (TestStepResult()==EPass)
		{
		INFO_PRINTF1(_L("Simulate both NRH and Loc Server sending location request"));
		INFO_PRINTF1(_L("There is no overlap. NRH first sends a request for now"));
		INFO_PRINTF1(_L("Then Loc Server does a request that starts and ends after the NRH request completes"));
		INFO_PRINTF1(_L("After the NRH request finishes then the Loc Server request should be issued"));

		User::After(10*1000000); // wait for any previous requests to finish (timeout)

		
		const RLbsPositionUpdateRequests::TChannelIdentifer KChannelIdentifierNRH = 
			{
				{KLbsGpsLocManagerUidValue},{KLbsNetRequestHandlerUidValue}
			};
		
		const RLbsPositionUpdateRequests::TChannelIdentifer KChannelIdentifierLocServer = 
			{
				{KLbsGpsLocManagerUidValue},{KLbsLocServerUidValue}
			};

		const TUint KLbsGpsLocManagerUidValue		= 0x10281D44;
		const TUid 	KLbsGpsLocManagerUid			= {KLbsGpsLocManagerUidValue};

		RLbsPositionUpdateRequests posUpdateReqLocSrv;
		posUpdateReqLocSrv.OpenL(KChannelIdentifierLocServer);
		CleanupClosePushL(posUpdateReqLocSrv);
		
		RLbsPositionUpdateRequests posUpdateReqNRH;
		posUpdateReqNRH.OpenL(KChannelIdentifierNRH);
		CleanupClosePushL(posUpdateReqNRH);

		RLbsPositionUpdates posUpdates;
		posUpdates.OpenL(KLbsGpsLocManagerUid);
		CleanupClosePushL(posUpdates);
		TRequestStatus stat;
		posUpdates.NotifyPositionUpdate(stat);
	
		// simulate what is done by NRH when it wants a location update
		TLbsPositionUpdateRequest locRequest;
		TLbsLocRequestQualityInt quality;
		quality.SetMaxFixTime(2000000);
		quality.SetMinHorizontalAccuracy(0.1);
		quality.SetMinVerticalAccuracy(0.1);
		TTime targetTime;
		targetTime.UniversalTime(); // target time is now and try for 5 seconds!
	
    	TTime NRHTargetTime = targetTime;
    	
    	locRequest.RequestQuality() = quality;
		locRequest.TargetTime() = NRHTargetTime;
		
		TLbsNetPosRequestMethodInt methods;
		TLbsNetPosMethodInt posMethods[1];
		posMethods[0].SetPosMethod(KLbsRootUid, (TPositionModuleInfo::ETechnologyTerminal|TPositionModuleInfo::ETechnologyAssisted));
		methods.SetPosMethods(posMethods, 1);
		locRequest.RequestMethod() = methods;
		
		INFO_PRINTF1(_L("NRH sends a location request to module, now for 2 secs"));
		User::LeaveIfError(posUpdateReqNRH.SetPositionUpdateRequest(locRequest));
	
		// consume what test module publishes (straight away) when it gets the request
		User::WaitForRequest(stat);
		if(stat.Int()!=KErrNone)
			User::Leave(KErrGeneral);
		
		posUpdates.NotifyPositionUpdate(stat);
		// simulate what is done by Loc Server when it wants a location update
		TLbsPositionUpdateRequest locRequestLS;
		locRequestLS.RequestMethod() = methods;
		TLbsLocRequestQualityInt qualityLS;
		qualityLS.SetMaxFixTime(4000000);
		qualityLS.SetMinHorizontalAccuracy(0.5);
		qualityLS.SetMinVerticalAccuracy(0.5);
    	locRequestLS.RequestQuality() = qualityLS;
		targetTime.UniversalTime();
		
		TTimeIntervalMicroSeconds threeSeconds = 3000000 ;
    	TTime LSTargetTime = targetTime + threeSeconds;
    
	
		locRequestLS.TargetTime() = LSTargetTime; 
		INFO_PRINTF1(_L("LS sends a location request to module, now+3 for 4 secs"));
		User::LeaveIfError(posUpdateReqLocSrv.SetPositionUpdateRequest(locRequestLS));

		User::WaitForRequest(stat);
		if(stat.Int()!=KErrNone)
			User::Leave(KErrGeneral);
		
		// the  integration module will ask for assistance data as a result of the network location request 
		WaitAndValidateNgMsg(TLbsNetInternalMsgBase::EAssistanceDataRequest);
		posUpdates.NotifyPositionUpdate(stat);
		User::After(3*1000000); 
		// after 3 seconds the second request should be launched ...
	    
	    // wait for test module to publish
	    
	  	User::WaitForRequest(stat);
	  	TInt 	error = 1; // something other than KErrNone
		targetTime = 0;
		TTime 	actualTime = 0;
		TPositionExtendedSatelliteInfo satInfo;
		TBool conflictControl;
		INFO_PRINTF1(_L("Manager correctly re-issued original request to module"));
		error = posUpdates.GetPositionInfo(conflictControl, &satInfo, sizeof(satInfo),targetTime, actualTime);

		if(stat.Int()!=KErrNone)
			User::Leave(KErrGeneral);
		
		error = 1; // something other than KErrNone
		targetTime = 0;
		actualTime = 0;
		
		error = posUpdates.GetPositionInfo(conflictControl, &satInfo, sizeof(satInfo),targetTime, actualTime);

		if(error!=KErrNone)
			User::Leave(KErrGeneral);
			
		if(targetTime!=locRequestLS.TargetTime())
			User::Leave(KErrGeneral);

		if(satInfo.SatelliteTime()!=locRequestLS.TargetTime())
			User::Leave(KErrGeneral);
		
		// the  integration module will ask for assistance data as a result of the network location request
		// and the manager will start a session 
		WaitAndValidateNgMsg(TLbsNetInternalMsgBase::ESelfLocationRequest);

		SendNgMsg(KSessionCompleteKErrNone);
	
		CleanupStack::PopAndDestroy(&posUpdates);

		CleanupStack::PopAndDestroy(&posUpdateReqNRH);
		CleanupStack::PopAndDestroy(&posUpdateReqLocSrv);
	

	 		//  **************   Block end ****************

		SetTestStepResult(EPass);
		}
	  return TestStepResult();
	}




TVerdict Cman8Step::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("Please delete this line or modify it. I am in Test Step Postamble in Class Cman8Step"));
	// process something post setting to the test step
	// uncomment the following line if you have common post setting to all the test steps in there
	// CTe_manSuiteStepBase::doTestStepPostambleL();
	// uncomment the following line if you have post process or remove the following line if no post process
	// SetTestStepResult(EPass);		// or EFail
	return TestStepResult();
	}
