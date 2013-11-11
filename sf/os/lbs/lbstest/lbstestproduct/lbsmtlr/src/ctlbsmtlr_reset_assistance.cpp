// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// @file ctlbsmtlr_reset_assistance.cpp
// 
//

// EPOC includes.
#include <e32property.h>


// LBS test includes.
#include "ctlbsmtlr_reset_assistance.h"
#include <lbs/test/tlbsutils.h>

_LIT8(KTestRequesterId, "MT-LR test requester id");
_LIT8(KTestClientName, "MT-LR test client name");
_LIT8(KTestClientExternalId, "MT-LR test client external id");

#define REFPOS_LAT				52.2
#define REFPOS_LONG				0.2
#define REFPOS_ALT				10
#define REFPOS_HORZ_ACCURACY	1000
#define REFPOS_VERT_ACCURACY	1000

const TInt KNumberOfRequests = 10;
const TInt KMaxFixTime = 40 * 1000 * 1000;
const TInt KHorAcc = 50;
const TInt KVerAcc = 100;
const TInt KWaitTimeForCoolDown = 10 * 1000 * 1000; // how long to wait to make sure the integration module gets turned off

/**
Static Constructor
*/
CT_LbsMTLRResetAssistance* CT_LbsMTLRResetAssistance::New(CT_LbsMTLRServer& aParent)
	{
	// Note the lack of ELeave.
	// This means that having insufficient memory will return NULL;
	CT_LbsMTLRResetAssistance* testStep = new CT_LbsMTLRResetAssistance(aParent);
	if (testStep)
		{
		TInt err = KErrNone;

		TRAP(err, testStep->ConstructL());
		if (err)
			{
			delete testStep;
			testStep = NULL;
			}
		}

	return testStep;
	}


/**
 * Constructor
 */
CT_LbsMTLRResetAssistance::CT_LbsMTLRResetAssistance(CT_LbsMTLRServer& aParent)
	: CT_LbsMTLRStep(aParent), iController(NULL), iWaiter(NULL) 
	{
	SetTestStepName(KLbsMTLRResetAssistace);
	}


void CT_LbsMTLRResetAssistance::ConstructL()
	{
	// Create the base class objects.
	CT_LbsMTLRStep::ConstructL();
	iController = CLbsPrivacyController::NewL(*this);
	iWaiter = CT_LbsAsyncWaiter::NewL();
	}


/**
 * Destructor
 */
CT_LbsMTLRResetAssistance::~CT_LbsMTLRResetAssistance()
	{
	delete iController;
	delete iWaiter;
	}


/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
TVerdict CT_LbsMTLRResetAssistance::doTestStepL()
	{
	// Generic test step used to test the LBS Client Notify position update API.
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsMTLRResetAssistance::doTestStepL()"));

	if (TestStepResult() == EPass)
		{		
		
		// Setup netsim.
		User::LeaveIfError(OpenNetSim());

		TTimeIntervalMicroSeconds resetRequestTime;
		resetRequestTime = DoRequests(EFalse, ETrue);
		INFO_PRINTF2(_L("Requests from warm with reset assistance data took on average %d miliseconds to complete."), (resetRequestTime.Int64()/KNumberOfRequests)/1000);
		
		TTimeIntervalMicroSeconds coldRequestTime;
		coldRequestTime = DoRequests(ETrue, EFalse);
		INFO_PRINTF2(_L("Requests from cold took on average %d miliseconds to complete."), (coldRequestTime.Int64()/KNumberOfRequests)/1000);
		
		TTimeIntervalMicroSeconds warmRequestTime;
		warmRequestTime = DoRequests(EFalse, EFalse);
		INFO_PRINTF2(_L("Requests from warm took on average %d miliseconds to complete."), (warmRequestTime.Int64()/KNumberOfRequests)/1000);

		INFO_PRINTF2(_L("Requests from warm with reset assistance data took on average %d miliseconds to complete."), (resetRequestTime.Int64()/KNumberOfRequests)/1000);
		INFO_PRINTF2(_L("Requests from cold took on average %d miliseconds to complete."), (coldRequestTime.Int64()/KNumberOfRequests)/1000);
/*		if(warmRequestTime >= resetRequestTime/2 || resetRequestTime < (coldRequestTime*3)/4)
			{
			INFO_PRINTF1(_L("Reseting assistance data didn't fit into the right time frame"));
			SetTestStepResult(EFail);
			}
*/
		CloseNetSim();
		}

	INFO_PRINTF1(_L("&lt;&lt;CT_LbsMTLRResetAssistance::doTestStepL()"));

	return TestStepResult();
	}


