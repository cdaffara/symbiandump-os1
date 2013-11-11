// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// AGPS Manager test 14
// 
//

/**
 @file man14Step.cpp
*/
 
#include "man14step.h"
#include "Te_manSuiteDefs.h"
#include "LbsInternalInterface.h"
#include <lbs/lbslocdatasourcegpsbase.h>
#include <lbs/lbsextendedsatellite.h>

Cman14Step::~Cman14Step()
	{
	}

Cman14Step::Cman14Step()
	{
	SetTestStepName(Kman14Step);
	}

TVerdict Cman14Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}

// Simulate location server sending early completion update
// The manager must publish the received position for the NRH
// Check that location published by manager is correct
TVerdict Cman14Step::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 */
	{
	if (TestStepResult()==EPass)
		{
		
		const RLbsPositionUpdateRequests::TChannelIdentifer KChannelIdentifierLocServer = 
			{
				{KLbsGpsLocManagerUidValue},{KLbsLocServerUidValue}
			};
		const RLbsPositionUpdateRequests::TChannelIdentifer KChannelIdentifierNRH = 
			{
				{KLbsGpsLocManagerUidValue},{KLbsNetRequestHandlerUidValue}
		    };
		    
		    
		// Cancel requests from LocServer and NRH so there is no effect from previous tests
		
	    RLbsPositionUpdateRequests posUpdateReqLocSrv;
		posUpdateReqLocSrv.OpenL(KChannelIdentifierLocServer);
		CleanupClosePushL(posUpdateReqLocSrv);
		
		RLbsPositionUpdateRequests posUpdateReqNRH;
		posUpdateReqNRH.OpenL(KChannelIdentifierNRH);
		CleanupClosePushL(posUpdateReqNRH);
		
		// Simulate the NRH subscribing to the Position Update Bus
		RLbsPositionUpdates posUpdateNRH;
		posUpdateNRH.OpenL(KLbsGpsLocManagerUid);
		CleanupClosePushL(posUpdateNRH);
		
		// Simulate the Loc Server opening the early completion updates bus
		RLbsPositionUpdates earlyCompletionPosUpdates;
		earlyCompletionPosUpdates.OpenL(KLbsLocServerUid);
		CleanupClosePushL(earlyCompletionPosUpdates);
				
		// Simulate the Location Server sending a location update request
		//
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

		User::After(1000000);
		
		// Build a location update with some non-default data in it
		// and with KPositionEarlyComplete as error code.
		TPositionExtendedSatelliteInfo satInfo;
		satInfo.SetSatelliteTime(TTime(500));
		TInt error=KPositionEarlyComplete;
		targetTime = 100;
		TTime actualTime(200);
		TBool conflictControl=EFalse;

		INFO_PRINTF1(_L("LocServer Early Completion Update -> AGPS Manager"));
		//Simulate the Location Server publishing the above position
		earlyCompletionPosUpdates.SetPositionInfo(error, conflictControl, &satInfo, sizeof(satInfo), targetTime, actualTime);

		// Wait a bit...
		INFO_PRINTF1(_L("AGPS Manager Early Completion Update -> GPS Location Bus"));
		User::After(2000000);
		
		// ...and expect the position to show up on the Position Update Bus
		TPositionExtendedSatelliteInfo satInfoNrh;
		INFO_PRINTF1(_L("NRH retrieves update <- GPS Location Bus"));
		error = posUpdateNRH.GetPositionInfo(conflictControl, &satInfoNrh, sizeof(satInfoNrh),targetTime, actualTime);

		// Check that the error code is no other than KPositionEarlyComplete
		if(error != KPositionEarlyComplete)
			{
			INFO_PRINTF1(_L("Unexpected error code"));
			User::Leave(KErrGeneral);
			}
		INFO_PRINTF1(_L("Completion code is KPositionEarlyComplete as expected"));

			
		// Check that the satellite data received by NRH is the same as published by LocServer		
		if(satInfoNrh.SatelliteTime() != satInfo.SatelliteTime())
			{
			INFO_PRINTF1(_L("Unexpected satellite data"));
			User::Leave(KErrGeneral);
			}
		INFO_PRINTF1(_L("Location Update received by NRH is the same as the one published by LocServer"));
	
	
		// Cancel requests so there is no effect on later tests
	//	User::LeaveIfError(posUpdateReqLocSrv.SetPositionUpdateRequest(cancel));
	//	User::LeaveIfError(posUpdateReqNRH.SetPositionUpdateRequest(cancel));
	//	User::After(1000000); // Allow one second for the cancel to take effect	
		
	//	WaitAndValidateNgMsg(TLbsNetInternalMsgBase::EAssistanceDataRequest);
	//	WaitAndValidateNgMsg(TLbsNetInternalMsgBase::ESelfLocationCancel);

		const TLbsNetSessionIdInt KDummySessionId;
		const TLbsNetSessionCompleteAgpsMsg KSessionCompleteKErrNone(KDummySessionId, KErrNone);
		SendNgMsg(KSessionCompleteKErrNone);

		CleanupStack::PopAndDestroy(&earlyCompletionPosUpdates);
		CleanupStack::PopAndDestroy(&posUpdateNRH);
	    CleanupStack::PopAndDestroy(&posUpdateReqNRH);
		CleanupStack::PopAndDestroy(&posUpdateReqLocSrv);
	
    	SetTestStepResult(EPass);
		}
	  return TestStepResult();
	}

TVerdict Cman14Step::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	return TestStepResult();
	}

