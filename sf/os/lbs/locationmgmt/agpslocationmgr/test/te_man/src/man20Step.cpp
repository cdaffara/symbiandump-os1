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
 @file man20Step.cpp
*/
#include "man20step.h"
#include "Te_manSuiteDefs.h"
#include "LbsInternalInterface.h"
#include <lbs/lbslocdatasourcegpsbase.h>
#include <lbs/lbsextendedsatellite.h>
#include <lbs/lbsgpsmeasurement.h>


Cman20Step::~Cman20Step()
/**
 * Destructor
 */
	{
	iUpdateCodeProperty.Close();
	}

Cman20Step::Cman20Step()
/**
 * Constructor
 */
	{
	SetTestStepName(Kman20Step);
	}

TVerdict Cman20Step::doTestStepPreambleL()
/*
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict Cman20Step::doTestStepL()
/**
 * @return - TVerdict code
  The Location Server requests a location update. The Molr starts and at some point the AGPS Manager has a request from
  the Location Server and a request from the NRH. The GPS Module produces an update with error code KPositionCalculationFutile.
  The test checks that the position is published again, with different attributes, when the session is completed.
 */
	{
	
	if (TestStepResult()==EPass)
		{

		// Subscribe to the property where the test GPS module reads the desired error code for
		// the location update
    	const TInt KTestLocSourceVal= 0x1028226E;// Secure ID of the test AGPS Manager
    	const TUid KTestLocSourceUid= {KTestLocSourceVal};
    	const TInt KUpdateCodePropertyKey = 0x11110100;
    	User::LeaveIfError(iUpdateCodeProperty.Attach(KTestLocSourceUid, 
	                                             KUpdateCodePropertyKey));
		
		// This test wants to receive a location update with error code KPositionCalculationFutile
		iUpdateCodeProperty.Set(KPositionCalculationFutile);
		
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

		TLbsPositionUpdateRequestCancel cancel;
		
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
		
		TLbsNetPosRequestMethodInt methods;
		TLbsNetPosMethodInt posMethods[1];
		posMethods[0].SetPosMethod(KLbsRootUid, (TPositionModuleInfo::ETechnologyTerminal|TPositionModuleInfo::ETechnologyAssisted));
		methods.SetPosMethods(posMethods, 1);
		locRequest.RequestMethod() = methods;
		
		INFO_PRINTF1(_L("Loc server sends a location request to integration module"));
		User::LeaveIfError(posUpdateReqLocSrv.SetPositionUpdateRequest(locRequest));
		
		User::After(1000000); //Allow a second for the request to sink in


		// Simulate what is done by NRH when it wants a hybrid location update
		TLbsPositionUpdateRequest locRequestNRH;
		TLbsLocRequestQualityInt qualityNRH;
		qualityNRH.SetMaxFixTime(5000000);// five seconds -shorter than LocServers's-
		qualityNRH.SetMinHorizontalAccuracy(0.5);
		qualityNRH.SetMinVerticalAccuracy(0.5);
    	locRequestNRH.RequestQuality() = qualityNRH;    	
		locRequestNRH.TargetTime() = targetTime; // Target time is now and try for 5 seconds
		// Make the request a 'hybrid' one
		TLbsNetPosRequestMethodInt nrhmethods;
		TLbsNetPosMethodInt nrhposMethods[2];
		nrhposMethods[0].SetPosMethod(KLbsRootUid, (TPositionModuleInfo::ETechnologyTerminal|TPositionModuleInfo::ETechnologyAssisted));
		nrhposMethods[1].SetPosMethod(KLbsRootUid, (TPositionModuleInfo::ETechnologyNetwork | TPositionModuleInfo::ETechnologyAssisted));
		nrhmethods.SetPosMethods(nrhposMethods, 2);
		locRequestNRH.RequestMethod() = nrhmethods;
		
		
		TRequestStatus locState;
		posUpdates.NotifyPositionUpdate(locState);
		INFO_PRINTF1(_L("NRH sends a location request to integration module"));
		User::LeaveIfError(posUpdateReqNRH.SetPositionUpdateRequest(locRequestNRH));
		
		// Check first that the GPS Manager publishes a position update as provided
		// by the test integration module (the test version of the integration module
		// is hardwired to send a test TPositionExtendedSatelliteInfo)
		// Check also that the error code is KPositionCalculationFutile
		//
		User::WaitForRequest(locState);
		
		if(locState.Int()!=KErrNone)
			User::Leave(KErrGeneral);
		
		TInt error = 1; // something other than KErrNone
		TTime actualTime = 0;
		targetTime = 0;

		TPositionExtendedSatelliteInfo satInfo;
		TBool conflictControl;
		TUint attributes;
		TUint gpsMode;
		error = posUpdates.GetPositionInfo(conflictControl, &satInfo, 
												   sizeof(satInfo), targetTime, 
												   actualTime, attributes, gpsMode);
		if(KPositionCalculationFutile != error)
			User::Leave(KErrGeneral);

		INFO_PRINTF1(_L("Test integration module correctly responds with KPositionCalculationFutile"));

		if(RLbsPositionUpdates::ESelfLocateSessionInProgress != attributes)
				User::Leave(KErrGeneral);
		INFO_PRINTF1(_L("Attribute obtained are correct (ESelfLocateSessionInProgress)"));

		if(targetTime!=locRequestNRH.TargetTime())
			User::Leave(KErrGeneral);

		if(satInfo.SatelliteTime()!=locRequestNRH.TargetTime())
			User::Leave(KErrGeneral);

		INFO_PRINTF1(_L("Test integration module has correctly responded with a position as expected"));

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
		
		if(error!=KPositionCalculationFutile)
			User::Leave(KErrGeneral);
		
		INFO_PRINTF1(_L("Measurement update error code is correct"));

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
	    
		// now we expect to see the ESelfLocationRequest and also a 
		// EAssistanceDataRequest (as a result of the integration module
		// asking for assisatance data when it receives the location request
		// from the NRH
   		WaitAndValidateNgMsg(TLbsNetInternalMsgBase::ESelfLocationRequest);
		WaitAndValidateNgMsg(TLbsNetInternalMsgBase::EAssistanceDataRequest);
	
		// Cancel NRH's location update request.
		// The AGPS Manager should ask the Integration Module to
		// configure the GPS Mode as per Admin setting; but it should not
		// cancel the location request in the GPS module.
		//
		User::LeaveIfError(posUpdateReqNRH.SetPositionUpdateRequest(cancel));
		
		//...let the cancel message do its thing for a second
		User::After(1000000);
		
	    User::LeaveIfError(modStatus.GetModuleStatus(&moduleStatus, sizeof(moduleStatus), occurredEvents));

		// Check that the cancellation has NOT resulted in a call to CancelLocationRequest of the
		// integration module (hack: the test integration module switches the Data Quality Status
		// when CancelLocationRequest is called while in 'hybrid'mode)
		//
		//if (moduleStatus.DataQualityStatus() == TPositionModuleStatus::EDataQualityPartial)
		//	{
		//	User::Leave(KErrNotFound);
		//	}
		
		// Check that the AGPS manager has reconfigured the Integration Module' GPS mode with the 
		// Admin setting after leaving the hybrid mode. (NOTE: for testing purposes, the test integration 
		// module switches state to EDeviceInitialising when it is configured with a new admin setting)
		//
		if(moduleStatus.DeviceStatus() == TPositionModuleStatus::EDeviceReady)
			{
			INFO_PRINTF1(_L("Error in DeviceStatus: GPS mode not changed."));
			User::Leave(KErrNotFound);
			}
		
		
		// Re-set property
		iUpdateCodeProperty.Set(KErrNone);

		// Clean up
		User::LeaveIfError(posUpdateReqLocSrv.SetPositionUpdateRequest(cancel));
		User::After(1000000);

		// and now the NetworkGateway should recieve a cancel as a result of the cancellation of the 
		// request from location server 
		WaitAndValidateNgMsg(TLbsNetInternalMsgBase::ESelfLocationCancel);
		
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




TVerdict Cman20Step::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	return TestStepResult();
	}
