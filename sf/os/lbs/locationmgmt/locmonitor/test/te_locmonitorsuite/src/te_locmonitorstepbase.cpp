/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/


/**
 @file te_locmonitorstepbase.cpp
*/
#include "te_locmonitorstepbase.h"
#include "te_locmonitorsuitedefs.h"
#include "tserverstartparams.h"
#include "cserverlaunch.h"
#include "lbslocmonitorserverdata.h"
#include "lbsdevloggermacros.h"
#include <lbs/lbsextendedsatellite.h>


TVerdict CTe_LocMonitorStepBase::doTestStepPreambleL()
	{
	LBSLOG(ELogP1, "CTe_LocMonitorStepBase::doTestStepPreambleL()\n");
	// START MEMORY TEST
	__UHEAP_MARK;

	// Construct a new ActiveScheduler and install it
	iActiveScheduler = new (ELeave) CActiveScheduler();
	CActiveScheduler::Install(iActiveScheduler);


	// Object used for injecting positions on different buses.
	// (Do this before creating the testlocationmonitor process
	//  because doing this will set in the cenrep the uid of 
	//  this process as the one that defines the P&S properties
	//  that conform the position buses. If we don't
	//  then the testlocationmonitor process subscribes to the wrong
	//  p&s property -category is different- and never gets updates)
	iPositionInjector = CPositionInjector::NewL();


	// Check if the process "testlocationmonitor.exe"
	// is already started
	_LIT(KTestLocMonFilename, "testlocationmonitor.exe");
	_LIT(KLocMonitorServerName, "locationmonitor.exe");

	// Check if the test location monitor process is running.
	// If it is not, then start it.
	TFindServer findServer(KLocMonitorServerName);
	TInt err;
	TFullName name;
	err = findServer.Next(name);
	
	if ( (err!=KErrNone) && (err!=KErrAlreadyExists) )
		{
		// Start the "test Location Monitor" with the same server
		// name as the real Location Monitor but different file name.
		// This ensures the client connects to the test version.
		TServerStartParams params;
		params.SetServerFileName(KTestLocMonFilename);
		params.SetServerName(KLocMonitorServerName);
		params.SetNumberOfServerSlots(KServerFrameworkDefaultMessageSlots);
		params.SetServerUids(KNullUid, KNullUid, TUid::Uid(0x1028227B));
		User::LeaveIfError(CServerLaunch::ServerLaunch(params));
		
		// Pause for 5 second to allow the location
		// monitor to receive network information from
		// etel (sim tsy)
		User::After(5000000);
		}

	// Attach to property created by the testlocationmonitor for communicating with this
	// test.
	err = iDbTestCommand.Attach(KTestLocMonUid, EUnitLocMonDbDelay, EOwnerThread);

	// Object used to wait for feedback from the Location Monitor DB.
	iLocMonDbListener = CLocMonDbFeedbackWaiter::NewL();

	SetTestStepResult(EPass);
	return TestStepResult();
	}

TVerdict CTe_LocMonitorStepBase::doTestStepPostambleL()
	{
	LBSLOG(ELogP1, "CTe_LocMonitorStepBase::doTestStepPostambleL()\n");
	// Bring down the test version of the location monitor
	KillLocationMonitorProcess();

	delete iPositionInjector;
	delete iLocMonDbListener;	
	delete iActiveScheduler;
	
	// END MEMORY TEST
	__UHEAP_MARKEND;
	
	return TestStepResult();
	}

CTe_LocMonitorStepBase::~CTe_LocMonitorStepBase()
	{
	LBSLOG(ELogP1, "CTe_LocMonitorStepBase::~CTe_LocMonitorStepBase()\n");
	}

CTe_LocMonitorStepBase::CTe_LocMonitorStepBase()
	{
	LBSLOG(ELogP1, "CTe_LocMonitorStepBase::CTe_LocMonitorStepBase()\n");
	}

// This method may be called by concrete test steps to inform the
// test version of the Database in the Location Monitor that it 
// must delay by 1 second the completion of the next request 
// (save or get position) it receives.
// The purpose is to allow testing of the queueing mechanisms in the 
// Location Monitor server: since the database is 'stuck' and can only
// handle one request at a time, the requests sent by the test while
// the delay lasts will pile up in the server's queues.
void CTe_LocMonitorStepBase::SetShortDbResponseDelay()
	{
	LBSLOG(ELogP1, "CTe_LocMonitorStepBase::SetShortDbResponseDelay()\n");
	iLocMonDbListener->ListenForLocMonDbFeedback();
	iDbTestCommand.Set(EDbSetupShortDelay); //1 second delay
	// Wait for feedback from the DB confirming the delay will be applied
	iLocMonDbListener->WaitForDbFeedback();
	}
	