void CT_LbsMTLRResetAssistance::ProcessNetworkLocationRequest(TUint aRequestId, const TLbsExternalRequestInfo& /*aRequestInfo*/, const TNotificationType& /*aNotificationType*/)
	{
	INFO_PRINTF1(_L("&gt;&gt;			ProcessNetworkLocationRequest()"));
	iController->RespondNetworkLocationRequest(aRequestId, CLbsPrivacyController::ERequestAccepted);
	}
   
void CT_LbsMTLRResetAssistance::ProcessNetworkPositionUpdate(TUint /*aRequestId*/, const TPositionInfo& /*aPosInfo*/)
	{
	INFO_PRINTF1(_L("&gt;&gt; Privacy App - ProcessNetworkPositionUpdate()"));
	}

void CT_LbsMTLRResetAssistance::ProcessRequestComplete(TUint /*aRequestId*/, TInt /*aReason*/)
	{
	INFO_PRINTF1(_L("&gt;&gt; Privacy App - ProcessRequestComplete()"));
	}

void CT_LbsMTLRResetAssistance::NotifyReleaseLcsLocationNotification(const CLbsNetworkProtocolBase::TLbsPrivacyResponse& aResult)
	{
	if(aResult == CLbsNetworkProtocolBase::EPrivacyResponseAccepted)
		{
		INFO_PRINTF1(_L("privacy request accepted"));
		TRequestStatus* status = &iWaiter->iStatus; 
		User::RequestComplete(status, KErrNone);
		}
	else
		{
		INFO_PRINTF2(_L("privacy request rejected with code %d"), aResult);
		TRequestStatus* status = &iWaiter->iStatus; 
		User::RequestComplete(status, KErrAccessDenied);
		}
	}

void CT_LbsMTLRResetAssistance::NotifyError(MLbsNetSimTestObserver::EFunction /*aFunction*/, int aError)
	{
	INFO_PRINTF2(_L("notified with error %d"), aError);
	TRequestStatus* status = &iWaiter->iStatus; 
	User::RequestComplete(status, aError);
	}

void CT_LbsMTLRResetAssistance::NotifyMeasurementReportLocation(const TPositionInfo& aPosition)
	{
	INFO_PRINTF1(_L("position obtained successfully"));
	const TPositionInfo& posInfo = reinterpret_cast<const TPositionInfo&>(aPosition);

	TPosition pos;
	posInfo.GetPosition(pos);
	TReal32 lat 	= pos.Latitude();
	TReal32 lng 	= pos.Longitude();
	TReal32 hac  	= pos.HorizontalAccuracy();
	TReal vac		= pos.VerticalAccuracy();

	INFO_PRINTF2(_L("lat: %f"),lat);
	INFO_PRINTF2(_L("lng: %f"),lng);
	INFO_PRINTF2(_L("hac: %f"),hac);
	INFO_PRINTF2(_L("vac: %f"),vac);
	TRequestStatus* status = &iWaiter->iStatus; 
	User::RequestComplete(status, KErrNone);
	}

void CT_LbsMTLRResetAssistance::NotifyMeasurementReportControlFailure(TInt aReason)
	{
	INFO_PRINTF2(_L("position update failed with error %d"), aReason);
	TRequestStatus* status = &iWaiter->iStatus; 
	User::RequestComplete(status, aReason);
	}

