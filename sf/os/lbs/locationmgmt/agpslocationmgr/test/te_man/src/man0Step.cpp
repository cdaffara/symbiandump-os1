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
 @file man0Step.cpp
*/
#include "man0Step.h"
#include "Te_manSuiteDefs.h"
#include "LbsInternalInterface.h"
#include "cprocesslaunch.h"
#include "tprocessstartparams.h"	
#include "lbsassistancedatacacheapi.h"
#include "lbsnetinternalapi.h"
#include "lbsnetregstatusint.h"
#include "LbsLocSourceTestChannel.h"
#include "lbsprocesssupervisor.h"

#include <lbs/lbsextendedsatellite.h>

Cman0Step::~Cman0Step()
/**
 * Destructor
 */
	{
	}

Cman0Step::Cman0Step()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	iStepCount = 0;
	SetTestStepName(Kman0Step);
	}

TVerdict Cman0Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	//INFO_PRINTF1(_L("Please delete this line or modify it. I am in Test Step Preamble in Class Cman0Step"));
	// uncomment the following 3 lines if you have common pre setting to all the test steps in there
	CTe_manSuiteStepBase::doTestStepPreambleL();
	// if (TestStepResult()!=EPass)
	//    return   TestStepResult();
	// process some pre setting to this test step then set SetTestStepResult to EFail or Epass.
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict Cman0Step::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	  if (TestStepResult()==EPass)
		{
		
		// Start test version of AGPS manager and use a test version if integration module
		// The test version of AGPS manager is same as actual but does not use the
		// ECOM plugin interface.

		// for real thing this is done by route (lbs startup) process
	    TPositionExtendedSatelliteInfo posSatInfo;
		RLbsPositionUpdates::InitializeL(posSatInfo); 	//	used to publish position
		
		TPositionModuleStatus defaultPositionModuleStatus;
    	RLbsModuleStatus::InitializeL(defaultPositionModuleStatus);
		
		RLbsPositionUpdateRequests::InitializeL();
		RLbsNetworkRegistrationStatus::InitializeL();
		RLbsNetChannel::InitializeL(RLbsNetChannel::EChannelAGPS2NG);
		RLbsNetChannel::InitializeL(RLbsNetChannel::EChannelNG2AGPS);
		RAssistanceDataCache::InitializeL();
		RAgpsModuleTestChannel::InitialiseL();
		
		RLbsGpsMeasurementUpdates::InitializeL();
		
	    /*
	     * Define the properties used by the Root Process API
	     */
	    RLbsProcessSupervisor::InitializeL();
	    
		TProcessStartParams params;
		_LIT(KDummyFileName, "\\sys\\bin\\testlbsgpslocmanager.exe");
		_LIT(KDummyProcessName, "testlbsgpslocmanager");
		params.SetProcessFileName(KDummyFileName);
		params.SetProcessName(KDummyProcessName);
		params.SetRendezvousRequired(ETrue);
		TInt r = CProcessLaunch::ProcessLaunch(params);
		User::LeaveIfError(r);
		
	 	INFO_PRINTF1(_L("AGPSManagerProcess started in the file man0Step.cpp"));  //Block start
		INFO_PRINTF1(_L("Advise test integration module EPowerModeStandby"));
 		TLbsPositionUpdateRequestStatus request;
 		request.Tracking() = ETrue;
 		request.SetPowerAdvice(TLbsPositionUpdateRequestBase::EPowerAdviceStandby);
 		RLbsPositionUpdateRequests posReqs;
 		posReqs.OpenL(KChannelArray[0]);
 		CleanupClosePushL(posReqs);
			
		const TUint KLbsGpsLocManagerUidValue		= 0x10281D44;
		const TUid KLbsGpsLocManagerUid				= {KLbsGpsLocManagerUidValue};
		
	    RLbsModuleStatus modStatus;
		modStatus.OpenL(KLbsGpsLocManagerUid);
		CleanupClosePushL(modStatus);
		TRequestStatus stat;
		modStatus.NotifyModuleStatusChange(stat);
		
 		User::LeaveIfError(posReqs.SetPositionUpdateRequest(request));
		/* now enable active scheduler and wait for call bacl to be called which exits sceduler */

				
		User::WaitForRequest(stat);
		if(stat.Int()!=KErrNone) User::Leave(KErrGeneral);

		TPositionModuleStatus status;
		TPositionModuleStatusEventBase::TModuleEvent occurredEvents =
				TPositionModuleStatusEventBase::EEventNone;
 		User::LeaveIfError(modStatus.GetModuleStatus(&status, sizeof(TPositionModuleStatus), occurredEvents));

 		if(status.DeviceStatus() != TPositionModuleStatus::EDeviceStandBy)
			{
			User::Leave(KErrNotFound);
			}
		INFO_PRINTF1(_L("Test integration module correctly responds with EDeviceStandBy"));
		
		// now iterate around all eight values for TDeviceStatus and
		// check correct publishing to CPosIntGpsHwStatus
		
		
 		CPosIntGpsHwStatus* posIntGpsHwStatus = CPosIntGpsHwStatus::NewL(*this);
  	   	CleanupStack::PushL(posIntGpsHwStatus);

  	   	CActiveScheduler* temp = CActiveScheduler::Current();

  	   	// this test checks that the correct hardware status is 
  	   	// sent to the OnStatusUpdateEvent (see  MPosIntGpsHwStatusObserver)
  	   	// The strategy is for the test to send eight EPowerAdviceClosed
  	   	// messages to the AGPS manager which then passes each in turn
  	   	// to a test agps module which responds to each with one of an 8 ordered list 
  	   	// of hw status updates to the agps manager.
  	   	// The manager publishes each value onto the api and
  	   	// this test checks that the correct values are oberved on OnStatusUpdateEvent.  
		for (TUint i= 0; i < 8; i++)
			{
			request.SetPowerAdvice(TLbsPositionUpdateRequestBase::EPowerAdviceClosed);
			//request.SetPowerAdvice((TLbsPositionUpdateRequestBase::TPowerAdvice)(TInt)(TLbsPositionUpdateRequestBase::EPowerAdviceClosed+1));
			User::LeaveIfError(posReqs.SetPositionUpdateRequest(request));
			temp->Start();
			
			}
		
	 	CleanupStack::PopAndDestroy(posIntGpsHwStatus);
		CleanupStack::PopAndDestroy(&modStatus);
 		CleanupStack::PopAndDestroy(&posReqs);
		// Set the network registration status to 'home network' - as this is what most of
		// the tests expect
		RLbsNetworkRegistrationStatus netRegStatus;
		netRegStatus.OpenL();
		CleanupClosePushL(netRegStatus);
		netRegStatus.SetNetworkRegistrationStatus(RLbsNetworkRegistrationStatus::ERegisteredHomeNetwork);
	
		CleanupStack::PopAndDestroy(&netRegStatus);

	 		//  **************   Block end ****************

		SetTestStepResult(EPass);
		}
	  return TestStepResult();
	}




