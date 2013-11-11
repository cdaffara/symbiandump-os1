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
 @file man4Step.cpp
*/


#include "man4Step.h"
#include "Te_manSuiteDefs.h"
#include "LbsInternalInterface.h"
#include <lbs/lbsassistancealmanac.h>
#include <lbs/lbsassistancereferencetime.h>
#include <lbs/lbsassistanceaquisitionassistance.h>
#include <lbs/lbsextendedsatellite.h>
#include "cprocesslaunch.h"

#include "lbsnetinternalapi.h"
//#include "lbsassistancedatacacheapi.h"
#include "LbsInternalInterface.h"

//-----------------------------------------------------------------------------------------
const TTimeIntervalMicroSeconds32 KNoModuleMsgTimeout = 5000000;//5s
const TTimeIntervalMicroSeconds32 KNoLocUpdTimeout    = 5000000;//5s
const TTimeIntervalMicroSeconds32 KFutureReqDelay     = 10000000;//10s

#if defined(__GCCXML__) || defined(__GCC__)
const TInt64 KLbsTestMaxFixTime	= 0x7FFFFFFFFFFFFFFFLL;	// GCC-XML expects LL appended to this long type
#else
const TInt64 KLbsTestMaxFixTime	= 0x7FFFFFFFFFFFFFFF;	// Setting Max Fix time for test 
#endif

const TLbsNetSessionCompleteAgpsMsg KSessionCompleteKErrBusy(KDummySessionId, KErrServerBusy);
const TLbsNetSessionCompleteAgpsMsg KSessionCompleteKErrHighPrior(KDummySessionId, KErrPositionHighPriorityReceive);
const TLbsNetSessionCompleteAgpsMsg KSessionCompleteKErrGeneral(KDummySessionId, KErrGeneral);
const TLbsNetSessionCompleteAgpsMsg KSessionCompleteKErrTimedOut(KDummySessionId, KErrTimedOut);
const TLbsNetAssistanceDataResponseMsg KAssistanceDataResponseErrNotFound(0, KErrNotFound);

TRequestStatus status;
TRequestStatus moduleMsgStatus;
TLbsPositionUpdateRequest LowAccurRequest;
TLbsPositionUpdateRequest HighAccurRequest;
TLbsPositionUpdateRequest absurdMaxFixTimeRequest;
TLbsPositionUpdateRequest TrackingNewClientRequest;
TLbsPositionUpdateRequestStatus StatusTrackingOffRequest;
TLbsPositionUpdateRequestStatus StatusTrackingOnRequest;
TLbsPositionUpdateRequestCancel CancelTrackingOffRequest;
TLbsPositionUpdateRequestCancel CancelTrackingOnRequest;

//-----------------------------------------------------------------------------------------
Cman4Step::~Cman4Step()
	{
	//TInt test = 6;
	}

Cman4Step::Cman4Step()
	{
	SetTestStepName(Kman4Step);
	}

