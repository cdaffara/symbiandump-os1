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
 @file man11Step.cpp
*/
#include "man11Step.h"
#include "Te_manSuiteDefs.h"
#include "LbsInternalInterface.h"
#include <lbs/lbslocdatasourcegpsbase.h>
#include <lbs/lbsextendedsatellite.h>


Cman11Step::~Cman11Step()
/**
 * Destructor
 */
	{
	}

Cman11Step::Cman11Step()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(Kman11Step);
	}

TVerdict Cman11Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("Please delete this line or modify it. I am in Test Step Preamble in Class Cman11Step"));
	// uncomment the following 3 lines if you have common pre setting to all the test steps in there
	// CTe_manSuiteStepBase::doTestStepPreambleL();
	// if (TestStepResult()!=EPass)
	//    return   TestStepResult();
	// process some pre setting to this test step then set SetTestStepResult to EFail or Epass.
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict Cman11Step::doTestStepL()
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
		INFO_PRINTF1(_L("The requests overlap."));
		INFO_PRINTF1(_L("Location server sends location update request for NOW and MaxFixTime=5s acc=0.5"));
		INFO_PRINTF1(_L("NRH sends location update request for NOW and  MaxFixTime=8s acc=0.1"));
		INFO_PRINTF1(_L("Check that int module gets a request for MaxFixTime 8s with acc=0.1"));
			
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
		
		// simulate what is done by Location Server when it wants a location update
		TLbsPositionUpdateRequest locRequest;
		TLbsLocRequestQualityInt quality;
		quality.SetMaxFixTime(5000000);
		quality.SetMinHorizontalAccuracy(0.1);
		quality.SetMinVerticalAccuracy(0.1);
		TTime targetTime;
		targetTime.UniversalTime(); // target time is now and try for 5 seconds!
    	locRequest.RequestQuality() = quality;
		locRequest.TargetTime() = targetTime;
		
		TLbsNetPosRequestMethodInt methods;
		TLbsNetPosMethodInt posMethods[1];
		posMethods[0].SetPosMethod(KLbsRootUid, (TPositionModuleInfo::ETechnologyTerminal|TPositionModuleInfo::ETechnologyAssisted));
		methods.SetPosMethods(posMethods, 1);
		locRequest.RequestMethod() = methods;
		
		INFO_PRINTF1(_L("Loc server sends a location request to integration module"));
		User::LeaveIfError(posUpdateReqLocSrv.SetPositionUpdateRequest(locRequest));
		
		// consume what test module publishes (straight away) when it gets the request
		User::WaitForRequest(stat);
		if(stat.Int()!=KErrNone)
			User::Leave(KErrGeneral);
		
		posUpdates.NotifyPositionUpdate(stat);
	
		TInt 	error = 1; // something other than KErrNone
		TTime readTargetTime = 0;
		TTime 	actualTime = 0;
		TPositionExtendedSatelliteInfo satInfo;
		TBool conflictControl;
		
		error = posUpdates.GetPositionInfo(conflictControl, &satInfo, sizeof(satInfo),readTargetTime, actualTime);

		if(error!=KErrNone)
			User::Leave(KErrGeneral);
			
		if(targetTime!=locRequest.TargetTime())
			User::Leave(KErrGeneral);

		if(satInfo.SatelliteTime()!=locRequest.TargetTime())
			User::Leave(KErrGeneral);

		
		// simulate what is done by NRHr when it wants a location update
		TLbsPositionUpdateRequest locRequestNRH;
		TLbsLocRequestQualityInt qualityNRH;
		qualityNRH.SetMaxFixTime(8000000);
		qualityNRH.SetMinHorizontalAccuracy(0.5);
		qualityNRH.SetMinVerticalAccuracy(0.5);

    	locRequestNRH.RequestQuality() = qualityNRH;
    	locRequestNRH.RequestMethod() = methods;
    	
   	 	TTimeIntervalMicroSeconds oneSecond = 1000000 ;
    	TTime secondTargetTime = targetTime + oneSecond;
    	
		locRequestNRH.TargetTime() = secondTargetTime; // previous plus one sec
		INFO_PRINTF1(_L("NRH sends a location request to integration module"));
		User::LeaveIfError(posUpdateReqNRH.SetPositionUpdateRequest(locRequestNRH));

	    // the test version of the integration module is hardwired to now send
	    // a test TPositionExtendedSatelliteInfo!
		// check that two requests are correctly combined
		User::WaitForRequest(stat);
		
		if(stat.Int()!=KErrNone)
			User::Leave(KErrGeneral);
		
		error = 1; // something other than KErrNone
		targetTime = 0;
		actualTime = 0;
				
		error = posUpdates.GetPositionInfo(conflictControl, &satInfo, sizeof(satInfo),targetTime, actualTime);

		if(error!=KErrNone)
			User::Leave(KErrGeneral);
			
		if(targetTime!=locRequestNRH.TargetTime())
			User::Leave(KErrGeneral);

		if(satInfo.SatelliteTime()!=locRequestNRH.TargetTime())
			User::Leave(KErrGeneral);

		
		WaitAndValidateNgMsg(TLbsNetInternalMsgBase::ESelfLocationRequest);
		WaitAndValidateNgMsg(TLbsNetInternalMsgBase::EAssistanceDataRequest);
			
		CleanupStack::PopAndDestroy(&posUpdates);

		INFO_PRINTF1(_L("Test integration module correctly responds with location"));

		SendNgMsg(KSessionCompleteKErrNone);

		CleanupStack::PopAndDestroy(&posUpdateReqNRH);
		CleanupStack::PopAndDestroy(&posUpdateReqLocSrv);
	

	 		//  **************   Block end ****************

		SetTestStepResult(EPass);
		}
	  return TestStepResult();
	}




TVerdict Cman11Step::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	INFO_PRINTF1(_L("Please delete this line or modify it. I am in Test Step Postamble in Class Cman11Step"));
	// process something post setting to the test step
	// uncomment the following line if you have common post setting to all the test steps in there
	// CTe_manSuiteStepBase::doTestStepPostambleL();
	// uncomment the following line if you have post process or remove the following line if no post process
	// SetTestStepResult(EPass);		// or EFail
	return TestStepResult();
	}
