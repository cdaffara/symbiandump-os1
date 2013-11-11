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
// This is the class implementation for the Notify Position Update Tests
// 
//
 
#include "ctlbsclientstep_reset_assistance.h"

#include <lbs.h>
#include <lbssatellite.h>

#include <lbs/test/tlbsutils.h>
#include <lbs/test/ctlbsasyncwaiter.h>


const TTimeIntervalMicroSeconds KModuleTimeout = 6000000;//makes test module time out(which should be more than the Maxtime in lbsprofile.ini)

const TInt KNumberOfRequests = 10; // the number of requests to do to be near the average

const TInt KWaitTimeForCoolDown = 10 * 1000 * 1000; // how long to wait to make sure the integration module gets turned off

/**
 *	Sets a Requestor for now - TODO will be removed with new location server
 */
void CT_LbsClientStep_ResetAssistance::TempSetRequestorL()
 	{	
	User::LeaveIfError(iPositioner.SetRequestor(	CRequestor::ERequestorService,
												CRequestor::EFormatApplication,
												_L("Tom Tom")));
 	}


/**
 * Destructor
 */
CT_LbsClientStep_ResetAssistance::~CT_LbsClientStep_ResetAssistance()
	{
	delete 	iLbsAdmin;	
	}


/**
 * Constructor
 */
CT_LbsClientStep_ResetAssistance::CT_LbsClientStep_ResetAssistance(CT_LbsClientServer& aParent) : CT_LbsClientStep(aParent),iLbsAdmin(NULL),iRequestedAssData(FALSE), iReceivedAssData(FALSE)
	{
	
	SetTestStepName(KLbsClientStep_ResetAssistance);
	}


/**
Static Constructor
*/
CT_LbsClientStep_ResetAssistance* CT_LbsClientStep_ResetAssistance::New(CT_LbsClientServer& aParent)
	{
	
	// Note the lack of ELeave.
	// This means that having insufficient memory will return NULL;
	
	CT_LbsClientStep_ResetAssistance* testStep = new CT_LbsClientStep_ResetAssistance(aParent);
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


void CT_LbsClientStep_ResetAssistance::ConstructL()
	{
	// create the admin
	iLbsAdmin = CLbsAdmin::NewL();

	}

//from MT_NotifyPosUpdateObserver:
/**
 * 	Callback - called when NotifyPositionUpdate request completes
 */

void CT_LbsClientStep_ResetAssistance::NotifyPositionUpdateCallback(TRequestStatus& aStatus)
	{
	TInt err = aStatus.Int();
	if (KErrCancel != err)	
		{
		SetTestStepResult(EFail); // the request always gets cancelled (see test 0122)
		}
	CActiveScheduler::Stop();
	}

/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
TVerdict CT_LbsClientStep_ResetAssistance::doTestStepL()
	{
	// Generic test step used to test the LBS Client Notify position update API.
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsClientStep_ResetAssistance::doTestStepL()"));

	if (TestStepResult()==EPass)
		{
		TInt err = KErrNone;
		
		iParent.iSharedData->iTestModuleInUse = EFalse;

		// Connect to self locate server.
		err = iServer.Connect();
		User::LeaveIfError(err);
		CleanupClosePushL(iServer);
		
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
			
		// All done, clean up.
		CleanupStack::PopAndDestroy(&iServer);		
		}

	INFO_PRINTF1(_L("&lt;&lt;CT_LbsClientStep_ResetAssistance::doTestStepL()"));

	return TestStepResult();
	}

TInt64 CT_LbsClientStep_ResetAssistance::DoRequests(TBool aCold, TBool aReset)
	{
	if(!aCold)
		{
		// *** Warming up module				
		// Open positioner.
		TInt err = iPositioner.Open(iServer);
		User::LeaveIfError(err);
		CleanupClosePushL(iPositioner);

		// Request notify for the expected reference position.
		TPositionInfo actualRefPosInfo;
		err = DoNotifyUpdateL(actualRefPosInfo);
		if (KErrNone != err)
			{
			INFO_PRINTF2(_L("Failed test, reference position request returned err %d."), err);
			SetTestStepResult(EFail);
			}		
	
		TPositionInfo posInfo;
		// Get the actual position
		err = DoNotifyUpdateL(posInfo);
		if (KErrNone != err)
			{
			INFO_PRINTF2(_L("Failed test, position request returned err %d."), err);
			SetTestStepResult(EFail);
			}
		}

	TTimeIntervalMicroSeconds requestTime;
	requestTime = 0;
	// *** Doing the repeated requests with reset assistance data in between if it's requested so
	for(TInt count = 0; count < KNumberOfRequests; ++count)
		{
		if(aReset)
			{
			SendResetAssistanceData(0xFFFFFFFF);
			}
		
		if(aCold)
			{
			// Open positioner.
			TInt err = iPositioner.Open(iServer);
			User::LeaveIfError(err);
			CleanupClosePushL(iPositioner);
			}
		
		TTime startTime;
		startTime.UniversalTime(); // start the clock

		if(aCold)
			{
			// Request notify for the expected reference position.
			TPositionInfo actualRefPosInfo;
			TInt err = DoNotifyUpdateL(actualRefPosInfo);
			if (KErrNone != err)
				{
				INFO_PRINTF2(_L("Failed test, reference position request returned err %d."), err);
				SetTestStepResult(EFail);
				}		
			}
		TPositionInfo posInfo;
		// Get the actual position
		TInt err = DoNotifyUpdateL(posInfo);
		if (KErrNone != err)
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
			CleanupStack::PopAndDestroy(&iPositioner);
			User::After(KWaitTimeForCoolDown);
			}
		}			

	if(!aCold)
		{
		// Cleanup.
		CleanupStack::PopAndDestroy(&iPositioner);
		User::After(KWaitTimeForCoolDown);
		}
	return requestTime.Int64();
	}