TVerdict Cman4Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	SetupNGInterface();
	
	// Open interfaces
	INFO_PRINTF1(_L("Openning Interfaces"));
	//iADCache.OpenL();
	//CleanupClosePushL(iADCache);
	
	iMSInt.OpenL(KLbsGpsLocManagerUid);
	CleanupClosePushL(iMSInt);

	iLSInt.OpenL(KChannelArray[0]);
	CleanupClosePushL(iLSInt);
	
	iNRHInt.OpenL(KChannelArray[2]);
	CleanupClosePushL(iNRHInt);
	
	iLocUpdInt.OpenL(KLbsGpsLocManagerUid);
	CleanupClosePushL(iLocUpdInt);
	
	iModuleInt.OpenL();
	CleanupClosePushL(iModuleInt);
	
	// Subscribe to the property where the test GPS module reads the desired error code for
	// the location update
	const TInt KTestLocSourceVal= 0x1028226E;// Secure ID of the test AGPS Manager
	const TUid KTestLocSourceUid= {KTestLocSourceVal};
	const TInt KUpdateCodePropertyKey = 0x11110100;
	User::LeaveIfError(iUpdateCodeProperty.Attach(KTestLocSourceUid, 
                                             KUpdateCodePropertyKey));
	const TInt KUpdateFixTimePropertyKey = 0x11110101;
	User::LeaveIfError(iUpdateFixTimeProperty.Attach(KTestLocSourceUid, 
                                             KUpdateFixTimePropertyKey));

		
	CleanupStack::Pop(5);
	INFO_PRINTF1(_L("Openning Interfaces OK"));

	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict Cman4Step::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	
	// Add this 'TerminalBased' Request Method to the Low/HighAccurRequest
	// to ensure that when the NRH issues either request it will get an
	// update with conflict flag set to zero. 
	TLbsNetPosRequestMethodInt methods;
	TLbsNetPosMethodInt posMethods[1];
	posMethods[0].SetPosMethod(KLbsRootUid, (TPositionModuleInfo::ETechnologyTerminal|TPositionModuleInfo::ETechnologyAssisted));
	//posMethods[1].SetPosMethod(KLbsRootUid, (TPositionModuleInfo::ETechnologyNetwork | TPositionModuleInfo::ETechnologyAssisted));
	methods.SetPosMethods(posMethods, 1);
	
	
	LowAccurRequest.NewClient()  = EFalse;
	LowAccurRequest.Tracking()   = EFalse;
	LowAccurRequest.RequestQuality().SetMaxFixTime(5000000);
	LowAccurRequest.RequestQuality().SetMinHorizontalAccuracy(100);
	LowAccurRequest.RequestQuality().SetMinVerticalAccuracy(100);
	LowAccurRequest.RequestMethod() = methods;
	LowAccurRequest.TargetTime() = 0;

	HighAccurRequest.NewClient()  = EFalse;
	HighAccurRequest.Tracking()   = EFalse;
	HighAccurRequest.RequestQuality().SetMaxFixTime(5000000);
	HighAccurRequest.RequestQuality().SetMinHorizontalAccuracy(1);
	HighAccurRequest.RequestQuality().SetMinVerticalAccuracy(1);
	HighAccurRequest.TargetTime() = 0;
	HighAccurRequest.RequestMethod() = methods;

	
	absurdMaxFixTimeRequest.NewClient()  = EFalse;
	absurdMaxFixTimeRequest.Tracking()   = EFalse;
	absurdMaxFixTimeRequest.RequestQuality().SetMaxFixTime(KLbsTestMaxFixTime);
	absurdMaxFixTimeRequest.RequestQuality().SetMinHorizontalAccuracy(100);
	absurdMaxFixTimeRequest.RequestQuality().SetMinVerticalAccuracy(100);
	absurdMaxFixTimeRequest.TargetTime() = 0;
	absurdMaxFixTimeRequest.RequestMethod() = methods;
	
	TrackingNewClientRequest = LowAccurRequest;
	TrackingNewClientRequest.NewClient()  = ETrue;
	TrackingNewClientRequest.Tracking()   = ETrue;
	
	StatusTrackingOffRequest.Tracking()   = EFalse;
	StatusTrackingOnRequest.Tracking()   = ETrue;

	CancelTrackingOffRequest.Tracking()   = EFalse;
	CancelTrackingOnRequest.Tracking()   = ETrue;

	//------------------------------------------------------------------------------------------------	
	SetTestStepResult(EPass);
	iUpdateCodeProperty.Set(KErrNone);
	
	//------------------------------------------------------------------------------------------------	

	INFO_PRINTF1(_L("Test0 - manager sends tracking status on then off"));
	// Location request from LS. Tracking and NewClient flags check
	iLocUpdInt.NotifyPositionUpdate(status);
	TrackingNewClientRequest.TargetTime().UniversalTime();


	TTime start;
	start.UniversalTime(); 	// actually, timeout starts when NPUD is completed - so this 
							// is a little arlier than that
							
	iLSInt.SetPositionUpdateRequest(TrackingNewClientRequest);

	// Wait for advice and Self Loc Request. Send error.
	WaitAndValidateNgMsg(TLbsNetInternalMsgBase::ESelfLocationRequest, ETrue);
	WaitAndValidateLocUpd(status);

	SendNgMsg(KSessionCompleteKErrNone);

	INFO_PRINTF1(_L("Test 0 - Wait for Tracking to be set to Off"));
	// now wait for tracking off after and not before 
	// the timeout period (hardwired in agps manager to 30s 


   	WaitAndValidateNgMsg(TLbsNetInternalMsgBase::ESystemStatusAdvice, EFalse);
	TTime end;
	end.UniversalTime();

	TTimeIntervalMicroSeconds diff = 0;
	diff = end.Int64()-start.Int64();
	
	if ( (diff < 30*1000*1000) || (diff > 35*1000*1000)) // approx!
		{
		SetTestStepResult(EFail);
		ERR_PRINTF1(_L(" <font color=FF0000><b>incorrect tracking timeout.</b></font>"));
		}
		
	//------------------------------------------------------------------------------------------------	
	INFO_PRINTF1(_L("Test 1 - Location request from LS"));
	iModuleInt.Subscribe(moduleMsgStatus);
	iLocUpdInt.NotifyPositionUpdate(status);
	LowAccurRequest.TargetTime().UniversalTime();
	iLSInt.SetPositionUpdateRequest(LowAccurRequest);
	
	WaitAndValidateNgMsg(TLbsNetInternalMsgBase::ESelfLocationRequest);
	WaitAndValidateModuleMsg(moduleMsgStatus, RAgpsModuleTestChannel::ETestMsgRequestLocationUpdate);
	WaitAndValidateLocUpd(status);
	SendNgMsg(KSessionCompleteKErrNone);

	//------------------------------------------------------------------------------------------------	
	INFO_PRINTF1(_L("Test 2 - Location request from NRH"));
	iModuleInt.Subscribe(moduleMsgStatus);
	iLocUpdInt.NotifyPositionUpdate(status);
	LowAccurRequest.TargetTime().UniversalTime();
	iNRHInt.SetPositionUpdateRequest(LowAccurRequest);
	
	WaitAndValidateNgMsg(TLbsNetInternalMsgBase::EAssistanceDataRequest);
	WaitAndValidateModuleMsg(moduleMsgStatus, RAgpsModuleTestChannel::ETestMsgRequestLocationUpdate);
	WaitAndValidateLocUpd(status);
			

	//------------------------------------------------------------------------------------------------	
	INFO_PRINTF1(_L("Test 2a - Location request from NRH followed by a Cancel"));
	LowAccurRequest.TargetTime().UniversalTime();

	iModuleInt.Subscribe(moduleMsgStatus);
	iNRHInt.SetPositionUpdateRequest(LowAccurRequest);
	WaitAndValidateNgMsg(TLbsNetInternalMsgBase::EAssistanceDataRequest);
	WaitAndValidateModuleMsg(moduleMsgStatus, RAgpsModuleTestChannel::ETestMsgRequestLocationUpdate);

	iModuleInt.Subscribe(moduleMsgStatus);
	iNRHInt.SetPositionUpdateRequest(CancelTrackingOffRequest);// Tracking flag not used by this interface
	WaitAndValidateModuleMsg(moduleMsgStatus, RAgpsModuleTestChannel::ETestMsgCancelLocationRequest);
	//------------------------------------------------------------------------------------------------	
	INFO_PRINTF1(_L("Test 3 - Location requests from LS followed by NRH. Same time window."));
	iModuleInt.Subscribe(moduleMsgStatus);
	iLocUpdInt.NotifyPositionUpdate(status);
	LowAccurRequest.TargetTime().UniversalTime();
	
	LowAccurRequest.RequestQuality().SetMaxFixTime(10000000);
	iLSInt.SetPositionUpdateRequest(LowAccurRequest);
	WaitAndValidateModuleMsg(moduleMsgStatus, RAgpsModuleTestChannel::ETestMsgRequestLocationUpdate);

	User::After(300000);	//0.3s
	LowAccurRequest.RequestQuality().SetMaxFixTime(5000000);
	LowAccurRequest.TargetTime().UniversalTime();
	
	iModuleInt.Subscribe(moduleMsgStatus);
	iNRHInt.SetPositionUpdateRequest(LowAccurRequest);
	WaitAndValidateModuleMsg(moduleMsgStatus, RAgpsModuleTestChannel::ETestMsgRequestLocationUpdate);

	WaitAndValidateNgMsg(TLbsNetInternalMsgBase::ESelfLocationRequest);
	WaitAndValidateLocUpd(status);
	// Validate the target time in the combined request.
	// In this case, should take NRH request time
	if (iRcvdTargetTime != LowAccurRequest.TargetTime())
		{
		SetTestStepResult(EFail);
		ERR_PRINTF1(_L(" <font color=FF0000><b>Incorrect target time.</b></font>"));
		}
	// Check fix time in combined request
	// In this case, should be around 9.7 secs
	if ((iRcvdFixTime > 9700000) || (iRcvdFixTime < 9500000))
		{
		SetTestStepResult(EFail);
		ERR_PRINTF1(_L(" <font color=FF0000><b>Incorrect fix time.</b></font>"));
		}

	SendNgMsg(KSessionCompleteKErrNone);
	
	//------------------------------------------------------------------------------------------------	
	INFO_PRINTF1(_L("Test 3a - Location requests from NRH followed by LS. Same time window."));
	iModuleInt.Subscribe(moduleMsgStatus);
	iLocUpdInt.NotifyPositionUpdate(status);
	LowAccurRequest.TargetTime().UniversalTime();
	TTime expectedTT(LowAccurRequest.TargetTime());

	LowAccurRequest.RequestQuality().SetMaxFixTime(10000000);
	iNRHInt.SetPositionUpdateRequest(LowAccurRequest);
	WaitAndValidateModuleMsg(moduleMsgStatus, RAgpsModuleTestChannel::ETestMsgRequestLocationUpdate);

	User::After(300000);	//0.3s
	LowAccurRequest.RequestQuality().SetMaxFixTime(5000000);
	LowAccurRequest.TargetTime().UniversalTime();
	iModuleInt.Subscribe(moduleMsgStatus);
	iLSInt.SetPositionUpdateRequest(LowAccurRequest);
	WaitAndValidateModuleMsg(moduleMsgStatus, RAgpsModuleTestChannel::ETestMsgRequestLocationUpdate);

	WaitAndValidateNgMsg(TLbsNetInternalMsgBase::ESelfLocationRequest);
	WaitAndValidateLocUpd(status);
	if (iRcvdTargetTime != expectedTT)
		{
		SetTestStepResult(EFail);
		ERR_PRINTF1(_L(" <font color=FF0000><b>Incorrect target time.</b></font>"));
		}
	// Check fix time in combined request
	// In this case, should be around 9.7 secs
	if ((iRcvdFixTime > 9700000) || (iRcvdFixTime < 9500000))
		{
		SetTestStepResult(EFail);
		ERR_PRINTF1(_L(" <font color=FF0000><b>Incorrect fix time.</b></font>"));
		}

	SendNgMsg(KSessionCompleteKErrNone);
	
	//------------------------------------------------------------------------------------------------	
	INFO_PRINTF1(_L("Test 3b - Immediate Location requests from LS followed by NRH. Same time window."));
	iModuleInt.Subscribe(moduleMsgStatus);
	iLocUpdInt.NotifyPositionUpdate(status);
	LowAccurRequest.TargetTime() = 0;

	LowAccurRequest.RequestQuality().SetMaxFixTime(10000000);
	iLSInt.SetPositionUpdateRequest(LowAccurRequest);
	WaitAndValidateModuleMsg(moduleMsgStatus, RAgpsModuleTestChannel::ETestMsgRequestLocationUpdate);

	User::After(300000);	//0.3s
	LowAccurRequest.RequestQuality().SetMaxFixTime(5000000);
	iModuleInt.Subscribe(moduleMsgStatus);
	iNRHInt.SetPositionUpdateRequest(LowAccurRequest);
	WaitAndValidateModuleMsg(moduleMsgStatus, RAgpsModuleTestChannel::ETestMsgRequestLocationUpdate);

	WaitAndValidateNgMsg(TLbsNetInternalMsgBase::ESelfLocationRequest);
	WaitAndValidateLocUpd(status);
	if (iRcvdTargetTime != 0)
		{
		SetTestStepResult(EFail);
		ERR_PRINTF1(_L(" <font color=FF0000><b>Incorrect target time.</b></font>"));
		}
	// Check fix time in combined request
	// In this case, should be around 9.7 secs
	if ((iRcvdFixTime > 9700000) || (iRcvdFixTime < 9500000))
		{
		SetTestStepResult(EFail);
		ERR_PRINTF1(_L(" <font color=FF0000><b>Incorrect fix time.</b></font>"));
		}

	SendNgMsg(KSessionCompleteKErrNone);
	
	//------------------------------------------------------------------------------------------------	
	INFO_PRINTF1(_L("Test 3c - Location requests from LS followed by immediate NRH request. Same time window."));
	iModuleInt.Subscribe(moduleMsgStatus);
	iLocUpdInt.NotifyPositionUpdate(status);
	LowAccurRequest.TargetTime().UniversalTime();
	
	LowAccurRequest.RequestQuality().SetMaxFixTime(10000000);
	iLSInt.SetPositionUpdateRequest(LowAccurRequest);
	WaitAndValidateModuleMsg(moduleMsgStatus, RAgpsModuleTestChannel::ETestMsgRequestLocationUpdate);

	User::After(300000);	//0.3s
	LowAccurRequest.RequestQuality().SetMaxFixTime(5000000);
	LowAccurRequest.TargetTime() = 0;
	iModuleInt.Subscribe(moduleMsgStatus);
	iNRHInt.SetPositionUpdateRequest(LowAccurRequest);
	WaitAndValidateModuleMsg(moduleMsgStatus, RAgpsModuleTestChannel::ETestMsgRequestLocationUpdate);

	WaitAndValidateNgMsg(TLbsNetInternalMsgBase::ESelfLocationRequest);
	WaitAndValidateLocUpd(status);
	if (iRcvdTargetTime != 0)
		{
		SetTestStepResult(EFail);
		ERR_PRINTF1(_L(" <font color=FF0000><b>Incorrect target time.</b></font>"));
		}
	// Check fix time in combined request
	// In this case, should be around 9.7 secs
	if ((iRcvdFixTime > 9700000) || (iRcvdFixTime < 9500000))
		{
		SetTestStepResult(EFail);
		ERR_PRINTF1(_L(" <font color=FF0000><b>Incorrect fix time.</b></font>"));
		}

	SendNgMsg(KSessionCompleteKErrNone);
	
	//------------------------------------------------------------------------------------------------	
	INFO_PRINTF1(_L("Test 3d - Location requests from NRH (immediate) followed by LS. Same time window."));
	iModuleInt.Subscribe(moduleMsgStatus);
	iLocUpdInt.NotifyPositionUpdate(status);

	LowAccurRequest.TargetTime() = 0;
	LowAccurRequest.RequestQuality().SetMaxFixTime(10000000);
	iNRHInt.SetPositionUpdateRequest(LowAccurRequest);
	WaitAndValidateModuleMsg(moduleMsgStatus, RAgpsModuleTestChannel::ETestMsgRequestLocationUpdate);

	User::After(300000);	//0.3s
	LowAccurRequest.RequestQuality().SetMaxFixTime(5000000);
	LowAccurRequest.TargetTime().UniversalTime();
	iModuleInt.Subscribe(moduleMsgStatus);
	iLSInt.SetPositionUpdateRequest(LowAccurRequest);
	WaitAndValidateModuleMsg(moduleMsgStatus, RAgpsModuleTestChannel::ETestMsgRequestLocationUpdate);

	WaitAndValidateNgMsg(TLbsNetInternalMsgBase::ESelfLocationRequest);
	WaitAndValidateLocUpd(status);
	if (iRcvdTargetTime != 0)
		{
		SetTestStepResult(EFail);
		ERR_PRINTF1(_L(" <font color=FF0000><b>Incorrect target time.</b></font>"));
		}
	// Check fix time in combined request
	// In this case, should be around 9.7 secs
	if ((iRcvdFixTime > 9700000) || (iRcvdFixTime < 9500000))
		{
		SetTestStepResult(EFail);
		ERR_PRINTF1(_L(" <font color=FF0000><b>Incorrect fix time.</b></font>"));
		}

	SendNgMsg(KSessionCompleteKErrNone);
	
	//------------------------------------------------------------------------------------------------	
	INFO_PRINTF1(_L("Test 3e - LS Req followed by timed out NRH req. Same time window."));
	// Tests that fix time gets updated correctly
	// This test uses a very short fix time for the NRH request.
	// This is to allow it to time-out before the test gps module returns
	// a location update (which is done after a 0.5 second timeout)
	// Could have increased the timeout used by the test gps module
	// but that would have meant the whole test takes way too long to run
	iModuleInt.Subscribe(moduleMsgStatus);
	iLocUpdInt.NotifyPositionUpdate(status);
	LowAccurRequest.TargetTime().UniversalTime();
	expectedTT = LowAccurRequest.TargetTime();
	LowAccurRequest.RequestQuality().SetMaxFixTime(3000000);//3 secs
	iLSInt.SetPositionUpdateRequest(LowAccurRequest);
	WaitAndValidateModuleMsg(moduleMsgStatus, RAgpsModuleTestChannel::ETestMsgRequestLocationUpdate);

	User::After(300000); // 0.3s
	LowAccurRequest.TargetTime() = 0;//Immediate request
	LowAccurRequest.RequestQuality().SetMaxFixTime(200000);//0.2s
	iModuleInt.Subscribe(moduleMsgStatus);
	iNRHInt.SetPositionUpdateRequest(LowAccurRequest);
	WaitAndValidateModuleMsg(moduleMsgStatus, RAgpsModuleTestChannel::ETestMsgRequestLocationUpdate);
	// Allow time for combined request to be issued
	User::After(100000);//0.1s
	// Check fix time in combined request
	// In this case, should be around 2.7 secs
	iUpdateFixTimeProperty.Get(iRcvdFixTime);
	if ((iRcvdFixTime > 2700000) || (iRcvdFixTime < 2500000))
		{
		SetTestStepResult(EFail);
		ERR_PRINTF1(_L(" <font color=FF0000><b>Incorrect fix time.</b></font>"));
		}
	WaitAndValidateNgMsg(TLbsNetInternalMsgBase::ESelfLocationRequest);
	// Should now get an update based on the original LS request
	// because the NRH request has timed out
	WaitAndValidateLocUpd(status);
	if (iRcvdTargetTime != expectedTT)
		{
		SetTestStepResult(EFail);
		ERR_PRINTF1(_L(" <font color=FF0000><b>Incorrect target time.</b></font>"));
		}
	// Check fix time in combined request
	// In this case, should be around 2.5 secs
	if ((iRcvdFixTime > 2500000) || (iRcvdFixTime < 2300000))
		{
		SetTestStepResult(EFail);
		ERR_PRINTF1(_L(" <font color=FF0000><b>Incorrect fix time.</b></font>"));
		}

	SendNgMsg(KSessionCompleteKErrNone);
	
	//------------------------------------------------------------------------------------------------	
	INFO_PRINTF1(_L("Test 4 - Location requests from LS. Cancel to the NG due to a timeout"));
	iModuleInt.Subscribe(moduleMsgStatus);
	iLocUpdInt.NotifyPositionUpdate(status);
	HighAccurRequest.TargetTime().UniversalTime();
	iLSInt.SetPositionUpdateRequest(HighAccurRequest);

	WaitAndValidateNgMsg(TLbsNetInternalMsgBase::ESelfLocationRequest);
	WaitAndValidateNgMsg(TLbsNetInternalMsgBase::ESelfLocationCancel);// Cancel due to a timeout
	WaitAndValidateModuleMsg(moduleMsgStatus, RAgpsModuleTestChannel::ETestMsgRequestLocationUpdate);
	
	iModuleInt.Subscribe(moduleMsgStatus);
	WaitNoModuleMsg(moduleMsgStatus);
	WaitAndValidateLocUpd(status); // The low accuracy location update
	
	SendNgMsg(KSessionCompleteKErrTimedOut);
	
	//------------------------------------------------------------------------------------------------	
	INFO_PRINTF1(_L("Test 5 - Location requests from NRH. No cancel due to a timeout"));
	iModuleInt.Subscribe(moduleMsgStatus);
	iLocUpdInt.NotifyPositionUpdate(status);
	HighAccurRequest.TargetTime().UniversalTime();
	iNRHInt.SetPositionUpdateRequest(HighAccurRequest);
	
	// We do not expect the Assistance Data Cancel
	WaitAndValidateNgMsg(TLbsNetInternalMsgBase::EAssistanceDataRequest);
	WaitAndValidateModuleMsg(moduleMsgStatus, RAgpsModuleTestChannel::ETestMsgRequestLocationUpdate);

	iModuleInt.Subscribe(moduleMsgStatus);
	WaitNoModuleMsg(moduleMsgStatus);
	WaitAndValidateLocUpd(status); // The low accuracy location update

	//------------------------------------------------------------------------------------------------	
	INFO_PRINTF1(_L("Test 6 - Location requests from LS followed by NRH. Different time windows."));
	iLocUpdInt.NotifyPositionUpdate(status);
	LowAccurRequest.TargetTime().UniversalTime();
	LowAccurRequest.TargetTime() += static_cast<TTimeIntervalSeconds>(10);
	LowAccurRequest.RequestQuality().SetMaxFixTime(5000000);
	iLSInt.SetPositionUpdateRequest(LowAccurRequest);
	LowAccurRequest.TargetTime().UniversalTime();