// This method may be called by concrete test steps to inform the
// test version of the Database in the Location Monitor that it 
// must delay by 15 seconds the completion of the next request 
// (save or get position) it receives.
// The purpose is to ensure the request times out in the Location
// Monitor (requestor) and it is cancelled.
void CTe_LocMonitorStepBase::SetLongDbResponseDelay()
	{
	LBSLOG(ELogP1, "CTe_LocMonitorStepBase::SetLongDbResponseDelay()\n");
	iLocMonDbListener->ListenForLocMonDbFeedback();
	iDbTestCommand.Set(EDbSetupLongDelay); //15 second delay
	// Wait for feedback from the DB confirming the delay will be applied
	iLocMonDbListener->WaitForDbFeedback();
	}
	
// This method may be called by concrete test steps to inform the
// test version of the Database in the Location Monitor that it 
// must complete the next request (save or get position) it receives.
// The completion reason will be KErrNotFound.
void CTe_LocMonitorStepBase::SetDbFailResponse()
	{
	LBSLOG(ELogP1, "CTe_LocMonitorStepBase::SetDbFailResponse()\n");
	iLocMonDbListener->ListenForLocMonDbFeedback();
	iDbTestCommand.Set(EDbSetupFail);
	// Wait for feedback from the DB confirming the delay will be applied
	iLocMonDbListener->WaitForDbFeedback();
	}
	
// This method is used to inform the test version of the Database
// in the Location Monitor that the Location Monitor process has to
// be finished. Normally called at the end of a test.
void CTe_LocMonitorStepBase::KillLocationMonitorProcess()
	{
	LBSLOG(ELogP1, "CTe_LocMonitorStepBase::KillLocationMonitorProcess()\n");
	iLocMonDbListener->ListenForLocMonDbFeedback();
	TInt error =  iDbTestCommand.Set(EDbKillProcess);
	iLocMonDbListener->WaitForDbFeedback();
	//User::After(5000000);
	}
/*******************************************************************************************************
	IMPLEMENTATION OF THE DATABASE FEEDBACK LISTENER
********************************************************************************************************/

CLocMonDbFeedbackWaiter:: CLocMonDbFeedbackWaiter():
						CActive(EPriorityStandard)
	{
	LBSLOG(ELogP1, "CLocMonDbFeedbackWaiter::CLocMonDbFeedbackWaiter()\n");
	CActiveScheduler::Add(this);
	}

