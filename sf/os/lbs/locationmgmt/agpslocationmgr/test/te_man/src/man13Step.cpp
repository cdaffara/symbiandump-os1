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
// Example CTestStep derived implementation
// 
//

/**
 @file man13Step.cpp
*/
#include "man13step.h"
#include "Te_manSuiteDefs.h"
#include "LbsInternalInterface.h"
#include <lbs/lbslocdatasourcegpsbase.h>
#include <lbs/lbsextendedsatellite.h>
#include <lbs/lbsgpsmeasurement.h>


Cman13Step::~Cman13Step()
/**
 * Destructor
 */
	{
	}

Cman13Step::Cman13Step()
/**
 * Constructor
 */
	{
	SetTestStepName(Kman13Step);
	}

TVerdict Cman13Step::doTestStepPreambleL()
/*
 * @return - TVerdict code
 * Override of base class virtual
 */
	{

	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict Cman13Step::doTestStepL()
/**
 * @return - TVerdict code
 * Test for the scenario where the Location Server requests a Location Update and this leads to a hybrid location
 * update request from the NRH. The request from the Location Server is maintained throughout the test (which means
 * the Integration Module must not be cancelled). 
 * The NRH request is cancelled at some point. The Integration Module is reconfigured
 * with the GPS Mode given by the corresponding Admin setting but the Location Request is not cancelled in the GPS module. 
 */
	{
	
	if (TestStepResult()==EPass)
		{
		INFO_PRINTF1(_L("Simulate NRH sending a 'hybrid' location request"));
		INFO_PRINTF1(_L("NRH sends location update request for NOW and  MaxFixTime=5s acc=0.1"));
			
		const RLbsPositionUpdateRequests::TChannelIdentifer KChannelIdentifierNRH = 
			{
				{KLbsGpsLocManagerUidValue},{KLbsNetRequestHandlerUidValue}
			};
			
		const RLbsPositionUpdateRequests::TChannelIdentifer KChannelIdentifierLocServer = 
			{
				{KLbsGpsLocManagerUidValue},{KLbsLocServerUidValue}
			};
			
		RLbsPositionUpdateRequests posUpdateReqLocSrv;
		posUpdateReqLocSrv.OpenL(KChannelIdentifierLocServer);
		CleanupClosePushL(posUpdateReqLocSrv);
	
		RLbsPositionUpdateRequests posUpdateReqNRH;
		posUpdateReqNRH.OpenL(KChannelIdentifierNRH);
		CleanupClosePushL(posUpdateReqNRH);
		
		RLbsPositionUpdates posUpdates;
		posUpdates.OpenL(KLbsGpsLocManagerUid);
		CleanupClosePushL(posUpdates);
	
		RLbsGpsMeasurementUpdates measurementUpdates;
		measurementUpdates.OpenL();
		CleanupClosePushL(measurementUpdates);
		TRequestStatus measurementStat;
		measurementUpdates.NotifyGpsMeasurementUpdate(measurementStat);
		
		RLbsModuleStatus modStatus;
		modStatus.OpenL(KLbsGpsLocManagerUid);
		CleanupClosePushL(modStatus);

		// simulate what is done by Location Server when it wants a location update
		TLbsPositionUpdateRequest locRequest;
		TLbsLocRequestQualityInt quality;
		quality.SetMaxFixTime(10000000);// 10 seconds
		quality.SetMinHorizontalAccuracy(0.1);
		quality.SetMinVerticalAccuracy(0.1);
		TTime targetTime;
		targetTime.UniversalTime(); // target time is now and try for 10 seconds!
    	locRequest.RequestQuality() = quality;
		locRequest.TargetTime() = targetTime;
		
		TLbsNetPosRequestMethodInt lsMethods;
		TLbsNetPosMethodInt lsPosMethods[1];
		lsPosMethods[0].SetPosMethod(KLbsRootUid, (TPositionModuleInfo::ETechnologyTerminal|TPositionModuleInfo::ETechnologyAssisted));
		lsMethods.SetPosMethods(lsPosMethods, 1);
		locRequest.RequestMethod() = lsMethods;

		INFO_PRINTF1(_L("Loc server sends a location request to integration module"));
		User::LeaveIfError(posUpdateReqLocSrv.SetPositionUpdateRequest(locRequest));
		
		User::After(2000000); //Allow a second for the request to sink in


		// Simulate what is done by NRH when it wants a hybrid location update
		TLbsPositionUpdateRequest locRequestNRH;
		TLbsLocRequestQualityInt qualityNRH;
		qualityNRH.SetMaxFixTime(5000000);// five seconds -shorter than LocServers's-
		qualityNRH.SetMinHorizontalAccuracy(0.5);
		qualityNRH.SetMinVerticalAccuracy(0.5);
    	locRequestNRH.RequestQuality() = qualityNRH;    	
		locRequestNRH.TargetTime() = targetTime; // Target time is now and try for 5 seconds
		// Make the request a 'hybrid' one
		TLbsNetPosRequestMethodInt methods;
		TLbsNetPosMethodInt posMethods[2];
		posMethods[0].SetPosMethod(KLbsRootUid, (TPositionModuleInfo::ETechnologyTerminal|TPositionModuleInfo::ETechnologyAssisted));
		posMethods[1].SetPosMethod(KLbsRootUid, (TPositionModuleInfo::ETechnologyNetwork | TPositionModuleInfo::ETechnologyAssisted));
		methods.SetPosMethods(posMethods, 2);
		locRequestNRH.RequestMethod() = methods;
		
		
		TRequestStatus locState;
		posUpdates.NotifyPositionUpdate(locState);
		INFO_PRINTF1(_L("NRH sends a location request to integration module"));
		User::LeaveIfError(posUpdateReqNRH.SetPositionUpdateRequest(locRequestNRH));
		
		// Check first that the GPS Manager publishes a position update as provided
		// by the test integration module (the test version of the integration module
		// is hardwired to send a test TPositionExtendedSatelliteInfo)
		// 
		User::WaitForRequest(locState);
		
		if(locState.Int()!=KErrNone)
			User::Leave(KErrGeneral);
		
		TInt error = 1; // something other than KErrNone
		TTime actualTime = 0;
		targetTime = 0;

		TPositionExtendedSatelliteInfo satInfo;
		TBool conflictControl;
		error = posUpdates.GetPositionInfo(conflictControl, &satInfo, sizeof(satInfo),targetTime, actualTime);

		if(error!=KErrNone)
			User::Leave(KErrGeneral);
			
		if(targetTime!=locRequestNRH.TargetTime())
			User::Leave(KErrGeneral);

		if(satInfo.SatelliteTime()!=locRequestNRH.TargetTime())
			User::Leave(KErrGeneral);

		INFO_PRINTF1(_L("Test integration module correctly responds with location"));

	
		// Check that the GPS Manager publishes a measurement update as provided
		// by the test integration module (the test version of the integration module
		// is hardwired to send a measurement update with 1 measured data)
		//
		User::WaitForRequest(measurementStat);
		
		if(measurementStat.Int()!=KErrNone)
			User::Leave(KErrGeneral);
		
		error = 1; // something other than KErrNone
		actualTime = 0;

		TPositionGpsMeasurementInfo measurementUpdate;
		error = measurementUpdates.GetGpsMeasurementInfo(&measurementUpdate, sizeof(measurementUpdate), actualTime);
		
		
		if (measurementUpdate.NumMeasurements() != 1)
			User::Leave(KErrGeneral);
		
		if(error!=KErrNone)
			User::Leave(KErrGeneral);
		
		INFO_PRINTF1(_L("Test integration module correctly responds with measurement"));
		

		// The last part of the test consists of cancelling the request and check the results
		// The test GPS Module will show those results by changes to the module status (this wouldn't
		// happen in production code, it is just for testing). Set the module status to some initial values
		// before cancelling.
		TPositionModuleStatus moduleStatus;
		moduleStatus.SetDeviceStatus(TPositionModuleStatus::EDeviceReady);
		moduleStatus.SetDataQualityStatus(TPositionModuleStatus::EDataQualityNormal);

        TPositionModuleStatusEventBase::TModuleEvent occurredEvents = 
						TPositionModuleStatusEventBase::EEventDeviceStatus | TPositionModuleStatusEventBase::EEventDataQualityStatus;
		User::LeaveIfError(modStatus.SetModuleStatus(&moduleStatus,sizeof(moduleStatus),occurredEvents));
		User::After(1000000);
	    

		// Cancel NRH's location update request.
		// The AGPS Manager should ask the Integration Module to
		// configure the GPS Mode as per Admin setting; but it should not
		// cancel the location request in the GPS module.
		//
		//

		TLbsPositionUpdateRequestCancel cancel;

		User::LeaveIfError(posUpdateReqNRH.SetPositionUpdateRequest(cancel));
		
		//...let the cancel message do its thing for a second
		User::After(1000000);
		
	    User::LeaveIfError(modStatus.GetModuleStatus(&moduleStatus, sizeof(moduleStatus), occurredEvents));

		// Check that the cancellation has NOT resulted in a call to CancelLocationRequest of the
		// integration module (hack: the test integration module switches the Data Quality Status
		// when CancelLocationRequest is called while in 'hybrid'mode)
		//
		if (moduleStatus.DataQualityStatus() == TPositionModuleStatus::EDataQualityPartial)
			{
			User::Leave(KErrNotFound);
			}
		

		// Clean up
		User::LeaveIfError(posUpdateReqLocSrv.SetPositionUpdateRequest(cancel));
		User::After(1000000);
		
		WaitAndValidateNgMsg(TLbsNetInternalMsgBase::ESelfLocationRequest);
		WaitAndValidateNgMsg(TLbsNetInternalMsgBase::EAssistanceDataRequest);
		
		SendNgMsg(KSessionCompleteKErrNone);

		CleanupStack::PopAndDestroy(&modStatus);
		CleanupStack::PopAndDestroy(&measurementUpdates);
		CleanupStack::PopAndDestroy(&posUpdates);
		CleanupStack::PopAndDestroy(&posUpdateReqNRH);
		CleanupStack::PopAndDestroy(&posUpdateReqLocSrv);

	 		//  **************   Block end ****************

		SetTestStepResult(EPass);
		}
	  return TestStepResult();
	}




TVerdict Cman13Step::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	return TestStepResult();
	}