static const CPosIntGpsHwStatus::TIntGpsHwStatus KResultStatusTable[8] = 
	{
	CPosIntGpsHwStatus::EStatusOff, 	// EDeviceUnknown 	 	-> EStatusOff
	CPosIntGpsHwStatus::EStatusOn, 		// EDeviceInitialising  -> EStatusOn
	CPosIntGpsHwStatus::EStatusOff, 	// EDeviceError 	 	-> EStatusOff
	CPosIntGpsHwStatus::EStatusOn, 		// EDeviceStandBy		-> EStatusOn
	CPosIntGpsHwStatus::EStatusOff, 	// EDeviceDisabled 		-> EStatusOff
	CPosIntGpsHwStatus::EStatusOn, 		// EDeviceReady			-> EStatusOn
	CPosIntGpsHwStatus::EStatusOff, 	// EDeviceInactive 		-> EStatusOff
	CPosIntGpsHwStatus::EStatusActive	// EDeviceActive 		-> EStatusActive
	};

void Cman0Step::OnStatusUpdateEvent(CPosIntGpsHwStatus::TIntGpsHwStatus aStatus, TInt /*aError*/)
{
 	INFO_PRINTF3(_L("Received TIntGpsHwStatus %d and expected %d"), aStatus , KResultStatusTable[iStepCount]);  
 
 	if (KResultStatusTable[iStepCount] != aStatus)
		{
		INFO_PRINTF1(_L("Test Failed"));  
	 	}

 	TESTL(aStatus == KResultStatusTable[iStepCount]);
	iStepCount++;
	
	CActiveScheduler::Stop();
}

TVerdict Cman0Step::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CTe_manSuiteStepBase::doTestStepPostambleL();
	return TestStepResult();
	}