//	iNRHInt.SetPositionUpdateRequest(LowAccurRequest);

	// First time window - NRH request, but LS request is active as well
	// so we expect ESelfLocationRequest rather than EAssistanceDataRequest
	
	WaitAndValidateNgMsg(TLbsNetInternalMsgBase::ESelfLocationRequest);
	iNRHInt.SetPositionUpdateRequest(LowAccurRequest);

	// Combined request sent to GPS module that requests assistance data again
	// (our test module follows that with a notify position update)
	WaitAndValidateNgMsg(TLbsNetInternalMsgBase::EAssistanceDataRequest);
	WaitAndValidateLocUpd(status);
	
	// The update has satisfied the NRH but not the LS whose target time is
	// in the future. Therefore a new request is sent to the module with
	// just LS requirements. We expect to continue the ongoing
	// Self Location session and receive EAssistanceDataRequest
	// rather than ESelfLocationRequest.
	iLocUpdInt.NotifyPositionUpdate(status);
	WaitAndValidateNgMsg(TLbsNetInternalMsgBase::EAssistanceDataRequest);
	//Location Update for the LS Request
	WaitAndValidateLocUpd(status);
	

	SendNgMsg(KSessionCompleteKErrNone);

	//------------------------------------------------------------------------------------------------	
	INFO_PRINTF1(_L("Test 7 - Location requests from NRH followed by LS. Different time windows."));
	// Expected message flow as in test 6
	iLocUpdInt.NotifyPositionUpdate(status);
	LowAccurRequest.TargetTime().UniversalTime();
	iNRHInt.SetPositionUpdateRequest(LowAccurRequest);
	LowAccurRequest.TargetTime() += static_cast<TTimeIntervalSeconds>(10);
	iLSInt.SetPositionUpdateRequest(LowAccurRequest);
	
	WaitAndValidateNgMsg(TLbsNetInternalMsgBase::EAssistanceDataRequest);
	WaitAndValidateLocUpd(status);
	
	iLocUpdInt.NotifyPositionUpdate(status);
	WaitAndValidateNgMsg(TLbsNetInternalMsgBase::ESelfLocationRequest);
	WaitAndValidateLocUpd(status);
	SendNgMsg(KSessionCompleteKErrNone);		

	//------------------------------------------------------------------------------------------------	
	INFO_PRINTF1(_L("Test 8 - Location requests from LS. Session Complete before Location Update."));
	iLocUpdInt.NotifyPositionUpdate(status);
	LowAccurRequest.TargetTime().UniversalTime();
	iLSInt.SetPositionUpdateRequest(LowAccurRequest);
	
	WaitAndValidateNgMsg(TLbsNetInternalMsgBase::ESelfLocationRequest);
	SendNgMsg(KSessionCompleteKErrNone);
	WaitAndValidateLocUpd(status);	
	
	//------------------------------------------------------------------------------------------------	
	INFO_PRINTF1(_L("Test 9 - Location requests from LS. Session Complete after Location Update."));
	iLocUpdInt.NotifyPositionUpdate(status);
	LowAccurRequest.TargetTime().UniversalTime();
	iLSInt.SetPositionUpdateRequest(LowAccurRequest);
	
	WaitAndValidateNgMsg(TLbsNetInternalMsgBase::ESelfLocationRequest);
	WaitAndValidateLocUpd(status);
	SendNgMsg(KSessionCompleteKErrNone);

	//------------------------------------------------------------------------------------------------	
	INFO_PRINTF1(_L("Test 10 - Location requests from LS. Session Complete(KErrServerBusy)"));
	iModuleInt.Subscribe(moduleMsgStatus);
	iLocUpdInt.NotifyPositionUpdate(status);
	LowAccurRequest.TargetTime().UniversalTime();
	LowAccurRequest.TargetTime() += static_cast<TTimeIntervalSeconds>(2);
	iLSInt.SetPositionUpdateRequest(LowAccurRequest);

	WaitAndValidateNgMsg(TLbsNetInternalMsgBase::ESelfLocationRequest);
	WaitAndValidateModuleMsg(moduleMsgStatus, RAgpsModuleTestChannel::ETestMsgRequestLocationUpdate);
	
	iModuleInt.Subscribe(moduleMsgStatus);
	SendNgMsg(KSessionCompleteKErrBusy);
	
	// The error goes to the LS and a Cancel to the AGPS Module
	WaitAndValidateLocUpd(status, KErrServerBusy, ETrue);
	WaitAndValidateModuleMsg(moduleMsgStatus, RAgpsModuleTestChannel::ETestMsgCancelLocationRequest);

	//------------------------------------------------------------------------------------------------	
	INFO_PRINTF1(_L("Test 11 - Location requests from LS. Session Complete(KErrPositionHighPriorityReceive)"));
	iModuleInt.Subscribe(moduleMsgStatus);
	iLocUpdInt.NotifyPositionUpdate(status);
	LowAccurRequest.TargetTime().UniversalTime();
	LowAccurRequest.TargetTime() += static_cast<TTimeIntervalSeconds>(5);
	iLSInt.SetPositionUpdateRequest(LowAccurRequest);

	WaitAndValidateModuleMsg(moduleMsgStatus, RAgpsModuleTestChannel::ETestMsgRequestLocationUpdate);
	iModuleInt.Subscribe(moduleMsgStatus);

	SendNgMsg(KSessionCompleteKErrHighPrior);
	WaitAndValidateLocUpd(status, KErrPositionHighPriorityReceive, ETrue);
	WaitAndValidateModuleMsg(moduleMsgStatus, RAgpsModuleTestChannel::ETestMsgCancelLocationRequest);


	//------------------------------------------------------------------------------------------------	
	INFO_PRINTF1(_L("Test 11a - Location requests from LS. Session Complete(KErrGeneral). Not a special error."));
	iModuleInt.Subscribe(moduleMsgStatus);
	iLocUpdInt.NotifyPositionUpdate(status);
	LowAccurRequest.TargetTime().UniversalTime();
	LowAccurRequest.TargetTime() += static_cast<TTimeIntervalSeconds>(5);
	iLSInt.SetPositionUpdateRequest(LowAccurRequest);
	WaitAndValidateModuleMsg(moduleMsgStatus, RAgpsModuleTestChannel::ETestMsgRequestLocationUpdate);
	iModuleInt.Subscribe(moduleMsgStatus);
	
	WaitAndValidateNgMsg(TLbsNetInternalMsgBase::ESelfLocationRequest);

	SendNgMsg(KSessionCompleteKErrGeneral);
	WaitAndValidateLocUpd(status);
	WaitNoModuleMsg(moduleMsgStatus);// Make sure KErrGeneral does not calcel the location request

	//------------------------------------------------------------------------------------------------	
	INFO_PRINTF1(_L("Test 12 - Location requests from LS followed by NRH. Different time windows. Session Complete(KErrPositionHighPriorityReceive) before Self Location Request"));
	iModuleInt.Subscribe(moduleMsgStatus);
	iLocUpdInt.NotifyPositionUpdate(status);
	// LS request
	LowAccurRequest.TargetTime().UniversalTime();
	LowAccurRequest.TargetTime() += static_cast<TTimeIntervalSeconds>(10);
	iLSInt.SetPositionUpdateRequest(LowAccurRequest);

	WaitAndValidateModuleMsg(moduleMsgStatus, RAgpsModuleTestChannel::ETestMsgRequestLocationUpdate);
	iModuleInt.Subscribe(moduleMsgStatus);

	// Wait for advice and send error.
	SendNgMsg(KSessionCompleteKErrHighPrior);
	WaitAndValidateModuleMsg(moduleMsgStatus, RAgpsModuleTestChannel::ETestMsgCancelLocationRequest);	
	iModuleInt.Subscribe(moduleMsgStatus);
		
	// Expect the error to be published
	WaitAndValidateLocUpd(status, KErrPositionHighPriorityReceive, ETrue);

	// Send NRH request
	LowAccurRequest.TargetTime().UniversalTime();
	iNRHInt.SetPositionUpdateRequest(LowAccurRequest);
	WaitAndValidateModuleMsg(moduleMsgStatus, RAgpsModuleTestChannel::ETestMsgRequestLocationUpdate);

	// Process the NRH request
	iLocUpdInt.NotifyPositionUpdate(status);
	WaitAndValidateNgMsg(TLbsNetInternalMsgBase::EAssistanceDataRequest);
	WaitAndValidateLocUpd(status);

	//------------------------------------------------------------------------------------------------	
	INFO_PRINTF1(_L("Test 13 - Location requests from LS followed by NRH. Different time windows. Session Complete(KErrServerBusy) after Self Location Request"));
	iModuleInt.Subscribe(moduleMsgStatus);
	iLocUpdInt.NotifyPositionUpdate(status);
	// LS request
	LowAccurRequest.TargetTime().UniversalTime();
	LowAccurRequest.TargetTime() += static_cast<TTimeIntervalSeconds>(10);
	iLSInt.SetPositionUpdateRequest(LowAccurRequest);

	WaitAndValidateModuleMsg(moduleMsgStatus, RAgpsModuleTestChannel::ETestMsgRequestLocationUpdate);
	iModuleInt.Subscribe(moduleMsgStatus);

	// Wait for advice and Self Loc Request. Send error.
	WaitAndValidateNgMsg(TLbsNetInternalMsgBase::ESelfLocationRequest);

	SendNgMsg(KSessionCompleteKErrBusy);
	WaitAndValidateModuleMsg(moduleMsgStatus, RAgpsModuleTestChannel::ETestMsgCancelLocationRequest);	
	iModuleInt.Subscribe(moduleMsgStatus);
		
	// Expect the error to be published
	WaitAndValidateLocUpd(status, KErrServerBusy, ETrue);

	// Send NRH request
	LowAccurRequest.TargetTime().UniversalTime();
	iNRHInt.SetPositionUpdateRequest(LowAccurRequest);
	
	WaitAndValidateModuleMsg(moduleMsgStatus, RAgpsModuleTestChannel::ETestMsgRequestLocationUpdate);
	
	// Process the NRH request
	iLocUpdInt.NotifyPositionUpdate(status);
	WaitAndValidateNgMsg(TLbsNetInternalMsgBase::EAssistanceDataRequest);
	WaitAndValidateLocUpd(status);


	//------------------------------------------------------------------------------------------------	
	INFO_PRINTF1(_L("Test 13a - Location requests from LS followed by NRH. Different time windows. Session Complete(KErrServerBusy) after Self Location Request. Race condition."));
	// NRH request gets to AGPS Manager before session complete.
	iModuleInt.Subscribe(moduleMsgStatus);
	iLocUpdInt.NotifyPositionUpdate(status);
	// LS request
	LowAccurRequest.TargetTime().UniversalTime();
	LowAccurRequest.TargetTime() += static_cast<TTimeIntervalSeconds>(10);
	iLSInt.SetPositionUpdateRequest(LowAccurRequest);
	
	// Wait for advice and Self Loc Request. Send error.
	WaitAndValidateModuleMsg(moduleMsgStatus, RAgpsModuleTestChannel::ETestMsgRequestLocationUpdate);
	iModuleInt.Subscribe(moduleMsgStatus);

	WaitAndValidateNgMsg(TLbsNetInternalMsgBase::ESelfLocationRequest);
	
	// Send NRH request
	LowAccurRequest.TargetTime().UniversalTime();
	iNRHInt.SetPositionUpdateRequest(LowAccurRequest);
	
	WaitAndValidateModuleMsg(moduleMsgStatus, RAgpsModuleTestChannel::ETestMsgRequestLocationUpdate);
	iModuleInt.Subscribe(moduleMsgStatus);

	SendNgMsg(KSessionCompleteKErrBusy);
	
	// The Server busy cancels SL req, but we still have the NRH req.
	// Because of that we expect a new Loc Req instead of a Cancel
	WaitAndValidateModuleMsg(moduleMsgStatus, RAgpsModuleTestChannel::ETestMsgRequestLocationUpdate);
	
	// Expect the error to be published
	WaitAndValidateLocUpd(status, KErrServerBusy, ETrue);
	
	// Process the NRH request
	iLocUpdInt.NotifyPositionUpdate(status);
	WaitAndValidateNgMsg(TLbsNetInternalMsgBase::EAssistanceDataRequest);
	WaitAndValidateLocUpd(status);

	//------------------------------------------------------------------------------------------------	
	INFO_PRINTF1(_L("Test 14 - Location requests from LS. Tracking and NewClient flags check."));
	iLocUpdInt.NotifyPositionUpdate(status);
	TrackingNewClientRequest.TargetTime().UniversalTime();
	iLSInt.SetPositionUpdateRequest(TrackingNewClientRequest);

	// Wait for advice and Self Loc Request. Send error.
	WaitAndValidateNgMsg(TLbsNetInternalMsgBase::ESystemStatusAdvice, ETrue);
	WaitAndValidateNgMsg(TLbsNetInternalMsgBase::ESelfLocationRequest, ETrue);
	WaitAndValidateLocUpd(status);
	SendNgMsg(KSessionCompleteKErrNone);
	
	//------------------------------------------------------------------------------------------------	
	INFO_PRINTF1(_L("Test 15 - Tracking On/Off using Status message."));
	
	iLSInt.SetPositionUpdateRequest(StatusTrackingOffRequest);
	WaitAndValidateNgMsg(TLbsNetInternalMsgBase::ESystemStatusAdvice, EFalse);
	
	iLSInt.SetPositionUpdateRequest(StatusTrackingOnRequest);
	WaitAndValidateNgMsg(TLbsNetInternalMsgBase::ESystemStatusAdvice, ETrue);

	//------------------------------------------------------------------------------------------------	
	INFO_PRINTF1(_L("Test 16 - Tracking On/Off using Cancel message."));
	
	iLSInt.SetPositionUpdateRequest(CancelTrackingOffRequest);
	WaitAndValidateNgMsg(TLbsNetInternalMsgBase::ESystemStatusAdvice, EFalse);
	
	iLSInt.SetPositionUpdateRequest(CancelTrackingOnRequest);
	WaitAndValidateNgMsg(TLbsNetInternalMsgBase::ESystemStatusAdvice, ETrue);

	//------------------------------------------------------------------------------------------------	
	INFO_PRINTF1(_L("Test 17 - Location requests from LS followed by Cancel before Self Location Request"));
	iModuleInt.Subscribe(moduleMsgStatus);
	LowAccurRequest.TargetTime().UniversalTime();
	LowAccurRequest.TargetTime() += static_cast<TTimeIntervalSeconds>(10);
	iLSInt.SetPositionUpdateRequest(LowAccurRequest);

	WaitAndValidateModuleMsg(moduleMsgStatus, RAgpsModuleTestChannel::ETestMsgRequestLocationUpdate);
	WaitAndValidateNgMsg(TLbsNetInternalMsgBase::ESystemStatusAdvice);
	
	iModuleInt.Subscribe(moduleMsgStatus);
	iLSInt.SetPositionUpdateRequest(CancelTrackingOffRequest);
	WaitAndValidateModuleMsg(moduleMsgStatus, RAgpsModuleTestChannel::ETestMsgCancelLocationRequest);
	
	//------------------------------------------------------------------------------------------------	
	INFO_PRINTF1(_L("Test 18 - Location requests from LS followed by Cancel after Self Location Request"));
	iModuleInt.Subscribe(moduleMsgStatus);
	LowAccurRequest.TargetTime().UniversalTime();
	LowAccurRequest.TargetTime() += static_cast<TTimeIntervalSeconds>(10);
	iLSInt.SetPositionUpdateRequest(LowAccurRequest);

	WaitAndValidateModuleMsg(moduleMsgStatus, RAgpsModuleTestChannel::ETestMsgRequestLocationUpdate);
	WaitAndValidateNgMsg(TLbsNetInternalMsgBase::ESelfLocationRequest);

	iModuleInt.Subscribe(moduleMsgStatus);
	iLSInt.SetPositionUpdateRequest(CancelTrackingOffRequest);
	WaitAndValidateNgMsg(TLbsNetInternalMsgBase::ESelfLocationCancel);
	WaitAndValidateModuleMsg(moduleMsgStatus, RAgpsModuleTestChannel::ETestMsgCancelLocationRequest);

	//------------------------------------------------------------------------------------------------	
	INFO_PRINTF1(_L("Test 19 - Status Trackin On msg. Tracking Off due to no Location Requests"));
	iLSInt.SetPositionUpdateRequest(StatusTrackingOnRequest);
	WaitAndValidateNgMsg(TLbsNetInternalMsgBase::ESystemStatusAdvice, ETrue);
	
	//Tracking off because we haven't sent any Loc Req
	WaitAndValidateNgMsg(TLbsNetInternalMsgBase::ESystemStatusAdvice, EFalse);
	
	//------------------------------------------------------------------------------------------------	
	INFO_PRINTF1(_L("Test 20 - LS Request with Tracking msg. Tracking Off due to no Location Requests"));
	iLocUpdInt.NotifyPositionUpdate(status);
	TrackingNewClientRequest.TargetTime().UniversalTime();
	iLSInt.SetPositionUpdateRequest(TrackingNewClientRequest);

	// Wait for advice and Self Loc Request. Send error.
	WaitAndValidateNgMsg(TLbsNetInternalMsgBase::ESystemStatusAdvice, ETrue);
	WaitAndValidateNgMsg(TLbsNetInternalMsgBase::ESelfLocationRequest, ETrue);
	WaitAndValidateLocUpd(status);
	
	//Tracking off because we haven't sent a new Loc Req
	WaitAndValidateNgMsg(TLbsNetInternalMsgBase::ESystemStatusAdvice, EFalse);
	SendNgMsg(KSessionCompleteKErrNone);
	
	//------------------------------------------------------------------------------------------------	
	INFO_PRINTF1(_L("Test 21 - Location requests from LS and NRH followed by AssistanceDataEvent(KErrNotFound)."));
	INFO_PRINTF1(_L("          LocationUpdate(KErrNotFound) expected. AGPS should internally invalidate all outstanding requests."));
	iModuleInt.Subscribe(moduleMsgStatus);
	iLocUpdInt.NotifyPositionUpdate(status);
	HighAccurRequest.TargetTime().UniversalTime();
	iLSInt.SetPositionUpdateRequest(HighAccurRequest);
	
	INFO_PRINTF1(_L("          LS -> AGPS Manager. High Accuracy Request. Target Time: now."));
	
	WaitAndValidateModuleMsg(moduleMsgStatus, RAgpsModuleTestChannel::ETestMsgRequestLocationUpdate);
	iModuleInt.Subscribe(moduleMsgStatus);
	
	HighAccurRequest.TargetTime().UniversalTime();
	iNRHInt.SetPositionUpdateRequest(HighAccurRequest);
	
	INFO_PRINTF1(_L("          NRH -> AGPS Manager. High Accuracy Request. Target Time: now."));
	
	WaitAndValidateModuleMsg(moduleMsgStatus, RAgpsModuleTestChannel::ETestMsgRequestLocationUpdate);
	iModuleInt.Subscribe(moduleMsgStatus);
	
	INFO_PRINTF1(_L("          AGPS Manager -> Network Gateway. Self Location Request."));
	WaitAndValidateNgMsg(TLbsNetInternalMsgBase::ESelfLocationRequest);
	
	// Set the error code that should be returned in the locationUpdate
	iUpdateCodeProperty.Set(KErrNotFound);
	
	INFO_PRINTF1(_L("          Network Gateway -> AGPS Manager. Response: KAssistanceDataResponseErrNotFound"));
	SendNgMsg(KAssistanceDataResponseErrNotFound);

	INFO_PRINTF1(_L("          AGPS Manager -> Test GPS Integration Module. AssistanceDataEvent"));
	WaitAndValidateModuleMsg(moduleMsgStatus, RAgpsModuleTestChannel::ETestMsgAssistanceDataEvent);
	iModuleInt.Subscribe(moduleMsgStatus);
	
	INFO_PRINTF1(_L("Test GPS Integration Module -> AGPS Manager. Location Update with KErrNotFound"));
	
	WaitAndValidateLocUpd(status, KErrNotFound);
		
	// Test that the recent location update invalidated all requests in the AGPS Manager
	// If it did, then the following call should cause explicit Cancel to be sent to the
	// AGPS Module.
	iLSInt.SetPositionUpdateRequest(CancelTrackingOffRequest);
	WaitAndValidateModuleMsg(moduleMsgStatus, RAgpsModuleTestChannel::ETestMsgCancelLocationRequest);
	iModuleInt.Subscribe(moduleMsgStatus);
	
	WaitNoModuleMsg(moduleMsgStatus);
	iUpdateCodeProperty.Set(KErrNone);

	
	//------------------------------------------------------------------------------------------------	
	INFO_PRINTF1(_L("Test 21a - Location requests from LS and NRH followed by AssistanceDataEvent(KErrNotFound)."));
	INFO_PRINTF1(_L("           LocationUpdate(KErrNotFound) expected. AGPS should internally invalidate all outstanding requests."));
	// It is like test 21, just we send a Cancel using the NRH channel
	iModuleInt.Subscribe(moduleMsgStatus);
	iLocUpdInt.NotifyPositionUpdate(status);
	HighAccurRequest.TargetTime().UniversalTime();
	iLSInt.SetPositionUpdateRequest(HighAccurRequest);
	
	WaitAndValidateModuleMsg(moduleMsgStatus, RAgpsModuleTestChannel::ETestMsgRequestLocationUpdate);
	iModuleInt.Subscribe(moduleMsgStatus);
	
	iNRHInt.SetPositionUpdateRequest(HighAccurRequest);
	
	WaitAndValidateModuleMsg(moduleMsgStatus, RAgpsModuleTestChannel::ETestMsgRequestLocationUpdate);
	iModuleInt.Subscribe(moduleMsgStatus);
	
	WaitAndValidateNgMsg(TLbsNetInternalMsgBase::ESelfLocationRequest);
	// Set the error code that should be returned in the locationUpdate
	iUpdateCodeProperty.Set(KErrNotFound);
	SendNgMsg(KAssistanceDataResponseErrNotFound);

	WaitAndValidateModuleMsg(moduleMsgStatus, RAgpsModuleTestChannel::ETestMsgAssistanceDataEvent);
	iModuleInt.Subscribe(moduleMsgStatus);
	
	WaitAndValidateLocUpd(status, KErrNotFound);
		
	// Test that the recent location update invalidated all requests in the AGPS Manager
	// If it did, then the following call should cause explicit Cancel to be sent to the
	// AGPS Mdoule.
	iNRHInt.SetPositionUpdateRequest(CancelTrackingOffRequest);
	WaitAndValidateModuleMsg(moduleMsgStatus, RAgpsModuleTestChannel::ETestMsgCancelLocationRequest);
	iModuleInt.Subscribe(moduleMsgStatus);
	
	WaitNoModuleMsg(moduleMsgStatus);
	SendNgMsg(KSessionCompleteKErrNone);
	iUpdateCodeProperty.Set(KErrNone);
	
	//------------------------------------------------------------------------------------------------	
	INFO_PRINTF1(_L("Test 22 - Two back-to-back location requests from LS (only one SessionComplete). SelfLoc and AsistData requests expected"));
	iModuleInt.Subscribe(moduleMsgStatus);
	iLocUpdInt.NotifyPositionUpdate(status);
	LowAccurRequest.TargetTime().UniversalTime();
	
	// 1st req
	iLSInt.SetPositionUpdateRequest(LowAccurRequest);
	WaitAndValidateModuleMsg(moduleMsgStatus, RAgpsModuleTestChannel::ETestMsgRequestLocationUpdate);
	iModuleInt.Subscribe(moduleMsgStatus);
	WaitAndValidateNgMsg(TLbsNetInternalMsgBase::ESelfLocationRequest);
	
	// 2nd req
	iLSInt.SetPositionUpdateRequest(LowAccurRequest);
	WaitAndValidateModuleMsg(moduleMsgStatus, RAgpsModuleTestChannel::ETestMsgRequestLocationUpdate);
	WaitAndValidateNgMsg(TLbsNetInternalMsgBase::EAssistanceDataRequest);
	
	WaitAndValidateLocUpd(status);
	SendNgMsg(KSessionCompleteKErrNone);


	//------------------------------------------------------------------------------------------------	
	INFO_PRINTF1(_L("Test 22a - Two location requests from LS (only one SessionComplete). SelfLoc and AsistData requests expected"));
	iModuleInt.Subscribe(moduleMsgStatus);
	iLocUpdInt.NotifyPositionUpdate(status);
	
	// 1st req
	LowAccurRequest.TargetTime().UniversalTime();
	iLSInt.SetPositionUpdateRequest(LowAccurRequest);
	WaitAndValidateModuleMsg(moduleMsgStatus, RAgpsModuleTestChannel::ETestMsgRequestLocationUpdate);
	iModuleInt.Subscribe(moduleMsgStatus);
	WaitAndValidateNgMsg(TLbsNetInternalMsgBase::ESelfLocationRequest);
	
	WaitAndValidateLocUpd(status);
	iLocUpdInt.NotifyPositionUpdate(status);
	
	// 2nd req
	LowAccurRequest.TargetTime().UniversalTime();
	iLSInt.SetPositionUpdateRequest(LowAccurRequest);
	WaitAndValidateModuleMsg(moduleMsgStatus, RAgpsModuleTestChannel::ETestMsgRequestLocationUpdate);
	WaitAndValidateNgMsg(TLbsNetInternalMsgBase::EAssistanceDataRequest);
	
	WaitAndValidateLocUpd(status);
	SendNgMsg(KSessionCompleteKErrNone);


	//------------------------------------------------------------------------------------------------	
	INFO_PRINTF1(_L("Test 23 - Two back-to-back New Client location requests from LS (only one SessionComplete). Two SelfLocs expected"));
	iModuleInt.Subscribe(moduleMsgStatus);
	iLocUpdInt.NotifyPositionUpdate(status);
	TrackingNewClientRequest.TargetTime().UniversalTime();
	
	// 1st req
	iLSInt.SetPositionUpdateRequest(TrackingNewClientRequest);
	WaitAndValidateNgMsg(TLbsNetInternalMsgBase::ESystemStatusAdvice, ETrue);
	WaitAndValidateModuleMsg(moduleMsgStatus, RAgpsModuleTestChannel::ETestMsgRequestLocationUpdate);
	WaitAndValidateNgMsg(TLbsNetInternalMsgBase::ESelfLocationRequest, ETrue);
	iModuleInt.Subscribe(moduleMsgStatus);
	
	// 2nd req
	iLSInt.SetPositionUpdateRequest(TrackingNewClientRequest);
	WaitAndValidateModuleMsg(moduleMsgStatus, RAgpsModuleTestChannel::ETestMsgRequestLocationUpdate);
	WaitAndValidateNgMsg(TLbsNetInternalMsgBase::ESelfLocationRequest, ETrue);
	
	WaitAndValidateLocUpd(status);
	SendNgMsg(KSessionCompleteKErrNone);


	//------------------------------------------------------------------------------------------------	
	INFO_PRINTF1(_L("Test 23a - Two New Client location requests from LS (only one SessionComplete). Two SelfLocs expected"));
	iModuleInt.Subscribe(moduleMsgStatus);
	iLocUpdInt.NotifyPositionUpdate(status);

	// 1st req
	TrackingNewClientRequest.TargetTime().UniversalTime();
	iLSInt.SetPositionUpdateRequest(TrackingNewClientRequest);
	WaitAndValidateModuleMsg(moduleMsgStatus, RAgpsModuleTestChannel::ETestMsgRequestLocationUpdate);
	iModuleInt.Subscribe(moduleMsgStatus);
	WaitAndValidateNgMsg(TLbsNetInternalMsgBase::ESelfLocationRequest, ETrue);
	
	WaitAndValidateLocUpd(status);
	iLocUpdInt.NotifyPositionUpdate(status);
	
	// 2nd req
	TrackingNewClientRequest.TargetTime().UniversalTime();
	iLSInt.SetPositionUpdateRequest(TrackingNewClientRequest);
	WaitAndValidateModuleMsg(moduleMsgStatus, RAgpsModuleTestChannel::ETestMsgRequestLocationUpdate);
	WaitAndValidateNgMsg(TLbsNetInternalMsgBase::ESelfLocationRequest, ETrue);
	
	WaitAndValidateLocUpd(status);
	SendNgMsg(KSessionCompleteKErrNone);

	//------------------------------------------------------------------------------------------------
	INFO_PRINTF1(_L("Test 24 - Location request from LS (absurd MaxFixTime)"));
	iModuleInt.Subscribe(moduleMsgStatus);
	iLocUpdInt.NotifyPositionUpdate(status);
	absurdMaxFixTimeRequest.TargetTime().UniversalTime();
	absurdMaxFixTimeRequest.TargetTime() = absurdMaxFixTimeRequest.TargetTime() + KFutureReqDelay;
	iLSInt.SetPositionUpdateRequest(absurdMaxFixTimeRequest);
	
	WaitAndValidateNgMsg(TLbsNetInternalMsgBase::ESystemStatusAdvice, EFalse);
	WaitAndValidateNgMsg(TLbsNetInternalMsgBase::ESelfLocationRequest);
	WaitAndValidateModuleMsg(moduleMsgStatus, RAgpsModuleTestChannel::ETestMsgRequestLocationUpdate);
	iModuleInt.Subscribe(moduleMsgStatus);
	WaitAndValidateNoLocUpd(status);
	iLSInt.SetPositionUpdateRequest(CancelTrackingOffRequest);
	WaitAndValidateModuleMsg(moduleMsgStatus, RAgpsModuleTestChannel::ETestMsgCancelLocationRequest);
	WaitAndValidateNgMsg(TLbsNetInternalMsgBase::ESelfLocationCancel);
	SendNgMsg(KSessionCompleteKErrNone);
	//------------------------------------------------------------------------------------------------	
	iUpdateCodeProperty.Set(KErrNone);
	return TestStepResult();
	}