CLocMonDbFeedbackWaiter* CLocMonDbFeedbackWaiter::NewL()
	{
	LBSLOG(ELogP1, "CLocMonDbFeedbackWaiter::NewL()\n");
	CLocMonDbFeedbackWaiter* self = new(ELeave) CLocMonDbFeedbackWaiter();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CLocMonDbFeedbackWaiter::ConstructL()
	{
	LBSLOG(ELogP1, "CLocMonDbFeedbackWaiter::ConstructL()\n");
	iNestedScheduler = (CActiveSchedulerWait*) new CActiveSchedulerWait;
	iTimer = CLbsCallbackTimer::NewL(*this);
	User::LeaveIfError(iDbFeedbackProperty.Attach(KTestLocMonUid,EUnitLocMonDbFeedback,EOwnerThread));
	}
	
CLocMonDbFeedbackWaiter::~CLocMonDbFeedbackWaiter()
	{
	LBSLOG(ELogP1, "CLocMonDbFeedbackWaiter::~CLocMonDbFeedbackWaiter()\n");
	Cancel();
	delete iTimer;
	delete iNestedScheduler;
	iDbFeedbackProperty.Close();
	}

void CLocMonDbFeedbackWaiter::RunL()
	{
	LBSLOG(ELogP1, "CLocMonDbFeedbackWaiter::RunL()\n");
	ASSERT(KErrNone == iStatus.Int());
	iTimer->Cancel();
	TPtr8 ptr(reinterpret_cast<TUint8*>(&iDbData), sizeof(TDbFeedback), sizeof(TDbFeedback));
	iDbFeedbackProperty.Get(ptr);
	iNestedScheduler->AsyncStop();
	}

TInt CLocMonDbFeedbackWaiter::RunError(TInt aError)
	{
	LBSLOG(ELogP1, "CLocMonDbFeedbackWaiter::RunError()\n");
	// Just panic
	ASSERT(EFalse);
	return aError;
	}

void CLocMonDbFeedbackWaiter::DoCancel()
	{
	LBSLOG(ELogP1, "CLocMonDbFeedbackWaiter::DoCancel()\n");
	iTimer->Cancel();
	iDbFeedbackProperty.Cancel();
	iNestedScheduler->AsyncStop();
	}


void CLocMonDbFeedbackWaiter::ListenForLocMonDbFeedback()
	{
	LBSLOG(ELogP1, "CLocMonDbFeedbackWaiter::ListenForLocMonDbFeedback()\n");
	iDbFeedbackProperty.Subscribe(iStatus);
	SetActive();
	}

void CLocMonDbFeedbackWaiter::WaitForDbFeedback()
	{
	LBSLOG(ELogP1, "CLocMonDbFeedbackWaiter::WaitForDbFeedback()\n");
	const TInt KFiveSecs = 5000000;
	const TInt KObserverTimerId = 1;
	// If the DB does not provide feedback in less than 5 seconds the test will fail
	iTimer->EventAfter(TTimeIntervalMicroSeconds32(KFiveSecs), KObserverTimerId);
	iNestedScheduler->Start();
	}


void CLocMonDbFeedbackWaiter::OnTimerEventL(TInt /*aTimerId*/)
	{
	LBSLOG(ELogP1, "CLocMonDbFeedbackWaiter::OnTimerEventL()\n");
	// The DB has not provided feedback on time. Terminate the test.
	__ASSERT_ALWAYS(EFalse, User::Invariant());	
	}

TInt CLocMonDbFeedbackWaiter::OnTimerError(TInt /*aTimerId*/, TInt /*aError*/)
	{
	LBSLOG(ELogP1, "CLocMonDbFeedbackWaiter::OnTimerError()\n");
	// nothing can be done
	__ASSERT_ALWAYS(EFalse, User::Invariant());
	return KErrTimedOut;			
	}


/*******************************************************************************************************
	IMPLEMENTATION OF THE POSITION INJECTOR
	(used to set a position in any of the internal LBS buses)
********************************************************************************************************/

CPositionInjector:: CPositionInjector()
	{
	LBSLOG(ELogP1, "CPositionInjector::doTestStepPostambleL()\n");
	}
	
CPositionInjector* CPositionInjector::NewL()
	{
	LBSLOG(ELogP1, "CPositionInjector::doTestStepPostambleL()\n");
	CPositionInjector* self = new(ELeave) CPositionInjector();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CPositionInjector::ConstructL()
	{
	LBSLOG(ELogP1, "CPositionInjector::doTestStepPostambleL()\n");
	TPositionExtendedSatelliteInfo posSatInfo;

	// Create bus used to publish GPS positions
	RLbsPositionUpdates::InitializeL(posSatInfo);
	iGpsPositionUpdates.OpenL(KLbsGpsLocManagerUid);
	}
   
CPositionInjector::~CPositionInjector()
	{
	LBSLOG(ELogP1, "CPositionInjector::doTestStepPostambleL()\n");
	TRAPD(err,RLbsPositionUpdates::ShutDownL());
	if(err!=KErrNone)
	    {
        LBSLOG2(ELogP3, "RLbsPositionUpdates::ShutDownL()left with error  = %d\n", err);
	    }
	}
	
void CPositionInjector::InjectGpsPosition(const TPosition& aPosition)
	{
	LBSLOG(ELogP1, "CPositionInjector::doTestStepPostambleL()\n");
	TPositionSatelliteInfo positionInfo;
	positionInfo.SetPosition(aPosition);
	
	// Variables whose value is not important for the test but are
	// needed nonetheless to inject a position in the GPS bus
	TTime irrelevantTargetTime, unimportantActualTime; 
	TUint insignificantAttributes, inconsecuentialGpsMode;
	
	// This is what the AGPS Manager does to inject a position into
	// the GPS bus for the LocServer and NRH
	TInt error = iGpsPositionUpdates.SetPositionInfo(KErrNone,
									EFalse, // no conflict control flag
									&positionInfo,
									sizeof(TPositionSatelliteInfo),
									irrelevantTargetTime,
									unimportantActualTime,
									insignificantAttributes,
									inconsecuentialGpsMode);
	}