TInt CT_LbsMTLRResetAssistance::OpenNetSim()
	{
	// Connect to net sim.
	TInt err = iNetSim.ConnectL(this);
	if (err)
		{
		return err;
		}

	TPosition refPos;
	// Set the reference position, which also be used for any required verification.
	refPos.SetCoordinate(REFPOS_LAT, REFPOS_LONG, REFPOS_ALT);
	refPos.SetAccuracy(REFPOS_HORZ_ACCURACY, REFPOS_VERT_ACCURACY);
	refPos.SetCurrentTime();

	if (!iNetSim.SetReferenceLocation(refPos))
		{
		iNetSim.Close();
		return KErrGeneral;
		}	

	// Set plugin to use.
	TUid pluginUid;
//	pluginUid = TUid::Uid(KSimpleAssistanceDataProviderPluginUidValue);
	pluginUid = TUid::Uid(KSuplAssistanceDataProviderPluginUidValue);

	if (!iNetSim.SetAssistanceDataProvider(pluginUid))
		{
		iNetSim.Close();
		return KErrGeneral;
		}
		
	TLbsNetPosRequestQuality netPosQuality;
	//Set the position quality required by the network.
	TTimeIntervalMicroSeconds maxFixTime(KMaxFixTime);
	netPosQuality.SetMaxFixTime(maxFixTime);
	netPosQuality.SetMinHorizontalAccuracy(KHorAcc);
	netPosQuality.SetMinVerticalAccuracy(KVerAcc);

	if (!iNetSim.SetQuality(netPosQuality))
		{
		INFO_PRINTF1(_L("Failed test, can't set NetSim's quality."));
		SetTestStepResult(EFail);
		iNetSim.Close();
		return TestStepResult();
		}

	iNetSim.SetEmergenyStatus(EFalse);
	iNetSim.SetRoamingStatus(EFalse);
    return err;
	}

void CT_LbsMTLRResetAssistance::CloseNetSim()
	{
	iNetSim.ClearAssistanceDataFilters();
	iNetSim.Close();
	}

TInt64 CT_LbsMTLRResetAssistance::DoRequests(TBool aCold, TBool aReset)
	{
	TLbsNetPosRequestPrivacy reqType;
	TLbsExternalRequestInfo reqInfo; 	
	reqInfo.SetRequesterId(KTestRequesterId);	
	reqInfo.SetClientName(KTestClientName);
	reqInfo.SetClientExternalId(KTestClientExternalId);			

	reqType.SetRequestAction(TLbsNetPosRequestPrivacy::ERequestActionAllow); 
	reqType.SetRequestAdvice(TLbsNetPosRequestPrivacy::ERequestAdviceVerify);

	if(!aCold)
		{
		// *** Warming up module				
		// Open positioner.

		// Request position.
		TInt err = GetLocationL(reqType, reqInfo);
		if (KErrNone != err)
			{
			INFO_PRINTF2(_L("Failed test, position request returned err %d."), err);
			SetTestStepResult(EFail);
			}		
		}

	TTimeIntervalMicroSeconds requestTime;
	requestTime = 0;
	// *** Doing the repeated requests from warm with reset assistance data				
	for(TInt count = 0; count < KNumberOfRequests; ++count)
		{
		if(aReset)
			{
			iNetSim.SendResetAssistanceData(0xFFFFFFFF);
			}
		
		TTime startTime;
		startTime.UniversalTime(); // start the clock

		// Request position.
		TInt err = GetLocationL(reqType, reqInfo);
		if(KErrNone != err)
			{
			INFO_PRINTF2(_L("Failed test, position request returned err %d."), err);
			SetTestStepResult(EFail);
			}		

		TTime endTime;
		endTime.UniversalTime(); // stop the clock
		TTimeIntervalMicroSeconds interval;
		interval = endTime.MicroSecondsFrom(startTime);
		INFO_PRINTF2(_L("Request took %d microseconds to complete."), interval.Int64());
		requestTime = requestTime.Int64() + interval.Int64();

		if(aCold)
			{
			User::After(KWaitTimeForCoolDown);
			}
		}			

	if(!aCold)
		{
		// Cleanup.
		User::After(KWaitTimeForCoolDown);
		}
	return requestTime.Int64();
	}

TInt CT_LbsMTLRResetAssistance::GetLocationL(TLbsNetPosRequestPrivacy aReqType,  
							TLbsExternalRequestInfo aReqInfo)
	{
	// kick off the privacy request:
	iNetSim.StartNetworkPrivacyRequest(aReqType, aReqInfo);

	iWaiter->iStatus = KRequestPending;
	// Wait for result of the privacy request
	iWaiter->StartAndWait();
	
	TInt err = iWaiter->Result();
	if (KErrNone == err)
		{ // if privacy request is accepted then do a location request
		iNetSim.StartNetworkLocationRequest();
		
		iWaiter->iStatus = KRequestPending;
		// Wait for result of the privacy request
		iWaiter->StartAndWait();
		err = iWaiter->Result();
		}

	return err;
	}
