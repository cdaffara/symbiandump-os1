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
 @file man10Step.cpp
*/
#include "man10Step.h"
#include "Te_manSuiteDefs.h"
#include "LbsInternalInterface.h"
#include <lbs/lbslocdatasourcegpsbase.h>
#include <lbs/lbsextendedsatellite.h>

Cman10Step::~Cman10Step()
/**
 * Destructor
 */
	{
	}

Cman10Step::Cman10Step()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(Kman10Step);
	}

TVerdict Cman10Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict Cman10Step::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	if (TestStepResult()==EPass)
		{
		INFO_PRINTF1(_L("Simulate both NRH and Loc Server sending location requests"));
		INFO_PRINTF1(_L("There is no overlap. Loc Server first sends a request in the future"));
		INFO_PRINTF1(_L("Then NRH does a request that starts and ends before the original"));
		INFO_PRINTF1(_L("After the NRH request finsihed then the Loc Server request should be re-issued"));
		
		
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
		quality.SetMinHorizontalAccuracy(100);
		quality.SetMinVerticalAccuracy(100);
		TTime targetTime;
		targetTime.UniversalTime(); // target time is now+10 and try for 5 seconds!
		TTimeIntervalMicroSeconds tenSeconds = 10000000 ;
    	TTime LSTargetTime = targetTime + tenSeconds;
    	
    	TLbsNetPosRequestMethodInt methods;
		TLbsNetPosMethodInt posMethods[1];
		posMethods[0].SetPosMethod(KLbsRootUid, (TPositionModuleInfo::ETechnologyTerminal|TPositionModuleInfo::ETechnologyAssisted));
		methods.SetPosMethods(posMethods, 1);
    	
    	locRequest.RequestQuality() = quality;
		locRequest.TargetTime() = LSTargetTime;
		locRequest.RequestMethod() = methods;
		INFO_PRINTF1(_L("Loc server sends a location request to module, now+10 for 5 secs"));
		User::LeaveIfError(posUpdateReqLocSrv.SetPositionUpdateRequest(locRequest));
		
		// The location request sent above gets to the test GPS Integration Module, which
		// in turn requests assistance data and that results in a SelfLocationRequest sent to the NG
		// Wait for advice and Self Loc Request. Send error.
		WaitAndValidateNgMsg(TLbsNetInternalMsgBase::ESelfLocationRequest);	

		// Simulate that the NRH makes a location request
		TLbsPositionUpdateRequest locRequestNRH;
		TLbsLocRequestQualityInt qualityNRH;
		qualityNRH.SetMaxFixTime(4000000);
		qualityNRH.SetMinHorizontalAccuracy(75);
		qualityNRH.SetMinVerticalAccuracy(75);
    	locRequestNRH.RequestQuality() = qualityNRH;
		targetTime.UniversalTime();
		locRequestNRH.TargetTime() = targetTime; // now
		locRequestNRH.RequestMethod() = methods;
		INFO_PRINTF1(_L("NRH sends a location request to module, now for 4 secs"));
		User::LeaveIfError(posUpdateReqNRH.SetPositionUpdateRequest(locRequestNRH));

		// Consume the position update that test module publishes inmediately after 
		// receiving the new request combined with the NRH request
		User::WaitForRequest(stat);
		if(stat.Int()!=KErrNone)
			User::Leave(KErrGeneral);

	    // Wait for the integration module to publish a location update
	    // due to the LS request
		posUpdates.NotifyPositionUpdate(stat);	
	  	User::WaitForRequest(stat);
	  	TInt 	error = 1; // something other than KErrNone
		targetTime = 0;
		TTime 	actualTime = 0;
		TPositionExtendedSatelliteInfo satInfo2;
		TBool conflictControl;
		INFO_PRINTF1(_L("Manager correctly re-issued original request to module"));
		error = posUpdates.GetPositionInfo(conflictControl, &satInfo2, sizeof(satInfo2),targetTime, actualTime);

		if(stat.Int()!=KErrNone)
			User::Leave(KErrGeneral);
		
		error = 1; // something other than KErrNone
		targetTime = 0;
		actualTime = 0;
		
		error = posUpdates.GetPositionInfo(conflictControl, &satInfo2, sizeof(satInfo2),targetTime, actualTime);

		if(error!=KErrNone)
			User::Leave(KErrGeneral);
			
		if(targetTime!=locRequest.TargetTime())
			User::Leave(KErrGeneral);
		
		TTime satTime = satInfo2.SatelliteTime();
		
		if(satTime!=locRequest.TargetTime())
			User::Leave(KErrGeneral);
	
	    // Check that there was an assistance data request sent after
	    // the NRH request and a new request was sent to the integration
	    // module
		WaitAndValidateNgMsg(TLbsNetInternalMsgBase::EAssistanceDataRequest);
		SendNgMsg(KSessionCompleteKErrNone);
	
		CleanupStack::PopAndDestroy(&posUpdates);

		CleanupStack::PopAndDestroy(&posUpdateReqNRH);
		CleanupStack::PopAndDestroy(&posUpdateReqLocSrv);
	

	 		//  **************   Block end ****************

		SetTestStepResult(EPass);
		}
	  return TestStepResult();
	}



TVerdict Cman10Step::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	return TestStepResult();
	}