TVerdict Cman4Step::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CTe_manSuiteStepBase::doTestStepPostambleL();
	// Open interfaces
	INFO_PRINTF1(_L("Closing Interfaces"));
	//iADCache.Close();
	iMSInt.Close();;
	iLSInt.Close();
	iNRHInt.Close();
	iLocUpdInt.Close();
	iModuleInt.Close();
	iUpdateCodeProperty.Close();	
	iUpdateFixTimeProperty.Close();	
	INFO_PRINTF1(_L("Interfaces Closed OK"));

	return TestStepResult();
	}

//-----------------------------------------------------------------------------------------
void Cman4Step::WaitAndValidateLocUpd(TRequestStatus& aStatus, TInt aError, TBool aConflictControl)
	{
	User::WaitForRequest(aStatus);
	
	if (aStatus.Int() == KErrNone)	
		{
		INFO_PRINTF1(_L("   Location update received"));
		
		TPositionExtendedSatelliteInfo satInfo;
		TTime actualTime;
		TBool conflictControl;
		TInt err = iLocUpdInt.GetPositionInfo(conflictControl, &satInfo, sizeof(satInfo), iRcvdTargetTime, actualTime);
		INFO_PRINTF2(_L("      Exp error code: %d"), aError);
		INFO_PRINTF2(_L("      Rcv error code: %d"), err);
					
		if (aError != err)
			{
			SetTestStepResult(EFail);
			ERR_PRINTF1(_L("   <font color=FF0000><b>Actual location update error does not match the expected one.</b></font>"));
			}
			
		INFO_PRINTF2(_L("      Exp conflict control flag: %d"), aConflictControl);
		INFO_PRINTF2(_L("      Rcv conflict control flag: %d"), conflictControl);
					
		if (aConflictControl != conflictControl)
			{
			SetTestStepResult(EFail);
			ERR_PRINTF1(_L("   <font color=FF0000><b>Actual conflict control flag does not match the expected one.</b></font>"));
			}

		iUpdateFixTimeProperty.Get(iRcvdFixTime);
		}
	else
		{
		SetTestStepResult(EFail);
		ERR_PRINTF2(_L("   <font color=FF0000><b>Location update status error: %d</b></font>"), aStatus.Int());
		}
	}


