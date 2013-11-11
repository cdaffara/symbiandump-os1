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
 @file man12Step.cpp
*/
#include "man12step.h"
#include "Te_manSuiteDefs.h"
#include "LbsInternalInterface.h"
#include <lbs/lbslocdatasourcegpsbase.h>
#include "lbsassistancedatacacheapi.h"
#include <lbs/lbsextendedsatellite.h>
#include <lbs/lbsgpsmeasurement.h>


Cman12Step::~Cman12Step()
/**
 * Destructor
 */
	{
	}

Cman12Step::Cman12Step()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(Kman12Step);
	}

TVerdict Cman12Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{                                  
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict Cman12Step::doTestStepL()
/**
 * @return - TVerdict code
 * 
 * This test checks that when the NRH requests a hybrid location update, a measurement update
 * is received (in addition to a location update).
 * The test also checks that when the NRH request is cancelled, the Integration Module is reconfigured
 * with the GPS Mode given by the corresponding admin setting and the location request is cancelled. 
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
		TRequestStatus locStat;
		posUpdates.NotifyPositionUpdate(locStat);
		
		RLbsGpsMeasurementUpdates measurementUpdates;
		measurementUpdates.OpenL();
		CleanupClosePushL(measurementUpdates);
		TRequestStatus measurementStat;
		measurementUpdates.NotifyGpsMeasurementUpdate(measurementStat);
		
		RLbsModuleStatus modStatus;
		modStatus.OpenL(KLbsGpsLocManagerUid);
		CleanupClosePushL(modStatus);
		
		// Start the testcase with a cancel request both from the LocServer
		// and the NRH to ensure the test AGPS Manager starts from scratch
		// (previous testcases may not have cleaned up correctly)
		TLbsPositionUpdateRequestCancel cancel;
		
		User::LeaveIfError(posUpdateReqLocSrv.SetPositionUpdateRequest(cancel));
		User::LeaveIfError(posUpdateReqNRH.SetPositionUpdateRequest(cancel));
	//	WaitAndValidateNgMsg(TLbsNetInternalMsgBase::ESelfLocationCancel);
		User::After(1000000); // Allow one second for the cancel to take effect
		
		// Simulate what is done by NRH when it wants a hybrid location update
		TLbsPositionUpdateRequest locRequestNRH;
		TLbsLocRequestQualityInt qualityNRH;
		qualityNRH.SetMaxFixTime(5000000);
		qualityNRH.SetMinHorizontalAccuracy(0.5);
		qualityNRH.SetMinVerticalAccuracy(0.5);
    	locRequestNRH.RequestQuality() = qualityNRH;    	
   	 	TTime targetTime;
		targetTime.UniversalTime(); // Target time is now and try for 5 seconds
		locRequestNRH.TargetTime() = targetTime;
		// Make the request a 'hybrid' one
		TLbsNetPosRequestMethodInt methods;
		TLbsNetPosMethodInt posMethods[2];
		posMethods[0].SetPosMethod(KLbsRootUid, (TPositionModuleInfo::ETechnologyTerminal|TPositionModuleInfo::ETechnologyAssisted));
		posMethods[1].SetPosMethod(KLbsRootUid, (TPositionModuleInfo::ETechnologyNetwork | TPositionModuleInfo::ETechnologyAssisted));
		methods.SetPosMethods(posMethods, 2);
		locRequestNRH.RequestMethod() = methods;
		
		INFO_PRINTF1(_L("NRH sends a location request to integration module"));
		User::LeaveIfError(posUpdateReqNRH.SetPositionUpdateRequest(locRequestNRH));

		// Check first that the GPS Manager publishes a position update as provided
		// by the test integration module (the test version of the integration module
		// is hardwired to send a test TPositionExtendedSatelliteInfo)
		// 
		User::WaitForRequest(locStat);
		
		if(locStat.Int()!=KErrNone)
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
		// The AGPS Manager should ask the Integration Module to:
		// 1.- Cancel the location request (because there is no outstanding request from the Location Server)
		// 2.- Configure GPS Mode as per Admin setting.
		//
		User::LeaveIfError(posUpdateReqNRH.SetPositionUpdateRequest(cancel));
		
		//...let the cancel message do its thing for a second
		User::After(1000000);
		
		occurredEvents = TPositionModuleStatusEventBase::EEventNone;
		User::LeaveIfError(modStatus.GetModuleStatus(&moduleStatus, sizeof(moduleStatus), occurredEvents));

		// Check that the cancellation has resulted in a call to CancelLocationRequest of the
		// integration module (hack: the test integration module switches the Data Quality Status
		// to EDataQualityPartial when CancelLocationRequest is called while in 'hybrid'mode)
		//
		if (moduleStatus.DataQualityStatus() != TPositionModuleStatus::EDataQualityPartial)
			{
			INFO_PRINTF1(_L("Incorrect DataQualityStatus: CancelLocationRequest has not been called."));
			User::Leave(KErrNotFound);
			}
		
		INFO_PRINTF1(_L("Test integration module correctly set TPositionModuleStatus::EDataQualityPartial"));
		
		// Check that the AGPS manager has reconfigured the Integration Module' GPS mode with the 
		// Admin setting after leaving the hybrid mode. (NOTE: for testing purposes, the test integration 
		// module changes the Device Status when it is configured with a new admin setting)
		//
		if(moduleStatus.DeviceStatus() == TPositionModuleStatus::EDeviceReady)
			{
			INFO_PRINTF1(_L("Incorrect DeviceStatus: GPS Mode hasn't changed."));
			User::Leave(KErrNotFound);
			}
		
		INFO_PRINTF1(_L("Checked that module status after cancelling is not EDeviceInitialising"));

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




TVerdict Cman12Step::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	return TestStepResult();
	}