//-----------------------------------------------------------------------------------------
void Cman4Step::WaitAndValidateNoLocUpd(TRequestStatus& aStatus)
	{
	User::After(KNoLocUpdTimeout);
	
	if (aStatus.Int() != KRequestPending)	
		{
		ERR_PRINTF1(_L("   <font color=FF0000><b>Unexpected location update received</b></font>"));
		
		TPositionExtendedSatelliteInfo satInfo;
		TTime actualTime;
		TBool conflictControl;
		TInt err = iLocUpdInt.GetPositionInfo(conflictControl, &satInfo, sizeof(satInfo), iRcvdTargetTime, actualTime);
		INFO_PRINTF2(_L("      Rcv error code: %d"), err);
		INFO_PRINTF2(_L("      Rcv conflict control flag: %d"), conflictControl);
		SetTestStepResult(EFail);
		}
	else
		{
		INFO_PRINTF1(_L("   No location update received (as expected)"));
		iLocUpdInt.CancelNotifyPositionUpdate();
		User::WaitForRequest(aStatus);
		}
	}


//-----------------------------------------------------------------------------------------
void Cman4Step::WaitAndValidateModuleMsg(TRequestStatus& aStatus, const RAgpsModuleTestChannel::TTestMsgType& aMsg)
	{
	User::WaitForRequest(aStatus);
	
	if (aStatus.Int() == KErrNone)	
		{
		INFO_PRINTF1(_L("   Module message received"));
		
		RAgpsModuleTestChannel::TTestMsgType msg;
		TInt err = iModuleInt.GetMsg(msg);
		INFO_PRINTF2(_L("      Exp error code: %d"), KErrNone);
		INFO_PRINTF2(_L("      Rcv error code: %d"), err);
					
		if (KErrNone != err)
			{
			SetTestStepResult(EFail);
			ERR_PRINTF1(_L("   <font color=FF0000><b>Actual error code does not match the expected one.</b></font>"));
			}
			
		INFO_PRINTF2(_L("      Exp msg type: %d"), aMsg);
		INFO_PRINTF2(_L("      Rcv msg type: %d"), msg);
					
		if (msg != aMsg)
			{
			SetTestStepResult(EFail);
			ERR_PRINTF1(_L("   <font color=FF0000><b>Actual msg type does not match the expected one.</b></font>"));
			}

		}
	else
		{
		SetTestStepResult(EFail);
		ERR_PRINTF2(_L("   <font color=FF0000><b>Status error: %d</b></font>"), aStatus.Int());
		}
	}


//-----------------------------------------------------------------------------------------
/** Make sure the module does not report any message */
void Cman4Step::WaitNoModuleMsg(TRequestStatus& aStatus)
	{
	User::After(KNoModuleMsgTimeout);

	if (aStatus != KRequestPending)
		{
		ERR_PRINTF1(_L("   <font color=FF0000><b>Unexpected module message received</b></font>"));
		INFO_PRINTF2(_L("     Status error: %d"), aStatus.Int());
		
		RAgpsModuleTestChannel::TTestMsgType msg;
		TInt err = iModuleInt.GetMsg(msg);
		INFO_PRINTF2(_L("      Rcv error code: %d"), err);
		INFO_PRINTF2(_L("      Rcv msg type: %d"), msg);
		
		SetTestStepResult(EFail);		
		}
	else
		{
		INFO_PRINTF1(_L("   No Module msg received (as expected)"));
		iModuleInt.Cancel();
		User::WaitForRequest(aStatus);
		}
	}
